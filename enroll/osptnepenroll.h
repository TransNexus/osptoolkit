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
#include "openssl/ssl.h"

/* This is the "operation" parameter that is sent in each request: */
#define OSPC_ENROLL_OPERATION_REQ_PARAM   "operation"

/* These are the possible values of the "operation" parameter: */
#define OSPC_ENROLL_OP_REQUEST_REQ_PARAM  "request"
#define OSPC_ENROLL_OP_RETRIEVE_REQ_PARAM "retrieve"
#define OSPC_ENROLL_OP_CA_CERT_REQ_PARAM  "getcacert"

/* These are the constants that may be returned in a response: */
#define OSPC_ENROLL_CA_CERT_RSP_PARAM     "certificate"

/* These are also the parameters that are sent in each certificate request
 * or retrieval: 
 */
#define OSPC_ENROLL_NONCE_REQ_PARAM       "nonce"
#define OSPC_ENROLL_USERNAME_REQ_PARAM    "username"
#define OSPC_ENROLL_PASSWORD_REQ_PARAM    "password"
#define OSPC_ENROLL_DEVICEID_REQ_PARAM    "device"
#define OSPC_ENROLL_CUSTOMERID_REQ_PARAM  "customer"
#define OSPC_ENROLL_CERT_REQ_PARAM        "request"

#define OSPC_ENROLL_CERT_RSP_PARAM        "cert"
#define OSPC_ENROLL_STATUS_RSP_PARAM      "status"

/* These signify successful and pending certificate requests, respectively: */
#define OSPC_ENROLL_STATUS_OK             0
#define OSPC_ENROLL_STATUS_PENDING        1

/* Denotes the failure of an enrollment request; this may happen even
 * if there aren't any problems with the enrollment client:
 */
#define OSPC_ENROLL_STATUS_FAILURE_DEFAULT 2


/* The possible field delimiters in a URL are an ampersand ( '&' ) and
 * a space, which would indicate some other field. Actually, this is true
 * for any whitespace character.
 */
#define OSPC_ENROLL_FIELD_DELIMITERS      "& "

/* The "normal" field delimiter for a url is an ampersand: */
#define OSPC_ENROLL_FIELD_DELIMITER       "&"

/* This separates names and values in an url. For example, 
 * in http://www.transnexus.com/some_path/enroll?name=value&another_name=x ,
 * the name and value in the HTTP get would be the '='.
 */
#define OSPC_ENROLL_NAME_VALUE_DELIMITER  "="

/* The content type of the message we post to the enrollment server: */
#define OSPC_ENROLL_CONTENT_TYPE          "text/html"

/* What is the maximum length of the request that could be transmitted?
 * This needs to take into account the maximum size of a 
 * RelativeDistinguishedName ( say, 2KB ), the size of a subjectPublicKeyInfo
 * and a signature ( 512 bytes, for 2 2048-bit values ), and the size
 * of the attributes and other cert request chaff ( say, less than 512 bytes ).
 * If we base64-encode this, then we get a max of about 4KB.
 */
#define OSPC_ENROLL_MAX_REQUEST_SIZE      4096

/* How long will the nonce be in bytes? */
#define OSPC_ENROLL_NONCE_LEN             16 

