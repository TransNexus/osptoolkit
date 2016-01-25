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

/* ospmime.h - includes for mime */
#ifndef _OSPMIME_H
#define _OSPMIME_H

#include "osp/osp.h"

#define _WIN32_WINNT    0x0400

#define OSPC_HTTP_MULTI_HEADER      "Connection: Keep-Alive\r\nContent-Type: multipart/signed; protocol=\"application/pkcs7-signature\"; micalg=sha1; boundary=bar\r\nContent-Length: "
#define OSPC_MIME_MSG_HEADER        "Content-Type: text/plain\r\nContent-Length: "
#define OSPC_MIME_SIG_HEADER        "Content-Type: application/pkcs7-signature\r\nContent-Length: "
#define OSPC_MIME_BOUNDARY_DELIM     "--"
#define OSPC_MIME_BOUNDARY          "bar"
#define OSPC_MIME_EOL               "\n"
#define OSPC_MIME_CRLF              "\r\n"

#define OSPC_MAXLINE                76
#define OSPC_MAX_FIELDS             32
#define OSPC_MAX_BODYPARTS          2
#define OSPC_MAX_PARAMS             10

#define OSPC_MAIN                   0x77
#define OSPC_BODY                   0x88
#define OSPC_SIGNATURE              0x99

typedef struct _OSPTMIMEPART {
    unsigned Length;
    unsigned char *Content;
} OSPTMIMEPART;

typedef struct _OSPTMIMEPARAM {
    OSPTMIMEPART *ParamName;
    OSPTMIMEPART *ParamValue;
} OSPTMIMEPARAM;

typedef struct _OSPTMIMEFIELD {
    OSPTMIMEPART FieldName;
    OSPTMIMEPART FieldBody;
    OSPTMIMEPARAM *Params[OSPC_MAX_PARAMS];
    unsigned NumParams;
} OSPTMIMEFIELD;

typedef struct _OSPTMIMEHEADER {
    unsigned HeaderLen;
    OSPTMIMEFIELD *Fields[OSPC_MAX_FIELDS];
    unsigned NumFields;
} OSPTMIMEHEADER;

typedef struct _OSPTMIMEBODY {
    unsigned BodyLen;
    OSPTMIMEPART *BodyParts[OSPC_MAX_BODYPARTS];
    unsigned NumBodyParts;
    OSPTMIMEPART *Boundary;
} OSPTMIMEBODY;

typedef struct _OSPTMIMEMSG {
    unsigned Length;
    int NumParts;
    OSPTMIMEPART *MsgPart;
    OSPTMIMEPART *SigPart;
} OSPTMIMEMSG;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void OSPPMimeBodyFree(OSPTMIMEBODY *);
    int OSPPMimeBodyPartsParse(OSPTMIMEPART *, OSPTMIMEBODY *, OSPTMIMEPART *);
    int OSPPMimeBufferParse(unsigned char *, unsigned, OSPTMIMEPART *, OSPTMIMEPART *);
    void OSPPMimeDataFree(OSPTMIMEMSG *);
    void OSPPMimeFieldFree(OSPTMIMEFIELD *);
    int OSPPMimeMessageBuild(OSPTMIMEMSG *, unsigned char **, unsigned *);
    int OSPPMimeMessageCreate(unsigned char *, unsigned, unsigned char *, unsigned, unsigned char **, unsigned *);
    int OSPPMimeMessageInit(OSPTMIMEMSG *);
    int OSPPMimeMessageParse(unsigned char *, unsigned, unsigned char *, unsigned, unsigned char **, unsigned *, unsigned char **, unsigned *);
    int OSPPMimeMessageSetContentAndLength(OSPTMIMEPART *, unsigned char *, unsigned);
    void OSPPMimeParamFree(OSPTMIMEPARAM *);
    int OSPPMimeParamsParse(OSPTMIMEFIELD *);
    void OSPPMimePartFree(OSPTMIMEPART *);
    int OSPPMimeVerifyParameters(OSPTMIMEFIELD *, OSPTMIMEPART *, int);
    int OSPPUtilMallocAndCopySubString(unsigned char *, unsigned char **, unsigned, unsigned);
    int OSPPUtilMemCaseCmp(unsigned char *, unsigned, char *, unsigned, int *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPMIME_H */
