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

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospb64.h"

/*** Constants ***/
#define B64FILL '='

/*** Local data ***/
static const unsigned char B64CharTab[64 + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";

int OSPPBase64Encode(
    const unsigned char *in, 
    unsigned inSize, 
    unsigned char *out, 
    unsigned *outSize)
{
    int Fretval;
    size_t x;
    size_t rem;

    Fretval = OSPC_ERR_B64_ENCODE_FAILED;
    if ((*outSize >= inSize * 2) && (*outSize >= 4) && (inSize > 0) && (in != OSPC_OSNULL) && (out != OSPC_OSNULL)) {
        *outSize = 0;
        Fretval = OSPC_ERR_NO_ERROR;
        for (x = 0; x < (inSize / 3); x++) {
            *out++ = B64CharTab[*in >> 2];
            *out++ = B64CharTab[((*in & 0x03) << 4) + ((*(in + 1)) >> 4)];
            in++;
            *out++ = B64CharTab[((*in & 0x0F) << 2) + (*(in + 1) >> 6)];
            in++;
            *out++ = B64CharTab[(*in++ & 0x3F)];
            (*outSize) += 4;
        }
        if ((rem = inSize % 3) == 1) {
            *out++ = B64CharTab[(*in >> 2)];
            *out++ = B64CharTab[((*in & 0x03) << 4)];
            *out++ = B64FILL;
            *out = B64FILL;
            (*outSize) += 4;
        } else if (rem == 2) {
            *out++ = B64CharTab[(*in >> 2)];
            *out++ = B64CharTab[((*in & 0x03) << 4) + (*(in + 1) >> 4)];
            in++;
            *out++ = B64CharTab[(*in & 0x0F) << 2];
            *out = B64FILL;
            (*outSize) += 4;
        }
    }

    return Fretval;
}   /* OSPPBase64Encode */

/* Ignores embedded whitespace, terminates on first '=' regardless of its position */
int OSPPBase64Decode(
    const char *in, 
    unsigned inSize, 
    unsigned char *out, 
    unsigned *outSize)
{
    int Fretval;
    int state;
    char c;
    unsigned char *ix64;
    size_t x;
    size_t b64;

    Fretval = OSPC_ERR_B64_DECODE_FAILED;
    for (x = inSize - 1; in[x] == '='; x--);
    b64 = (x + 1) - ((x + 1) / 4);
    if ((in != OSPC_OSNULL) && (out != OSPC_OSNULL) && (inSize > 0) && (outSize != OSPC_OSNULL) && (*outSize >= b64)) {
        Fretval = OSPC_ERR_NO_ERROR;
        *outSize = 0;
        state = 0;
        c = 0;
        for (x = 0; (x < inSize) && (Fretval == OSPC_ERR_NO_ERROR); x++) {
            c = in[x];
            if (!isspace((int) c)) {
                if ((ix64 = (unsigned char *) strchr((char *) B64CharTab, c)) != NULL) {
                    if ((ix64 >= B64CharTab) && (ix64 < (B64CharTab + 64))) {
                        b64 = ix64 - B64CharTab;
                        switch (state) {
                        case 0:
                            out[(*outSize)] = (unsigned char) (b64 << 2);
                            state = 1;
                            break;

                        case 1:
                            out[(*outSize)++] |= (unsigned char) (b64 >> 4);
                            out[(*outSize)] = (unsigned char) ((b64 & 0x0F) << 4);
                            state = 2;
                            break;

                        case 2:
                            out[(*outSize)++] |= (b64 >> 2);
                            out[(*outSize)] = (unsigned char) ((b64 & 0x03) << 6);
                            state = 3;
                            break;

                        case 3:
                            out[(*outSize)++] |= b64;
                            state = 0;
                            break;

                        default:
                            Fretval = OSPC_ERR_B64_DECODE_FAILED;
                            break;
                        }
                    } else {    /* Unexpected condition */
                        Fretval = OSPC_ERR_B64_DECODE_FAILED;
                    }
                } else {        /* '=' or garbage */
                    if (c != B64FILL) {
                        Fretval = OSPC_ERR_B64_DECODE_FAILED;
                    }
                    break;
                }
            }   /* Else: ignore whitespace */
        }
    }

    return Fretval;
}   /* OSPPBase64Decode */


/* End of file */
