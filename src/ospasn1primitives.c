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
 * ospasn1primitives.c - ASN1 Primitive encode/decode functions 
 */

#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/osptnlog.h"

/*
 * Member functions
 */

/*****************************************
    GENERAL PRIMITIVE ROUTINES 
*****************************************/
#ifdef PRIMITIVEDECODE
int OSPPASN1PrimitiveDecode(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    int errorcode = OSPC_ERR_NO_ERROR;
    char msg[100];
    void *contentValue = OSPC_OSNULL;

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        eInfo = ospvElementInfo;

        sprintf(msg, "PrimitiveDecode - Tag=%0x", eInfo->Tag);  

        /* Decode the item and save the value */
        switch (OSPM_BASE_TAG(eInfo->Tag)) {
        case OSPC_TAG_TYPE_EOC:
            break;

        case OSPC_TAG_TYPE_NULL:
            break;

        case OSPC_TAG_TYPE_INTEGER:
            errorcode = OSPPASN1IntegerDecode((OSPTASN1INTEGER**)&contentValue, eInfo->Content, eInfo->ContentLength);
            break;

        case OSPC_TAG_TYPE_OBJECT_IDENTIFIER:
            errorcode = OSPPASN1ObjectIdentifierDecode((OSPTASN1BUFFER **)&contentValue, eInfo->Content, eInfo->ContentLength);
            break;

        case OSPC_TAG_TYPE_IA5STRING:
        case OSPC_TAG_TYPE_T61STRING:
        case OSPC_TAG_TYPE_PRINTABLESTRING:
        case OSPC_TAG_TYPE_BMPSTRING:
            errorcode = OSPPASN1PrintableStringDecode((OSPTASN1BUFFER **)&contentValue, eInfo->Content, eInfo->ContentLength);
            break;

        case OSPC_TAG_TYPE_UTCTIME:
            errorcode = OSPPASN1UTCTimeDecode((OSPTASN1BUFFER **)&contentValue, eInfo->Content, eInfo->ContentLength);
            break;

        case OSPC_TAG_TYPE_BIT_STRING:
            errorcode = OSPPASN1BitStringDecode(OSPTASN1BITSTRING **)&contentValue, eInfo->Content, eInfo->ContentLength);
            break;

        case OSPC_TAG_TYPE_OCTET_STRING:
            errorcode = OSPPASN1OctetStringDecode((OSPTASN1BUFFER**)&contentValue, eInfo->Content, eInfo->ContentLength);
            break;

        case OSPC_TAG_TYPE_SEQUENCE:
        case OSPC_TAG_TYPE_SET:
        default:
            errorcode = OSPC_ERR_ASN1_INVALID_PRIMITIVE_TAG;
            sprintf(msg, "Invalid/Unsupported primitive tag %02x", eInfo->Tag);
            OSPM_DBGERRORLOG(errorcode, msg);
            break;
        }
    }

    return errorcode;
}

int OSPPASN1PrimitiveDelete(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    int errorcode = OSPC_ERR_NO_ERROR;
    char msg[100];

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        eInfo = ospvElementInfo;

        if (eInfo->ContentValue != OSPC_OSNULL) {
            /* Decode the item and save the value */
            switch (OSPM_BASE_TAG(eInfo->Tag)) {
            case OSPC_TAG_TYPE_EOC:
            case OSPC_TAG_TYPE_NULL:
                break;

            case OSPC_TAG_TYPE_INTEGER:
                OSPPASN1IntegerDelete((OSPTASN1INTEGER **)&contentValue);
                break;

            case OSPC_TAG_TYPE_OBJECT_IDENTIFIER:
                OSPPASN1ObjectIdentifierDelete((OSPTASN1OBJECTID **)&contentValue);
                break;

            case OSPC_TAG_TYPE_PRINTABLESTRING:
                OSPPASN1BufferDelete((OSPTASN1BUFFER **)&contentValue));
                break;

            case OSPC_TAG_TYPE_UTCTIME:
                OSPPASN1BufferDelete((OSPTASN1BUFFER **)&contentValue);
                break;

            case OSPC_TAG_TYPE_BIT_STRING:
                OSPPASN1BitStringDelete((OSPTASN1BITSTRING**)&contentValue));
                break;

            case OSPC_TAG_TYPE_OCTET_STRING:
                OSPPASN1BufferDelete((OSPTASN1BUFFER**)&contentValue)); 
                break;

            case OSPC_TAG_TYPE_SEQUENCE:
            case OSPC_TAG_TYPE_SET:
            default:
                errorcode = OSPC_ERR_ASN1_INVALID_PRIMITIVE_TAG;
                sprintf(msg, "Invalid/Unsupported primitive tag %02x", eInfo->Tag);
                OSPM_DBGERRORLOG(errorcode, msg);
                break;
            }
        }
    }

    return errorcode;
}

