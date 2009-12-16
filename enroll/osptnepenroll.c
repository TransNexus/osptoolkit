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

#include "osp/ospmsginfo.h"
#include "osp/osp.h"
#include "osp/ospssl.h"
#include "osp/osputils.h"
#include "osp/ospcomm.h"
#include "osp/ospsecurity.h"
#include "osp/ospb64.h"

#include "osptnep.h"
#include "osptnepenroll.h"

/*
 * Perform all of the operations for starting up a communications manager,
 * retrieving a CA certificate, formulating requests, and sending requests.
 *
 * Input: a pointer to the enrollment and communications parameters, as
 *        well as a pointer to the certificate that is being requested and
 *        the status of the request.
 *
 * Output: The success for creating an enrollment request, starting up the
 *         communications manager, and parsing the response. Note that an
 *         error code will not be generated if the response fails; an error
 *         could be due to the enrollment server, so any failed requests
 *         will be recorded in the ospvEnrollStatusOut parameter.
 */
int OSPPEnroll(OSPTENROLLPARAMS *ospvEnrollParamsIn,
    OSPTCOMMPARAMS *ospvCommParamsIn,
    unsigned char **ospvLocalCertOut,
    unsigned *ospvLocalCertLenOut,
    unsigned *ospvEnrollStatusOut)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /*
     * This is for any warnings that may pop up; it should only be
     * used for errors that occur while cleaning up or other non-fatal errors.
     */
    int warnVal = OSPC_ERR_NO_ERROR;

    /*
     * This is the structure whose request is transmitted to the enrollment
     * server and whose Response field is scanned for the response.
     */
    OSPT_MSG_INFO *enrollMsg = OSPC_OSNULL;

    /* This is the communications manager that will handle all of the  requests to the enrollment server: */
    OSPTCOMM *commMgr = OSPC_OSNULL;

    /* This is used for comparison against the public key that is returned. */
    OSPTASN1OBJECT requestPublicKey;

    OSPM_DBGENTER(("ENTER: OSPPEnroll\n"));

    /*
     * IF ( the parameters passed in are ok ) then
     *  o proceed with initialization and enrollment...
     */
    if ((ospvEnrollParamsIn == OSPC_OSNULL) || (ospvLocalCertOut == OSPC_OSNULL) || (ospvLocalCertLenOut == OSPC_OSNULL) ||
        (ospvEnrollStatusOut == OSPC_OSNULL))
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The parameters passed in for enrollment were invalid. )\n");
    }
    /*
     * If ( the input parameters were ok ) then
     *  o decode the base64-encoded certificate in the enrollment parameters
     *  o complain if we can't do it
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPBase64DecodeCACert(ospvEnrollParamsIn);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to base64-decode the CA certificate passed in.\n");
        }
    }

    /*
     * Create the structure that will be used for transmitting the
     * enrollment request:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPMsgInfoNew(&enrollMsg);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to initialize MessageInfo for enrollment request.\n");
        }
    }

    /*
     * If ( the initialization was successful ) then:
     *  - initialize the response message to be empty
     *  - retrieve the subject public key info from the certificate
     *    request ( so that we can compare it to what we receive. )
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        enrollMsg->ResponseMsg = OSPC_OSNULL;

        retVal = OSPPGetPublicKeyInfoFromCertReq(ospvEnrollParamsIn->CertReq, &requestPublicKey);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Cannot find public key info in PKCS#10 cert request.\n");
        }
    }

    /* Initialize the request for the enrollMsg using the input parameters: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPConstructEnrollmentRequest(ospvEnrollParamsIn, enrollMsg);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to construct enrollment request for transmission.\n");
        }
    }

    /* If ( we could initialize the MsgInfo's Request ) then Initialize the comm mgr. */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPInitSecureCommMgr(ospvEnrollParamsIn, ospvCommParamsIn, &commMgr);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to initialize secure enrollment communications.\n");
        }
    }

    /* If ( there's a comm mgr that works ) then try enrolling the device. */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPEnrollDevice(commMgr, enrollMsg, &requestPublicKey, ospvEnrollStatusOut, ospvLocalCertOut, ospvLocalCertLenOut);

        if (retVal == OSPC_ERR_NO_ERROR) {
            OSPM_DBGMISC(("Message successfully received from enrollment server.\n"));

            if (*ospvEnrollStatusOut == OSPC_ENROLL_STATUS_OK) {
                OSPM_DBGMISC(("Certificate request successful.\n"));
            } else if (*ospvEnrollStatusOut == OSPC_ENROLL_STATUS_PENDING) {
                OSPM_DBGMISC(("Certificate requeest pending approval.\n"));
            } else {
                OSPM_DBGMISC(("Request rejected: <%d>\n", *ospvEnrollStatusOut));
                retVal = *ospvEnrollStatusOut;
            }
        } else {
            OSPM_DBGERRORLOG(retVal, "Unable to retrieve certificate from enrollment server.\n");
        }
    }

    /* Cleanup the comm mgr: */
    if (commMgr != OSPC_OSNULL) {
        warnVal = OSPPEnrollCleanupCommMgr(&commMgr);
        if (warnVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGMISC(("Warning: error while cleaning up comm manager: <%d>\n", warnVal));
        }
    }

    /* Cleanup the message info: */
    if (enrollMsg != OSPC_OSNULL) {
        OSPPMsgInfoDelete(&enrollMsg);
    }

    /* Cleanup the public key info's element info: */
    if (requestPublicKey.ElementInfo != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&(requestPublicKey.ElementInfo), 0);
    }

    /*
     * Now cleanup the public key info's parse results ( which links the
     * whole ASN1 structure together ):
     */
    if (requestPublicKey.ParseResults != OSPC_OSNULL) {
        PTPResultsDelete(&(requestPublicKey.ParseResults));
    }

    OSPM_DBGEXIT(("EXIT: OSPPEnroll\n"));

    return retVal;
}

/*
 * Given the enrollment parameters, base64-decode the CACertB64 parameter
 * of the enrollment parmaeters and place the binary CA certificate in the
 * CACert field of the enrollment parameters instead.
 *
 * Input: pointer to the enrollment parameters, which should contain a pointer
 *        the base64-encoded CA certificate and its length, as well as pointers
 *        to the binary CA certificate and its length ( which will initially
 *        be NULL and 0. )
 *
 * Output: OSPC_ERR_NO_ERROR is returned if there are no problems,  or
 *         a non-OSPC_ERR_NO_ERROR value is returned. If everything is
 *         successful, then the CACertB64 field should be populated as well.
 */
int OSPPBase64DecodeCACert(OSPTENROLLPARAMS *ospvEnrollParamsIn)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* The base64-decoded certificate, if it can be determined: */
    unsigned char *caCert = OSPC_OSNULL;

    /* The length of the binary ( base64-decoded ) certificate: */
    unsigned caCertLen = 0;

    /*
     * Check the pointer to the enrollment parameters and the base64-encoded
     * certificate. If they're not quite right, then set an error code and
     * complain.
     */
    if ((ospvEnrollParamsIn == OSPC_OSNULL) || (ospvEnrollParamsIn->CACertB64 == OSPC_OSNULL) || (ospvEnrollParamsIn->CACertB64Len <= 0)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "Invalid arguments passed in for decoding the CA certificate.\n");
    }

    /*
     * If ( the input parameters look ok ) then
     *  o set the maximum length of the output CA certificate to be twice
     *    that of the base64-encoded cert.
     *  o allocate the memory for the binary cert.
     *  o set an error code if  we couldn't allocate it.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        caCertLen = 2 * ospvEnrollParamsIn->CACertB64Len;
        OSPM_MALLOC(caCert, unsigned char, caCertLen + 1);

        if (caCert == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to allocate memory for the CA cert.\n");
        }
    }

    /*
     * If ( we could allocate the memory ) then
     *  o initialize its contents
     *  o try base64 decoding the certificate into the newly-allocated memory
     *  o complain if the decoding didn't work.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(caCert, 0, caCertLen + 1);
        retVal = OSPPBase64Decode((const char *)ospvEnrollParamsIn->CACertB64, ospvEnrollParamsIn->CACertB64Len, caCert, &caCertLen);
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to base64-decode CA cert.\n");
        }
    }

    /*
     * Assign the certificates to the outgoing enrollment parameters if
     * the decoding worked.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        ospvEnrollParamsIn->CACert = caCert;
        ospvEnrollParamsIn->CACertLen = caCertLen;
    }

    /*
     * Free up the memory allocated ( if it was allocated ) if anything
     * went wrong. A better approach might be to allocate fresh memory for
     * the outgoing CACert and then deallocate the memory for the caCert
     * regardless of the return value.
     */
    if (retVal != OSPC_ERR_NO_ERROR) {
        if (caCert != OSPC_OSNULL) {
            OSPM_FREE(caCert);
        }
    }

    return retVal;
}

