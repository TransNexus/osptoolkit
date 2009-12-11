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

#include "osp/ospcomm.h"
#include "osp/ospmsginfo.h"
#include "osptnep.h"
#include "osp/ospb64.h"

#define HW_ENABLE 0

/*
 * Enroll this device. This will consist of:
 *   o parsing command line options;
 *   o retrieving the CA certificate from the enrollment server;
 *   o generating a request to be sent to the enrollment server;
 *   o initializing the communications manager;
 *   o transmitting the request to the enrollment server;
 *   o parsing the response for a status ( success, pending, failed ) and
 *     a certificate ( if the request was successful ).
 *
 * Requesting a CA certificate and requesting the device's own certificate
 * are two separate functions and require that this program be invoked
 * twice. This is just a sample program; retrieving a CA certificate and
 * retrieving the device's certificate can probably linked together, but
 * some care and caution must be taken in how the communications manager(s)
 * are started, stopped, and configured.
 */
int main(int argc, char *argv[])
{
    /*
     * The return value from each function; it's set to be a failure code,
     * just in case a function that fails doesn't set it.
     */
    unsigned retVal = OSPC_ENROLL_FAILURE_DEFAULT;

    /*
     * The enrollment parameters that are received on the command line,
     * as documented in the requirements and design:
     */
    OSPTENROLLPARAMS enrollParams;

    /*
     * These are the parameters that define the communication manager's
     * running parameters, but which are not defined as input for hte
     * OSP TNEP Client Requirements. These will most likely not be entered
     * by an end user in the same scope as an enrollment request, but as
     * part of some device configuration that may just rely on system-
     * generated defaults. They are only used for contacting an enrollment
     * server, so they will not be used for the initial setup that's done
     * for generating private keys, certificate requests, and so on.
     */
    OSPTCOMMPARAMS commParams;

    /*
     * The certificate retrieved from the enrollment server. This
     * may not be saved in stable storage, so it is possible that the
     * enrollment client will have to fetch it for every operation.
     */
    unsigned char *localCert = OSPC_OSNULL;

    /* The length of the certificate returned from the enrollment server. */
    unsigned localCertLen = 0;

    /* The status of an enrollment: 0 for success, 1 for pending,  >= 2 for failure: */
    unsigned enrollStatus = OSPC_ENROLL_STATUS_FAILURE_DEFAULT;

    /* Initialize the Toolkit library */
    OSPPInit(HW_ENABLE);

    /*
     * Set the communications parameters to some sample values; these
     * can be set on standard input ( along with all of the other
     * enrollment parameters ), but the list of values required on
     * the command line begins to get too long.
     */
    /* Set an SSL lifetime of 5 minutes: */
    commParams.SSLLifetime = 300;

    /* Max of 2 HTTP connections: */
    commParams.HTTPMaxConnections = 2;

    /* 10 second persistence for HTTP connections: */
    commParams.HTTPPersistence = 10;

    /* Wait 5 seconds before retrying a server: */
    commParams.HTTPRetryDelay = 5;

    /* Then again, don't retry at all: */
    commParams.HTTPRetryLimit = 0;

    /* 10 second HTTP timeout: */
    commParams.HTTPTimeout = 10000;

    /* initialize Winsock Library if necessary */
    OSPM_INITWINSOCK(retVal);

    /* Initialize the enrollment parameters first: */
    retVal = OSPPInitEnrollParams(&enrollParams);

    /*
     * o Parse the parameter list;
     * o Check that the user chose a function or operation to perform
     *   ( not necessarily valid - just that they entered a function );
     * o Now choose an operation based on the function entered:
     *     - retrieve a CA cert if the function was "cacert"
     *     - request or retrieve the device's certificate if the function
     *       was "request" or "retrieve"
     *
     * o Free up the memory in the enrollment parameters; this includes
     *   everything but the certificate for the device.
     * o Free up the memory in the certificate generated, if one was generated
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPEnrollParseParameters(argc, argv, &enrollParams);
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to parse input parameters.\n");
        }
    }

    /* Now check that the user entered a function: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        /*
         * The parameters could be read, so now check the
         * function's integrity.
         */
        if ((enrollParams.Function == OSPC_OSNULL) || (OSPM_STRLEN((const char *)enrollParams.Function) <= 0)) {
            retVal = OSPC_ERR_ENROLL_PARAMS_FUNCTION;
            OSPM_DBGERRORLOG(retVal, "No function was requested.\n");
            OSPPEnrollUsage(argv[0], OSPC_ENROLL_HELP_MSG);
        }
    }

    /* Now execute whatever function the user entered, if it's a valid operation request. */
    if (retVal == OSPC_ERR_NO_ERROR) {
        /*
         * If ( the function is for retrieving the CA certificate ) then
         *  o retrieve the CA certificate
         *  o if ( there was a problem ) then complain;
         *  o else ( dump the hex encoding of the certificate's contents )
         */
        if (OSPM_STRCMP((const char *)enrollParams.Function, OSPC_ENROLL_CA_CERT_FUNCTION) == 0) {
            retVal = OSPPRetrieveCACert(&enrollParams, &commParams);

            if (retVal != OSPC_ERR_NO_ERROR) {
                OSPM_DBGERRORLOG(retVal, "Unable to retrieve the CA certificate\n");
            } else {
                OSPM_PRINTTOERR((stderr, "\nCA certificate received\n"));
                OSPM_PRINTF ("-----BEGIN CERTIFICATE-----\r\n%s\r\n-----END CERTIFICATE-----\r\n", enrollParams.CACertB64);
            }
        }
        /*
         * Else ( if the function is to request or retrieve a cert ) then
         *     o enroll the device with the enrollment server
         */
        else if ((OSPM_STRCMP((const char *)enrollParams.Function, OSPC_ENROLL_REQUEST_FUNCTION) == 0) ||
            (OSPM_STRCMP((const char *)enrollParams.Function, OSPC_ENROLL_RETRIEVE_FUNCTION) == 0))
        {
            retVal = OSPPEnroll(&enrollParams, &commParams, &localCert, &localCertLen, &enrollStatus);

            if (retVal != OSPC_ERR_NO_ERROR) {
                OSPM_DBGERRORLOG(retVal, "Error encountered with enrollment.\n");
            }

            OSPPPrintCertAndStatus(localCert, localCertLen, enrollStatus);
        }

        else {
            OSPM_DBGMISC(("Invalid function parameter passed in: <%s>\n", enrollParams.Function));
            OSPPEnrollUsage(argv[0], OSPC_ENROLL_HELP_MSG);
        }
    }

    OSPPFreeEnrollParams(&enrollParams);

    /* Free up the certificate generated: */
    if (localCert != OSPC_OSNULL) {
        OSPM_FREE(localCert);
    }

    /* Clean up the toolkit library */
    OSPPCleanup();
    return retVal;
}

