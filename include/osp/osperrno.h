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

/*
 * osperrno.h - Error numbers.
 */

#ifndef _OSPERRNO_H
#define _OSPERRNO_H

#define OSPC_ERR_OS_FAILURE                             -1
#define OSPC_ERR_NO_ERROR                               0
#define OSPC_ERR_BAD_SERVICE                            16

#define OSPC_ERR_MODULE                                 9000
#define OSPC_ERR_INVALID_POINTER                        (OSPC_ERR_MODULE + 10)
#define OSPC_ERR_TIME_ERROR                             (OSPC_ERR_MODULE + 20)

#define OSPC_ERR_PROV_MODULE                            10000
#define OSPC_ERR_PROV_INIT_FAILURE                      (OSPC_ERR_PROV_MODULE + 10)
#define OSPC_ERR_PROV_NULL_POINTER                      (OSPC_ERR_PROV_MODULE + 20)
#define OSPC_ERR_PROV_TRANSCOLLECTION_NOT_FOUND         (OSPC_ERR_PROV_MODULE + 30)
#define OSPC_ERR_PROV_NO_PROV_SPACE                     (OSPC_ERR_PROV_MODULE + 40)
#define OSPC_ERR_PROV_HANDLE_INVALID                    (OSPC_ERR_PROV_MODULE + 50)
#define OSPC_ERR_PROV_MAX_PROVIDERS                     (OSPC_ERR_PROV_MODULE + 60)
#define OSPC_ERR_PROV_MUTEX_FAILURE                     (OSPC_ERR_PROV_MODULE + 70)
#define OSPC_ERR_PROV_INVALID_VALUE                     (OSPC_ERR_PROV_MODULE + 80)