/*
 * Create an enrollment request for requesting or retrieving a certificate,
 * send the request to the enrollment server ( using the communications
 * manager passed in ); extract the status and certificate from the
 * response, validate the certificate, and return the certificate.
 *
 * Input: pointer to a communications manager for transmitting the request;
 *        a pointer to a MessageInfo ( OSPT_MSG_INFO* ) that should contain
 *        the request; a pointer to the subjectPublicKeyInfo of the
 *        request ( for validation ); a pointer to an integer for returning
 *        the status of the request; and a pointer to a (null) string for
 *        returning the certificate.
 *
 * Output: *ospvEnrollStatusOut and *ospvCertOut may contain the status
 *         and certificate for the device if everything went ok. That is,
 *         if the enrollment server could be contacted and it serviced the
 *         request, the the status and possibly the certificate should
 *         be returned. In this case, OSPC_ERR_NO_ERROR should be returned
 *         as well. Otherwise, an error code other than OSPC_ERR_NO_ERROR
 *         will be returned and the status and certificate are
 *         potentially useless.
 */
int OSPPEnrollDevice(OSPTCOMM *ospvCommMgrIn,
    OSPT_MSG_INFO *ospvEnrollMsgIn,
    OSPTASN1OBJECT *ospvRequestPublicKeyIn,
    unsigned *ospvEnrollStatusOut,
    unsigned char **ospvCertOut,
    unsigned *ospvCertLenOut)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /*
     * The security manager for the communication manager; it keeps track
     * of certificates used and so on:
     */
    OSPTSEC *commMgrSec = OSPC_OSNULL;

    /*
     * The status of the certificate request that we retrieve from teh
     * enrollment server's response:
     */
    unsigned char *statusStr = OSPC_OSNULL;

    /*
     * The base64-encoded cert that we retrieve from the enrollment
     * server's response:
     */
    unsigned char *certb64Buf = OSPC_OSNULL;

    /* The status of the request: */
    unsigned status = OSPC_ENROLL_STATUS_FAILURE_DEFAULT;

    /*
     * This will represent the null-terminated response received from the
     * enrollment server; the response that we get from the communications
     * manager isn't null-terminated, which may otherwise introduce problems
     * with reading non-heap memory with common functions:
     */
    unsigned char *enrollResponse = OSPC_OSNULL;

    /*
     * Check the parameters; if any of them are null, then set an
     * error code and complain.
     */
    if ((ospvCommMgrIn == OSPC_OSNULL) || (ospvEnrollMsgIn == OSPC_OSNULL) || (ospvEnrollStatusOut == OSPC_OSNULL) ||
        (ospvCertOut == OSPC_OSNULL) || (ospvCertLenOut == OSPC_OSNULL))
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The parameters for enrolling a device are invalid.\n");
    }

    /*
     * Now add the enrollment transaction to the communication manager's
     * message queue and complain if we have any problems doing it:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("Enrollment request: <%d> bytes: <%s>\n", ospvEnrollMsgIn->RequestSz, ospvEnrollMsgIn->RequestMsg));
        OSPM_DBGMISC(("Adding enrollment message to transaction queue.\n"));
        retVal = OSPPCommAddTransaction(ospvCommMgrIn, ospvEnrollMsgIn);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to add the enrollment request to the communications manager.\n");
        }
    }

    /*
     * We may have gotten a valid return value for adding a request to the
     * communication manager's message queue even if the server rejected it.
     * So, we'll check the error code of the message here to make sure that
     * we could in fact send it:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = ospvEnrollMsgIn->ErrorCode;
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to transmit request to the enrollment server.\n");
        }
    }

    /*
     * We'll need a copy of the response message that's null-terminated in
     * order to use the parsing mechanism. Another solution would be to
     * rewrite the string parser and every other piece of code that uses
     * the response to avoid using strlen, strcpy, and any other function
     * that may potentially access characters outside the bounds of the
     * non-null-terminated response, but that isn't done here.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(enrollResponse, unsigned char, ospvEnrollMsgIn->ResponseSz + 1);

        /* Complain if the memory isn't available: */
        if (enrollResponse == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to allocate memory for null-terminated response.\n");
        }
    }

    /*
     * Now we can create a null-terminated version of the response.
     * Then look for the status and the certificate that should be returned
     * from the enrollment server within the null-terminated response.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        /*
         * Create the null-terminated response by zeroing out everything
         * and then copying in the string that fits it:
         */
        OSPM_MEMSET(enrollResponse, 0, ospvEnrollMsgIn->ResponseSz + 1);
        OSPM_STRNCPY(enrollResponse, ospvEnrollMsgIn->ResponseMsg, ospvEnrollMsgIn->ResponseSz);

        /* Get the status code from the response: */
        retVal = OSPPExtractFieldFromResponse(enrollResponse, ospvEnrollMsgIn->ResponseSz, (unsigned char *)OSPC_ENROLL_STATUS_RSP_PARAM, &statusStr);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to find the cert request status in the response.\n");
        }
    }

    /*
     * Check to make sure that the status is a string of digits. Since atoi
     * returns a 0 for strings that are invalid, we will interpret a garbled
     * message the same way that we'll interpret a successful enrollment if
     * we just use atoi first.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("status: <%s>\n", statusStr));
        retVal = OSPPValidateDigitString(statusStr);

        if (retVal == OSPC_ERR_NO_ERROR) {
            status = OSPM_ATOI((const char *)statusStr);
        } else {
            OSPM_DBGERRORLOG(retVal, "Invalid status returned - assuming cert request failed.\n");
        }
    }

    /*
     * Now check the status code that was returned. If the status is
     * equal to a code that says that the request was successful, then
     * look for the certificate in the response. If the status is equal to
     * a code that says that the request is pending approval, then do
     * nothing. If the status is equal to anything else, then print out
     * an error.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("status returned: <%d>\n", status));
        *ospvEnrollStatusOut = status;

        if (status == OSPC_ENROLL_STATUS_OK) {
            retVal = OSPPExtractFieldFromResponse(enrollResponse, ospvEnrollMsgIn->ResponseSz, (unsigned char *)OSPC_ENROLL_CERT_RSP_PARAM, &certb64Buf);

            if (retVal == OSPC_ERR_NO_ERROR) {
                commMgrSec = OSPPCommGetSecurity(ospvCommMgrIn);

                if (commMgrSec == OSPC_OSNULL) {
                    OSPM_DBGMISC(("Warning: Unable to validate the certificate returned.\n"));
                }
            }

            if (retVal == OSPC_ERR_NO_ERROR) {
                retVal = OSPPValidateDeviceCert(commMgrSec, ospvRequestPublicKeyIn, certb64Buf, ospvCertOut, ospvCertLenOut);

                if (retVal != OSPC_ERR_NO_ERROR) {
                    retVal = OSPC_ERR_ENROLL_INVALID_RESPONSE;
                    OSPM_DBGERRORLOG(retVal, "Unable to extract certificate from response indicating success.\n");
                }
            }
        }

        /* Nothing to do in this case. */
        else if (status == OSPC_ENROLL_STATUS_PENDING) {
            OSPM_DBGMISC(("Enrollment status pending\n"));
        }

        /*
         * It looks like the request failed for some reason. We can try to
         * decode it, but we definitely need to tell the user what the error
         * code is. This code should be propagated to a customer service rep
         * or someone who can tell the user what this error indicates, so
         * more diagnostics output would be nice.
         */
        else {
            OSPM_DBGMISC(("Warning: Failure code received from server: %d\n", *ospvEnrollStatusOut));
        }
    }

    /* Now free everything up: */
    if (statusStr != OSPC_OSNULL) {
        OSPM_FREE(statusStr);
    }

    /*
     * The certificate should be contained in the ospvCertOut field
     * if it was successfully obtained, so this is no longer useful:
     */
    if (certb64Buf != OSPC_OSNULL) {
        OSPM_FREE(certb64Buf);
    }

    /* The null-terminated response is no longer necessary: */
    if (enrollResponse != OSPC_OSNULL) {
        OSPM_FREE(enrollResponse);
    }

    return retVal;
}

/*
 * Given a certificate for a device that has been received, validate it
 * against the public key used ( to be added ), the CA certificate that
 * we expected to sign it, and its general construction. The certificate
 * is placed in *ospvCertOut.
 *
 * Input:
 *   o ospvSecIn: contains the authority certificate, and will be used for
 *                validating the CA certificate used for signing the request.
 *   o ospvRequestPublicKeyIn: contains the public key that was contained in
 *                the certificate request.
 *   o ospvCertB64In: the certificate to be taken apart and validated.
 *
 * Output:
 *   o ospvCertOut: a pointer to the unsigned char* that will ultimately
 *                  contain the certificate that was transmitted.
 *
 *   - the return value is OSPC_ERR_NO_ERROR( 0 ) if the certificate is ok,
 *     or non-zero if there's an error.
 *
 * Errors: Errors are generated when:
 *   o any of the parameters are null;
 *   o the CA certificate(s) in the OSPTSEC object are not the signers of
 *     the received certificate;
 *   o the public key of the received certificate does not match the one
 *     that was requested;
 *   o the certificate received was improperly base64-encoded.
 */