/*
 * BUFFER ROUTINES
 */
int OSPPASN1BufferCreate(
    unsigned char *ospvContent,
    unsigned ospvContentLength,
    OSPTASN1BUFFER **ospvBuffer)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /* Allocate space for buffer structure */
    OSPM_MALLOC(*ospvBuffer, OSPTASN1BUFFER, sizeof(OSPTASN1BUFFER));
    if (*ospvBuffer == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate an ASN1 BUFFER");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvContentLength != 0) {
            /* Allocate space for buffer contents */
            OSPM_MALLOC((*ospvBuffer)->Buffer, unsigned char, ospvContentLength);
            if ((*ospvBuffer)->Buffer == OSPC_OSNULL) {
                errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
                OSPM_DBGERRORLOG(errorcode, 
                    "Unable to allocate space for buffer content");
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvContentLength != 0) {
            OSPM_MEMCPY((*ospvBuffer)->Buffer, ospvContent, ospvContentLength);
        }
        (*ospvBuffer)->BufferLength = ospvContentLength;
    }

    return errorcode;
}

void OSPPASN1BufferDelete(
    OSPTASN1BUFFER **ospvBuffer)
{
    /* Make sure the buffer exists */
    if ((ospvBuffer != OSPC_NULL) && (*ospvBuffer != OSPC_OSNULL) && ((*ospvBuffer)->Buffer != OSPC_OSNULL)) {
        /* Delete the buffer data */
        OSPM_FREE((*ospvBuffer)->Buffer);
    }

    /* Delete the buffer structure */
    OSPM_FREE(*ospvBuffer);
}

/*
 * OCTET STRING ROUTINES 
 */
int OSPPASN1OctetStringDecode(
    OSPTASN1BUFFER **ospvOctetStringBUffer,
    unsigned char *ospvEncodedData,
    unsigned int ospvEncodedDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvOctetStringBuffer == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_PARAMETER;
        OSPM_DBGERRORLOG(errorcode, "Null pointer supplied for return value");
    } else if ((ospvEncodedData != OSPC_OSNULL) && (ospvEncodedDataLength == 0)) {
        errorcode = OSPPASN1BufferCreate(ospvEncodedData, ospvEncodedDataLength, ospvOctetStringBuffer);
    }

    return errorcode;
}

/*
 * INTEGER ROUTINES 
 */
int OSPPASN1IntegerDecode(
    void **osvpContentValue,
    unsigned char *ospvEncodedData,
    unsigned int  ospvEncodedDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvContentValue == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_PARAMETER;
        OSPM_DBGERRORLOG(errorcode, "Null pointer supplied for return value");
    } else if ((ospvEncodedData != OSPC_OSNULL) && (ospvEncodedDataLength == 0)) {
        errorcode =OSPPASN1IntegerCreate(ospvEncodedData, ospvEncodedDataLength,
            (OSPTASN1INTEGER **)ospvContentValue);
    }
    return errorcode;
}

void OSPPASN1IntegerDelete(
    OSPTASN1INTEGER **ospvInteger)
{
    if (*ospvInteger != OSPC_OSNULL) {
        if ((*ospvInteger)->Digits != OSPC_OSNULL) {
            OSPM_FREE((*ospvInteger)->Digits);
        }

        if ((*ospvInteger)->SmallValue != OSPC_OSNULL) {
            OSPM_FREE((*ospvInteger)->SmallValue);
        }

        OSPM_FREE(*ospvInteger);
    }
}