#define OSPC_ERR_TRAN_MODULE                            11000
#define OSPC_ERR_TRAN_DELETE_NOT_ALLOWED                (OSPC_ERR_TRAN_MODULE + 10)
#define OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND             (OSPC_ERR_TRAN_MODULE + 20)
#define OSPC_ERR_TRAN_FIRST_DATA_ALREADY_DELIVERED      (OSPC_ERR_TRAN_MODULE + 30)
#define OSPC_ERR_TRAN_RESPONSE_NOT_FOUND                (OSPC_ERR_TRAN_MODULE + 40)
#define OSPC_ERR_TRAN_NO_TRANS_SPACE                    (OSPC_ERR_TRAN_MODULE + 50)
#define OSPC_ERR_TRAN_MALLOC_FAILED                     (OSPC_ERR_TRAN_MODULE + 60)
#define OSPC_ERR_TRAN_TRANSACTION_NOT_ALLOWED           (OSPC_ERR_TRAN_MODULE + 70)
#define OSPC_ERR_TRAN_NO_MORE_RESPONSES                 (OSPC_ERR_TRAN_MODULE + 80)
#define OSPC_ERR_TRAN_VALUE_NOT_FOUND                   (OSPC_ERR_TRAN_MODULE + 90)
#define OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY         (OSPC_ERR_TRAN_MODULE + 100)
#define OSPC_ERR_TRAN_CALLID_DEST_MISMATCH              (OSPC_ERR_TRAN_MODULE + 110)
#define OSPC_ERR_TRAN_INVALID_ENTRY                     (OSPC_ERR_TRAN_MODULE + 120)
#define OSPC_ERR_TRAN_TOO_SOON_TO_USE_TOKEN             (OSPC_ERR_TRAN_MODULE + 130)
#define OSPC_ERR_TRAN_TOO_LATE_TO_USE_TOKEN             (OSPC_ERR_TRAN_MODULE + 140)
#define OSPC_ERR_TRAN_SOURCE_INVALID                    (OSPC_ERR_TRAN_MODULE + 150)
#define OSPC_ERR_TRAN_DEST_INVALID                      (OSPC_ERR_TRAN_MODULE + 160)
#define OSPC_ERR_TRAN_CALLING_INVALID                   (OSPC_ERR_TRAN_MODULE + 170)
#define OSPC_ERR_TRAN_CALLED_INVALID                    (OSPC_ERR_TRAN_MODULE + 180)
#define OSPC_ERR_TRAN_CALL_ID_INVALID                   (OSPC_ERR_TRAN_MODULE + 190)
#define OSPC_ERR_TRAN_AUTH_ID_INVALID                   (OSPC_ERR_TRAN_MODULE + 200)
#define OSPC_ERR_TRAN_CALLID_NOT_FOUND                  (OSPC_ERR_TRAN_MODULE + 210)
#define OSPC_ERR_TRAN_NUMBER_CALL_IDS_INVALID           (OSPC_ERR_TRAN_MODULE + 220)
#define OSPC_ERR_TRAN_NOT_IMPLEMENTED                   (OSPC_ERR_TRAN_MODULE + 230)
#define OSPC_ERR_TRAN_HANDLE_INVALID                    (OSPC_ERR_TRAN_MODULE + 240)
#define OSPC_ERR_TRAN_INVALID_FAILURE_CODE              (OSPC_ERR_TRAN_MODULE + 250)
#define OSPC_ERR_TRAN_STATUS_INVALID                    (OSPC_ERR_TRAN_MODULE + 260)
#define OSPC_ERR_TRAN_TOKEN_INVALID                     (OSPC_ERR_TRAN_MODULE + 270)
#define OSPC_ERR_TRAN_STATUS_NOT_FOUND                  (OSPC_ERR_TRAN_MODULE + 280)
#define OSPC_ERR_TRAN_VALID_AFTER_NOT_FOUND             (OSPC_ERR_TRAN_MODULE + 290)
#define OSPC_ERR_TRAN_DEST_NOT_FOUND                    (OSPC_ERR_TRAN_MODULE + 300)
#define OSPC_ERR_TRAN_VALID_UNTIL_NOT_FOUND             (OSPC_ERR_TRAN_MODULE + 310)
#define OSPC_ERR_TRAN_SIGADDR_INVALID                   (OSPC_ERR_TRAN_MODULE + 320)
#define OSPC_ERR_TRAN_TXID_NOT_FOUND                    (OSPC_ERR_TRAN_MODULE + 330)
#define OSPC_ERR_TRAN_SOURCE_NUMBER_NOT_FOUND           (OSPC_ERR_TRAN_MODULE + 340)
#define OSPC_ERR_TRAN_DEST_NUMBER_NOT_FOUND             (OSPC_ERR_TRAN_MODULE + 350)
#define OSPC_ERR_TRAN_TOKEN_NOT_FOUND                   (OSPC_ERR_TRAN_MODULE + 360)
#define OSPC_ERR_TRAN_LIST_NOT_FOUND                    (OSPC_ERR_TRAN_MODULE + 370)
#define OSPC_ERR_TRAN_ACCUMULATE_NOT_ALLOWED            (OSPC_ERR_TRAN_MODULE + 380)
#define OSPC_ERR_TRAN_USAGE_ALREADY_REPORTED            (OSPC_ERR_TRAN_MODULE + 390)
#define OSPC_ERR_TRAN_STATS_NOT_FOUND                   (OSPC_ERR_TRAN_MODULE + 400)
#define OSPC_ERR_TRAN_STATS_NEW_FAIL                    (OSPC_ERR_TRAN_MODULE + 410)
#define OSPC_ERR_TRAN_INVALID_CALC                      (OSPC_ERR_TRAN_MODULE + 420)
#define OSPC_ERR_TRAN_GET_DEST_NOT_ALLOWED              (OSPC_ERR_TRAN_MODULE + 430)
#define OSPC_ERR_TRAN_AUTH_REQ_NOT_FOUND                (OSPC_ERR_TRAN_MODULE + 440)
#define OSPC_ERR_TRAN_TXID_INVALID                      (OSPC_ERR_TRAN_MODULE + 450)
#define OSPC_ERR_TRAN_DATA_NOT_FOUND                    (OSPC_ERR_TRAN_MODULE + 460)
#define OSPC_ERR_TRAN_NO_NEW_AUTHREQ                    (OSPC_ERR_TRAN_MODULE + 470)
#define OSPC_ERR_TRAN_TOKEN_REUSE                       (OSPC_ERR_TRAN_MODULE + 480)
#define OSPC_ERR_TRAN_CLIENT_ERROR                      (OSPC_ERR_TRAN_MODULE + 481)
#define OSPC_ERR_TRAN_BAD_REQUEST                       (OSPC_ERR_TRAN_MODULE + 482)
#define OSPC_ERR_TRAN_UNAUTHORIZED                      (OSPC_ERR_TRAN_MODULE + 483)
#define OSPC_ERR_TRAN_CHAR_ENC_NOT_SUPD                 (OSPC_ERR_TRAN_MODULE + 484)
#define OSPC_ERR_TRAN_PARS_UNSUCCESSFUL                 (OSPC_ERR_TRAN_MODULE + 485)
#define OSPC_ERR_TRAN_UNSUPD_CRIT_ELEM                  (OSPC_ERR_TRAN_MODULE + 486)
#define OSPC_ERR_TRAN_SECURITY_PROBLEM                  (OSPC_ERR_TRAN_MODULE + 487)
#define OSPC_ERR_TRAN_SIG_INVALID                       (OSPC_ERR_TRAN_MODULE + 488)
#define OSPC_ERR_TRAN_CRYPTO_ALG_NOT_SUPD               (OSPC_ERR_TRAN_MODULE + 489)
#define OSPC_ERR_TRAN_CERT_INVALID                      (OSPC_ERR_TRAN_MODULE + 490)
#define OSPC_ERR_TRAN_CERT_REVOKED                      (OSPC_ERR_TRAN_MODULE + 491)
#define OSPC_ERR_TRAN_ENCRYPTION_REQD                   (OSPC_ERR_TRAN_MODULE + 492)
#define OSPC_ERR_TRAN_SERVER_ERROR                      (OSPC_ERR_TRAN_MODULE + 493)
#define OSPC_ERR_TRAN_INTERNAL_SRVR_ERR                 (OSPC_ERR_TRAN_MODULE + 494)
#define OSPC_ERR_TRAN_UNIMPLEMENTED                     (OSPC_ERR_TRAN_MODULE + 495)
#define OSPC_ERR_TRAN_SERVICE_NOT_AVAIL                 (OSPC_ERR_TRAN_MODULE + 496)
#define OSPC_ERR_TRAN_TRANSIENT_SRVR_PROB               (OSPC_ERR_TRAN_MODULE + 497)
#define OSPC_ERR_TRAN_LONG_TERM_SRVR_PROB               (OSPC_ERR_TRAN_MODULE + 498)
#define OSPC_ERR_TRAN_TIME_PROB                         (OSPC_ERR_TRAN_MODULE + 499)
#define OSPC_ERR_TRAN_VALID_TIME_TOO_SOON               (OSPC_ERR_TRAN_MODULE + 500)
#define OSPC_ERR_TRAN_TIME_INTERVAL_TOO_SMALL           (OSPC_ERR_TRAN_MODULE + 501)
#define OSPC_ERR_TRAN_GENERIC_FAILURE                   (OSPC_ERR_TRAN_MODULE + 502)
#define OSPC_ERR_TRAN_ROUTE_NOT_FOUND                   (OSPC_ERR_TRAN_MODULE + 503)
#define OSPC_ERR_TRAN_DUPLICATE_REQUEST                 (OSPC_ERR_TRAN_MODULE + 504)
#define OSPC_ERR_TRAN_REQ_OUT_OF_SEQ                    (OSPC_ERR_TRAN_MODULE + 505)
#define OSPC_ERR_TRAN_UNSIGNED_TOKEN_NOT_ALLOWED        (OSPC_ERR_TRAN_MODULE + 506)
#define OSPC_ERR_TRAN_SIGNED_TOKEN_NOT_ALLOWED          (OSPC_ERR_TRAN_MODULE + 507)
#define OSPC_ERR_TRAN_NETWORKADDR_INVALID               (OSPC_ERR_TRAN_MODULE + 508)
#define OSPC_ERR_TRAN_NO_NETWORK_ID_IN_DEST             (OSPC_ERR_TRAN_MODULE + 509)
#define OSPC_ERR_TRAN_ROUTE_BLOCKED                     (OSPC_ERR_TRAN_MODULE + 510)
#define OSPC_ERR_TRAN_MAY_NOT_ORIGINATE                 (OSPC_ERR_TRAN_MODULE + 511)
#define OSPC_ERR_TRAN_CALL_RATE_EXCEEDED                (OSPC_ERR_TRAN_MODULE + 512)
#define OSPC_ERR_TRAN_DESTINATION_INFO_INVALID          (OSPC_ERR_TRAN_MODULE + 513)
#define OSPC_ERR_TRAN_DETAILS_NEW_FAIL                  (OSPC_ERR_TRAN_MODULE + 514)

