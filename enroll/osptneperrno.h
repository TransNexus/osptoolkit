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

#define OSPC_ERR_ENROLL                             24000

/*
 * Errors generated when there's something syntactically wrong
 * with the inputs ( such as how they were entered, their length,
 * and so on ): */
#define OSPC_ERR_ENROLL_PARAMS_FUNCTION             (OSPC_ERR_ENROLL + 10)
#define OSPC_ERR_ENROLL_PARAMS_USERNAME             (OSPC_ERR_ENROLL + 11)
#define OSPC_ERR_ENROLL_PARAMS_PASSWORD             (OSPC_ERR_ENROLL + 12)
#define OSPC_ERR_ENROLL_PARAMS_DEVICEID             (OSPC_ERR_ENROLL + 13)
#define OSPC_ERR_ENROLL_PARAMS_CUSTOMERID           (OSPC_ERR_ENROLL + 14)
#define OSPC_ERR_ENROLL_PARAMS_SSL_URL              (OSPC_ERR_ENROLL + 15)
#define OSPC_ERR_ENROLL_PARAMS_CA_URL               (OSPC_ERR_ENROLL + 16)
#define OSPC_ERR_ENROLL_PARAMS_CACERT               (OSPC_ERR_ENROLL + 17)
#define OSPC_ERR_ENROLL_PARAMS_CERTREQ              (OSPC_ERR_ENROLL + 18)
#define OSPC_ERR_ENROLL_PARAMS_CA_FPRINT            (OSPC_ERR_ENROLL + 19)

/*
 * Errors generated when there's something semantically
 * wrong with the inputs ( such as the wrong datatype, etc. )
 */
#define OSPC_ERR_ENROLL_BAD_USERNAME                (OSPC_ERR_ENROLL + 30)
#define OSPC_ERR_ENROLL_BAD_PASSWORD                (OSPC_ERR_ENROLL + 31)
#define OSPC_ERR_ENROLL_BAD_DEVICE_ID               (OSPC_ERR_ENROLL + 32)
#define OSPC_ERR_ENROLL_BAD_CUSTOMER_ID             (OSPC_ERR_ENROLL + 33)
#define OSPC_ERR_ENROLL_BAD_CERT_B64                (OSPC_ERR_ENROLL + 34)
#define OSPC_ERR_ENROLL_BAD_CERT                    (OSPC_ERR_ENROLL + 35)
#define OSPC_ERR_ENROLL_BAD_CERT_REQ                (OSPC_ERR_ENROLL + 36)
#define OSPC_ERR_ENROLL_BAD_FPRINT                  (OSPC_ERR_ENROLL + 37)

/* Errors generated when parameters are invalid: */
#define OSPC_ERR_ENROLL_INVALID_ARG                 (OSPC_ERR_ENROLL + 40)
#define OSPC_ERR_ENROLL_NO_MEMORY                   (OSPC_ERR_ENROLL + 41)
#define OSPC_ERR_ENROLL_NULL_PTR                    (OSPC_ERR_ENROLL + 42)

/* Errors generated while creating a nonce: */
#define OSPC_ERR_ENROLL_CREATE_NONCE_BAD_PARAMS     (OSPC_ERR_ENROLL + 50)
#define OSPC_ERR_ENROLL_GEN_NONCE                   (OSPC_ERR_ENROLL + 51)

/* String-related errors: */
#define OSPC_ERR_BAD_ALNUM_STR                      (OSPC_ERR_ENROLL + 60)
#define OSPC_ERR_BAD_ASCII_STR                      (OSPC_ERR_ENROLL + 61)
#define OSPC_ERR_ENROLL_BAD_DIGIT_STR               (OSPC_ERR_ENROLL + 62)

/* Errors related to finding names and values within strings: */
#define OSPC_ERR_ENROLL_VALUE_NOT_FOUND             (OSPC_ERR_ENROLL + 70)
#define OSPC_ERR_ENROLL_SEARCH_FIELD_NOT_FOUND      (OSPC_ERR_ENROLL + 71)

/* Errors related to constructing enrollment requests: */
#define OSPC_ERR_ENROLL_INVALID_REQUEST_PARAMS      (OSPC_ERR_ENROLL + 80)
#define OSPC_ERR_ENROLL_CREATE_REQUEST_BODY         (OSPC_ERR_ENROLL + 81)
#define OSPC_ERR_ENROLL_INVALID_RESPONSE            (OSPC_ERR_ENROLL + 82)

/* Errors related to ASN1 comparison and parsing functionality: */
#define OSPC_ERR_ENROLL_ASN1_CMP_LENGTH_MISMATCH    (OSPC_ERR_ENROLL + 90)
#define OSPC_ERR_ENROLL_ASN1_CMP_MISMATCH           (OSPC_ERR_ENROLL + 91)
#define OSPC_ERR_ENROLL_ASN1_PARSE                  (OSPC_ERR_ENROLL + 92)

/* Errors related to appending name-value pairs, copying strings, and
 * other miscellaneous convenience functions:
 */
#define OSPC_ERR_ENROLL_APPENDING_NAME_VALUE_PAIR   (OSPC_ERR_ENROLL + 100)
#define OSPC_ERR_ENROLL_SOURCE_STRING_EMPTY         (OSPC_ERR_ENROLL + 101)