int OSPPASN1IntegerCreate(
    unsigned char *ospvContent,
    unsigned int ospvContentLength,
    OSPTASN1INTEGER **ospvInteger)
{
    OSPTASN1INTEGER *intBuf=OSPC_OSNULL;
    int smallValue = 0;
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned int i;

    OSPM_MALLOC(intBuf, OSPTASN1INTEGER, sizeof(OSPTASN1INTEGER));
    if (intBuf == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate integer buffer");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Set up Integer Buffer */
        OSPM_MALLOC(intBuf->Digits, unsigned char, ospvContentLength);
        if (intBuf->Digits == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate integer buffer");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MEMCPY(intBuf->Digits, ospvContent, ospvContentLength);
        intBuf->DigitCount = ospvContentLength;

        if ((intBuf->Digits[0] & 0x8f) == 0) {
            for (i = 0 ; (i < intBuf->DigitCount) && (smallValue <= 0x7FFFFF) ; i++) {
                smallValue *= 256;
                smallValue += intBuf->Digits[i];
            }

            if (i == intBuf->DigitCount) {
                OSPM_MALLOC(intBuf->SmallValue, int, sizeof(int));
                if (intBuf->SmallValue == OSPC_OSNULL) {
                    errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
                    OSPM_DBGERRORLOG(errorcode,
                        "Unable to allocate integer smallvalue");
                } else {
                    *(intBuf->SmallValue) = smallValue;
                }
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvInteger = intBuf;
    } else {
        OSPPASN1IntegerDelete(&intBuf);     
    }

    return errorcode;
}

int  OSPPASN1IntegerEncode(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1INTEGER *intBuf;

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        eInfo = ospvElementInfo;
        intBuf = (OSPTASN1INTEGER *)eInfo->ContentValue;
        eInfo->Content = intBuf->Digits;
        eInfo->ContentLength = intBuf->DigitCount;
    }

    return errorcode;
}

/*
 * OBJECT IDENTIFIER ROUTINES 
 */
int  OSPPASN1ObjectIdentifierDecode(
    void **osvpContentValue,
    unsigned char *ospvEncodedData,
    unsigned int  ospvEncodedDataLength)
{
    OSPTASN1ELEMENTINFO *eInfo;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvContentValue == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_PARAMETER;
        OSPM_DBGERRORLOG(errorcode, "Null pointer supplied for return value");
    } else if ((ospvEncodedData != OSPC_OSNULL) && (ospvEncodedDataLength == 0)) {
        errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            eInfo = ospvElementInfo;
            errorcode = OSPPASN1BufferCreate(eInfo->Content, eInfo->ContentLength, (OSPTASN1BUFFER **)&(eInfo->ContentValue));
        }
   }

   return errorcode;
}

/*
 * PRINTABLE STRING ROUTINES 
 */
int OSPPASN1PrintableStringEncode(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    OSPTASN1BUFFER *buffer;
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        eInfo = ospvElementInfo;
        buffer = (OSPTASN1BUFFER *)eInfo->ContentValue;

        eInfo->Content = buffer->Buffer; 
        eInfo->ContentLength = buffer->BufferLength;
    }

    return errorcode;
}

int OSPPASN1PrintableStringDecode(
    void **osvpContentValue,
    unsigned char *ospvEncodedData,
    unsigned int  ospvEncodedDataLength)
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvContentValue == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_PARAMETER;
        OSPM_DBGERRORLOG(errorcode, "Null pointer supplied for return value");
    } else if ((ospvEncodedData != OSPC_OSNULL) && (ospvEncodedDataLength == 0)) {
        errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            eInfo = ospvElementInfo;
            errorcode = OSPPASN1BufferCreate(eInfo->Content, eInfo->ContentLength,
                (OSPTASN1BUFFER **)&(eInfo->ContentValue));
        }
    }

    return errorcode;
}

/*
 * UTCTIME ROUTINES 
 */