#define OSPC_ERR_SEC_MODULE                             12000
#define OSPC_ERR_SEC_NOT_ENOUGH_SPACE_FOR_COPY          (OSPC_ERR_SEC_MODULE + 10)
#define OSPC_ERR_SEC_INVALID_ARG                        (OSPC_ERR_SEC_MODULE + 20)
#define OSPC_ERR_SEC_PRIVATE_KEY_NOT_FOUND              (OSPC_ERR_SEC_MODULE + 30)
#define OSPC_ERR_SEC_NOT_IMPLEMENTED                    (OSPC_ERR_SEC_MODULE + 40)
#define OSPC_ERR_SEC_NO_MEMORY                          (OSPC_ERR_SEC_MODULE + 50)
#define OSPC_ERR_SEC_INVALID_SIG                        (OSPC_ERR_SEC_MODULE + 60)
#define OSPC_ERR_SEC_SSL_INIT_DB_FAILED                 (OSPC_ERR_SEC_MODULE + 65)
#define OSPC_ERR_SEC_NO_SPACE_FOR_CERTIFICATE           (OSPC_ERR_SEC_MODULE + 70)
#define OSPC_ERR_SEC_LOCAL_CERTINFO_UNDEFINED           (OSPC_ERR_SEC_MODULE + 80)
#define OSPC_ERR_SEC_ZERO_LENGTH_CERTIFICATE            (OSPC_ERR_SEC_MODULE + 90)
#define OSPC_ERR_SEC_CERTIFICATE_TOO_BIG                (OSPC_ERR_SEC_MODULE + 100)
#define OSPC_ERR_SEC_INVALID_CERTIFICATE                (OSPC_ERR_SEC_MODULE + 110)
#define OSPC_ERR_SEC_NULL_CERTIFICATE                   (OSPC_ERR_SEC_MODULE + 120)
#define OSPC_ERR_SEC_TOO_MANY_CERTIFICATES              (OSPC_ERR_SEC_MODULE + 130)
#define OSPC_ERR_SEC_NO_STORAGE_PROVIDED                (OSPC_ERR_SEC_MODULE + 140)
#define OSPC_ERR_SEC_NO_PRIVATE_KEY                     (OSPC_ERR_SEC_MODULE + 150)
#define OSPC_ERR_SEC_INVALID_CONTEXT                    (OSPC_ERR_SEC_MODULE + 160)
#define OSPC_ERR_SEC_UNABLE_TO_ALLOCATE_SPACE           (OSPC_ERR_SEC_MODULE + 170)
#define OSPC_ERR_SEC_CA_CERTIFICATES_DONT_MATCH         (OSPC_ERR_SEC_MODULE + 180)
#define OSPC_ERR_SEC_NO_AUTHORITY_CERTIFICATES          (OSPC_ERR_SEC_MODULE + 190)
#define OSPC_ERR_SEC_CACERT_INDEX_OVERFLOW              (OSPC_ERR_SEC_MODULE + 200)