#ifdef __cplusplus
extern "C" 
{
#endif

/* 
 * Given the input parameters for enrollment, setup all of the communications
 * and structures for enrollment and then send a request to the enrollment
 * server. The results should be stored in the ospvEnrollStatusOUt
 * ( failed, pending, or success ) and ospvLocalCertOut ( if the request
 * was successful.
 */
int OSPPEnroll( 
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    unsigned char**   ospvLocalCertOut,
    unsigned*         ospvLocalCertLenOut,
    unsigned*         ospvEnrollStatusOut
);

/* Given the enrollment parameters, base64-decode the CACertB64 parameter
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
int OSPPBase64DecodeCACert(
    OSPTENROLLPARAMS* ospvEnrollParamsIn
);

/*
 * Create an enrollment request for requesting or retrieving a certificate, 
 * send the request to the enrollment server ( using the communications 
 * manager passed in ); extract the status and certificate from the
 * response, validate the certificate, and return the certificate.
 */
int OSPPEnrollDevice (
    OSPTCOMM*       ospvCommMgr,
    OSPTMSGINFO*    ospvEnrollMsg,
    OSPTASN1OBJECT* ospvRequestPublicKeyIn,
    unsigned*       ospvEnrollStatusOut,
    unsigned char** ospvCertOut,
    unsigned*       ospvCertLenOut
);

/*
 * Given a certificate for a device that has been received, validate it
 * against the public key used ( to be added ), the CA certificate that
 * we expected to sign it, and its general construction. The certificate
 * is placed in *ospvCertOut.
 */
int OSPPValidateDeviceCert (
    OSPTSEC*        ospvSecIn,
    OSPTASN1OBJECT* ospvRequestPublicKeyIn,
    unsigned char*  ospvCertB64In,
    unsigned char** ospvCertOut,
    unsigned*       ospvCertLenOut 
);


int OSPPCreateEnrollmentRequestHeader(
    OSPTMSGINFO* ospvEnrollmentReqMsgInfo
);

int OSPPCreateEnrollmentRequestBody(
    unsigned char**   ospvRequestBfrOut,
    OSPTENROLLPARAMS* ospvEnrollParamsIn
);

/* 
 * Create a nonce value that will serve for both stronger encryption ( to
 * eliminate known-ciphertext attacks ) and for preventing flooding at the
 * server. This isn't required by an enrollment server, but it is recommended.
 *
 * Input: references to the nonce's character string and the nonce's length;
 *        both are set on output. 
 *
 * Output: success in generating the nonce: OSPC_ERR_NO_ERROR if successful,
 *         another value otherwise. *ospvNonce should be allocated a block
 *         of memory whose length is specified by ospvNonceLenIn. Note that
 *         existing memory will not be used.
 * 
 */
int OSPPFillBufWithRandomBytes( 
    unsigned char*  ospvNonce,
    unsigned*       ospvNonceLenOut,
    unsigned        ospvNonceLenIn
);

/* Check all of the enrollment parameters for any problems that might cause
 * it to be rejected by an enrollment server. We'll check that the
 * username and password are alphanumeric, that the customer and device ids
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
int OSPPCheckEnrollmentParams (
    OSPTENROLLPARAMS* ospvEnrollParams
);

/* 
 * Given a pointer to some enrollment parameters, check the validity of each
 * parameter for requesting a certificate. This function returns an error
 * if any of the values are invalid.
 *
 * Input: pointer to an enrollment parameter list; there is only one structure,
 *        and the use of the reference is done for the sake of efficiency.
 *
 * Output: OSPC_ERR_NO_ERROR if all of the parameters can be used in a cert
 *         request; otherwise, an error is returned.
 */
int OSPPCheckEnrollmentRequestParams( 
    OSPTENROLLPARAMS* ospvEnrollParams 
);

/* Given a pointer to a string to write, and a pointer to its length, and
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
int OSPPCreateNonce(
    unsigned char** ospvNonceOut,
    unsigned*       ospvNonceLenOut,
    unsigned        ospvNonceLenIn
);

/*
 * Given a character string that will eventually be sent to an enrollment
 * server as part of a MsgInfo's Request, add a name-value pair that reflects
 * an enrollment parameter. 
 *
 * Input: the request's character string, plus the k
 *
 *
 */
int OSPPAddNameValuePair(
    unsigned char* ospvDestStr,
    unsigned char* ospvName,
    unsigned       ospvNameLen,
    unsigned char* ospvValue,
    unsigned       ospvValueLen,
    unsigned       ospvPrependAmpersand
);

/* Given a source string of characters, URL-encode it and append it to
 * the destination string:
 */
int OSPPAppendUrlEncodedString (
    unsigned char* destStr,
    unsigned char* srcStr,
    unsigned       srcStrLen
);

/*
 * Given a MessageInfo structure, extract the status from the "status=" field
 * that's contained in the Response field. If the status field cannot be found,
 * then this function will return an error.
 */
int OSPPGetStatusFromResponse (
    const OSPTMSGINFO* ospvMsgInfo,
    unsigned* ospvEnrollStatusOut
);

/*
 * Given a MessageInfo structure, extract the certificate from it. There
 * is no need to check the base64 encoding of the certificate or any other
 * structural constraints.
 */
int OSPPGetCertificateFromResponse (
    const OSPTMSGINFO* ospvMsgInfo,
    unsigned char*     ospvCertOut,
    unsigned*          ospvCertLenOut
);

/*
 * Given the CA certificate and a certificate that is supposedly for the
 * router, check its validity. That is, decode the base64 encoding of the
 * certificate, make sure that it's an X.509 cert, that it has the correct
 * version ( 2, to indicate a version 3 cert ); and that it's signed by
 * the CA. This should also include something for checking the public key
 * at some point.
 */
int OSPPValidateCert (
    const unsigned char* ospvCACertIn,
    const unsigned char* ospvCertIn,
    const unsigned ospvCertLenIn,
    const OSPTASN1OBJECT* ospvSubjectPublicKeyInfoIn,
    OSPTASN1OBJECT* ospvCertOut
);

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
int OSPPASN1Compare (
    OSPTASN1OBJECT* ospvLHSObject,
    OSPTASN1OBJECT* ospvRHSObject
);


/*
 * This is for constructing the enrollment request that is sent to the 
 * enrollment server; the output is an OSPTMSGINFO structure that
 * contains it.
 *
 * Input: A pointer to the enrollment parameters and to the MessageInfo that
 *        contains the parameters for the enrollment request.
 *
 * Output: The OSPTMSGINFO should contain all of the information necessary
 *         for transmitting a request to the enrollment server. In this case,
 *         the return value will be OSPC_ERR_NO_ERROR. If something
 *         goes wrong, then the return value will be something other than
 *         OSPC_ERR_NO_ERROR. In that case, there is no guarantee about what
 *         the OSPTMSGINFO* structure will contain.
 */
int OSPPConstructEnrollmentRequest (
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTMSGINFO*      ospvMsgInfoOut
);

/* 
 * Given a base64 encoding of a certificate request, retrieve the 
 * subjectPublicKeyInfo from that certificate request and store it in the
 * given ASN.1 element info's placeholder. 
 *
 * Input: base64 certificate request 
 *        ( unsigned char* ospvBase64CertReq )
 *
 * Output: *ospvPublicKeyInfoOut should, if successful, contain the 
 *         subjectPublicKeyInfo of the certificate request. If not, then 
 *         an errorcode should be returned.
 *
 * Errors: Errorcodes other than OSPC_ERR_NO_ERROR are returned when:
 *     o either parameter is null;
 *     o memory cannot be allocated for ephemeral variables;
 *     o the certificate request is improperly base64 encoded;
 *     o the certificate request is invalid ( i.e., the subjectPublicKeyInfo
 *       could not be found in the desired location );
 */
int OSPPGetPublicKeyInfoFromCertReq( 
    unsigned char*   ospvBase64CertReq,
    OSPTASN1OBJECT*  ospvPublicKeyInfoOut 
);

/* Given a character string, make sure that it's valid: it's non-null,
 * and has nothing but ascii characters:
 */
int OSPPValidateAsciiString( 
    unsigned char* ospvAlnumStr 
);

/* Given a string of digits, make sure that it's valid: it's non-null,
 * and has nothing but digits.
 */
int OSPPValidateDigitString( 
    unsigned char* ospvDigitStr
); 

/* Given an ASN1 object that represents an X.509 certificate, store its
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
int OSPPGetPublicKeyInfoFromCert( 
    OSPTASN1OBJECT*  ospvCertIn, 
    OSPTASN1OBJECT*  ospvPublicKeyOut 
);

/* Given a binary string that represents a PKCS#10 request, create an
 * ASN1 object that contains the subjectPublicKeyInfo of the certificate
 * request. The subjectPublicKeyInfo is found as follows:
 *
 * SEQUENCE CertificateRequest        
 *     SEQUENCE certificateRequestInfo 
 *         INTEGER  version 
 *         SEQUENCE subjectName 
 *         SEQUENCE subjectPublicKeyInfo 
 *         SEQUENCE attributes
 *     OID signatureAlgorithm
 *     BIT STRING signature
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
int OSPPGetPublicKeyInfoFromCertReq(
    unsigned char*   ospvCertReqIn, 
    OSPTASN1OBJECT*  ospvPublicKeyOut 
); 

/* Cleanup the communications manager: its HTTP connections, security manager,
 * and the communications manager itself. The only way that this function
 * will return an error is if the communications manager passed in for deletion
 * is null.
 *
 * Input: reference to the communications manager pointer to be deleted.
 */
int OSPPEnrollCleanupCommMgr (
    OSPTCOMM** ospvCommMgrIn
);

#ifdef __cplusplus
}
#endif