int OSPPValidateDeviceCert(OSPTSEC *ospvSecIn,
    OSPTASN1OBJECT *ospvRequestPublicKeyIn,
    unsigned char *ospvCertB64In,
    unsigned char **ospvCertOut,
    unsigned *ospvCertLenOut)
{
    int retVal = OSPC_ERR_NO_ERROR;

    int certb64BufLen = -1;

    /*
     * This object will be used for validating that the certificate
     * passed in is in fact an X.509 certificate:
     */
    OSPTASN1OBJECT *x509CertOut = OSPC_OSNULL;

    /* This is the temporary variable for holding the  outgoing subjectPublicKeyInfo: */
    OSPTASN1OBJECT certOutPublicKey;

    /* This is the index of the root CA certificate within the CA cert chain: */
    int caCertIndex = -1;

    OSPM_DBGENTER(("ENTER: OSPPValidateDeviceCert\n"));

    /*
     * If ( the parameters are ok ) then
     *  o get the size of the base64-encoded cert and set the size of
     *    the binary cert to be twice that ( this is only needed for the
     *    base64 decoding; in reality the base64-decoded message should be
     *    no more than 4/3 the size of the base64-encoded message. )
     *  o Allocate the memory for the output certificate
     */
    if ((ospvCertB64In == OSPC_OSNULL) || (ospvCertOut == OSPC_OSNULL) || (ospvCertLenOut == OSPC_OSNULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "Invalid parameters for validating a device cert.\n");
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        certb64BufLen = OSPM_STRLEN((const char *)ospvCertB64In);
        *ospvCertLenOut = 2 * certb64BufLen;
        OSPM_MALLOC(*ospvCertOut, unsigned char, *ospvCertLenOut + 1);

        /*
         * If ( the memory couldn't be allocated ) then
         *  o we're in trouble; set the error code to indicate that no
         *    memory could be allocated.
         */
        if (*ospvCertOut == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to allocate memory");
        }
    }

    /*
     * Else ( everything's ok ) so
     *  o decode the base64 encoding of the certificate and put it into
     *    ospvCertOut and its length in the certificate's output length.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(*ospvCertOut, 0, *ospvCertLenOut + 1);
        retVal = OSPPBase64Decode((const char *)ospvCertB64In, certb64BufLen, *ospvCertOut, ospvCertLenOut);

        /* If ( there was a problem ) then set the error code to indicate the certificate received was bogus. */
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to base64-decode certificate\n");
        }
    }

    /* If ( everything's ok ) then  Check the certificate's public key and CA certificate. */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("creating x509 cert..\n"));
        retVal = OSPPX509CertCreate(*ospvCertOut, &x509CertOut);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Invalid X.509 certificate returned.\n");
        }
    }

    /*
     * Now get the public key out of the certificate that was
     * retrieved so that we can compare it to the public key that
     * was used in the certificate request.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("getting public key info..\n"));
        retVal = OSPPGetPublicKeyInfoFromCert(x509CertOut, &certOutPublicKey);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to retrieve public key from certificate retrieved.\n");
        }
    }

    /* Now compare the two public keys: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("comparing asn1..\n"));
        retVal = OSPPASN1Compare(&certOutPublicKey, ospvRequestPublicKeyIn);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Public Key in certificate does not match public key in request.\n");
        }
    }

    /*
     * The public key is ok, so now make sure that the authority
     * certificate ( or CA certificate ) is in fact the certificate
     * used for signing the certificate we received. If not, then
     * we'll fail the enrollment.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("validating cert..\n"));
        retVal = OSPPX509CertValidateCertificate(x509CertOut, ospvSecIn->AuthorityCertInfo, ospvSecIn-> NumberOfAuthorityCertificates, &caCertIndex);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "The certificate returned was not signed by the given CA certificate.\n");
        }
    }

    /*
     * Now let's make sure that the contents are valid. Let's decode it and make
     * sure that the certificate is signed by the CA certificate. If it's not,
     * then we'll set an error code and return. If it is, then we'll need to
     * check the public key ( although that's not done right now, but it should be. )
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("certificate returned:\n"));
        OSPPDumpHex(x509CertOut->ElementInfo->Element,
                    x509CertOut->ElementInfo->ElementLength);
    }

    /* Delete the X.509 cert object; we only needed it for validation: */
    if (x509CertOut != OSPC_ERR_NO_ERROR) {
        OSPPASN1ObjectDelete(&x509CertOut);
    }

    /*
     * Delete the element info and parse results of the certOutPublicKey,
     * which was only needed as a comparison tool for the subjectPublicKeyInfo
     * in the certificate request.
     */
    if (certOutPublicKey.ElementInfo != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&(certOutPublicKey.ElementInfo), 0);
    }

    if (certOutPublicKey.ParseResults != OSPC_OSNULL) {
        PTPResultsDelete(&(certOutPublicKey.ParseResults));
    }

    OSPM_DBGMISC(("done!\n"));

    OSPM_DBGEXIT(("EXIT: OSPPValidateDeviceCert\n"));

    return retVal;
}

/*
 * Compare the two ASN1 objects. Returns OSPC_ERR_NO_ERROR if they're the
 * same, and a non-zero value otherwise.
 *
 * Input:
 *   ospvLHSObject: a pointer to an ASN1 Object ( the LeftHand Side )
 *   ospvRHSObject: a pointer to an ASN1 Object ( the RightHand Side )
 *
 * Output:
 *   OSPC_ERR_NO_ERROR if the two are the same, or some other value otherwise.
 *
 * Errors: Errors will be returned when
 *   o at least one of the parameters is null ( OSPC_ERR_ENROLL_INVALID_PARAMS )
 *   o the two have different lengths ( OSPC_ERR_ENROLL_LENGTH_MISMATCH );
 *   o the two have different contents ( OSPC_ERR_ENROLL_CONTENT_MISMATCH );
 */
int OSPPASN1Compare(OSPTASN1OBJECT *ospvLHSObject,
    OSPTASN1OBJECT *ospvRHSObject)
{
    int retVal = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPASN1Compare\n"));

    /* If ( either of the parameters are null ) then set an error code and complain. */
    if ((ospvLHSObject == OSPC_OSNULL) || (ospvRHSObject == OSPC_OSNULL) || (ospvLHSObject->ElementInfo == OSPC_OSNULL) ||
        (ospvRHSObject->ElementInfo == OSPC_OSNULL) || (ospvLHSObject->ElementInfo->Element == OSPC_OSNULL) ||
        (ospvRHSObject->ElementInfo->Element == OSPC_OSNULL))
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "At least one of the ASN1 objects being compared is invalid.\n");

        if (ospvLHSObject == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The first parameter to OSPPASN1Compare is null.\n");
        }

        if (ospvRHSObject == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The second parameter to OSPPASN1Compare is null.\n");
        }

        if (ospvLHSObject->ElementInfo == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The first parameter's element info  is null.\n");
        }

        if (ospvRHSObject->ElementInfo == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The second parameter's element info  is null.\n");
        }

        if (ospvLHSObject->ElementInfo->Element == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The first parameter's element info content  is null.\n");
        }

        if (ospvRHSObject->ElementInfo->Element == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The second parameter's element info content  is null.\n");
        }
    }

    /*
     * If ( there haven't been any problems ) then
     *  o if ( the two objects have different lengths ) then
     *      o set the error code and complain
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        if ((ospvLHSObject->ElementInfo)->ElementLength != (ospvRHSObject->ElementInfo)->ElementLength) {
            retVal = OSPC_ERR_ENROLL_ASN1_CMP_LENGTH_MISMATCH;
            OSPM_DBGERRORLOG(retVal, "The lengths of the two objects compared are different\n");
            OSPM_DBGMISC(("Length of content of LHS: %d\n", (ospvLHSObject->ElementInfo)->ElementLength));
            OSPM_DBGMISC(("Length of content of RHS: %d\n", (ospvRHSObject->ElementInfo)->ElementLength));
            OSPM_DBGMISC(("LHS: \n"));
            OSPPDumpHex((ospvLHSObject->ElementInfo)->Element, (ospvLHSObject->ElementInfo)->ElementLength);
            OSPPDumpHex((ospvRHSObject->ElementInfo)->Element, (ospvRHSObject->ElementInfo)->ElementLength);
        }
    }

    /*
     * If ( there still aren't any errors ) then
     *  o if ( the two objects have different contents ) then
     *      o set the error code and complain
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPM_STRNCMP((const char *)((ospvLHSObject->ElementInfo)-> Element), (const char *)((ospvRHSObject->ElementInfo)-> Element),
                     (ospvLHSObject->ElementInfo)->ElementLength);
        if (retVal != 0) {
            retVal = OSPC_ERR_ENROLL_ASN1_CMP_MISMATCH;
            OSPM_DBGERRORLOG(retVal, "The two ASN1 objects are of unequal value.\n");
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPASN1Compare\n"));

    return retVal;
}

/*
 * Given an ASN1 object that represents an X.509 certificate, store its
 * subjectPublicKeyInfo in the outbound ospvPublicKeyOut structure.
 * This subjectPublicKeyInfo will be compared against what we get from
 * the server in the form of a certificate; if they match, then the
 * certificate may be ok - otherwise, the certificate is bogus.
 *
 * Input: references to the input certificate and the outgoing
 *        subjectPublicKeyInfo
 *
 * Output: If the subjectPublicKeyInfo can be found, then it should be
 *         stored in *ospvPublicKeyOut and the return value will be
 *         OSPC_ERR_NO_ERROR. Otherwise, the return value will be
 *         something other than OSPC_ERR_NO_ERROR.
 */
