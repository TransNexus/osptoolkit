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
 * ospmime.cpp - mime stuff
 */
#include "ospmime.h"

void
OSPPMimeBodyFree(
    OSPTMIMEBODY    *ospvMimeBody)
{
    int count = 0;

    OSPM_DBGENTER(("ENTER: OSPPMimeBodyFree()\n"));
    if(ospvMimeBody != (OSPTMIMEBODY *)NULL)
    {
        if(ospvMimeBody->Boundary != (OSPTMIMEPART *)NULL)
        {
            OSPPMimePartFree(ospvMimeBody->Boundary);
        }

        while(count < OSPC_MAX_BODYPARTS)
        {
            if(ospvMimeBody->BodyParts[count] != (OSPTMIMEPART *)NULL)
            {
                OSPPMimePartFree(ospvMimeBody->BodyParts[count]);
            }
            count++;
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeBodyFree()\n"));
    return;
}

int
OSPPMimeBodyPartsParse(
    OSPTMIMEPART    *ospvBodyData,
    OSPTMIMEBODY    *ospvMimeBody,
    OSPTMIMEPART    *ospvBoundary)
{
    int             errorcode = OSPC_ERR_NO_ERROR;
    unsigned char   *buf = ospvBodyData->Content;
    unsigned        currpos = 0;
    unsigned        bufend = ospvBodyData->Length;
    unsigned        start = 0;
    int             found = 0, bfound = 0;
    int             bresult = -1;
    OSPTMIMEPART    header;
    int             partsfound=0;

    OSPM_DBGENTER(("ENTER: OSPPMimeBodyPartsParse() bufend == [%d] buf = [%*s]\n", 
        bufend, bufend, (char *)buf));
    OSPM_MEMSET(&header, 0, sizeof(OSPTMIMEPART));

    /* strip off body parts, send to parser */
    while((partsfound < OSPC_MAX_BODYPARTS) && (currpos < bufend))
    {
        /* if there is no boundary set, we have text/plain */
        if((ospvBoundary != OSPC_OSNULL) &&
            (ospvBoundary->Content != OSPC_OSNULL))
        {
            while((currpos < bufend) &&
                (!found))
            {

                if(buf[currpos] == ospvBoundary->Content[0])
                {

                    /* make sure we really are at the boundary */
                    bresult = OSPM_MEMCMP(&buf[currpos], ospvBoundary->Content,
                        ospvBoundary->Length);

                    if(bresult == 0)
                    {
                        found = 1;
                        break;
                    }
                }
                currpos++;
            } /* end while(currpos < bufend... */

            if(found)
            {

                /* get past the boundary, CRLF */
                currpos += ospvBoundary->Length;

                while(currpos < bufend)
                {
                    if((buf[currpos] != '\n') &&
                        (buf[currpos] != ' ') &&
                        (buf[currpos] != '\t') &&
                        (buf[currpos] != '\r'))
                    {
                        bfound = 1;
                        break;
                    }
                    currpos++;
                }

                if(bfound)
                {
                    int cresult = -1;

                    start = currpos;
                    /* get to the next boundary (CRLF--boundary) */
                    while(currpos < bufend)
                    {
                        if((buf[currpos] == '\r') &&
                            (currpos+3 < bufend) &&
                            (buf[currpos+1] == '\n') &&
                            (buf[currpos + 2] == '-') &&
                            (buf[currpos + 3] == '-')) 
                        {

                            /* make sure we really are at the boundary */
                            cresult = OSPM_MEMCMP(&buf[currpos+4], ospvBoundary->Content,
                                ospvBoundary->Length);
                            if(cresult == 0)
                            {
                                /* move past the CRLF */
                                currpos+=2;
                                break;
                            }
                        }
                        currpos++;
                    }
                }
            }
        }
        else
        {
            /* get to the next CRLFCRLF (CRLFCRLF) */
            while(currpos < bufend)
            {
                if((buf[currpos] == '\r') &&
                    (currpos+3 < bufend) &&
                    (buf[currpos+1] == '\n') &&
                    (buf[currpos + 2] == '\r') &&
                    (buf[currpos + 3] == '\n')) 
                {
                    /*now we are in the right spot */
                    break;
                }
                currpos++;
            }

            start = currpos;

            /* set currpos to bufend, so we can parse the whole buffer. this is text/plain */
            currpos = bufend;
        }

        if(currpos <= bufend)
        {

            OSPM_MALLOC(ospvMimeBody->BodyParts[partsfound],
                OSPTMIMEPART,
                sizeof(OSPTMIMEPART));

            if(ospvMimeBody->BodyParts[partsfound] != (OSPTMIMEPART *)NULL)
            {


                OSPM_MEMSET(ospvMimeBody->BodyParts[partsfound],
                    0,
                    sizeof(ospvMimeBody->BodyParts[partsfound]));

                errorcode = OSPPMimeBufferParse(
                    &buf[start],
                    currpos - start,
                    &header,
                    ospvMimeBody->BodyParts[partsfound]);

                if (header.Content)
                {
                    OSPM_FREE(header.Content);
                    header.Content = OSPC_OSNULL;
                }

                if(errorcode == OSPC_ERR_NO_ERROR)
                {

                    partsfound++;
                    found = 0;
                    bfound = 0;
                }
                else
                {
                    break;
                }
            }
            else
            {
                errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
                OSPM_DBGERRORLOG(errorcode, "malloc failed for body part");
                break;
            }
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        ospvMimeBody->NumBodyParts = partsfound;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeBodyPartsParse()\n"));
    return errorcode;
}

/* Pulls apart header from body */
int 
OSPPMimeBufferParse(
    unsigned char   *ospvInputbuffer,       /* In - buffer to be parsed */
    unsigned        ospvSizeOfInputbuffer,  /* In - size of buffer to be parsed */
    OSPTMIMEPART    *ospvHeader,            /* Out - Mime header data */
    OSPTMIMEPART    *ospvBody)              /* Out - Mime body data */
{
    unsigned char* buffin = ospvInputbuffer;
    unsigned buffinend = ospvSizeOfInputbuffer;
    unsigned currpos = 0;
    unsigned hdrstart = 0;
    unsigned hdrlen = 0;
    unsigned bdystart = 0;
    unsigned bdylen = 0;
    int     errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMimeBufferParse()\n"));
    /* If first character is a LF (ANSI C or UNIX)
     * or if first two characters are CR LF (MIME or DOS),
     * there are no headers.
     */
    if ((currpos < buffinend) && (buffin[currpos] != '\n') 
        && ! ((buffin[currpos] == '\r') && (currpos+1 < buffinend)
        && (buffin[currpos+1] == '\n'))) 
    {

        while (currpos < buffinend) 
        {
            /* Is it LF LF ? */
            if ((buffin[currpos] == '\n')
                && (currpos+1 < buffinend)
                && (buffin[currpos+1] == '\n')) 
            {

                ++hdrlen;
                ++currpos;
                break;
            }
            /* Is it CR LF CR LF? */
            else if ((buffin[currpos] == '\r')
                && (currpos+3 < buffinend)
                && (buffin[currpos+1] == '\n')
                && (buffin[currpos+2] == '\r')
                && (buffin[currpos+3] == '\n')) 
            {

                hdrlen += 2;
                currpos += 2;
                break;
            }
            ++currpos;
            ++hdrlen;
        }
    }

    errorcode = OSPPUtilMallocAndCopySubString(ospvInputbuffer, 
        &(ospvHeader->Content), 
        hdrstart, 
        hdrlen);
    ospvHeader->Length = hdrlen;

    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        /* Skip blank line
         * LF (ANSI C or UNIX)
         */
        if (currpos < buffinend && buffin[currpos] == '\n') 
        {
            ++currpos;
        }
        /* CR LF (MIME or DOS) */
        else if (currpos < buffinend && buffin[currpos] == '\r'
            && currpos+1 < buffinend && buffin[currpos+1] == '\n') 
        {

            currpos += 2;
        }

        bdystart = currpos;
        bdylen = ospvSizeOfInputbuffer;

        /* get rid of whitspace, CRLFs at end of body string */
        while (bdylen > bdystart) 
        {
            if ((!isspace(buffin[bdylen-1])) &&
                (buffin[bdylen-1] != '\n') &&
                (buffin[bdylen-1] != '\r'))
            {
                break;
            }
            --bdylen;
        }

        bdylen = bdylen - bdystart;

        errorcode = OSPPUtilMallocAndCopySubString(ospvInputbuffer, 
            &(ospvBody->Content), 
            bdystart, 
            bdylen);
        ospvBody->Length = bdylen;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeBufferParse()\n"));
    return errorcode;
}

void
OSPPMimeDataFree(
    OSPTMIMEMSG *ospvMessage)
{

    OSPM_DBGENTER(("ENTER: OSPPMimeDataFree()\n"));
    OSPPMimePartFree(ospvMessage->MsgPart);
    OSPPMimePartFree(ospvMessage->SigPart);
    OSPM_DBGEXIT(("EXIT : OSPPMimeDataFree()\n"));

    return;
}

void
OSPPMimeFieldFree(
    OSPTMIMEFIELD   *ospvField)
{
    int count = 0;

    OSPM_DBGENTER(("ENTER: OSPPMimeFieldFree()\n"));
    if(ospvField != (OSPTMIMEFIELD *)NULL)
    {
        if(ospvField->Params != (OSPTMIMEPARAM **)NULL)
        {
            for(count = 0; count < OSPC_MAX_PARAMS;count++)
            {
                if(ospvField->Params[count] != (OSPTMIMEPARAM *)NULL)
                {
                    OSPPMimeParamFree(ospvField->Params[count]);

                    OSPM_FREE(ospvField->Params[count]);
                    ospvField->Params[count] = NULL;
                }
            }
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeFieldFree()\n"));
    return;
}

int
OSPPMimeMessageBuild(
    OSPTMIMEMSG     *ospvMessage,
    unsigned char   **ospvMimeOut,
    unsigned        *ospvSizeOfMimeOut)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    unsigned char   *next       = NULL;
    char            msgtmp[20];
    char            sigtmp[20];
    int             boundln     = 0, 
                    mshdrln     = 0,
                    msszln      = 0,
                    sighdrln    = 0, 
                    sigszln     = 0,
                    count       = 0,
                    numdelims   = 0,
                    numcrlfs    = 0;

    if(ospvMessage->SigPart->Length > 0)
    {
        msszln = sprintf(msgtmp, "%d", ospvMessage->MsgPart->Length);
        mshdrln = strlen(OSPC_MIME_MSG_HEADER);
        boundln = strlen(OSPC_MIME_BOUNDARY);
        sigszln = sprintf(sigtmp, "%d", ospvMessage->SigPart->Length),
        sighdrln = strlen(OSPC_MIME_SIG_HEADER);
        numdelims = 4;
        numcrlfs = 12;
    }

    OSPM_DBGENTER(("ENTER: OSPPMimeMessageBuild()\n"));
    count = (boundln*3) +
        mshdrln +
        msszln +
        ospvMessage->MsgPart->Length +
        sighdrln +
        sigszln +
        ospvMessage->SigPart->Length +
        2*numdelims +
        2*numcrlfs;  /* CRLFs, boundary delimiters */


    ospvMessage->Length = count;

    *ospvSizeOfMimeOut = ospvMessage->Length;

    OSPM_MALLOC(*ospvMimeOut,
        unsigned char,
        *ospvSizeOfMimeOut);

    if(*ospvMimeOut != (unsigned char *)NULL)
    {

        OSPM_MEMSET(*ospvMimeOut,
            0,
            *ospvSizeOfMimeOut);

        next = *ospvMimeOut;

        if(ospvMessage->SigPart->Length > 0) 
        {
            /* CRLF for beginning of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* -- for start of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY_DELIM,
                2);

            next += 2;

            /* boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY,
                boundln);

            next += boundln;


            /* CRLF */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* Message header */
            OSPM_MEMCPY(next,
                OSPC_MIME_MSG_HEADER,
                mshdrln);

            next += mshdrln;

            /* Message length */
            OSPM_MEMCPY(next,
                msgtmp,
                msszln);
            next+=msszln;

            /* CRLF for end of message header */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* CRLF for beginning of message */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

        }

        /* Message content */

        OSPM_MEMCPY(next,
            ospvMessage->MsgPart->Content,
            ospvMessage->MsgPart->Length);

        next+=ospvMessage->MsgPart->Length;

        if(ospvMessage->SigPart->Length > 0)
        {

            /* CRLF for end of message body */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* CRLF for beginning of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* -- for start of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY_DELIM,
                2);

            next += 2;

            /* boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY,
                boundln);

            next += boundln;

            /* CRLF for end of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* Signature Header */
            OSPM_MEMCPY(next,
                OSPC_MIME_SIG_HEADER,
                sighdrln);

            next += sighdrln;

            /* Signature length */
            OSPM_MEMCPY(next,
                sigtmp,
                sigszln);
            next+=sigszln;

            /* CRLF for end of signature header */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* CRLF for beginning of content */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;


            /* Signature content */
            OSPM_MEMCPY(next,
                ospvMessage->SigPart->Content,
                ospvMessage->SigPart->Length);

            next+=ospvMessage->SigPart->Length;

            /* CRLF for end of signature body */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* CRLF for start of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;

            /* -- for start of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY_DELIM,
                2);

            next += 2;

            /* boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY,
                boundln);

            next += boundln;

            /* -- for end of final boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_BOUNDARY_DELIM,
                2);

            next += 2;

            /* CRLF for end of boundary */
            OSPM_MEMCPY(next,
                OSPC_MIME_CRLF,
                2);

            next += 2;
        }

    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeMessageBuild()\n"));
    return errorcode;
}

int
OSPPMimeMessageCreate(
    unsigned char   *ospvMessage,
    unsigned        ospvSizeOfMessage,
    unsigned char   *ospvSignature,
    unsigned        ospvSizeOfSignature,
    unsigned char   **ospvMimeMessage,
    unsigned        *ospvSizeOfMimeMessage)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTMIMEMSG msg;

    OSPM_DBGENTER(("ENTER: OSPPMimeMessageCreate()\n"));
    /* Initialize the structure to hold the Mime message data */
    errorcode = OSPPMimeMessageInit(&msg);

    /* Set up the message and signature parts. We will use this function for the 
     * signature part as well when we get a real signature in */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMimeMessageSetContentAndLength(msg.MsgPart, ospvMessage, ospvSizeOfMessage);
    }

    if((errorcode == OSPC_ERR_NO_ERROR) && (ospvSignature != OSPC_OSNULL))
    {
        errorcode = OSPPMimeMessageSetContentAndLength(msg.SigPart, ospvSignature, ospvSizeOfSignature);
    }

    /* Build the MIME message */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMimeMessageBuild(&msg, ospvMimeMessage, ospvSizeOfMimeMessage);
    }

    /* Free the structure holding the message data */
    OSPPMimeDataFree(&msg);

    OSPM_DBGEXIT(("EXIT : OSPPMimeMessageCreate()\n"));
    return errorcode;
}

int
OSPPMimeMessageInit(
    OSPTMIMEMSG *ospvMessage)
{
    int     errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMimeMessageInit()\n"));
    if(ospvMessage != (OSPTMIMEMSG *)NULL)
    {
        OSPM_MEMSET(ospvMessage,
            0,
            sizeof(OSPTMIMEMSG));
    }
    else
    {
        errorcode = OSPC_ERR_MIME_VAR_NOT_FOUND;
        OSPM_DBGERRORLOG(errorcode, "no pointer found for mime message");
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPM_MALLOC(ospvMessage->MsgPart,
            OSPTMIMEPART,
            sizeof(OSPTMIMEPART));

        if(ospvMessage->MsgPart == (OSPTMIMEPART *)NULL)
        {
            errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
            OSPM_DBGERRORLOG(errorcode, "can't malloc message part");
        }
        else
        {
            OSPM_MEMSET(ospvMessage->MsgPart,
                0,
                sizeof(OSPTMIMEPART));
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_MALLOC(ospvMessage->SigPart,
                OSPTMIMEPART,
                sizeof(OSPTMIMEPART));

            if(ospvMessage->SigPart == (OSPTMIMEPART *)NULL)
            {
                errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
                OSPM_DBGERRORLOG(errorcode, "can't malloc signature part");
            }
            else
            {
                OSPM_MEMSET(ospvMessage->SigPart,
                    0,
                    sizeof(OSPTMIMEPART));
            }

        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            ospvMessage->Length = strlen(OSPC_MIME_MSG_HEADER) + strlen(OSPC_MIME_SIG_HEADER);
            ospvMessage->NumParts = 2;
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeMessageInit()\n"));
    return errorcode;
}

int
OSPPMimeMessageParse(
    unsigned char   *ospvMessageBuffer,
    unsigned        ospvSizeOfMessageBuffer,
    unsigned char   *ospvContent,
    unsigned        ospvSizeOfContent,
    unsigned char   **ospvMessageData,
    unsigned        *ospvSizeOfMessageData,
    unsigned char   **ospvSignatureData,
    unsigned        *ospvSizeOfSignatureData)
{
    int             errorcode   = OSPC_ERR_NO_ERROR,
                    result      = -1;
    OSPTMIMEFIELD   content;
    OSPTMIMEBODY    mimebody;
    OSPTMIMEPART    boundary, body;
    unsigned        currpos     = 0;

    OSPM_DBGENTER(("ENTER: OSPPMimeMessageParse()\n"));
    OSPM_MEMSET(&mimebody, 0, sizeof(OSPTMIMEBODY));
    OSPM_MEMSET(&body, 0, sizeof(OSPTMIMEPART));
    OSPM_MEMSET(&boundary, 0, sizeof(OSPTMIMEPART));
    OSPM_MEMSET(&content, 0, sizeof(OSPTMIMEFIELD));

    /* Make sure we have only the field body without the field name.*/
    while((isspace(ospvContent[currpos])) && 
        (currpos < ospvSizeOfContent))
    {
        currpos++;
    }
    /* if we find a 'c' we have the field name. Get past it */
    if(tolower(ospvContent[currpos]) == 'c')
    {
        while(currpos < ospvSizeOfContent)
        {
            if(ospvContent[currpos] == ':')
            {
                break;
            }
            currpos++;
        }
        /* get past the ':' */
        currpos++;
    }

    /* Is there anything left? If so, get to the chars */
    if(currpos < ospvSizeOfContent)
    {
        while((isspace(ospvContent[currpos])) && 
            (currpos < ospvSizeOfContent))
        {
            currpos++;
        }
    }

    content.FieldBody.Content = &ospvContent[currpos];
    content.FieldBody.Length = ospvSizeOfContent - currpos;
    body.Content = ospvMessageBuffer;
    body.Length = ospvSizeOfMessageBuffer;

    /* Make sure all the parameters meet our criteria, and copy 
     * the ones we intend to keep into their places 
     */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        errorcode = OSPPMimeVerifyParameters(&content, &boundary, OSPC_MAIN);
    }

    /* Pull out the body parts, i.e. the message and the signature */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPUtilMemCaseCmp(content.FieldBody.Content,
            content.FieldBody.Length,
            "text/plain",
            strlen("text/plain"),
            &result);

        if((errorcode == OSPC_ERR_NO_ERROR) &&
            (result == 0))
        {
            /* Possible Optimization -
             * When it is not a multibody message, there is no need to allocate memory
             * for mimebody.BodyParts[0], we can just set - mimebody.NumBodyParts = 1
             * and that will do. Allocating memory for the 0th element is not required.
             */
            OSPM_MALLOC(mimebody.BodyParts[0],
                OSPTMIMEPART,
                sizeof(OSPTMIMEPART));

            if(mimebody.BodyParts[0] != (OSPTMIMEPART *)NULL)
            {


                OSPM_MEMSET(mimebody.BodyParts[0],
                    0,
                    sizeof(mimebody.BodyParts[0]));

                mimebody.BodyParts[0]->Content = ospvMessageBuffer;
                mimebody.BodyParts[0]->Length = ospvSizeOfMessageBuffer;
                mimebody.NumBodyParts = 1;
            }
        }
        else
        {
            errorcode = OSPPMimeBodyPartsParse(&body, &mimebody, &boundary);
        }
    }

    /* Copy the body parts into the buffers to be sent back. Set the length of each */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(mimebody.NumBodyParts > 0)
        {

            errorcode = OSPPUtilMallocAndCopySubString(mimebody.BodyParts[0]->Content,
                ospvMessageData,
                0,
                mimebody.BodyParts[0]->Length);
            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                *ospvSizeOfMessageData = mimebody.BodyParts[0]->Length;

                if(mimebody.NumBodyParts > 1 )
                {
                    errorcode = OSPPUtilMallocAndCopySubString(mimebody.BodyParts[1]->Content,
                        ospvSignatureData,
                        0,
                        mimebody.BodyParts[1]->Length);

                    if(errorcode == OSPC_ERR_NO_ERROR)
                    {

                        *ospvSizeOfSignatureData = mimebody.BodyParts[1]->Length;
                    }
                }
            }    
        }
        else
        {
            errorcode = OSPC_ERR_MIME_MSG_PARTS_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, "not enough message parts for full message");
        }
    }

    OSPPMimeFieldFree(&content);

    if(strstr((const char *)content.FieldBody.Content, "multipart"))
    {
        OSPPMimeBodyFree(&mimebody);
    }
    else
    {
        OSPM_FREE(mimebody.BodyParts[0]);
    }

    if (boundary.Content)
    {
        OSPM_FREE(boundary.Content);
        boundary.Content = OSPC_OSNULL;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeMessageParse()\n"));
    return errorcode;
}


