/**************************************************************************
*** COPYRIGHT (c) 2002 by TransNexus, Inc.                              ***
***                                                                     ***
*** This software is property of TransNexus, Inc.                       ***
*** This software is freely available under license from TransNexus.    ***
*** The license terms and conditions for free use of this software by   ***
*** third parties are defined in the OSP Toolkit Software License       ***
*** Agreement (LICENSE.txt).  Any use of this software by third         ***
*** parties, which does not comply with the terms and conditions of the ***
*** OSP Toolkit Software License Agreement is prohibited without        ***
*** the prior, express, written consent of TransNexus, Inc.             ***
***                                                                     ***
*** Thank you for using the OSP ToolKit(TM).  Please report any bugs,   ***
*** suggestions or feedback to support@transnexus.com                   ***
***                                                                     ***
**************************************************************************/







#include "osposincl.h" 
#include "ospcomm.h"
#include "ospossys.h"
#include "osputils.h"
#include "ospsocket.h"

#include "openssl/ssl.h"
#include "openssl/bio.h"
#include "openssl/pem.h"
#include "openssl/evp.h"

#include "osptnepdatatypes.h"
#include "osptneperrno.h"
#include "osptnepinit.h"
#include "osptnepenroll.h"
#include "osptneputil.h"

/**
 * These are the function prototypes as given in the TNEP ( TransNexus
 * Enrollment Protocol ) Client specifications.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#define OSPC_DEBUG 1

/* Denotes the failure of a function to perform some task: */
#define OSPC_ENROLL_FAILURE_DEFAULT 0xFFFF 

/* The character string that denotes a function for creating enrollment
 * parameters; this should be passed in through the command line or whatever
 * input method is being used:
 */
#define OSPC_ENROLL_CREATE_ENROLL_PARAMS_FUNCTION "init"

#define OSPC_ENROLL_CA_CERT_FUNCTION       "getcacert"
#define OSPC_ENROLL_REQUEST_FUNCTION       "request"
#define OSPC_ENROLL_RETRIEVE_FUNCTION      "retrieve"

#define OSPC_ENROLL_FUNCTION_PARAM         "-function"
#define OSPC_ENROLL_USERNAME_PARAM         "-username"
#define OSPC_ENROLL_PASSWORD_PARAM         "-password"
#define OSPC_ENROLL_DEVICEID_PARAM         "-device"
#define OSPC_ENROLL_CUSTOMERID_PARAM       "-customer"
#define OSPC_ENROLL_CA_URL_PARAM           "-caurl"
#define OSPC_ENROLL_SSL_URL_PARAM          "-sslurl"
#define OSPC_ENROLL_CACERT_PARAM           "-cacert"
#define OSPC_ENROLL_CERTREQ_PARAM          "-certreq"
#define OSPC_ENROLL_CA_FPRINT_PARAM        "-fprint"
#define OSPC_ENROLL_NONCE_PARAM            "-nonce"

/* These arguments specify that the enrollment process should be verbose: */
#define OSPC_ENROLL_VERBOSE_PARAM          "-v"
#define OSPC_ENROLL_DEBUG_PARAM            "-debug"
    
/* Additional macros for common strings.h, ctype.h, and stdlib.h functions: */
#define OSPM_ISALNUM(c)                    isalnum(c)
#define OSPM_ISALPHA(c)                    isalpha(c)
#define OSPM_ISASCII(c)                    isascii(c)
#define OSPM_ISDIGIT(c)                    isdigit(c)
#define OSPM_ISXDIGIT(c)                   isxdigit(c)
#define OSPM_ATOI(s)                       atoi(s)

#define OSPM_STRCHR(s,c)                   strchr(s,c)
#define OSPM_STRSPN(s1,s2)                 strspn(s1, s2)
#define OSPM_STRPBRK(s1,s2)                strpbrk(s1, s2)

#define OSPM_STRTOL(ptr,endptr,base)       strtol( ptr, endptr, base )

/* The number of characters that will be on each line of base64-encoded
 * text that is printed out:
 */
#define OSPC_ENROLL_B64_COLUMNS_PER_LINE   64