int OSPPGetPublicKeyInfoFromCert(OSPTASN1OBJECT *ospvCertIn,
    OSPTASN1OBJECT *ospvPublicKeyOut)
{
    int retVal = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *publicKeyInfoElementInfo = OSPC_OSNULL;
    OSPTASN1PARSERESULT *publicKeyInfoParseResults = OSPC_OSNULL;

    if ((ospvCertIn == OSPC_OSNULL) || (ospvPublicKeyOut == OSPC_OSNULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        if (ospvCertIn == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The certificate passed in is null.\n");
        }

        if (ospvPublicKeyOut == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The public key reference passed in is null.\n");
        }
    }

    /*
     * Now parse the certificate's contents for the subjectPublicKeyInfo,
     * using the formulation for an X.509 subjectPublicKeyInfo that's
     * readily available in the OSP:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPASN1ObjectGetElementByDataRef(ospvCertIn, &publicKeyInfoElementInfo, OSPEDRID_CERT_SUBJPUBKEYINFO);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to find subjectPublicKeyInfo in certificate\n");
            retVal = OSPC_ERR_ENROLL_BAD_CERT;
        }
    }

    /* Now generate some parse results so that the element information can be turned into a "complete" OSPTASN1OBJECT: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = PTPResultsCreate(&publicKeyInfoParseResults, publicKeyInfoElementInfo, OSPEDRID_CERT_SUBJPUBKEYINFO);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to reverse-engineer parse results from the given subjectPublicKeyInfo\n");
            retVal = OSPC_ERR_ENROLL_ASN1_PARSE;
        }
    }

    /*
     * If ( we could finish generating the ASN1 object for the
     *      subjectPublicKeyInfo ) then
     *   o finish the generation of the public key by assigning the
     *     contents we created to the outgoing public key info:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        ospvPublicKeyOut->ElementInfo = publicKeyInfoElementInfo;
        ospvPublicKeyOut->ParseResults = publicKeyInfoParseResults;
    }
    /*
     * Else ( we ran into  some problems ) so
     *  o delete the element info and parse results we created:
     */
    else {
        if (publicKeyInfoElementInfo != OSPC_OSNULL) {
            OSPPASN1ElementDelete(&publicKeyInfoElementInfo, 0);
        }

        if (publicKeyInfoParseResults != OSPC_OSNULL) {
            PTPResultsDelete(&publicKeyInfoParseResults);
        }
    }

    return retVal;
}

/*
 * Given a binary string that represents a PKCS#10 request, create an
 * ASN1 object that contains the subjectPublicKeyInfo of the certificate
 * request. The subjectPublicKeyInfo is found as follows:
 *
 * SEQUENCE          -- Certificate request
 *     SEQUENCE      -- Certificate request info
 *         INTEGER   -- version
 *         SEQUENCE  -- subject name
 *         SEQUENCE  -- subject public key info
 *         SEQUENCE  -- optional attributes
 *     OID           -- signature algorithm
 *     BIT STRING    -- signature
 *
 * We'll use the ASN1 module from the OSP to decode the binary string
 * and extract the public key from the certificate request.
 *
 * Input: string representing a PKCS#10 certificate request, and a pointer
 *        to an ASN1 object for storing its subjectPublicKeyInfo.
 *
 * Output: the subjectPublicKeyInfo should be found, in which case we'll
 *         return OSPC_ERR_NO_ERROR. Otherwise, a different error code will
 *         be returned.
 */
int OSPPGetPublicKeyInfoFromCertReq(unsigned char *ospvCertReqB64In,
    OSPTASN1OBJECT *ospvPublicKeyOut)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* This represents the length of the base64-encoded request passed in: */
    unsigned certReqB64LenIn = 0;

    /*
     * This will be the binary for storing the base64-decoded certificate
     * request; it should be deleted at the end of this function:
     */
    unsigned char *certReqBinary = OSPC_OSNULL;

    /* This will store the length of that binary string: */
    unsigned certReqBinaryLen = 0;

    /* This points to the description of the certificate's structure: */
    OSPTASN1ELEMENTINFO *certReqElementInfo = OSPC_OSNULL;

    /*
     * This is used for linking together all of the ElementInfos into
     * a tree-like structure and for validating the certificate request:
     */
    OSPTASN1PARSERESULT *certReqParseResults = OSPC_OSNULL;

    /*
     * This will be the union of the certReqElementInfo and certReqParseResults;
     * we'll need it for extracting the subjectPublicKeyInfo.
     */
    OSPTASN1OBJECT certReqASN1;

    /* This points to the description of the public key's structure: */
    OSPTASN1ELEMENTINFO *publicKeyInfoElementInfo = OSPC_OSNULL;

    /*
     * These will be used for validating the structure of the
     * public key as well:
     */
    OSPTASN1PARSERESULT *publicKeyInfoParseResults = OSPC_OSNULL;


    OSPM_DBGENTER(("ENTER: OSPPGetPublicKeyInfoFromCertReq\n"));

    /* See if any of the parameters are null: */
    if ((ospvCertReqB64In == OSPC_OSNULL) || (OSPM_STRLEN((const char *)ospvCertReqB64In) <= 0) || (ospvPublicKeyOut == OSPC_OSNULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        if (ospvCertReqB64In == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The cert request is null.\n");
        } else if (OSPM_STRLEN((const char *)ospvCertReqB64In) <= 0) {
            OSPM_DBGERRORLOG(retVal, "The cert request is empty.\n");
        }

        if (ospvPublicKeyOut == OSPC_OSNULL) {
            OSPM_DBGERRORLOG(retVal, "The public key reference passed in is null.\n");
        }
    }

    /*
     * The certificate request should be base64-encoded, so we'll need
     * to decode it. First, though, create the memory to store it:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        certReqB64LenIn = OSPM_STRLEN((const char *)ospvCertReqB64In);
        certReqBinaryLen = 2 * certReqB64LenIn;
        OSPM_MALLOC(certReqBinary, unsigned char, certReqBinaryLen + 1);

        if (certReqBinary == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "No memory available for the binary certificate request.\n");
        }
    }

    /* Now perform the base64 decoding: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(certReqBinary, 0, certReqBinaryLen + 1);

        retVal = OSPPBase64Decode((const char *)ospvCertReqB64In, certReqB64LenIn, certReqBinary, &certReqBinaryLen);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to base64-decode the certificate request.\n");
        } else if ((OSPM_STRLEN((const char *)certReqBinary) <= 0) || (certReqBinaryLen <= 0)) {
            retVal = OSPC_ERR_B64_DECODE_FAILED;
            OSPM_DBGERRORLOG(retVal, "Unable to base64-decode the certificate request.\n");
        }
    }

    /*
     * Now we'll create a PKCS#10 request object from the ospvCertReqB64In.
     * If we can't do it, then the request is bad or misformulated and
     * we'll have to stop the enrollment process, anyway. This doesn't
     * guarantee that the enrollment server will accept the request,
     * but it should be a minimal check against what it's expecting, anyway.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPASN1ElementDecode(certReqBinary, &certReqElementInfo, OSPC_ASN1_DATAREFID_CERTREQ);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to decode the certificate request.\n");
        }
    }

    /*
     *  Now try to parse the contents of the certificate request to ensure
     * that at least a minimal subset of the required elements exist. If
     * not, then this function will catch it as it tries to find the
     * CertReqInfo, then the signatureAlgorithm, and finally the signature.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPASN1ElementParse(certReqElementInfo, OSPEPTID_CERTREQ, OSPC_OSNULL, &certReqParseResults, OSPC_ASN1_DATAREFID_CERTREQ);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to parse the contents of the certificate request.\n");
        }
    }

    /*
     * Now parse the certificate request's contents for the
     * subjectPublicKeyInfo, using the formulation for a PKCS#10
     * readily available in the OSP:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        certReqASN1.ElementInfo = certReqElementInfo;
        certReqASN1.ParseResults = certReqParseResults;

        retVal = OSPPASN1ObjectGetElementByDataRef(&certReqASN1, &publicKeyInfoElementInfo, OSPEDRID_CERTREQINFO_SUBJPUBKEYINFO);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to find subjectPublicKeyInfo in cert request\n");
            retVal = OSPC_ERR_ENROLL_BAD_CERT_REQ;
        }
    }

    /*
     * Now generate some parse results so that the element information can
     * be turned into a "complete" OSPTASN1OBJECT:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = PTPResultsCreate(&publicKeyInfoParseResults, publicKeyInfoElementInfo, OSPEDRID_CERT_SUBJPUBKEYINFO);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to reverse-engineer parse results from the given subjectPublicKeyInfo\n");
            retVal = OSPC_ERR_ENROLL_ASN1_PARSE;
        }
    }

    /*
     * If ( we could finish generating the ASN1 object for the
     *      subjectPublicKeyInfo ) then
     *   o finish the generation of the public key by assigning the
     *     contents we created to the outgoing public key info:
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        ospvPublicKeyOut->ElementInfo = publicKeyInfoElementInfo;
        ospvPublicKeyOut->ParseResults = publicKeyInfoParseResults;
        OSPM_DBGMISC(("public key content: \n"));
        OSPPDumpHex(ospvPublicKeyOut->ElementInfo->Element, ospvPublicKeyOut->ElementInfo->ElementLength);
    }
    /*
     * Else ( we ran into  some problems ) so
     *  o delete the publicKeyInfo, element info and parse results we created:
     */
    else {
        if (ospvPublicKeyOut != OSPC_OSNULL) {
            ospvPublicKeyOut->ElementInfo = OSPC_OSNULL;
            ospvPublicKeyOut->ParseResults = OSPC_OSNULL;
        }

        if (publicKeyInfoElementInfo != OSPC_OSNULL) {
            OSPPASN1ElementDelete(&publicKeyInfoElementInfo, 0);
        }

        if (publicKeyInfoParseResults != OSPC_OSNULL) {
            PTPResultsDelete(&publicKeyInfoParseResults);
        }
    }

    /* Delete the element info, since we won't need it any more: */
    if (certReqElementInfo != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&certReqElementInfo, 0);
    }
    /*
     * Else ( the certificate request wasn't successfully parsed into an
     *        ASN1 structure, so it may still be allocated ) so
     *  o free up the memory that it would have pointed to ( if necessary )
     */
    else {
        if (certReqBinary != OSPC_OSNULL) {
            OSPM_FREE(certReqBinary);
        }
    }

    /* Delete the parse results as well: */
    if (certReqParseResults != OSPC_OSNULL) {
        PTPResultsDelete(&certReqParseResults);
    }

    OSPM_DBGEXIT(("EXIT: OSPPGetPublicKeyInfoFromCertReq\n"));

    return retVal;
}