/*
 * This is a convenience function for checking arguments. For each
 * character in the input string, change the character to lowercase
 * if it's A-Z. OSPM_TOLOWER will be used for the sake of compatibility.
 *
 * Input: String to be converted to lowercase, and the output string to
 *        place it in.
 */
int OSPPEnrollStringLowercase(const char *ospvStringIn, char *ospvStringLowercaseOut)
{
    /* The length of the input string: */
    int inputStringLen = 0;

    /* Index for parsing the input string: */
    int inputStringIndex = 0;

    /*
     * There is initially no error to be worried about; there are
     * only errors if the output string cannot be initialized.
     */
    int retVal = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPEnrollStringLowercase\n"));

    /*
     * If ( the input string and output string aren't null ) then
     *  o set the input string length;
     *  o if ( we can set the memory of the output string to 0's ) then
     *      - for ( each character in the input string )
     *          o copy it to the output string.
     *  o else ( we couldn't initialize the memory ) so
     *      - record an error.
     */
    if ((ospvStringIn != OSPC_OSNULL) && (ospvStringLowercaseOut != OSPC_OSNULL)) {
        inputStringLen = OSPM_STRLEN(ospvStringIn);

        /* Now blank out the output string: */
        OSPM_MEMSET(ospvStringLowercaseOut, 0, inputStringLen);

        for (inputStringIndex = 0; inputStringIndex < inputStringLen; inputStringIndex++) {
            ospvStringLowercaseOut[inputStringIndex] = (char) (OSPM_TOLOWER(ospvStringIn[inputStringIndex]));
        }
        ospvStringLowercaseOut[inputStringIndex] = 0;
    }

    OSPM_DBGEXIT(("EXIT: OSPPEnrollStringLowercase\n"));

    return retVal;
}

