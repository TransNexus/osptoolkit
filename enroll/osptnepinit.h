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







#include "osp/osposincl.h"
#include "openssl/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* Constants for the maximum sizes of the enrollment parameters: */
#define OSPC_ENROLL_MAX_FUNCTION_SIZE    16 
#define OSPC_ENROLL_MAX_USERNAME_SIZE    32 
#define OSPC_ENROLL_MAX_PASSWORD_SIZE    32
#define OSPC_ENROLL_MAX_DEVICEID_SIZE    20 
#define OSPC_ENROLL_MAX_CUSTOMERID_SIZE  20 
#define OSPC_ENROLL_MAX_CERTREQ_SIZE     2048
#define OSPC_ENROLL_MAX_CACERT_SIZE      2048
#define OSPC_ENROLL_MAX_URL_SIZE         2048 

/* An SHA-1 fingerprint is 20 bytes long, so expect at most
 * 40 bytes of hex characters ( since an MD5 digest is 16 bytes
 * long ):
 */
#define OSPC_ENROLL_MAX_FPRINT_SIZE      40 


/* This template will be used for finding the value of a field being
 * searched for in a string. Here's what it does:
 *
 * %*[^=]     matches on the beginning of the string up to an "="; this 
 *            field's value is discarded.
 *
 * =          matches on the "=" itself
 *
 * %[^& ]     matches on everything up to an "&" or a " "; this field is
 *            saved in the first argument to sscanf
 */
#define OSPC_ENROLL_SEARCH_PATTERN       "%*[^=]=%[^& ]"

/* These are the only characters that may precede a field name in a URL.
 * The '&' comes up when the field follows another field ( as in
 * a=b&c=d, where 'c' is the field name being searched for. ) The
 * newline may come up in cases where we receive the entire HTTP header
 * and body in the message ( which shouldn't happen, but which is accounted
 * for anyway ). The space ( " " ) shouldn't occur in any normal situation,
 * but it may if the body has other information in it ( and the space is
 * a delimiter. )
 */
#define OSPC_ENROLL_URL_FIELD_DELIMITERS "&\n "

/* 
 * Initialize the Enrollment Parameter object sent in. All of the
 * structures will have to be malloced in this case.
 *
 * Input: Pointer to an OSPTENROLLPARAMS object
 *
 * Errors:
 *     o the pointer to the OSPTENROLLPARAMS is null;
 *     o malloc or initialization fails on any member of the OSPTENROLLPARAMS.
 */
int OSPPInitEnrollParams (
    OSPTENROLLPARAMS* ospvEnrollParamsIn
);

/* 
 * This function is called for freeing up all the memory taken up
 * by an enrollment parameter object.
 *
 * Input: 
 *     o pointer to an OSPTENROLLPARAMS;
 *
 * Errors:
 *     o if the input variable is null ( OSPC_ERR_ENROLL_ENROLL_PARAMS_FREE ) 
 */
int OSPPFreeEnrollParams (
    OSPTENROLLPARAMS* ospvEnrollParamsIn
);

/* 
 * This function will take a list of enrollment parameters and communications
 * parameters that specify how a communications manager will be set up.
 * The communications manager will first be created and then initialized
 * with all of the non-SSL-related parameters, followed by all of the
 * SSL-related parameters.
 *
 * Input: a pointer to the enrollment server parameters ( for the enrollment
 *        server's url ); a pointer to the communication parameters; and a 
 *        pointer to the reference to the communication manager to be initialized.
 *        *ospvCommOut should be null ( but ospvCommOut should not. )
 *
 * Output: *ospvCommMgrOut should be initialized as a communications manager.
 *         If it can't be initialized, then it should be null and the 
 *         return value will be non-zero. Otherwise, the return value should
 *         be zero.
 */
int OSPPInitSecureCommMgr(
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*  ospvCommParamsIn,
    OSPTCOMM**       ospvCommOut 
);

/* This is just a minimal wrapper for calling OSPPInitCommMgr;
 * it just extracts the service point for the CA from the enrollment 
 * parameters and passes it along as the service point to be explicitly 
 * contacted.
 *
 * Input: a pointer to the enrollment server parameters ( for the CA's
 *        url ); a pointer to the communication parameters; and a pointer
 *        to the reference to the communication manager to be initialized.
 *
 * Output: *ospvCommMgrOut should be initialized as a communications manager.
 *         If it can't be initialized, then it should be null and the 
 *         return value will be non-zero. Otherwise, the return value should
 *         be zero.
 */