/*
 * This is for constructing the enrollment request that is sent to the
 * enrollment server; the output is an OSPT_MSG_INFO structure that
 * contains it.
 *
 * Input: A pointer to the enrollment parameters and to the MessageInfo that
 *        contains the parameters for the enrollment request.
 *
 * Output: The OSPT_MSG_INFO should contain all of the information necessary
 *         for transmitting a request to the enrollment server. In this case,
 *         the return value will be OSPC_ERR_NO_ERROR. If something
 *         goes wrong, then the return value will be something other than
 *         OSPC_ERR_NO_ERROR. In that case, there is no guarantee about what
 *         the OSPT_MSG_INFO* structure will contain.
 */
int OSPPConstructEnrollmentRequest(OSPTENROLLPARAMS *ospvEnrollParamsIn,
    OSPT_MSG_INFO *ospvMsgInfoOut)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* This is the temporary storage for the enrollment request: */
    unsigned char *requestBfr = OSPC_OSNULL;

    OSPM_DBGENTER(("OSPPConstructEnrollmentRequest enter\n"));

    /* set an error code and complain if there's something wrong with the input parameters: */
    if ((ospvEnrollParamsIn == NULL) || (ospvMsgInfoOut == NULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "At least one parameter passed in was null.\n");
    }

    /*
     * If ( the parameters weren't null ) then
     *  o check the enrollment request parameters for any null or invalid
     *    values first.
     *  o complain if there's something wrong.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPCheckEnrollmentParams(ospvEnrollParamsIn);
        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_INVALID_REQUEST_PARAMS;
            OSPM_DBGERRORLOG(retVal, "The cert request's construction parameters were invalid.\n");
        }
    }

    /*
     * If ( the enrollment parameters are ok ) then
     *  o Create the nonce if necessary. The method for creating this
     *    nonce may not be random enough, so it's advisable to send this
     *    value in on input. We'll free up any memory that was already
     *    created for the nonce if it's not null.
     *  o complain if we couldn't create the nonce.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        if ((ospvEnrollParamsIn->Nonce == OSPC_OSNULL) || (ospvEnrollParamsIn->NonceLen <= 0)) {
            retVal = OSPPCreateNonce(&(ospvEnrollParamsIn->Nonce), &(ospvEnrollParamsIn->NonceLen), OSPC_ENROLL_NONCE_LEN);

            if (retVal != OSPC_ERR_NO_ERROR) {
                OSPM_DBGERRORLOG(retVal, "Unable to create nonce for the enrollment request.\n");
            } else if ((ospvEnrollParamsIn->Nonce == OSPC_OSNULL) || (ospvEnrollParamsIn->NonceLen <= 0)) {
                retVal = OSPC_ERR_ENROLL_GEN_NONCE;
                OSPM_DBGERRORLOG(retVal, "Unable to generate nonce.\n");
            }
        }
    }

    /*
     * If ( the enrollment parameters look ok ) then
     *  o generate the body for the enrollment request
     *  o complain if we had problems.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPCreateEnrollmentRequestBody(&requestBfr, ospvEnrollParamsIn);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to create enrollment request's body.\n");
        }
        /* check the return values to make sure they're not invalid: */
        else if ((requestBfr == OSPC_OSNULL) || (OSPM_STRLEN((const char *)requestBfr) <= 0)) {
            retVal = OSPC_ERR_ENROLL_CREATE_REQUEST_BODY;
            OSPM_DBGERRORLOG(retVal, "The request body generated was invalid.\n");
        }
    }

    /*
     * IF ( the request body could be generated ) then
     *  o create the enrollment request header.
     *  o complain if we had problems.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPCreateEnrollmentRequestHeader(ospvMsgInfoOut);
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to create enrollment request's header.\n");
        }
    }

    /*
     * If ( we could create the body and header without a glitch ) then
     *  o assign the request to the message info.
     *  o set the size of the message
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        ospvMsgInfoOut->RequestMsg = requestBfr;
        ospvMsgInfoOut->RequestSz = OSPM_STRLEN((const char *)requestBfr);
    }
    /* Else ( we ran into some problem ) so free up the requestBfr if necessary */
    else {
        if (requestBfr != OSPC_OSNULL) {
            OSPM_FREE(requestBfr);
        }
    }

    OSPM_DBGEXIT(("OSPPConstructEnrollmentRequest exit\n"));

    return retVal;
}

/*
 * Check all of the enrollment parameters for any problems that might cause
 * it to be rejected by an enrollment server. We'll check that the
 * username and password are alnum, that the customer and device ids
 * are numeric, and that the base64-encoded certificate request exists
 * ( the contents of the base64-encoded certificate request are validated
 * elsewhere. ) We'll get an error if any of these values are empty or null.
 *
 * Input: reference to the enrollment parameters
 *
 * Output: OSPC_ERR_NO_ERROR if all of the parameters are non-null, non-empty,
 *         and have the appropriate type. Otherwise, an error code other
 *         than OSPC_ERR_NO_ERROR will be returned.
 */