#define OSPC_ERR_MSG_MODULE                             13000
#define OSPC_ERR_MIME_BUFFER_TOO_SMALL                  (OSPC_ERR_MSG_MODULE + 110)
#define OSPC_ERR_MIME_MALLOC_FAILED                     (OSPC_ERR_MSG_MODULE + 115)
#define OSPC_ERR_MIME_VAR_NOT_FOUND                     (OSPC_ERR_MSG_MODULE + 120)
#define OSPC_ERR_MIME_NO_INPUT                          (OSPC_ERR_MSG_MODULE + 125)
#define OSPC_ERR_MIME_INVALID_ARG                       (OSPC_ERR_MSG_MODULE + 130)
#define OSPC_ERR_MIME_NO_MORE_SPACE                     (OSPC_ERR_MSG_MODULE + 135)
#define OSPC_ERR_MIME_INVALID_TYPE                      (OSPC_ERR_MSG_MODULE + 140)
#define OSPC_ERR_MIME_INVALID_SUBTYPE                   (OSPC_ERR_MSG_MODULE + 145)
#define OSPC_ERR_MIME_PROTOCOL_NOT_FOUND                (OSPC_ERR_MSG_MODULE + 150)
#define OSPC_ERR_MIME_MICALG_NOT_FOUND                  (OSPC_ERR_MSG_MODULE + 155)
#define OSPC_ERR_MIME_BOUNDARY_NOT_FOUND                (OSPC_ERR_MSG_MODULE + 160)
#define OSPC_ERR_MIME_CONTENT_TYPE_NOT_FOUND            (OSPC_ERR_MSG_MODULE + 165)
#define OSPC_ERR_MIME_MSG_PARTS_NOT_FOUND               (OSPC_ERR_MSG_MODULE + 170)

#define OSPC_ERR_XML_INCOMPLETE                         (OSPC_ERR_MSG_MODULE + 301)
#define OSPC_ERR_XML_BAD_ENC                            (OSPC_ERR_MSG_MODULE + 302)
#define OSPC_ERR_XML_BAD_ENTITY                         (OSPC_ERR_MSG_MODULE + 303)
#define OSPC_ERR_XML_BAD_NAME                           (OSPC_ERR_MSG_MODULE + 304)
#define OSPC_ERR_XML_BAD_TAG                            (OSPC_ERR_MSG_MODULE + 305)
#define OSPC_ERR_XML_BAD_ATTR                           (OSPC_ERR_MSG_MODULE + 306)
#define OSPC_ERR_XML_BADCID_ENC                         (OSPC_ERR_MSG_MODULE + 307)
#define OSPC_ERR_XML_BAD_ELEMENT                        (OSPC_ERR_MSG_MODULE + 308)
#define OSPC_ERR_XML_NO_ELEMENT                         (OSPC_ERR_MSG_MODULE + 309)
#define OSPC_ERR_XML_NO_ATTR                            (OSPC_ERR_MSG_MODULE + 310)
#define OSPC_ERR_XML_INVALID_ARGS                       (OSPC_ERR_MSG_MODULE + 311)
#define OSPC_ERR_XML_BUFR_NEW_FAILED                    (OSPC_ERR_MSG_MODULE + 312)
#define OSPC_ERR_XML_BFR_SZ_FAIL                        (OSPC_ERR_MSG_MODULE + 313)
#define OSPC_ERR_XML_BFR_READ_BLOCK_FAIL                (OSPC_ERR_MSG_MODULE + 314)
#define OSPC_ERR_XML_MALLOC_FAILED                      (OSPC_ERR_MSG_MODULE + 315)
#define OSPC_ERR_XML_INVALID_TYPE                       (OSPC_ERR_MSG_MODULE + 316)
#define OSPC_ERR_XML_PARENT_NOT_FOUND                   (OSPC_ERR_MSG_MODULE + 317)
#define OSPC_ERR_XML_CHILD_NOT_FOUND                    (OSPC_ERR_MSG_MODULE + 318)
#define OSPC_ERR_XML_DATA_TYPE_NOT_FOUND                (OSPC_ERR_MSG_MODULE + 319)
#define OSPC_ERR_XML_BFR_WRITE_BLOCK_FAILED             (OSPC_ERR_MSG_MODULE + 320)