int OSPPInitCACommMgr(
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM**        ospvCommOut 
);

/*
 * Initialize the communications manager. First a service point for the
 * given ospvServicePointURL will be generated. Then, if the 
 * ospvNumberofAuthoirtyCertificates is less than or equal to 0, then
 * the CA certificate will be retrieved from the enrollment server defined
 * in the OSPTSVCPT that was just created. OSPPCommNew will be called to
 * create a new communications manager ( OSPTCOMM ); which will then have
 * various connection-oriented parameters ( denoted by ospvSSLLifetime,
 * ospvHTTPMaxConnections, ospvHTTPPersistance, ospvHTTPRetryDelay,
 * ospvHTTPRetryLimit, and ospvHTTPTimeout in the OSPTCOMMPARAMS parameter ) 
 * set.
 */
int OSPPInitCommMgr(
    unsigned char*    ospvServicePtUrlIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM**        ospvCommOut
);

/*
 * Initialize the communications manager's SSL-related parameters. In this
 * case, we'll need: the CA certificate ( given in the CACert field of
 * the enrollment parameters ); the service point to be accessed
 * ( given in the SSLUrl field of the enrollment parameters ); the
 * lifetime of an SSL session ( given in the SSLLifetime field of the
 * communications parameters ); and the communications manager itself.
 *
 * Input: a list of enrollment parameters ( for the CACert and SSLUrl fields of
 *        the OSPTENROLLPARAMS* ); a list of communications parameters
 *        ( for the SSLLifetime field of the OSPTCOMMPARAMS* ); and a
 *        reference to a communications manager.
 *
 * Output: the communications manager will have its parameters set, if
 *         possible. A return value of OSPC_ERR_NO_ERROR should indicate that
 *         there have been no problems. Any other return value will indicate
 *         an error.
 */
int OSPPInitSSLCommMgrParams(
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM*         ospvCommOut
);

/* Initialize all of the non-SSL related parameters; we'll set the
 * retry delay, http timeout, persistence values, and so on. All of
 * the SSL-related parameters are set in the OSPPInitSSLCommMgrParams,
 * which won't be necessary for certain functions ( like retrieving the
 * CA certificate, which is currently done without SSL. )
 *
 * Input: pointers to the communications parameters and the service point's
 *        url, as well as the outgoing communications manager. 
 *
 * Output: The communication manager's parameters should be set up.
 *         A return value of OSPC_ERR_NO_ERROR is returned if everything
 *         worked out; otherwise, an error code is returned.
 */
int OSPPInitNonSSLCommMgrParams(
    unsigned char*    ospvServicePtUrlIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM*         ospvCommOut 
);

/*
 * This function will: initialize a communications manager; construct
 * a message to be sent to a service point; parse the results; and validate
 * the contents of the certificate. We'll need the enrollment parameters
 * so that we know which CA to contact for  a certificate, and we'll
 * need the communications parameters for initializing the communications
 * manager. The enrollment parameters will also be used to store the
 * CA certificate that is returned ( accessed by 
 * ospvEnrollParamsInOut->CACert ).
 *
 * Input: a pointer to the enrollment parameter list and a pointer to the
 *        communications parameter list.
 *
 * Output: the CA certificate ( if it was found ) in the enrollment parameter
 *         list ( under the CACert field ). OSPC_ERR_NO_ERROR is returned
 *         if everything went ok; otherwise, some other error code is returned.
 */
int OSPPRetrieveCACert (
    OSPTENROLLPARAMS* ospvEnrollParamsInOut,
    OSPTCOMMPARAMS*   ospvCommParamsIn
);

/*
 * Create the request for retrieving a CA certificate and store it in
 * ospvCARequestOut. We'll need the enrollment parameters so that we can
 * add in the username, password, device id, and customer id.
 */
int OSPPCreateCARetrievalRequest (
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    unsigned char**   ospvCARequestOut
);