int OSPPCheckEnrollmentParams(OSPTENROLLPARAMS *ospvEnrollParams)
{
    int retVal = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPCheckEnrollmentParams\n"));

    /* Set an error code if the input parameters are null: */
    if (ospvEnrollParams == OSPC_OSNULL) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        if (ospvEnrollParams->Username == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_BAD_USERNAME;
            OSPM_DBGERRORLOG(retVal, "The username was null.\n");
        }
        /* Check the username: */
        retVal = OSPPValidateAsciiString(ospvEnrollParams->Username);
        OSPM_DBGMISC(("username: <%s>\n", ospvEnrollParams->Username));

        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_BAD_USERNAME;
            OSPM_DBGERRORLOG(retVal, "An invalid ( non-alnum ) username was entered.\n");
        }
    }

    /* Check the password: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        if (ospvEnrollParams->Password == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_BAD_PASSWORD;
            OSPM_DBGERRORLOG(retVal, "The password was null.\n");
        }

        retVal = OSPPValidateAsciiString(ospvEnrollParams->Password);
        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_BAD_PASSWORD;
            OSPM_DBGERRORLOG(retVal, "An invalid ( non-alnum ) password was entered.\n");
        }
    }

    /* Check the device id: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPValidateDigitString(ospvEnrollParams->DeviceId);
        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_BAD_DEVICE_ID;
            OSPM_DBGERRORLOG(retVal, "An invalid ( non-digit ) device id was entered.\n");
        }
    }

    /* Check the customer id: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPValidateDigitString(ospvEnrollParams->CustomerId);
        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_BAD_CUSTOMER_ID;
            OSPM_DBGERRORLOG(retVal, "An invalid ( non-digit ) customer id was entered.\n");
        }
    }

    /* check the certificate request: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        if ((ospvEnrollParams->CertReq == OSPC_OSNULL) || (OSPM_STRLEN((const char *)ospvEnrollParams->CertReq) <= 0)) {
            retVal = OSPC_ERR_ENROLL_BAD_CERT_B64;
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPCheckEnrollmentParams\n"));

    return retVal;
}


/*
 * Given a pointer to a string to write, and a pointer to its length, and
 * the length of a nonce to generate, generate a nonce and place it in the
 * output string and its referenced length. The nonce will be binary, not
 * ASCII.
 *
 * This function will use OSPPFillBufWithRandomBytes, which in turn
 * relies on OSPPUtilGetRandom, for generating the random bytes of the nonce.
 * Since OSPPUtilGetRandom doesn't rely on anything special for the
 * entropy of these random values, it would be best to either modify
 * OSPPUtilGetRandom or just pass the value of the nonce in ( so that we
 * don't generate it here. )
 *
 * Input: reference to a string and its length to write to, as well as a
 *        length that specifies how long the nonce should be.
 *
 * Output: OSPC_ERR_NO_ERROR if there aren't any problems; in this case,
 *         the *ospvNonceOut should be non-null and *ospvNonceLenOut should
 *         specify its length. Otherwise, an error code other than
 *         OSPC_ERR_NO_ERROR will be returned if a problem comes up. In this
 *         case, *ospvNonceOut should be OSPC_OSNULL and *ospvNonceLenOut
 *         should be 0 ( but this isn't guaranteed, especially if
 *         ospvNonceOut or ospvNonceLenOut were OSPC_OSNULL to begin with. )
 */
int OSPPCreateNonce(unsigned char **ospvNonceOut,
    unsigned *ospvNonceLenOut,
    unsigned ospvNonceLenIn)
{
    int retVal = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPCreateNonce\n"));

    /* Check the input parameters for any problems: */
    if ((ospvNonceOut == OSPC_OSNULL) || (ospvNonceLenOut == OSPC_OSNULL) || (ospvNonceLenIn <= 0)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "Bad arguments for creating a nonce.\n");
    }

    /*
     * If ( we had didn't have any problems ) then
     *  o allocate space for the outgoing nonce
     *  o complain if we couldn't create the space.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        /* Start by freeing up space so that we don't get a memory leak: */
        if (*ospvNonceOut != OSPC_OSNULL) {
            OSPM_FREE(*ospvNonceOut);
        }

        OSPM_MALLOC(*ospvNonceOut, unsigned char, ospvNonceLenIn + 1);

        if (*ospvNonceOut == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to alloc memory for nonce.\n");
        }
    }

    /*
     * If ( we could allocate memory for the nonce ) then
     *  o initialize it.
     *  o fill it with random bytes
     *  o complain if the return value is bad or if the output nonce or
     *    length are empty.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(*ospvNonceOut, 0, ospvNonceLenIn + 1);

        retVal = OSPPFillBufWithRandomBytes(*ospvNonceOut, ospvNonceLenOut, ospvNonceLenIn);

        if ((retVal != OSPC_ERR_NO_ERROR) || (*ospvNonceOut == OSPC_OSNULL) || (*ospvNonceLenOut <= 0)) {
            retVal = OSPC_ERR_ENROLL_GEN_NONCE;
            OSPM_DBGERRORLOG(retVal, "Unable to generate nonce.\n");
        }
    }

    return retVal;
}


/*
 * Create the enrollment request body that will be sent to the enrollment
 * server. We'll need the enrollment parameters passed in on the command
 * line as well as the random bytes that constitute the nonce. The
 * request buffer should be alphanumeric, so we'll need the string ( but
 * not a pointer to its length. )
 *
 * Input: the buffer to write the body to; the nonce and its length ( since
 *        the nonce may be binary ); and the enrollment parameters.
 *
 * Output: OSPC_ERR_NO_ERROR if everything goes ok; in this case, the buffer
 *         should contain a request that can be transmitted as part of an
 *         HTTP POST to the enrollment server. Otherwise, an error code will
 *         be returned and the buffer will be worthless.
 */