/*
 * This function will parse the parameters that are sent on the command line.
 *
 * Input:
 *  ospvArgc ( a count of all the parameters in ospvArgv )
 *  ospvArgv ( all of the input parameters to be parsed )
 *  enrollParams ( a structure with all of the parameters that could
 *    possibly be collected on the command line )
 *
 */
int OSPPEnrollParseParameters(int ospvArgc, char *ospvArgv[], OSPTENROLLPARAMS * enrollParams)
{
    /*
     * The return value of this function; it's initially set to true,
     * and gets set to the subsequent value of each function call.
     */
    int retVal = OSPC_ERR_NO_ERROR;

    /*
     * The next argument in the parameter list, converted to lowercase
     * ( for the sake of comparisons. )
     */
    char nextArg[10000];

    /* The index into the list of arguments: */
    int argvIndex = 0;

    int len;
    unsigned char buf[4096], buf2[4096], *temp;
    unsigned char Reqbuf[4096], Reqbuf2[4096];
    BIO *bioIn = OSPC_OSNULL;
    X509 *cert = OSPC_OSNULL;
    X509_REQ *certreq = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER: OSPPEnrollParseParameters\n"));

    if ((enrollParams == OSPC_OSNULL) || (ospvArgv == OSPC_OSNULL) || (ospvArgc <= 2)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The parameters passed in for parsing are invalid.\n");

        /*
         * Tell the user how to use the program, but only if we know what
         * command was entered on the command line to begin with:
         */
        if ((ospvArgv != OSPC_OSNULL) && (ospvArgv[0] != OSPC_OSNULL)) {
            OSPPEnrollUsage(ospvArgv[0], OSPC_ENROLL_HELP_MSG);
        }
        /*
         * Else
         *  o use the default command for enrollment, just for the sake
         *    of syntax:
         */
        else {
            OSPPEnrollUsage(OSPC_ENROLL_DEFAULT_CMD, OSPC_ENROLL_HELP_MSG);
        }
    }
    /*
     * For ( all of the arguments in the list, as long as there are no errors )
     *     o get the lowercase version of the next argument
     *     o if ( the arg could be converted to lowercase ) then
     *          - compare it to each possible argument and check its validity.
     *          - set the return value to an error code if anything goes wrong.
     */
    else {
        for (argvIndex = 1; (argvIndex < ospvArgc) && (retVal == OSPC_ERR_NO_ERROR); argvIndex++) {
            retVal = OSPPEnrollStringLowercase(ospvArgv[argvIndex], nextArg);

            if ((retVal == OSPC_ERR_NO_ERROR) && (nextArg != OSPC_OSNULL)) {
                if (argvIndex < ospvArgc - 1) {
                    if (OSPM_STRCMP(nextArg, OSPC_ENROLL_FUNCTION_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->Function), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->Function == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_FUNCTION;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the operation.\n");
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_USERNAME_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->Username), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->Username == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_USERNAME;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the username.\n");
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_PASSWORD_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->Password), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->Password == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_PASSWORD;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the password.\n");
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_DEVICEID_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->DeviceId), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->DeviceId == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_DEVICEID;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the device id.\n");
                        }
                    } else if (OSPM_STRCMP (nextArg, OSPC_ENROLL_CUSTOMERID_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->CustomerId), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->CustomerId == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_CUSTOMERID;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the customer id.\n");
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_CA_URL_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->CAUrl), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->CAUrl == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_CA_URL;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the CA's url.\n");
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_SSL_URL_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->SSLUrl), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->SSLUrl == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_SSL_URL;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the enrollment server's url.\n");
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_CACERT_PARAM) == 0) {
                        bioIn = BIO_new_file(ospvArgv[argvIndex + 1], "r");
                        if (bioIn == OSPC_OSNULL) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_CACERT;
                            OSPM_DBGERROR(("Error in Opening the File : %s. File does not exist\n", ospvArgv[argvIndex + 1]));
                        } else {
                            temp = buf;
                            cert = PEM_read_bio_X509(bioIn, NULL, NULL, NULL);
                            len = i2d_X509(cert, &temp);
                            EVP_EncodeBlock(buf2, buf, len);

                            if ((cert == NULL) || (len == 0)) {
                                retVal = OSPC_ERR_ENROLL_PARAMS_CACERT;
                                OSPM_DBGERROR(("Unable to Read the File : %s. File contains Garbage !\n", ospvArgv[argvIndex + 1]));
                            } else {
                                OSPPCopyString(&(enrollParams->CACertB64), (unsigned char *)buf2);
                            }
                            if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->CACertB64 == OSPC_OSNULL)) {
                                retVal = OSPC_ERR_ENROLL_PARAMS_CACERT;
                                OSPM_DBGERRORLOG(retVal, "Unable to copy the CA's certificate.\n");
                            } else {
                                enrollParams->CACertB64Len = OSPM_STRLEN((char *)buf2); X509_free(cert);
                                temp = NULL;
                                BIO_free(bioIn);
                                len = 0;
                            }
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_CERTREQ_PARAM) == 0) {
                        bioIn = BIO_new_file(ospvArgv[argvIndex + 1], "r");
                        if (bioIn == OSPC_OSNULL) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_CERTREQ;
                            OSPM_DBGERROR(("Error in Opening the File : %s. File does not exist\n", ospvArgv[argvIndex + 1]));
                        } else {
                            temp = Reqbuf;
                            certreq = PEM_read_bio_X509_REQ(bioIn, NULL, NULL, NULL);
                            len = i2d_X509_REQ(certreq, &temp);
                            EVP_EncodeBlock(Reqbuf2, Reqbuf, len);
                            if ((certreq == NULL) || (len == 0)) {
                                retVal = OSPC_ERR_ENROLL_PARAMS_CACERT;
                                OSPM_DBGERROR(("Unable to Read the File : %s. File contains Garbage !\n", ospvArgv[argvIndex + 1]));
                            } else {
                                OSPPCopyString(&(enrollParams->CertReq), (unsigned char *)Reqbuf2);
                            }
                            if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->CertReq == OSPC_OSNULL)) {
                                retVal = OSPC_ERR_ENROLL_PARAMS_CERTREQ;
                                OSPM_DBGERRORLOG(retVal, "Unable to copy the cert request.\n");
                            } else {
                                X509_REQ_free(certreq);
                                BIO_free(bioIn);
                                temp = NULL;
                                len = 0;
                            }
                        }
                    } else if (OSPM_STRCMP(nextArg, OSPC_ENROLL_CA_FPRINT_PARAM) == 0) {
                        retVal = OSPPCopyString(&(enrollParams->CAFprint), (unsigned char *)ospvArgv[argvIndex + 1]);

                        if ((retVal != OSPC_ERR_NO_ERROR) || (enrollParams->CAFprint == OSPC_OSNULL)) {
                            retVal = OSPC_ERR_ENROLL_PARAMS_CA_FPRINT;
                            OSPM_DBGERRORLOG(retVal, "Unable to copy the CA cert's fingerprint.\n");
                        }
                    }
                }

                /* End of test for what to do if the argument is binary */
                /*
                 * We can do all of the unary arguments separately from the
                 * binary arguments, since the binary arguments require some
                 * sort of parameter value. The unary arguments shouldn't be
                 * the same as the binary arguments ( otherwise there's a naming
                 * collision ), so this should be acceptable.
                 */
                if ((OSPM_STRCMP(nextArg, OSPC_ENROLL_VERBOSE_PARAM) == 0) ||
                    (OSPM_STRCMP(nextArg, OSPC_ENROLL_DEBUG_PARAM) == 0)) {
                    enrollParams->Verbose = OSPC_TRUE;
                }

                if (retVal != OSPC_ERR_NO_ERROR) {
                    OSPM_DBGERRORLOG(retVal, "An invalid enrollment parameter was parsed.\n");
                }

            } /* End of test for whether the next argument is non-null */
        } /* End of for loop for arguments */

        if (retVal != OSPC_ERR_NO_ERROR) {
            if (ospvArgv[0] != OSPC_OSNULL) {
                OSPPEnrollUsage(ospvArgv[0], OSPC_ENROLL_HELP_MSG);
            } else {
                OSPPEnrollUsage(OSPC_ENROLL_DEFAULT_CMD, OSPC_ENROLL_HELP_MSG);
            }
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPEnrollParseParameters\n"));

    return retVal;
}