long __mptr=0;

int
OSPPMimeMessageSetContentAndLength(
    OSPTMIMEPART    *ospvMimePart,
    unsigned char   *ospvMsgBuffer,
    unsigned        ospvSizeOfMsgBuffer)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMimeMessageSetContentAndLength()\n"));
    if(ospvSizeOfMsgBuffer > 0)
    {

        OSPM_MALLOC(ospvMimePart->Content, 
            unsigned char, 
            ospvSizeOfMsgBuffer + 1);


        if(ospvMimePart->Content != (unsigned char *)NULL)
        {

            OSPM_MEMCPY(ospvMimePart->Content,
                ospvMsgBuffer,
                ospvSizeOfMsgBuffer);

            ospvMimePart->Length = ospvSizeOfMsgBuffer;

        }
        else
        {
            errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
            OSPM_DBGERRORLOG(errorcode, "can't malloc msg content part");
        }
    }
    else
    {
        errorcode = OSPC_ERR_MIME_NO_INPUT;
        OSPM_DBGERRORLOG(errorcode, "no data input for msg part");
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeMessageSetContentAndLength()\n"));
    return errorcode;
}

void
OSPPMimeParamFree(
    OSPTMIMEPARAM   *ospvParam)
{
    OSPM_DBGENTER(("ENTER: OSPPMimeParamFree()\n"));
    if(ospvParam != (OSPTMIMEPARAM  *)NULL)
    {
        OSPPMimePartFree(ospvParam->ParamName);
        OSPPMimePartFree(ospvParam->ParamValue);
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeParamFree()\n"));
    return;
}

int
OSPPMimeParamsParse(
    OSPTMIMEFIELD   *ospvField)
{
    int             errorcode = OSPC_ERR_NO_ERROR;
    unsigned        start = 0;
    unsigned        currpos = 0;
    unsigned        bufend = ospvField->FieldBody.Length;
    unsigned char   *bufptr = ospvField->FieldBody.Content;
    unsigned        count = 0;
    unsigned        len = 0;

    OSPM_DBGENTER(("ENTER: OSPPMimeParamsParse()\n"));
    if(bufptr != (unsigned char *)NULL)
    {

        /* Get past the type/subtype string */
        while((bufptr[currpos] != ';') &&
              (bufptr[currpos] != '\n') &&
              (currpos < bufend))
        {
            currpos++;
        }

        /* pass the ; */
        if(bufptr[currpos] == ';')
        {
            currpos++;

            /* Now get the name = value pairs and store them */
            while((ospvField->NumParams < OSPC_MAX_PARAMS) &&
                (currpos < bufend))
            {

                /* Skip spaces, tabs, newlines, and stray ';' */
                while (currpos < bufend) 
                {
                    if ((bufptr[currpos] != ' ') && 
                        (bufptr[currpos] != '\t') &&
                        (bufptr[currpos] != '\n') &&
                        (bufptr[currpos] != ';'))
                    {
                        break;
                    }
                    ++currpos;
                }
                start = currpos;

                while(currpos < bufend)
                {
                    if(bufptr[currpos] == '=')
                    {
                        break;
                    }
                    currpos++;
                }
                len = currpos - start;

                /* remove white space at end */
                while(len > 0)
                {
                    if(!isspace(bufptr[currpos-1]))
                    {
                        break;
                    }
                    --len;
                }

                /* Create Param structure */
                OSPM_MALLOC(ospvField->Params[count],
                    OSPTMIMEPARAM,
                    sizeof(OSPTMIMEPARAM));

                if(ospvField->Params[count] != (OSPTMIMEPARAM *)NULL)
                {

                    OSPM_MEMSET(ospvField->Params[count],
                        0,
                        sizeof(OSPTMIMEPARAM));

                    /* Create Param Name Structure */
                    OSPM_MALLOC(ospvField->Params[count]->ParamName,
                        OSPTMIMEPART,
                        sizeof(OSPTMIMEPART));

                    if(ospvField->Params[count]->ParamName != (OSPTMIMEPART *)NULL)
                    {

                        OSPM_MEMSET(ospvField->Params[count]->ParamName,
                            0,
                            sizeof(OSPTMIMEPART));


                        errorcode = OSPPUtilMallocAndCopySubString(ospvField->FieldBody.Content,
                            &(ospvField->Params[count])->ParamName->Content,
                            start,
                            len);
                    }
                    else
                    {
                        errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
                        OSPM_DBGERRORLOG(errorcode, "malloc failed for param name structure");
                    }
                }
                else
                {
                    errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
                    OSPM_DBGERRORLOG(errorcode, "malloc failed for param structure");
                }

                /* Now get value */
                if(errorcode == OSPC_ERR_NO_ERROR)
                {

                    ospvField->Params[count]->ParamName->Length = len;

                    if((currpos < bufend) && (bufptr[currpos] == '='))
                    {
                        currpos++;
                    }

                    /* Skip spaces and tabs */
                    while (currpos < bufend) 
                    {
                        if ((bufptr[currpos] != ' ') && 
                            (bufptr[currpos] != '\t')) 
                        {
                            break;
                        }
                        ++currpos;
                    }
                    start = currpos;
                    len = 0;

                    /* Get value */
                    while(currpos < bufend)
                    {

                        /* ; means we are at the end of the param value
                         * get rid of it */
                        if(bufptr[currpos] == ';')
                        {
                            break;
                        }

                        if(bufptr[currpos] == '\n')
                        {

                            /* Are we at the end of the data? */
                            if(currpos == bufend -1)
                            {
                                currpos++;
                                break;
                            }

                            /* Is this really the end of the param value, and not just the end
                             * of a wrapped line? */
                            else if ((bufptr[currpos+1] != ' ') && 
                                (bufptr[currpos+1] != '\t')) 
                            {
                                currpos++;
                                break;
                            }
                        }
                        currpos++;
                    }

                    /* remove whitespace at end of value */
                    while(currpos > start)
                    {
                        if(!isspace(bufptr[currpos-1]))
                        {
                            break;
                        }
                        --currpos;
                    }
                    len = currpos - start;

                    /* Create Param Value Structure */
                    OSPM_MALLOC(ospvField->Params[count]->ParamValue,
                        OSPTMIMEPART,
                        sizeof(OSPTMIMEPART));

                    if(ospvField->Params[count]->ParamValue != (OSPTMIMEPART *)NULL)
                    {

                        OSPM_MEMSET(ospvField->Params[count]->ParamValue,
                            0,
                            sizeof(OSPTMIMEPART));


                        errorcode = OSPPUtilMallocAndCopySubString(ospvField->FieldBody.Content, 
                            &(ospvField->Params[count])->ParamValue->Content, 
                            start, 
                            len);
                    }
                    else
                    {
                        errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
                        OSPM_DBGERRORLOG(errorcode, "malloc failed for param value structure");
                    }

                    if(errorcode == OSPC_ERR_NO_ERROR)
                    {
                        ospvField->Params[count]->ParamValue->Length = len;
                        ospvField->NumParams++;
                    }
                }
                count++;
            }

            if((count == OSPC_MAX_PARAMS) && (currpos != bufend))
            {

                errorcode = OSPC_ERR_MIME_NO_MORE_SPACE;
                OSPM_DBGERRORLOG(errorcode, "no space for this parameter");
            }
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeParamsParse()\n"));
    return errorcode;
}

void
OSPPMimePartFree(
    OSPTMIMEPART    *ospvPart)
{
    OSPM_DBGENTER(("ENTER: OSPPMimePartFree()\n"));
    if(ospvPart->Content != (unsigned char *)NULL)
    {
        OSPM_FREE(ospvPart->Content);
        ospvPart->Content = OSPC_OSNULL;
    }

    if(ospvPart != (OSPTMIMEPART *)NULL)
    {
        OSPM_FREE(ospvPart);
        ospvPart = OSPC_OSNULL;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimePartFree()\n"));
    return;
}

int
OSPPMimeVerifyParameters(
    OSPTMIMEFIELD   *ospvContent,
    OSPTMIMEPART    *ospvBoundary,
    int             ospvHeaderType)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned    pcount = 0;
    int         result = 0;
    int         pfound = 0;

    OSPM_DBGENTER(("ENTER: OSPPMimeVerifyParameters()\n"));
    switch(ospvHeaderType)
    {
        case OSPC_MAIN:
        /* verify content-type, protocol, micalg, boundary, length */

        /* found content-type, now check type and subtype */
        errorcode = OSPPUtilMemCaseCmp(ospvContent->FieldBody.Content,
            ospvContent->FieldBody.Length,
            "multipart/signed",
            strlen("multipart/signed"),
            &result);

        if((errorcode == OSPC_ERR_NO_ERROR) &&
            (result == 0))
        {

            /* Parse parameters into structure for easier compares */
            errorcode = OSPPMimeParamsParse(ospvContent);

            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                pcount = 0;
                pfound = 0;

                /* check protocol */
                while((pcount < ospvContent->NumParams) &&
                    (!pfound))
                {


                    errorcode = OSPPUtilMemCaseCmp( ospvContent->Params[pcount]->ParamName->Content,
                        ospvContent->Params[pcount]->ParamName->Length,
                        "protocol",
                        strlen("protocol"),
                        &result);

                    if((errorcode == OSPC_ERR_NO_ERROR) &&
                        (result == 0))
                    {
                        pfound = 1;
                        break;
                    }

                    pcount++;
                }

                if(pfound)
                {

                    /* copy the protocol string somewhere to save it for SECURITY 
                      * remember to strip off double quotes 
                      */
                }
                else
                {
                    errorcode = OSPC_ERR_MIME_PROTOCOL_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "protocol parameter not found");
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                pcount = 0;
                pfound = 0;

                /* check micalg */
                while((pcount < ospvContent->NumParams) &&
                    (!pfound))
                {


                    errorcode = OSPPUtilMemCaseCmp( ospvContent->Params[pcount]->ParamName->Content,
                        ospvContent->Params[pcount]->ParamName->Length,
                        "micalg",
                        strlen("micalg"),
                        &result);

                    if((errorcode == OSPC_ERR_NO_ERROR) &&
                        (result == 0))
                    {
                        pfound = 1;
                        break;
                    }

                    pcount++;
                }

                if(pfound)
                {
                    /* copy this out to hand up to SECURITY */

                }
                else
                {
                    errorcode = OSPC_ERR_MIME_MICALG_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "micalg not found");
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                pcount = 0;
                pfound = 0;

                /* get boundary */
                while((pcount < ospvContent->NumParams) &&
                    (!pfound))
                {


                    errorcode = OSPPUtilMemCaseCmp( ospvContent->Params[pcount]->ParamName->Content,
                        ospvContent->Params[pcount]->ParamName->Length,
                        "boundary",
                        strlen("boundary"),
                        &result);

                    if((errorcode == OSPC_ERR_NO_ERROR) &&
                        (result == 0))
                    {
                        pfound = 1;
                        break;
                    }

                    pcount++;
                }

                if(pfound)
                {
                    OSPM_MALLOC(ospvBoundary->Content,
                        unsigned char,
                        ospvContent->Params[pcount]->ParamValue->Length);

                    if(ospvBoundary->Content != (unsigned char *)NULL)
                    {

                        OSPM_MEMSET(ospvBoundary->Content,
                            0,
                            ospvContent->Params[pcount]->ParamValue->Length);

                        OSPM_MEMCPY(ospvBoundary->Content,
                            ospvContent->Params[pcount]->ParamValue->Content,
                            ospvContent->Params[pcount]->ParamValue->Length);

                        ospvBoundary->Length = ospvContent->Params[pcount]->ParamValue->Length;
                    }
                    else
                    {
                        errorcode = OSPC_ERR_MIME_MALLOC_FAILED;
                        OSPM_DBGERRORLOG(errorcode, "malloc failed for boundary content");
                    }
                } /* end if (pfound) */
                else
                {
                    errorcode = OSPC_ERR_MIME_BOUNDARY_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "boundary not found");
                }
            }
            else if(result != 0)
            {

                errorcode = OSPC_ERR_MIME_INVALID_SUBTYPE;
                OSPM_DBGERRORLOG(errorcode, "subtype does not match");
            }
        }
        else
        {
            errorcode = OSPPUtilMemCaseCmp(ospvContent->FieldBody.Content,
                ospvContent->FieldBody.Length,
                "text/plain",
                strlen("text/plain"),
                &result);
            
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* Parse parameters into structure for easier compares */
                errorcode = OSPPMimeParamsParse(ospvContent);
            }
        }
        break;
        case OSPC_BODY:
        /* verify content-type, length */
        break;
        case OSPC_SIGNATURE:
        /* verify content-type, length */
        break;
        default:
        errorcode = OSPC_ERR_MIME_INVALID_TYPE;
        OSPM_DBGERRORLOG(errorcode, "invalid header type");
        break;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMimeVerifyParameters()\n"));
    return errorcode;
}

int
OSPPUtilMallocAndCopySubString(
    unsigned char   *ospvSrcString,
    unsigned char   **ospvDestString,
    unsigned        ospvStartOfSubString,
    unsigned        ospvSizeOfSubString)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPUtilMallocAndCopySubString()\n"));
    if(&ospvSrcString[ospvStartOfSubString] != (unsigned char *)NULL)
    {
        OSPM_MALLOC(*ospvDestString,
            unsigned char,
            ospvSizeOfSubString+1);

        if(*ospvDestString != (unsigned char *)NULL)
        {
            OSPM_MEMSET(*ospvDestString,
                0,
                ospvSizeOfSubString +1);

            OSPM_MEMCPY(*ospvDestString,
                &ospvSrcString[ospvStartOfSubString],
                ospvSizeOfSubString);
        }
        else
        {
            errorcode = OSPC_ERR_UTIL_MALLOC_FAILED;
            OSPM_DBGERRORLOG(errorcode, "malloc failed for mallocandcopystring");
        }
    }
    else
    {
        errorcode = OSPC_ERR_UTIL_VALUE_NOT_FOUND;
        OSPM_DBGERRORLOG(errorcode, "no string for mallocandcopystring");
    }
    OSPM_DBGEXIT(("EXIT : OSPPUtilMallocAndCopySubString()\n"));
    return errorcode;
}

int 
OSPPUtilMemCaseCmp(
    unsigned char   *ospvMem1, 
    unsigned        ospvLen1, 
    char            *ospvMem2,
    unsigned        ospvLen2,
    int             *ospvResult)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned count=0;
    unsigned len = OSPM_MIN(ospvLen1, ospvLen2);
    int char1 = 0;
    int char2 = 0;

    OSPM_DBGENTER(("ENTER: OSPPUtilMemCaseCmp()\n"));
    *ospvResult = 0;

    if((ospvMem1 != (unsigned char *)NULL) &&
        (ospvMem2 != (char *)NULL)) 
    {

        for (count=0; count < len; ++count) 
        {
            char1 = tolower(ospvMem1[count]);
            char2 = tolower(ospvMem2[count]);
            if (char1 < char2) 
            {
                *ospvResult = -1;
                break;
            }
            else if (char1 > char2) 
            {
                *ospvResult = 1;
                break;
            }
        } /* end for */
    }
    else
    {
        errorcode = OSPC_ERR_MIME_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "no data for compare");
    }
    OSPM_DBGEXIT(("EXIT : OSPPUtilMemCaseCmp()\n"));
    return errorcode;
}

/* End of file */