int OSPPCreateEnrollmentRequestBody(unsigned char **ospvRequestBfrOut,
    OSPTENROLLPARAMS *ospvEnrollParamsIn)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /*
     * This is the maximum size of the  request being generated; it
     * will be used for allocating memory later on.
     */
    int requestBfrSize = 0;

    OSPM_DBGENTER(("ENTER: OSPPCreateEnrollmentRequestBody\n"));

    /* Check the parameters first: */
    if (ospvRequestBfrOut == OSPC_OSNULL) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The buffer for the enrollment request's body was null.\n");
    }

    /* Now check the enrollment parameters: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        if (ospvEnrollParamsIn == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG(retVal, "The enrollment parameters for the enrollment request were null.\n");
        }
    }

    /* Check the enrollment request parameters: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPCheckEnrollmentParams(ospvEnrollParamsIn);
        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "The enrollment request parameters are invlaid.\n");
        }
    }

    /*
     * Now check the nonce for a non-null value; this isn't done in
     * OSPPCheckEnrollParams because the nonce may not have been
     * entered on the command line.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        if ((ospvEnrollParamsIn->Nonce == OSPC_OSNULL) ||
            (ospvEnrollParamsIn->NonceLen <= 0)) {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG(retVal, "The nonce for the enrollment request was null or empty.\n");
        }
    }

    /*
     * Allocate enough memory to store all of these values. We'll need
     * enough memory to hold all of the url-encoded characters. The maximum
     * size is then 3 times the size of all of the parameters combined
     * ( since each character may need to be url-encoded. )
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        requestBfrSize = 3 * (ospvEnrollParamsIn->NonceLen + OSPM_STRLEN((const char *)ospvEnrollParamsIn->Username) +
            OSPM_STRLEN((const char *)ospvEnrollParamsIn->Password) + OSPM_STRLEN((const char *)ospvEnrollParamsIn->DeviceId) +
            OSPM_STRLEN((const char *)ospvEnrollParamsIn->CustomerId) + OSPM_STRLEN((const char *)ospvEnrollParamsIn->CertReq) +
            OSPM_STRLEN((const char *)ospvEnrollParamsIn->Function) + 100);
        OSPM_MALLOC(*ospvRequestBfrOut, unsigned char, requestBfrSize + 1);

        if (*ospvRequestBfrOut == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to allocate memory for the outbound request.\n");
        }
    }

    /* Now add the values; add the requested operation first: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(*ospvRequestBfrOut, 0, requestBfrSize + 1);

        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_OPERATION_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_OPERATION_REQ_PARAM), ospvEnrollParamsIn->Function,
                     OSPM_STRLEN((const char *)ospvEnrollParamsIn->Function), OSPC_FALSE);

        OSPM_PRINTTOERR((stderr, "\noutput buffer after operation: %s\n", *ospvRequestBfrOut));
        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal, "Unable to append operation to request body.\n");
        }
    }

    /* Now add the nonce: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_NONCE_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_NONCE_REQ_PARAM), ospvEnrollParamsIn->Nonce, ospvEnrollParamsIn->NonceLen, OSPC_TRUE);

        OSPM_PRINTTOERR((stderr, "output buffer after nonce: %s\n", *ospvRequestBfrOut));

        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal, "Unable to append nonce to request body.\n");
        }
    }

    /* Now add the username: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_USERNAME_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_USERNAME_REQ_PARAM), ospvEnrollParamsIn->Username,
                     OSPM_STRLEN((const char *)ospvEnrollParamsIn->Username), OSPC_TRUE);
        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal, "Unable to append username to request body.\n");
        }
    }

    /* Now add the password: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_PASSWORD_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_PASSWORD_REQ_PARAM), ospvEnrollParamsIn->Password,
                     OSPM_STRLEN((const char *)ospvEnrollParamsIn->Password), OSPC_TRUE);

        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal, "Unable to append password to request body.\n");
        }
    }

    /* Now add the device id: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_DEVICEID_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_DEVICEID_REQ_PARAM), ospvEnrollParamsIn->DeviceId,
                     OSPM_STRLEN((const char *)ospvEnrollParamsIn->DeviceId), OSPC_TRUE);

        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal, "Unable to append device id to request body.\n");
        }
    }

    /* Now add the customer id: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_CUSTOMERID_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_CUSTOMERID_REQ_PARAM), ospvEnrollParamsIn->CustomerId,
                     OSPM_STRLEN((const char *)ospvEnrollParamsIn->CustomerId), OSPC_TRUE);

        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal, "Unable to append customer id to request body.\n");
        }
    }

    /* Now add the certificate request: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        retVal = OSPPAddNameValuePair(*ospvRequestBfrOut, (unsigned char *)OSPC_ENROLL_CERT_REQ_PARAM,
                     OSPM_STRLEN(OSPC_ENROLL_CERT_REQ_PARAM), ospvEnrollParamsIn->CertReq,
                     OSPM_STRLEN((const char *)ospvEnrollParamsIn->CertReq), OSPC_TRUE);

        if (retVal != OSPC_ERR_NO_ERROR) {
            retVal = OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR;
            OSPM_DBGERRORLOG(retVal,
                             "Unable to append certificate request to request body.\n");
        }
    }

    /*
     * If ( we couldn't generate the body ) then
     *  o make sure that we free up the memory, since the caller might
     *    not ( especially if the return value indicates otherwise. )
     */
    if (retVal != OSPC_ERR_NO_ERROR) {
        if (*ospvRequestBfrOut != OSPC_OSNULL) {
            OSPM_FREE(*ospvRequestBfrOut);
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPCreateEnrollmentRequestBody\n"));

    return retVal;
}

/*
 * Create the enrollment request header that will be sent to the enrollment
 * server. We'll just set the content type here.
 *
 * Input: A pointer to the MessageInfo structure that will contain the
 *        enrollment request ( and a pointer to the request's header. )
 *
 * Output: OSPC_ERR_NO_ERROR if everything goes ok; in this case, the
 *         ospvEnrollReqMsgInfo->ContentType should be set as well.
 *         Otherwise, an errorcode should be returned and
 *         ospvEnrollReqMsgInfo->ContentType should be null or empty.
 */
int OSPPCreateEnrollmentRequestHeader(OSPT_MSG_INFO *ospvEnrollReqMsgInfo)
{
    int retVal = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPCreateEnrollmentRequestHeader\n"));

    if (ospvEnrollReqMsgInfo == OSPC_OSNULL) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "Parameter was null.\n");
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(ospvEnrollReqMsgInfo->ContentType, unsigned char, OSPM_STRLEN(OSPC_ENROLL_CONTENT_TYPE) + 1);

        if (ospvEnrollReqMsgInfo->ContentType == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to allocate memory for content type.\n");
        }
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_STRCPY((char *)ospvEnrollReqMsgInfo->ContentType, OSPC_ENROLL_CONTENT_TYPE);
    }

    OSPM_DBGEXIT(("EXIT: OSPPCreateEnrollmentRequestHeader\n"));

    return retVal;
}

/*
 * Create a nonce value that will serve for both stronger encryption ( to
 * eliminate known-ciphertext attacks ) and for preventing flooding at the
 * server. This isn't required by an enrollment server, but it is recommended.
 *
 * Input: references to the nonce's character string and the nonce's length;
 *        both are set on output.
 *
 * Output: success in generating the nonce: OSPC_ERR_NO_ERROR if successful,
 *         another value otherwise. *ospvRandomByteBuf should be allocated a block
 *         of memory whose length is specified by ospvRandomByteBufLenIn. Note that
 *         existing memory will not be used.
 *
 */
int OSPPFillBufWithRandomBytes(unsigned char *ospvRandomByteBuf,
    unsigned *ospvBytesCreated,
    unsigned ospvBytesRequest)
{
    int retVal = 0;

    /* How many bytes have been put into the nonce so far? */
    int bytesFilled = 0;

    /* How many bytes are needed to fill the nonce? */
    int bytesToBeFilled = 0;

    /* How many bytes were created in the last call to OSPPGetRandomBytes? */
    int numBytesCreated = 0;

    /* How many bytes should be copied? We may need less if we're going to potentially overflow ospvRandomByteBuf. */
    int bytesToCopy = 0;

    /* How many more bytes are needed for the buffer?  Equal to  bytesToBeFilled - bytesFilled */
    int bytesRemainingToFill = 0;

    /* What do we seed the rng with? */
    int seed = 0;

    /*
     * We don't know in advance just how many bytes we're going to
     * get, so make sure that we just use a buffer and then copy
     * over the appropriate amount.
     */
    unsigned char *tempRandomBytes = OSPC_OSNULL;


    OSPM_DBGENTER(("ENTER: OSPPFillBufWithRandomBytes\n"));

    /* If ( the parameters are invalid ) then set an error code and complain. */
    if ((ospvRandomByteBuf == OSPC_OSNULL) || (ospvBytesCreated == OSPC_OSNULL) || (ospvBytesRequest <= 0)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "Invalid arguments for generating random bytes.\n");
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        /*
         * Allocate the temporary storage; make sure that it will accommodate
         * as many bytes as may be returned. Each byte may have 3 digits, so
         * we will just need 3 bytes for every byte in an unsigned ( plus 1. )
         */
        OSPM_MALLOC(tempRandomBytes, unsigned char, 3 * sizeof(unsigned) + 1);

        if (tempRandomBytes == OSPC_OSNULL) {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(retVal, "Unable to allocate memory for random bytes.\n");
        }
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_DBGMISC(("Able to allocate memory for random buf.\n"));
        OSPM_MEMSET(tempRandomBytes, 0, 3 * sizeof(unsigned) + 1);

        /*
         * Initialize the loop constraints: bytesToBeFilled will be fixed,
         * but bytesFilled will increase as more bytes are added:
         */
        bytesToBeFilled = ospvBytesRequest;
        bytesFilled = 0;

        /*
         * While ( we still have bytes to fill ) do
         *  o create some random bytes; the number created is recorded
         *    in numBytesCreated. seed should be the seed:
         *  o if ( the number of bytes made will overflow the nonce ) then
         *      - set the number of bytes to copy to just be the number
         *        of bytes that can be filled.
         *  o else ( it's safe to just copy everything ) so
         *      - set the number of bytes to copy to be the number of
         *        bytes generated.
         *  o now copy the appropriate number of bytes
         *  o update the seed ( this does not have a lot of entropy, and
         *    it is based off of all of the prior seeds indirectly )
         *  o update the number of bytes copied.
         */
        bytesRemainingToFill = bytesToBeFilled;

        while (bytesRemainingToFill > 0) {
            numBytesCreated = OSPPUtilGetRandom((char *)tempRandomBytes, seed);

            if (numBytesCreated > bytesRemainingToFill) {
                bytesToCopy = bytesRemainingToFill;
            } else {
                bytesToCopy = numBytesCreated;
            }

            OSPM_STRNCPY(ospvRandomByteBuf + bytesFilled, tempRandomBytes, bytesToCopy);

            seed += bytesFilled + bytesToCopy + ospvRandomByteBuf[bytesFilled];
            bytesFilled += bytesToCopy;
            bytesRemainingToFill = bytesToBeFilled - bytesFilled;
        }

        OSPM_DBGMISC(("bytesFilled: <%d>\n", bytesFilled));
    }

    /* Now finish the operation by assigning the bytes created to the output buffer: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        *ospvBytesCreated = bytesFilled;
    }

    /* Free up the temporary memory we created; the same bytes should be ospvRandomByteBuf: */
    if (tempRandomBytes != OSPC_OSNULL) {
        OSPM_FREE(tempRandomBytes);
    }

    OSPM_DBGEXIT(("EXIT OSPPFillBufWithRandomBytes\n"));

    return retVal;
}

/*
 * Given a MsgInfo structure that will eventually be sent to an enrollment
 * server, add the given name-value pair to the MsgInfo's RequestMsg field.
 * We may want to add a name, but not a value. Therefore, it is ok for the
 * ospvValue parameter to be OSPC_OSNULL.
 *
 * Input: a pointer to the MsgInfo being updated, as well as the name
 *        and value being added.
 *
 * Output: OSPC_ERR_NO_ERROR if the name-value pair can be added;
 *         otherwise, an error.
 */