/*
 * Given the binary of a BER-encoded certificate, print out its base64
 * encoding to STDOUT. We'll need the length of the certificate just as
 * a convenience for knowing where to stop printing.
 *
 * Input: the certificate and its length ( minus the terminating null. )
 */
int OSPPPrintCert(unsigned char *ospvCert, unsigned ospvCertLen)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /*
     * This is initially the length allocated for encoding the certificate
     * to a base64-encoded string, and after that it will be the actual
     * length of the base64 encoding of the same string.
     */
    unsigned certB64Len = 0;

    /* The base64-encoded certificate: */
    unsigned char *certB64 = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER: OSPPPrintCert\n"));

    /* If ( the certificate is null or it's empty ) then warn the user. */
    if ((ospvCert == OSPC_OSNULL) || (ospvCertLen <= 0)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGMISC(("Warning; trying to print a null or empty cert.\n"));
    }

    /*
     * Set the amount of memory for the base64-encoded cert and then
     * allocate it. If we have a null pointer, then set an error code
     * and print a warning.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        certB64Len = 2 * ospvCertLen;
        OSPM_MALLOC(certB64, unsigned char, certB64Len + 1);

        if (certB64 == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGMISC(("Warning; unable to allocate memory.\n"));
        }
    }

    /*
     * Now initialize the memory we just allocated and base64-encode the
     * certificate. If we have any problems encoding it, then print a
     * warning.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(certB64, 0, certB64Len + 1);

        retVal = OSPPBase64Encode(ospvCert, ospvCertLen, certB64, &certB64Len);
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGMISC(("Warning: unable to base64 encode cert.\n"));
        }
    }

    /* Now print out the base64-encoded text, but only if we didn't have
     * any problems. Complain if we have problems printing it.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPPrintB64Text(certB64, certB64Len);
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGMISC(("Warning: unable to print base64 encoded cert.\n"));
        }
    }

    /* Free up the base64-encoded certificate we created if necessary: */
    if (certB64 != OSPC_OSNULL) {
        OSPM_FREE(certB64);
    }

    OSPM_DBGEXIT(("EXIT: OSPPPrintCert\n"));

    return retVal;
}