int OSPPASN1UTCTimeEncode( 
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    OSPTASN1BUFFER *buffer;
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        eInfo = ospvElementInfo;
        buffer = (OSPTASN1BUFFER *)eInfo->ContentValue;

        eInfo->Content = buffer->Buffer; 
        eInfo->ContentLength = buffer->BufferLength;
    }

    return errorcode;
}

int OSPPASN1UTCTimeDecode(
    void **osvpContentValue,
    unsigned char *ospvEncodedData,
    unsigned int  ospvEncodedDataLength)
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvContentValue == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_PARAMETER;
        OSPM_DBGERRORLOG(errorcode, "Null pointer supplied for return value");
    } else if ((ospvEncodedData != OSPC_OSNULL) && (ospvEncodedDataLength == 0)) {
        errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            eInfo = ospvElementInfo;
            errorcode = OSPPASN1BufferCreate(eInfo->Content, eInfo->ContentLength, (OSPTASN1BUFFER **)&(eInfo->ContentValue));
        }
    }
    return errorcode;
}

/*
 * BIT STRING ROUTINES 
 */
int  OSPPASN1BitStringDecode(
    void **osvpContentValue,
    unsigned char *ospvEncodedData,
    unsigned int  ospvEncodedDataLength)
{
    OSPTASN1ELEMENTINFO *eInfo=OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1BITSTRING *buffer=OSPC_OSNULL;
    unsigned char *cptr=OSPC_OSNULL;
    unsigned unusedBits=0;
    unsigned length = 0;

    if (ospvContentValue == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_PARAMETER;
        OSPM_DBGERRORLOG(errorcode, "Null pointer supplied for return value");
    } else if ((ospvEncodedData != OSPC_OSNULL) && (ospvEncodedDataLength == 0)) {
        errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            eInfo = ospvElementInfo;
            cptr = eInfo->Content;  

            /* First byte is #unused bits in last byte */
            unusedBits = *cptr;
            length = eInfo->ContentLength;

            /* Create the Buffer */
            errorcode = OSPPASN1BitStringCreate(cptr, length, unusedBits, &buffer);
        }
    }

    return errorcode;
}

int OSPPASN1BitStringEncode(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    OSPTASN1ELEMENTINFO *eInfo;
    OSPTASN1BITSTRING *bitString;
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        eInfo = ospvElementInfo;
        bitString = (OSPTASN1BITSTRING *)eInfo->ContentValue;

        eInfo->Content = bitString->Buffer->Buffer; 
        eInfo->ContentLength = bitString->Buffer->BufferLength;
    }

    return errorcode;
}

int OSPPASN1BitStringCreate(
    unsigned char *ospvContent,
    unsigned ospvContentLength,
    unsigned ospvUnusedBits,
    OSPTASN1BITSTRING **ospvBitString)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1BITSTRING *bitString;

    /* Allocate space for buffer structure */
    OSPM_MALLOC(bitString, OSPTASN1BITSTRING, sizeof(OSPTASN1BITSTRING));
    if (bitString == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate an ASN1 BITSTRING");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Create a buffer containing bitstring contents */
        errorcode = OSPPASN1BufferCreate(ospvContent,ospvContentLength,
            &(bitString->Buffer));
    }

    /* Mkae sure buffer got created */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Finish creating bit string buffer */
        bitString->UnusedBits = ospvUnusedBits;
        *ospvBitString = bitString;
    } else {
        /* An error occured, get rid of allocated stuff - don't update  ospvBitString ptr */
        OSPM_FREE(bitString);
    }

    return errorcode;
}

void  OSPPASN1BitStringDelete(
    OSPTASN1BITSTRING **ospvBitString)
{
    /* Make sure the buffer exists */
    if (*ospvBitString != OSPC_OSNULL) {
        /* Make sure storage exists */
        if ((*ospvBitString)->Buffer != OSPC_OSNULL) {
            /* Delete the buffer */
            OSPPASN1BufferDelete(&((*ospvBitString)->Buffer));
        }

        /* Delete the buffer */
        OSPM_FREE(*ospvBitString);
    }
}

#endif