#define OSPC_ERR_DATA_NOCALLID                          (OSPC_ERR_MSG_MODULE + 410)
#define OSPC_ERR_DATA_NO_TOKEN                          (OSPC_ERR_MSG_MODULE + 415)
#define OSPC_ERR_DATA_BAD_NUMBER                        (OSPC_ERR_MSG_MODULE + 420)
#define OSPC_ERR_DATA_NO_DEST                           (OSPC_ERR_MSG_MODULE + 425)
#define OSPC_ERR_DATA_NO_USAGEIND                       (OSPC_ERR_MSG_MODULE + 430)
#define OSPC_ERR_DATA_NO_STATUS                         (OSPC_ERR_MSG_MODULE + 435)
#define OSPC_ERR_DATA_NO_USAGECNF                       (OSPC_ERR_MSG_MODULE + 440)
#define OSPC_ERR_DATA_NO_AUTHIND                        (OSPC_ERR_MSG_MODULE + 445)
#define OSPC_ERR_DATA_NO_AUTHREQ                        (OSPC_ERR_MSG_MODULE + 450)
#define OSPC_ERR_DATA_NO_AUTHRSP                        (OSPC_ERR_MSG_MODULE + 455)
#define OSPC_ERR_DATA_NO_AUTHCNF                        (OSPC_ERR_MSG_MODULE + 460)
#define OSPC_ERR_DATA_NO_REAUTHREQ                      (OSPC_ERR_MSG_MODULE + 465)
#define OSPC_ERR_DATA_NO_REAUTHRSP                      (OSPC_ERR_MSG_MODULE + 470)
#define OSPC_ERR_DATA_INVALID_TYPE                      (OSPC_ERR_MSG_MODULE + 475)
#define OSPC_ERR_DATA_NO_USAGE                          (OSPC_ERR_MSG_MODULE + 480)
#define OSPC_ERR_DATA_NO_TOKENINFO                      (OSPC_ERR_MSG_MODULE + 485)
#define OSPC_ERR_DATA_INVALID                           (OSPC_ERR_MSG_MODULE + 490)
#define OSPC_ERR_DATA_NO_ALTINFO                        (OSPC_ERR_MSG_MODULE + 500)
#define OSPC_ERR_DATA_NO_STATS                          (OSPC_ERR_MSG_MODULE + 510)
#define OSPC_ERR_DATA_NO_DELAY                          (OSPC_ERR_MSG_MODULE + 520)

#define OSPC_ERR_CERT_MALLOC_FAILED                     (OSPC_ERR_MSG_MODULE + 610)
#define OSPC_ERR_ASCII_NO_ELEMENT                       (OSPC_ERR_MSG_MODULE + 620)
#define OSPC_ERR_ASCII_BAD_ELEMENT                      (OSPC_ERR_MSG_MODULE + 621)

#define OSPC_ERR_COMM_MODULE                            14000
#define OSPC_ERR_COMM_INVALID_SIZE                      (OSPC_ERR_COMM_MODULE + 10)
#define OSPC_ERR_COMM_BAD_VALUE                         (OSPC_ERR_COMM_MODULE + 20)
#define OSPC_ERR_COMM_PARSER                            (OSPC_ERR_COMM_MODULE + 30)
#define OSPC_ERR_COMM_NO_MEMORY                         (OSPC_ERR_COMM_MODULE + 40)
#define OSPC_ERR_COMM_INUSE                             (OSPC_ERR_COMM_MODULE + 50)
#define OSPC_ERR_COMM_INVALID_ARG                       (OSPC_ERR_COMM_MODULE + 60)
#define OSPC_ERR_COMM_NO_SVC_PTS                        (OSPC_ERR_COMM_MODULE + 70)
#define OSPC_ERR_COMM_NO_SVC_PTS_AVAIL                  (OSPC_ERR_COMM_MODULE + 80)
#define OSPC_ERR_COMM_THREAD_INIT_FAILED                (OSPC_ERR_COMM_MODULE + 85)

#define OSPC_ERR_MSGQ_NO_MEMORY                         (OSPC_ERR_COMM_MODULE + 110)