/*
 * Print a block of base64-encoded text, given the text and the length
 * to print. We'll separate each line with a newline and a carriage return
 * so that this function will write truly base64-encoded text.
 *
 * Input: the base64-encoded block to be printed and the length of the block.
 */
int OSPPPrintB64Text(unsigned char *ospvTextBlock, unsigned ospvTextBlockLen)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /*
     * The number of columns to be used per line. This may be something
     * other than a constant ( or we may use termcap or some other
     * information ), so let's use an lvalue instead of a constant.
     */
    unsigned columnsPerLine = OSPC_ENROLL_B64_COLUMNS_PER_LINE;

    /* an offset into the baes64-encoded string that's being printed: */
    unsigned printIndex = 0;

    /*
     * This is the temporary variable used for printing out the
     * characters, 64 ( or OSPC_ENROLL_B64_COLUMNS_PER_LINE ) at a time.
     */
    unsigned outputText[OSPC_ENROLL_B64_COLUMNS_PER_LINE + 1];

    OSPM_DBGENTER(("ENTER: OSPPPrintB64Text\n"));

    /* Report an error if the input parameters can't be used: */
    if ((ospvTextBlock == OSPC_OSNULL) || (ospvTextBlockLen <= 0)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGMISC(("Warning: Bad parameters for printing base64 text\n"));
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(outputText, 0, OSPC_ENROLL_B64_COLUMNS_PER_LINE + 1);

        /*
         * For ( each possible line in the text ) do
         *  o copy the next line into another buffer
         *  o print that buffer in a format that is truly base64-formatted.
         *    That is, print 64 characters per line and print a CRLF at the
         *    end:
         */
        OSPM_PRINTF("-----BEGIN CERTIFICATE-----\r\n");
        for (printIndex = 0; printIndex < ospvTextBlockLen; printIndex += columnsPerLine) {
            OSPM_STRNCPY(outputText, ospvTextBlock + printIndex, columnsPerLine);
            OSPM_PRINTF("%s\r\n", (char *)outputText);
        }
        OSPM_PRINTF("-----END CERTIFICATE-----");
    }

    OSPM_DBGEXIT(("EXIT: OSPPPrintB64Text\n"));

    return retVal;
}