int OSPPASN1OctetStringEncode(
    OSPTASN1OBJECT **ospvEncodedObject, 
    unsigned char *ospvData, 
    unsigned int ospvDataLength, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *encodedObject = OSPC_OSNULL;
    unsigned char tag;

    /* Set up data and dataLength for octetstring - not much to do here */
    tag = OSPC_TAG_TYPE_OCTET_STRING;

    errorcode = OSPPASN1ElementFormat(&eInfo, &tag, OSPC_BER_MODE_PRIMITIVE, 1, ospvData, ospvDataLength);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectFormat(&encodedObject, eInfo, ospvDataRefId);
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPPASN1ElementDelete(&eInfo, 0);
        OSPPASN1ObjectDelete(&encodedObject);
    } else {
        *ospvEncodedObject = encodedObject;
    }

    return errorcode;
}

/* Encodes small integer (represented by unsigned) as an ASN1 Integer.
Only encodes in base 256.  Only encodes unsigned integers.  */
int OSPPASN1SmallIntegerEncode(
    OSPTASN1OBJECT **ospvEncodedObject, 
    unsigned ospvInteger, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    unsigned char tag = 0;
    unsigned char *tmpData = OSPC_OSNULL;
    unsigned tmpLength = 0;

    /* Set up data and dataLength for octetstring - not much to do here */
    /* ...except creating memory leaks */
    tag = OSPC_TAG_TYPE_INTEGER;

    errorcode = OSPPASN1SmallInt2UnsignedChar(ospvInteger, 256, &tmpData, &tmpLength);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectEncode(ospvEncodedObject, &tag, 1, OSPC_BER_MODE_PRIMITIVE, tmpData, tmpLength, ospvDataRefId);
    }
    if (OSPC_OSNULL != tmpData) {
        OSPM_FREE(tmpData);
    }

    return errorcode;
}

int OSPPASN1IntegerGetSmallValue(
    OSPTASN1ELEMENTINFO *ospvElementInfo, 
    int *ospvValue)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *digits = OSPC_OSNULL;
    unsigned int digitCount = 0;
    int smallValue = 0;
    unsigned i = 0;

    errorcode = OSPPASN1ElementTestContext(ospvElementInfo);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvValue == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_NULL_POINTER;
            OSPM_DBGERRORLOG(errorcode, "Invalid pointer to integer storage");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPC_ERR_ASN1_INTEGER_OVERFLOW;
        *ospvValue = 0;
        digits = ospvElementInfo->Content;
        digitCount = ospvElementInfo->ContentLength;

        if ((digits[0] & 0x8f) == 0) {
            for (i = 0; (i < digitCount) && (smallValue <= 0x7FFFFF); i++) {
                smallValue *= 256;
                smallValue += digits[i];
            }

            if (i == digitCount) {
                *ospvValue = smallValue;
            }
        }
    }

    return errorcode;
}

/* 
 * SmallInt2UnsignedChar
 * Converts integer valuses to unsigned char buffer containing the value in
 * base specified.  Unused bits in each byte are set to 1 except for the last
 * byte in the unsigned char value.  Does not support negative numbers. Use
 * for base 256 and base 128 values in ASN1 format.
 */