int OSPPAddNameValuePair(unsigned char *ospvDestStr,
    unsigned char *ospvName,
    unsigned ospvNameLen,
    unsigned char *ospvValue,
    unsigned ospvValueLen,
    unsigned ospvPrependAmpersand)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* If ( the destination string or name to be appended are null ) then set an error code and complain. */
    if ((ospvDestStr == OSPC_OSNULL) || (ospvName == OSPC_OSNULL) || (ospvValue == OSPC_OSNULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The parameters for appending a name-value pair are invalid.\n");
    }

    /*
     * If ( the parameters are ok ) then
     *  o add an ampersand to the beginning if necessary;
     *  o append the name as a url-encoded string
     *  o complain if we couldn't append the name
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        /* Add an ampersand, but only if the flag was set: */
        if (ospvPrependAmpersand) {
            OSPM_STRNCAT((char *)ospvDestStr, OSPC_ENROLL_FIELD_DELIMITER, 1);
        }

        retVal = OSPPAppendUrlEncodedString(ospvDestStr, ospvName, ospvNameLen);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to append the url-encoded name in the name-value pair.\n");
        }
    }

    /*
     * If ( the name could be added ) then
     *  o add the name-value delimiter to the string
     *  o add the value
     *  o complain if we had any problems.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        OSPM_STRNCAT((char *)ospvDestStr, OSPC_ENROLL_NAME_VALUE_DELIMITER, OSPM_STRLEN(OSPC_ENROLL_NAME_VALUE_DELIMITER));

        retVal = OSPPAppendUrlEncodedString(ospvDestStr, ospvValue, ospvValueLen);

        if (retVal != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(retVal, "Unable to append the url-encoded value in the name-value pair.\n");
        }
    }

    return retVal;
}

/*
 * Given a source string, its length, and a destination string,
 * url-encode that string and append to the destination string.
 *
 * Input: the string to be appended to, the string being appended, and
 *        the length of that string.
 *
 * Output: the destination string should have the url-encoded source string
 *         appended to it if there are no problems. In this case, the
 *         return value should be OSPC_ERR_NO_ERROR. Otherwise, an error
 *         code other than OSPC_ERR_ENROLL_INVALID_ARG will be returned.
 */
int OSPPAppendUrlEncodedString(unsigned char *destStr,
    unsigned char *srcStr,
    unsigned srcStrLen)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* Index for parsing the source string: */
    unsigned srcStrIndex = 0;

    /* Offset into the destination string for appending: */
    int destStrIndex = 0;

    /* The next character from the source string: */
    unsigned nextChar = 0;

    OSPM_DBGENTER(("ENTER: OSPPAppendUrlEncodedString\n"));

    if ((destStr == OSPC_OSNULL) && (srcStr == OSPC_OSNULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The source or destination string is null.\n");
    }

    /*
     * If ( the parameters are non-null ) then
     *  o get the length of the destination string
     *  o for ( every character in the source string )
     *      - if ( the next character is alphanumeric ) then
     *          o append it directly to the destination string
     *      - else
     *          o url-encode the character before appending it.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        /*
         * Measure the length of the destination string, so that we know
         * where to start appending:
         */
        destStrIndex = OSPM_STRLEN((const char *)destStr);

        for (srcStrIndex = 0; srcStrIndex < srcStrLen; srcStrIndex++) {
            nextChar = srcStr[srcStrIndex];

            if (OSPM_ISALNUM(nextChar)) {
                destStr[destStrIndex++] = (unsigned char) nextChar;
            } else {
                destStr[destStrIndex++] = '%';
                sprintf((char *)destStr + destStrIndex, "%.2X", nextChar);
                destStrIndex += 2;
            }
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPAppendUrlEncodedString\n"));

    return retVal;
}

/*
 * Given an alnum string, make sure that it's valid: it's non-null,
 * and has nothing but alnum characters.
 *
 * Input: character string to be scanned
 * Output: OSPC_ERR_NO_ERROR if it's a non-null alnum string, or
 *         a non-zero value otherwise:
 */
int OSPPValidateAsciiString(unsigned char *ospvAsciiStr)
{
    int retVal = OSPC_ERR_NO_ERROR;
    int strIndex = 0;
    int asciiStrLen = 0;

    OSPM_DBGENTER(("ENTER: OSPPValidateAsciiString\n"));

    /* check the string before we try to parse it: */
    if (ospvAsciiStr == OSPC_OSNULL) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "Invalid parameter for checking an ascii string\n");
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        asciiStrLen = OSPM_STRLEN((const char *)ospvAsciiStr);

        if (asciiStrLen <= 0) {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG(retVal, "The ascii string passed in was empty");
        }
    }

    /*
     * Now loop through each character; if it's not ascii, then
     * set a flag that indicates that the string isn't ascii.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        for (strIndex = 0; (strIndex < asciiStrLen) && (retVal == OSPC_ERR_NO_ERROR); strIndex++) {
            if (OSPM_ISASCII(ospvAsciiStr[strIndex]) == 0) {
                retVal = OSPC_ERR_BAD_ASCII_STR;
                OSPM_DBGMISC(("<%c> is not an ascii character\n", ospvAsciiStr[strIndex]));
            }
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPValidateAsciiString\n"));

    return retVal;
}

/*
 * Given a string of digits, make sure that it's valid: it's non-null,
 * and has nothing but digits.
 *
 * Input: character string to be scanned
 * Output: OSPC_ERR_NO_ERROR if it's a non-null string of digits, or
 *         an error otherwise.
 *
 * Errors:
 * OSPC_ERR_ENROLL_BAD_DIGIT_STR: non-null, non-empty string that contains
 *                                non-digit characters.
 * OSPC_ERR_ENROLL_INVALID_ARG: a null or empty string was passed in.
 */
int OSPPValidateDigitString(unsigned char *ospvDigitStr)
{
    /* The return value: */
    int retVal = OSPC_ERR_NO_ERROR;

    /* The string index: */
    int strIndex = 0;

    /* The length of the string that is input: */
    int digitStrLen = 0;

    OSPM_DBGENTER(("ENTER: OSPPValidateDigitString\n"));

    /* Check to make sure that the string is non-null: */
    if (ospvDigitStr == OSPC_OSNULL) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG(retVal, "The string to be checked was null.\n");
    }

    /* Check to make sure that the string is non-empty: */
    if (retVal == OSPC_ERR_NO_ERROR) {
        digitStrLen = OSPM_STRLEN((const char *)ospvDigitStr);

        if (digitStrLen <= 0) {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG(retVal, "The string to be checked was empty.\n");
        }
    }

    /* if ( the string isn't empty or null ) then
     *  for ( each character )
     *      if ( the character isn't a digit ) then
     *          - set the return value to an error code.
     */
    if (retVal == OSPC_ERR_NO_ERROR) {
        for (strIndex = 0;
             (strIndex < digitStrLen) && (retVal == OSPC_ERR_NO_ERROR);
             strIndex++) {
            if (OSPM_ISDIGIT(ospvDigitStr[strIndex]) == 0) {
                retVal = OSPC_ERR_ENROLL_BAD_DIGIT_STR;
                OSPM_DBGMISC(("%c is not a digit", ospvDigitStr[strIndex]));
            }
        }
    }

    OSPM_DBGEXIT(("EXIT: OSPPValidateDigitString\n"));

    return retVal;
}

/*
 * Cleanup the communications manager: its HTTP connections, security manager,
 * and the communications manager itself. The only way that this function
 * will return an error is if the communications manager passed in for deletion
 * is null.
 *
 * Input: reference to the communications manager pointer to be deleted.
 */
int OSPPEnrollCleanupCommMgr(OSPTCOMM **ospvCommMgrIn)
{
    int retVal = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPEnrollCleanupCommMgr\n"));

    if ((ospvCommMgrIn == OSPC_OSNULL) || (*ospvCommMgrIn == OSPC_OSNULL)) {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGMISC(("Warning: The comm manager to be cleaned up is invalid.\n"));
    }

    if (retVal == OSPC_ERR_NO_ERROR) {
        if ((*ospvCommMgrIn)->Security != OSPC_OSNULL) {
            OSPPSSLSessionCleanup((void *)(*ospvCommMgrIn)->Security);
        }

        if ((*ospvCommMgrIn)->Security != OSPC_OSNULL) {
            OSPPSecDelete(&((*ospvCommMgrIn)->Security));
        }

        OSPPCommSetShutdown(ospvCommMgrIn, 0);
    }

    OSPM_DBGEXIT(("EXIT: OSPPEnrollCleanupCommMgr\n"));

    return retVal;
}