/*
 * Given a certificate ( which may be null ), its length ( which may be
 * less than or equal to zero ), and the status of an enrollment request,
 * report the enrollment request's status and certificate ( if available )
 * to the user.
 *
 * Input: the certificate ( in binary ), the cert's length, and the
 *        status of the request ( as returned from the enrollment server. )
 */
void OSPPPrintCertAndStatus(unsigned char *ospvCert, unsigned ospvCertLen, unsigned ospvEnrollStatus)
{
    OSPM_DBGENTER(("ENTER: OSPPPrintCertAndStatus\n"));

    /*
     * Now print out the status of the certificate.
     *  o if ( the certificate isn't null and it isn't empty ) then
     *      - print the cert.
     *  o else
     *      - print out an error depending on whether the certificate
     *        was null or empty.
     */
    if (ospvEnrollStatus == OSPC_ENROLL_STATUS_OK) {
        OSPM_PRINTTOERR((stderr, "The certificate request was successful.\n"));

        if ((ospvCert != OSPC_OSNULL) && (ospvCertLen > 0)) {
            OSPPPrintCert(ospvCert, ospvCertLen);
        } else {
            if (ospvCert == OSPC_OSNULL) {
                OSPM_PRINTTOERR((stderr, "Error: The certificate returned was null.\n"));
            } else if (ospvCertLen == 0) {
                OSPM_PRINTTOERR((stderr, "Error: An empty certificate was retrieved.\n"));
            }
        }
    }
    /*
     * Else
     *  o Report that the certificate request is still pending approval:
     */
    else if (ospvEnrollStatus == OSPC_ENROLL_STATUS_PENDING) {
        OSPM_PRINTF("The certificate request is pending approval.\n");
    }
    /*
     * Else
     *  o report that the certificate request failed and give the reason
     *    ( returned from the enrollment server ) why.
     */
    else {
        OSPM_PRINTTOERR((stderr, "The certificate request failed with the following code: %d\n", ospvEnrollStatus));
        if (ospvEnrollStatus == 2) {
            OSPM_PRINTTOERR((stderr, "Device not defined on the Server. Define device and try again\n"));
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPPrintCertAndStatus\n"));
}

/*
 * Print the usage for the enrollment command. The first string in
 * is what the user typed in for the command; it may not necessarily
 * be "enroll". The second string is the help message that the user
 * receives. The message should explain what the user should enter
 * in order for the enrollment process to be successful.
 *
 * Input: string entered for executing this program.
 */
void OSPPEnrollUsage(char *ospvCmd, char *ospvHelpMsg)
{
    /*
     * Print both of these strings out, but only if they're valid.
     * Other logic can be used for how to handle these cases,
     * if necessary.
     */
    if ((ospvCmd != OSPC_OSNULL) && (ospvHelpMsg != OSPC_OSNULL)) {
        OSPM_PRINTF("Usage: %s%s", ospvCmd, ospvHelpMsg);
    }
}