/* Define how to extract one byte from a long; if the long is 64-bit, then
 * this will have to be expanded to 14 "0"s.
 */
#define OSPC_LONG_TO_BYTE_MASK            0x000000FF

/* Define what the command executed is named as a default. We'll only
 * use this if we can't determine what was entered on the command line
 * for executing this program:
 */
#define OSPC_ENROLL_DEFAULT_CMD           "enroll"

/* Define the help message that gets displayed when no parameters or
 * incorrect parameters are passed in on input:
 */
#define OSPC_ENROLL_HELP_MSG " \
-function { getcacert | request | retrieve } [params]\n\
\n\
  getcacert get Certificate Authority information \n\
  request   start enrollment by issuing an enrollment request\n\
  retrieve  retrieve a certificate (if available)\n\
\n\
  [getcacert params]\n\
  -caurl <URL>         http://   URL for retrieving CA info\n\
  -fprint <fprint>     optional CA certificate fingerprint\n\
                       (in hexadecimal)\n\
\n\
  [retrieve and request params]\n\
  -cacert <cert>       base64-encoded authority certificate\n\
  -certreq <pkcs10>    base64-encoded certificate request\n\
  -customer <custID>   TransNexus-assigned customer number\n\
  -device <devID>      TransNexus-assigned device id\n\
  -nonce <nonce>       random value to increase security\n\
  -password <pwd>      password for TransNexus services\n\
  -username <username> username for TransNexus services\n\
  -sslurl <URL>        https://   URL for enrollment server\n\
\n\
   exit level:\n\
   0  -  enrollment succeeded\n\
   1  -  enrollment request pending\n\
   2  -  user error\n\
  "


/*************************************************************************
 * Function prototypes:
 *************************************************************************/
/* This is a convenience function for checking arguments. For each
 * character in the input string, change the character to lowercase
 * if it's A-Z. OSPM_TOLOWER will be used for the sake of compatibility.
 *
 * Input: String to be converted to lowercase, and the output string to
 *        place it in.
 */
int OSPPEnrollStringLowercase( 
    const char* ospvStringIn,
    char* ospvStringLowercaseOut
);

/* 
 * This function will parse the parameters that are sent on the command line. 
 */
int OSPPEnrollParseParameters (
    int                  ospvArgc,
    char*                ospvArgv[],
    OSPTENROLLPARAMS*    enrollParams
);

/* Given the binary of a BER-encoded certificate, print out its base64
 * encoding to STDOUT. We'll need the length of the certificate just as
 * a convenience for knowing where to stop printing.
 *
 * Input: the certificate and its length ( minus the terminating null. ) 
 */
int OSPPPrintCert(
    unsigned char* ospvCert,
    unsigned       ospvCertLen 
);

/* Print a block of base64-encoded text, given the text and the length 
 * to print. We'll separate each line with a newline and a carriage return
 * so that this function will write truly base64-encoded text.
 *
 * Input: the base64-encoded block to be printed and the length of the block.
 */
int OSPPPrintB64Text(
    unsigned char* ospvTextBlock,
    unsigned       ospvTextBlockLen
);

/* Given a certificate ( which may be null ), its length ( which may be
 * less than or equal to zero ), and the status of an enrollment request,
 * report the enrollment request's status and certificate ( if available )
 * to the user.
 *
 * Input: the certificate ( in binary ), the cert's length, and the
 *        status of the request ( as returned from the enrollment server. )
 */
void OSPPPrintCertAndStatus(
	unsigned char* ospvCert,
	unsigned       ospvCertLen,
	unsigned       ospvEnrollStatus 
);

/* Print the usage for the enrollment command. The first string in
 * is what the user typed in for the command; it may not necessarily
 * be "enroll". The second string is the help message that the user
 * receives. The message should explain what the user should enter
 * in order for the enrollment process to be successful.
 *
 * Input: string entered for executing this program.
 */
void OSPPEnrollUsage( 
	char* ospvCmd, 
	char* ospvHelpMsg 
);

#ifdef __cplusplus
}
#endif