/* Extract the CA certificate from the response and base64-decode it.
 * We'll also use the base64-encoded certificate to be passed back to
 * the user so that they can use it again on input ( instead of having
 * to redo the base64 encoding. )
 *
 * Input: a character string representing the response to be scanned, 
 *        along with outgoing parameters for the binary BER-encoding
 *        of the CA certificate, its length, the base64 encoding of the
 *        certificate, and its length as well.
 *
 * Output: Along with the values of the outgoing parameters, a return value
 *         of OSPC_ERR_NO_ERROR is returned if the variables could be
 *         allocated and assigned properly. Otherwise, a return value of
 *         something other than OSPC_ERR_NO_ERROR is returned.
 */
int OSPPExtractCACertFromResponse( 
    unsigned char*  ospvResponseIn,
    unsigned        ospvResponseLenIn,
    unsigned char** ospvCACertOut,
    unsigned*       ospvCACertLenOut,
    unsigned char** ospvCACertB64Out,
    unsigned*       ospvCACertB64LenOut
);

/* Now take the enrollment parameter's CA certificate fingerprint in hex
 * ( which should have been taken from a secure channel, such as paper 
 * [ if ubiquity can be considered security ] or an SSL session with the
 * enrollment server's accompanying web site ( if available. ) If the 
 * fingerprint is missing, then an error will not be generated; we'll assume
 * for now ( although this is debatable ) that the fingerprint isn't 
 * necessary. If any of the other parameters are otherwise null, then
 * this function will fail. If the CA certificate is missing, then this
 * function will fail regardless of whether or not we care about checking
 * for the fingerprint. This may be changed too ( just rearrange the ordering
 * of the statements for checking the null values. )
 *
 * Input: a pointer to the enrollment parameters, which contains the CA 
 *        certificate and the CA's intended fingerprint.
 *
 * Output: an error code if anything is null or if the fingerprint cannot be
 *         taken or doesn't match up to what's expected.
 */
int OSPPValidateCACertificate(
    OSPTENROLLPARAMS* ospvEnrollParams
);

/* Translate the given hex string into binary, if possible. */
int OSPPHexToBinary (
    unsigned char* ospvHexStr,
    unsigned       ospvHexStrLen,
    unsigned char* ospvBinaryStr,
    unsigned*      ospvBinaryStrLen 
);

/* Given the response to scan and the field to search for, extract the
 * contents of that field into the outgoing value. We'll be careful
 * not to accidentally use names that look similar but which have different
 * lengths. For example, if we're looking for the field "def" and we're
 * given "abcdef=ghi&def=jkl", then we want to return "jkl" and not
 * "ghi". If we find an appropriate field, then we'll save it in 
 * the outgoing pointer ( which we'll also allocate. )
 *
 * Input: the response to be scanned, the field to look for, and a pointer
 *        to a string that will contain the field's value ( if we find it. )
 *
 * Output: *ospvValueOut will be non-null and will contain the contents of
 *         the field if the field can be found and is non-empty. Otherwise,
 *         if the field is empty or can't be found, then an error code
 *         other than OSPC_ERR_NO_ERROR will be returned and *ospvValueOut
 *         will be untouched.
 */
int OSPPExtractFieldFromResponse (
    unsigned char*  ospvResponse,
    unsigned        ospvResponseLen,
    unsigned char*  ospvSearchField,
    unsigned char** ospvValueOut 
);

/* copy the source string to the destination string; allocate and initialize
 * enough space in the destination string that we can fit everything in
 * the source string.
 *
 * Input: pointer to the destination string and the source string to be
 *        copied.
 *
 * Output: *ospvDestStr should be a replica of ospvSrcStr if everything goes
 *         well; in this case, OSPC_ERR_NO_ERROR is returned. Otherwise,
 *         an error code other than OSPC_ERR_NO_ERROR is returned.
 */
int OSPPCopyString (
    unsigned char** ospvDestStr,
    unsigned char*  ospvSrcStr
);

/*
 * Cleanup everything that we initialized. In this case, we just need to
 * cleanup the communications manager; other structures or objects may need
 * to be cleaned up, too.
 */
int OSPPEnrollCleanup ( 
    OSPTCOMM* ospvCommMgr
);

#define OSPPCommMgrSetSPMessageCount(ospvComm,ospvMessageCount) OSPPProviderSetSPMessageCount(ospvComm,ospvMessageCount)
#ifdef __cplusplus
}
#endif