#define OSPC_ERR_HTTP_BAD_HEADER_100                    (OSPC_ERR_COMM_MODULE + 210)
#define OSPC_ERR_HTTP_BAD_HEADER_200                    (OSPC_ERR_COMM_MODULE + 220)
#define OSPC_ERR_HTTP_BAD_QUEUE                         (OSPC_ERR_COMM_MODULE + 230)
#define OSPC_ERR_HTTP_BAD_MESSAGE                       (OSPC_ERR_COMM_MODULE + 240)
#define OSPC_ERR_HTTP_INVALID_ARG                       (OSPC_ERR_COMM_MODULE + 250)
#define OSPC_ERR_HTTP_MALLOC_FAILED                     (OSPC_ERR_COMM_MODULE + 260)
#define OSPC_ERR_HTTP_NEWCONN_FAILED                    (OSPC_ERR_COMM_MODULE + 270)
#define OSPC_ERR_HTTP_CONN_SRCH_TIMEOUT                 (OSPC_ERR_COMM_MODULE + 275)
#define OSPC_ERR_HTTP_SERVER_ERROR                      (OSPC_ERR_COMM_MODULE + 280)
#define OSPC_ERR_HTTP_SHUTDOWN                          (OSPC_ERR_COMM_MODULE + 290)
#define OSPC_ERR_HTTP_SSL_NEW_FAILED                    (OSPC_ERR_COMM_MODULE + 292)
#define OSPC_ERR_HTTP_SSL_CTX_NEW_FAILED                (OSPC_ERR_COMM_MODULE + 295)
#define OSPC_ERR_HTTP_UNAUTHORIZED                      (OSPC_ERR_COMM_MODULE + 296)
#define OSPC_ERR_HTTP_SERVICE_UNAVAILABLE               (OSPC_ERR_COMM_MODULE + 297)
#define OSPC_ERR_HTTP_BAD_REQUEST                       (OSPC_ERR_COMM_MODULE + 298)
#define OSPC_ERR_HTTP_NOT_FOUND                         (OSPC_ERR_COMM_MODULE + 299)

#define OSPC_ERR_SOCK_SELECT_FAILED                     (OSPC_ERR_COMM_MODULE + 300)
#define OSPC_ERR_SOCK_RECV_FAILED                       (OSPC_ERR_COMM_MODULE + 310)
#define OSPC_ERR_SOCK_SEND_FAILED                       (OSPC_ERR_COMM_MODULE + 315)
#define OSPC_ERR_SOCK_RECVBUF_MALLOC_FAILED             (OSPC_ERR_COMM_MODULE + 320)
#define OSPC_ERR_SOCK_RESET                             (OSPC_ERR_COMM_MODULE + 321)
#define OSPC_ERR_SOCK_CREATE_FAILED                     (OSPC_ERR_COMM_MODULE + 330)
#define OSPC_ERR_SOCK_CLOSE_FAILED                      (OSPC_ERR_COMM_MODULE + 340)
#define OSPC_ERR_SOCK_CONNECT_FAILED                    (OSPC_ERR_COMM_MODULE + 350)
#define OSPC_ERR_SOCK_BLOCKIO_FAILED                    (OSPC_ERR_COMM_MODULE + 360)
#define OSPC_ERR_SOCK_DISABLE_NAGLE_FAILED              (OSPC_ERR_COMM_MODULE + 370)
#define OSPC_ERR_SOCK_CONN_IN_PROGRESS                  (OSPC_ERR_COMM_MODULE + 380)

#define OSPC_ERR_SSL_MALLOC_FAILED                      (OSPC_ERR_COMM_MODULE + 400)
#define OSPC_ERR_SSL_INIT_CTX_FAILED                    (OSPC_ERR_COMM_MODULE + 410)
#define OSPC_ERR_SSL_VERSION_FAILED                     (OSPC_ERR_COMM_MODULE + 420)
#define OSPC_ERR_SSL_INIT_SESSION_FAILED                (OSPC_ERR_COMM_MODULE + 430)
#define OSPC_ERR_SSL_ATTACH_SOCK_FAILED                 (OSPC_ERR_COMM_MODULE + 440)
#define OSPC_ERR_SSL_HANDSHAKE_FAILED                   (OSPC_ERR_COMM_MODULE + 450)
#define OSPC_ERR_SSL_CLOSE_FAILED                       (OSPC_ERR_COMM_MODULE + 460)
#define OSPC_ERR_SSL_READ_FAILED                        (OSPC_ERR_COMM_MODULE + 470)
#define OSPC_ERR_SSL_WRITE_FAILED                       (OSPC_ERR_COMM_MODULE + 480)
#define OSPC_ERR_SSL_GETCERT_FAILED                     (OSPC_ERR_COMM_MODULE + 490)
#define OSPC_ERR_SSL_NO_ROOT_CA                         (OSPC_ERR_COMM_MODULE + 495)
#define OSPC_ERR_SSL_SET_PRIVKEY_FAILED                 (OSPC_ERR_COMM_MODULE + 496)
#define OSPC_ERR_SSL_PARSE_PRIVKEY_FAILED               (OSPC_ERR_COMM_MODULE + 497)
#define OSPC_ERR_SSL_ADD_CERTS_FAILED                   (OSPC_ERR_COMM_MODULE + 498)
#define OSPC_ERR_SSL_ADD_DN_FAILED                      (OSPC_ERR_COMM_MODULE + 499)