int OSPPASN1SmallInt2UnsignedChar(
    unsigned ospvIntegerValue, 
    unsigned ospvBase, 
    unsigned char **ospvBuffer, 
    unsigned *ospvBufferLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned ucCnt = 0;
    int i = 0;
    unsigned div = 0;
    unsigned char tmpBuffer[129];
    unsigned char *ucPtr = OSPC_OSNULL;
    unsigned char mask = 0x00;

    div = ospvIntegerValue;

    switch (ospvBase) {
    case 256:
        mask = 0x00;
        break;
    case 128:
        mask = 0x80;
        break;
    default:
        errorcode = OSPC_ERR_ASN1_UNEXPECTED_INT_BASE;
        OSPM_DBGERRORLOG(errorcode, "Base other than 128 or 256 specified");
        break;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Create the unsigned char buffer - backwards */
        ucCnt = 0;
        do {
            tmpBuffer[ucCnt++] = (unsigned char) (div % ospvBase);
            div /= ospvBase;
        } while (div);

        /* Create buffer for data */
        OSPM_MALLOC(*ospvBuffer, unsigned char, ucCnt);
        if (ospvBuffer == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate parse element");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        ucPtr = *ospvBuffer;
        *ospvBufferLength = ucCnt;

        OSPM_MEMSET(ucPtr, mask, ucCnt);
        for (i = (ucCnt - 1); i >= 0; i--, ucPtr++) {
            *ucPtr |= tmpBuffer[i];
        };
        mask = (unsigned char) ~mask;    /* Complement Mask */
        (*ospvBuffer)[ucCnt - 1] &= mask;    /* Apply complement mask */
    }

    return errorcode;
}

int OSPPASN1ObjectIdentifierEncode(
    OSPTASN1OBJECT **ospvEncodedObject, 
    OSPEASN1ID ospvObjectIdentifier, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    unsigned char tag;
    unsigned char *tmpData;
    unsigned tmpLength;

    /* Set up data and dataLength for ObjectIdentifier - not much to do here */
    tag = OSPC_TAG_TYPE_OBJECT_IDENTIFIER;

    errorcode = OSPPASN1IdGetValue(ospvObjectIdentifier, &tmpData, &tmpLength);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectEncode(ospvEncodedObject, &tag, 1, OSPC_BER_MODE_PRIMITIVE, tmpData, tmpLength, ospvDataRefId);
    }

    return errorcode;
}

/*
 * ALGORITHM ID ROUTINES 
 */
int OSPPASN1AlgorithmIdEncode(
    OSPTASN1OBJECT **ospvEncodedObject, 
    OSPEASN1ID ospvAlgorithmId, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1OBJECT *encodedObject = OSPC_OSNULL;
    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;

    int i = 0;

    OSPM_ARGUSED(ospvDataRefId);

    errorcode = OSPPASN1ObjectNew(&encodedObject, OSPEDRID_ALGORITHMID);

    for (i = 0; errorcode == OSPC_ERR_NO_ERROR; i++) {
        switch (i) {
        case 0: /* Create Digest Algorithm OID */
            dataRefId = OSPEDRID_ALGID_OID;
            errorcode = OSPPASN1ObjectIdentifierEncode(&newObject, ospvAlgorithmId, dataRefId);
            break;

        case 1: /* Add Digest */
            dataRefId = OSPEDRID_ALGID_ATTRIBUTES;
            newObject = OSPC_OSNULL;    /* None required for supported algorithm suite */
            break;

        case 2: /* Add NULL */
            if (newObject) {    /* !!! PS */
                OSPM_FREE(newObject->ElementInfo->Element);
                OSPM_FREE(newObject->ElementInfo);
            }
            dataRefId = OSPEDRID_ALGID_TERMINATOR;
            errorcode = OSPPASN1NullEncode(&newObject);
            break;

        case 3:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

        default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, "Unknown case encountered encoding PKCS7 DigestInfoCreate");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL) {
                errorcode = OSPPASN1ObjectAddChild(encodedObject, newObject, dataRefId);
                OSPM_FREE(newObject);
                newObject = OSPC_OSNULL;
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE) {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Complete the encoding for this object. Update results, elements, 
           etc. */
        errorcode = OSPPASN1ObjectDeparse(encodedObject, OSPEPTID_ALGORITHMID, OSPEDRID_ALGORITHMID);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvEncodedObject = encodedObject;
    } else {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&encodedObject);
    }

    return errorcode;

}

/*
 * NULL ROUTINES 
 */
int OSPPASN1NullEncode(
    OSPTASN1OBJECT **ospvEncodedObject)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *encodedObject = OSPC_OSNULL;
    unsigned char tag;

    /* Set up data and dataLength for octetstring - not much to do here */
    tag = OSPC_TAG_TYPE_NULL;

    errorcode = OSPPASN1ElementFormat(&eInfo, &tag, OSPC_BER_MODE_PRIMITIVE, 1, OSPC_OSNULL, 0);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectFormat(&encodedObject, eInfo, OSPEDRID_NOTDEFINED);
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPPASN1ElementDelete(&eInfo, 0);
        OSPPASN1ObjectDelete(&encodedObject);
    } else {
        *ospvEncodedObject = encodedObject;
    }

    return errorcode;
}