#define OSPC_ERR_UTIL_MODULE                            15400
#define OSPC_ERR_UTIL_NOT_ENOUGH_SPACE_FOR_COPY         (OSPC_ERR_UTIL_MODULE + 10)
#define OSPC_ERR_UTIL_NO_TIME_STAMP_CREATED             (OSPC_ERR_UTIL_MODULE + 20)
#define OSPC_ERR_UTIL_VALUE_NOT_FOUND                   (OSPC_ERR_UTIL_MODULE + 30)
#define OSPC_ERR_UTIL_MALLOC_FAILED                     (OSPC_ERR_UTIL_MODULE + 40)
#define OSPC_ERR_UTIL_INVALID_ARG                       (OSPC_ERR_UTIL_MODULE + 50)

#define OSPC_ERR_BUF_EMPTY                              (OSPC_ERR_UTIL_MODULE + 100)
#define OSPC_ERR_BUF_INCOMPLETE                         (OSPC_ERR_UTIL_MODULE + 110)

#define OSPC_ERR_FAILRSN_MODULE                         15600
#define OSPC_ERR_FAILRSN_NO_DATA                        (OSPC_ERR_FAILRSN_MODULE + 10)
#define OSPC_ERR_FAILRSN_INVALID                        (OSPC_ERR_FAILRSN_MODULE + 20)

#define OSPC_ERR_POW                                    15980
#define OSPC_ERR_OS_CONDVAR_TIMEOUT                     15990

#define OSPC_ERR_X509_MODULE                            16000
#define OSPC_ERR_X509_SERIAL_NUMBER_UNDEFINED           (OSPC_ERR_X509_MODULE + 10)
#define OSPC_ERR_X509_CERTIFICATE_UNDEFINED             (OSPC_ERR_X509_MODULE + 20)
#define OSPC_ERR_X509_INVALID_CONTEXT                   (OSPC_ERR_X509_MODULE + 30)
#define OSPC_ERR_X509_DECODING_ERROR                    (OSPC_ERR_X509_MODULE + 40)
#define OSPC_ERR_X509_UNABLE_TO_ALLOCATE_SPACE          (OSPC_ERR_X509_MODULE + 50)
#define OSPC_ERR_X509_INVALID_DATE                      (OSPC_ERR_X509_MODULE + 60)
#define OSPC_ERR_X509_CERTIFICATE_EXPIRED               (OSPC_ERR_X509_MODULE + 70)
#define OSPC_ERR_X509_CA_NOT_FOUND                      (OSPC_ERR_X509_MODULE + 80)
#define OSPC_ERR_X509_STORE_ERROR                       (OSPC_ERR_X509_MODULE + 90)

#define OSPC_ERR_PKCS1_MODULE                           17000
#define OSPC_ERR_PKCS1_INVALID_PRIVATE_KEY_POINTER      (OSPC_ERR_PKCS1_MODULE + 10)
#define OSPC_ERR_PKCS1_UNABLE_TO_ALLOCATE_SPACE         (OSPC_ERR_PKCS1_MODULE + 20)
#define OSPC_ERR_PKCS1_INVALID_CONTEXT                  (OSPC_ERR_PKCS1_MODULE + 30)
#define OSPC_ERR_PKCS1_NULL_POINTER                     (OSPC_ERR_PKCS1_MODULE + 40)
#define OSPC_ERR_PKCS1_PRIVATE_KEY_OVERFLOW             (OSPC_ERR_PKCS1_MODULE + 50)

#define OSPC_ERR_PKCS7_MODULE                           18000
#define OSPC_ERR_PKCS7_SIGNER_MISSING                   (OSPC_ERR_PKCS7_MODULE + 10)
#define OSPC_ERR_PKCS7_INVALID_SIGNATURE                (OSPC_ERR_PKCS7_MODULE + 20)
#define OSPC_ERR_PKCS7_UNABLE_TO_ALLOCATE_SPACE         (OSPC_ERR_PKCS7_MODULE + 21)
#define OSPC_ERR_PKCS7_ENCODING_ERROR                   (OSPC_ERR_PKCS7_MODULE + 30)
#define OSPC_ERR_PKCS7_INVALID_POINTER                  (OSPC_ERR_PKCS7_MODULE + 40)
#define OSPC_ERR_PKCS7_BUFFER_OVERFLOW                  (OSPC_ERR_PKCS7_MODULE + 50)

#define OSPC_ERR_ASN1_MODULE                            19000
#define OSPC_ERR_ASN1_NULL_POINTER                      (OSPC_ERR_ASN1_MODULE + 10)
#define OSPC_ERR_ASN1_INVALID_ELEMENT_TAG               (OSPC_ERR_ASN1_MODULE + 20)
#define OSPC_ERR_ASN1_UNEXPECTED_HIGH_TAG               (OSPC_ERR_ASN1_MODULE + 30)
#define OSPC_ERR_ASN1_INVALID_PRIMITIVE_TAG             (OSPC_ERR_ASN1_MODULE + 40)
#define OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE          (OSPC_ERR_ASN1_MODULE + 50)
#define OSPC_ERR_ASN1_INVALID_CONTEXT                   (OSPC_ERR_ASN1_MODULE + 60)
#define OSPC_ERR_ASN1_INVALID_TIME                      (OSPC_ERR_ASN1_MODULE + 70)
#define OSPC_ERR_ASN1_PARSE_ERROR                       (OSPC_ERR_ASN1_MODULE + 80)
#define OSPC_ERR_ASN1_PARSE_COMPLETE                    (OSPC_ERR_ASN1_MODULE + 90)
#define OSPC_ERR_ASN1_PARSE_DEFAULTED                   (OSPC_ERR_ASN1_MODULE +100)
#define OSPC_ERR_ASN1_LENGTH_OVERFLOW                   (OSPC_ERR_ASN1_MODULE +110)
#define OSPC_ERR_ASN1_UNSUPPORTED_TAG                   (OSPC_ERR_ASN1_MODULE +120)
#define OSPC_ERR_ASN1_OBJECTID_NOT_FOUND                (OSPC_ERR_ASN1_MODULE +130)
#define OSPC_ERR_ASN1_OBJECTID_MISMATCH                 (OSPC_ERR_ASN1_MODULE +140)
#define OSPC_ERR_ASN1_UNEXPECTED_INT_BASE               (OSPC_ERR_ASN1_MODULE +150)
#define OSPC_ERR_ASN1_BUFFER_OVERFLOW                   (OSPC_ERR_ASN1_MODULE +160)
#define OSPC_ERR_ASN1_INVALID_DATAREFID                 (OSPC_ERR_ASN1_MODULE +170)
#define OSPC_ERR_ASN1_NO_CONTENT_VALUE_FOR_ELEMENT      (OSPC_ERR_ASN1_MODULE +180)
#define OSPC_ERR_ASN1_INTEGER_OVERFLOW                  (OSPC_ERR_ASN1_MODULE +190)

#define OSPC_ERR_CRYPTO_MODULE                          20000
#define OSPC_ERR_CRYPTO_INVALID_PARAMETERS              (OSPC_ERR_CRYPTO_MODULE + 10)
#define OSPC_ERR_CRYPTO_UNABLE_TO_ALLOCATE_SPACE        (OSPC_ERR_CRYPTO_MODULE + 20)
#define OSPC_ERR_CRYPTO_SIGNATURE_DID_NOT_VERIFY        (OSPC_ERR_CRYPTO_MODULE + 30)
#define OSPC_ERR_CRYPTO_IMPLEMENTATION_SPECIFIC_ERROR   (OSPC_ERR_CRYPTO_MODULE + 40)
                                      
#define OSPC_ERR_CRYPTO_INVALID_POINTER                 (OSPC_ERR_CRYPTO_MODULE + 50)
#define OSPC_ERR_CRYPTO_NOT_ENOUGH_SPACE                (OSPC_ERR_CRYPTO_MODULE + 60)
#define OSPC_ERR_CRYPTO_FILE_OPEN_ERROR                 (OSPC_ERR_CRYPTO_MODULE + 70)
#define OSPC_ERR_CRYPTO_FILE_LOAD_ERROR                 (OSPC_ERR_CRYPTO_MODULE + 80)
#define OSPC_ERR_CRYPTO_FILE_PARSE                      (OSPC_ERR_CRYPTO_MODULE + 90)

#define OSPC_ERR_PKCS8_MODULE                           21000
#define OSPC_ERR_PKCS8_INVALID_PRIVATE_KEY_POINTER      (OSPC_ERR_PKCS1_MODULE + 10)
#define OSPC_ERR_PKCS8_UNABLE_TO_ALLOCATE_SPACE         (OSPC_ERR_PKCS1_MODULE + 20)
#define OSPC_ERR_PKCS8_INVALID_CONTEXT                  (OSPC_ERR_PKCS1_MODULE + 30)
#define OSPC_ERR_PKCS8_NULL_POINTER                     (OSPC_ERR_PKCS1_MODULE + 40)
#define OSPC_ERR_PKCS8_PRIVATE_KEY_OVERFLOW             (OSPC_ERR_PKCS1_MODULE + 50)

#define OSPC_ERR_B64_MODULE                             22000
#define OSPC_ERR_B64_ENCODE_FAILED                      (OSPC_ERR_B64_MODULE+10)
#define OSPC_ERR_B64_DECODE_FAILED                      (OSPC_ERR_B64_MODULE+20)

#define OSPC_ERR_AUDIT_MODULE                           22500
#define OSPC_ERR_AUDIT_MALLOC_FAILED                    (OSPC_ERR_AUDIT_MODULE+10)
#define OSPC_ERR_AUDIT_NOT_FOUND                        (OSPC_ERR_AUDIT_MODULE+20)
#define OSPC_ERR_AUDIT_THREAD_INIT_FAILED               (OSPC_ERR_AUDIT_MODULE+30)
#define OSPC_ERR_AUDIT_DUMP_FAILED                      (OSPC_ERR_AUDIT_MODULE+40)

#define OSPC_ERR_TRANSID_MODULE                         23000
#define OSPC_ERR_TRANSID                                (OSPC_ERR_TRANSID_MODULE + 10)

#endif /* _OSPERRNO_H */
