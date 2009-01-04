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
 * ospasn1.c - Member functions for ASN1 decode/encode library
 */

#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/osptnlog.h"

/*
 * Member functions
 */
int OSPPASN1ElementGetContentData(
    OSPTASN1ELEMENTINFO *ospvElement, 
    unsigned char **ospvContent, 
    unsigned int *ospvContentLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if ((ospvElement == OSPC_OSNULL) || (ospvContent == OSPC_OSNULL) || (ospvContentLength == OSPC_OSNULL)) {
        errorcode = OSPC_ERR_ASN1_NULL_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Invalid parameters to ElementGetContent");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvContent = ospvElement->Content;
        *ospvContentLength = ospvElement->ContentLength;
    } else {
        *ospvContent = OSPC_OSNULL;
        *ospvContentLength = 0;
    }

    return errorcode;
}

int OSPPASN1ElementCopyElementData(
    OSPTASN1ELEMENTINFO *ospvSrcElement, 
    unsigned char **ospvData, 
    unsigned int *ospvDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(*ospvData, unsigned char, ospvSrcElement->ElementLength);
        if (*ospvData == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate data storage");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Finish up the element */
        *ospvDataLength = ospvSrcElement->ElementLength;
        OSPM_MEMCPY(*ospvData, ospvSrcElement->Element, *ospvDataLength);
    }

    return errorcode;
}

int OSPPASN1ElementGetElementData(
    OSPTASN1ELEMENTINFO *ospvSrcElement, 
    unsigned char **ospvData, 
    unsigned int *ospvDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvSrcElement != OSPC_OSNULL) {
        /* Finish up the element */
        *ospvDataLength = ospvSrcElement->ElementLength;
        *ospvData = ospvSrcElement->Element;
    }

    return errorcode;
}

int OSPPASN1ElementTestContext(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvElementInfo == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_INVALID_CONTEXT;
        OSPM_DBGERRORLOG(errorcode, "Invalid ASN1 ElementInfo pointer");
    }

    return errorcode;
}

int OSPPASN1ElementEncode(
    OSPTASN1ELEMENTINFO *ospvElementInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned elementLength = 0;
    unsigned contentLength = 0;
    unsigned newLength = 0;
    unsigned i = 0;
    unsigned char *elementData = OSPC_OSNULL;
    unsigned char *contentData = OSPC_OSNULL;
    unsigned char *newData = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *contentElements = OSPC_OSNULL;
    unsigned char *lengthBuffer = OSPC_OSNULL;
    unsigned lengthLength = 0;

    if (ospvElementInfo == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
    } else {
        contentElements = ospvElementInfo->ContentElementInfo;
        if (contentElements == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
        }
    }

    for (; (errorcode == OSPC_ERR_NO_ERROR) && (contentElements != OSPC_OSNULL); contentElements = contentElements->NextElementInfo) {
        errorcode = OSPPASN1ElementGetElementData(contentElements, &newData, &newLength);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_REALLOC(contentData, contentData, unsigned char, contentLength + newLength);
            if (contentData == OSPC_OSNULL) {
                errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
                OSPM_DBGERRORLOG(errorcode, "Unable to realloc element data store");
            }
        }
        if (errorcode == OSPC_ERR_NO_ERROR) {
            unsigned char *tmpPtr;

            tmpPtr = (contentData + contentLength);
            OSPM_MEMCPY(tmpPtr, newData, newLength);
            contentLength += newLength;
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1SmallInt2UnsignedChar(contentLength, 256, &lengthBuffer, &lengthLength);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            elementLength = contentLength + 1 + lengthLength;
            if (contentLength > 127) {
                elementLength++;
            }

            OSPM_MALLOC(elementData, unsigned char, elementLength);

            if (elementData == OSPC_OSNULL) {
                errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
                OSPM_DBGERRORLOG(errorcode, "Unable to malloc space for element data");
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            i = 0;
            elementData[i++] = ospvElementInfo->Tag;
            if (contentLength > 127) {
                elementData[i++] = (unsigned char) (lengthLength | OSPC_BER_LENGTH_MODE_LONG);
            }
            OSPM_MEMCPY(&elementData[i], lengthBuffer, lengthLength);
            i += lengthLength;

            OSPM_MEMCPY(&(elementData[i]), contentData, contentLength);

            ospvElementInfo->Element = elementData;
            ospvElementInfo->ElementLength = elementLength;
            ospvElementInfo->ElementSpaceAllocated = 1;

            OSPM_FREE(contentData);
        }
    }

    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE) {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (lengthBuffer != OSPC_OSNULL) {
        OSPM_FREE(lengthBuffer);
    }

    return errorcode;
}

int OSPPASN1ElementDecode(
    unsigned char *ospvASN1Element, 
    OSPTASN1ELEMENTINFO **ospvASN1ElementInfo, 
    unsigned int ospvLevel)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *eptr = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *cInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *hPtr = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *tPtr = OSPC_OSNULL;
    unsigned headerLength = 0;
    char msg[100];
    unsigned length = 0;
    unsigned i = 0;
    unsigned char baseTag = 0;
    unsigned char dataByte = 0;

    OSPM_MEMSET(msg, 0, 100);
    /* Test for a valid element data pointer */
    if (ospvASN1Element == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_NULL_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Invalid element pointer");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Allocate space for an element */
        errorcode = OSPPASN1ElementCreate(&eInfo);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Initialize element Info Structure */
        eptr = ospvASN1Element;

        /* Extract the tag */
        eInfo->Tag = *(eptr++);
        headerLength++;            /* Keep track of info bytes in element */

        if (OSPM_IS_HIGH_TAG(eInfo->Tag)) {
            errorcode = OSPC_ERR_ASN1_UNEXPECTED_HIGH_TAG;
            OSPM_DBGERRORLOG(errorcode, "Unsupported high tag encountered.");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Validate Tag */
            baseTag = (unsigned char)OSPM_BASE_TAG(eInfo->Tag);

            /* Test the tag */
            switch (baseTag) {
            case OSPC_TAG_TYPE_SEQUENCE:
            case OSPC_TAG_TYPE_SET:
            case OSPC_TAG_TYPE_BOOLEAN:
            case OSPC_TAG_TYPE_EOC:
            case OSPC_TAG_TYPE_NULL:
            case OSPC_TAG_TYPE_INTEGER:
            case OSPC_TAG_TYPE_OBJECT_IDENTIFIER:
            case OSPC_TAG_TYPE_PRINTABLESTRING:
            case OSPC_TAG_TYPE_T61STRING:
            case OSPC_TAG_TYPE_IA5STRING:
            case OSPC_TAG_TYPE_UTCTIME:
            case OSPC_TAG_TYPE_BIT_STRING:
            case OSPC_TAG_TYPE_OCTET_STRING:
            case OSPC_TAG_TYPE_BMPSTRING:
                errorcode = OSPC_ERR_NO_ERROR;
                break;
            default:
                errorcode = OSPC_ERR_ASN1_INVALID_ELEMENT_TAG;
                sprintf(msg, "Invalid ASN1 element tag: %02x at %06d", eInfo->Tag, (int) (eptr - ospvASN1Element));
                OSPM_DBGERRORLOG(errorcode, msg);
                sprintf(msg, "ERROR:ElementDecode - Tag=%0x", eInfo->Tag);

                break;
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Valid tag, get the length of the content */
            dataByte = *(eptr++);
            headerLength++;
            length = OSPM_BER_LENGTH(dataByte);

            if (OSPM_IS_LONG_LENGTH(dataByte)) {
                /* Length is set to length of length */
                unsigned value = 0;

                /* Long Length Format */
                for (i = 0; i < length; i++, headerLength++) {
                    value *= 256;
                    value += *(eptr++);
                }
                length = value;
            }

            eInfo->ElementLength = headerLength + length;
            eInfo->ContentLength = length;
        }
    }

    if (ospvLevel == 0) {
        /* Allocate space for the element so we don't rely on some
           other functions storage (only top level) */

        OSPM_MALLOC(eInfo->Element, unsigned char, eInfo->ElementLength);
        if (eInfo->Element == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for element");
        } else {
            eInfo->ElementSpaceAllocated = 1;
            OSPM_MEMCPY(eInfo->Element, ospvASN1Element, eInfo->ElementLength);
        }
    } else {
        /* Rely on higher level EINFO to maintain storage for
           element list - just set pointer here */
        eInfo->Element = ospvASN1Element;
    }

    eInfo->Content = eInfo->Element + headerLength;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Finish the elementInfo structure */
        if (OSPM_IS_PRIMITIVE(eInfo->Tag)) {
            /* Finish decoding a primitive element - possible expansion
               here if necessary */
        } else {
            /* Parse the content you found */
            hPtr = OSPC_OSNULL;    /* Pointer to first element in content */
            tPtr = OSPC_OSNULL;    /* Pointer to current last element in content */

            /* Process chunks of content till done */
            for (i = 0; i < eInfo->ContentLength; i += length, eptr += length) {
                /* Parse next content element into new structure */
                errorcode = OSPPASN1ElementDecode(eptr, &cInfo, ospvLevel + 1);

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* Establish the list head pointer */
                    if (hPtr == OSPC_OSNULL) {
                        hPtr = cInfo;
                    }
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* Parsing content element done */

                    /* Get the Length of content element */
                    length = cInfo->ElementLength;

                    /* Attach content info to tail of list */
                    if (tPtr != OSPC_OSNULL) {
                        tPtr->NextElementInfo = cInfo;
                    }
                    /* Establish/Update tail pointer */
                    tPtr = cInfo;
                }

                if (errorcode != OSPC_ERR_NO_ERROR) {
                    break;
                }
            }
            eInfo->ContentElementInfo = hPtr;
        }
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        /* Get rid of base element info structure */
        if (eInfo) {
            OSPPASN1ElementDelete(&eInfo, ospvLevel);
        }
    } else {
        *ospvASN1ElementInfo = eInfo;
    }

    return errorcode;
}

int OSPPASN1ElementCreate(
    OSPTASN1ELEMENTINFO **ospvElementInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    /* Create next content info structure */
    OSPM_MALLOC(eInfo, OSPTASN1ELEMENTINFO, sizeof(OSPTASN1ELEMENTINFO));
    if (eInfo == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate parse element");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(eInfo, 0, sizeof(OSPTASN1ELEMENTINFO));
    }

    *ospvElementInfo = eInfo;

    return errorcode;
}

void OSPPASN1ElementDelete(
    OSPTASN1ELEMENTINFO **ospvElement, 
    unsigned int ospvLevel)
{
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    eInfo = *ospvElement;
    if (eInfo != OSPC_OSNULL) {
        /* Traverse and Delete the peer elements */
        if (eInfo->NextElementInfo != OSPC_OSNULL) {
            OSPPASN1ElementDelete(&(eInfo->NextElementInfo), ospvLevel + 1);
        }

        /* Traverse and Delete the content elements */
        if (eInfo->ContentElementInfo != OSPC_OSNULL) {
            OSPPASN1ElementDelete(&(eInfo->ContentElementInfo), ospvLevel + 1);
        }

        /* All done with elements lists, clean up this element */
        if ((ospvLevel == 0) || (eInfo->ElementSpaceAllocated == 1)) {
            if (eInfo->Element != OSPC_OSNULL) {
                /* Top eInfo in list contains storage for data that was decoded */
                OSPM_FREE(eInfo->Element);
                eInfo->ElementLength = 0;
            }
        }

        if (eInfo != OSPC_OSNULL) {
            /* Everything else is freed up - get rid of the element storage */
            OSPM_FREE(eInfo);
        }

        *ospvElement = OSPC_OSNULL;
    }
}                                /* OSPPASN1ElementDelete */

int OSPPASN1ElementGet(
    OSPEASN1DATAREFID ospvDataReferenceId, 
    OSPTASN1PARSERESULT *ospvParseResults, 
    OSPTASN1ELEMENTINFO **ospvElementInfo)
{
    int errorcode;

    errorcode = PTPResultsGetElement(ospvDataReferenceId, ospvParseResults, ospvElementInfo);

    return errorcode;
}

/* OSPPASN1ElementParse 
    Walk through element tree in a depth first search (child ptr then next
    ptr).  Validate each element against the parse table. 

    Process each element by starting another parse with the appropriate
    parsetable, generate a primitive and add it to the return list, etc.

    Element Info is a pointer to the current ElementInfo Tree node
    (possibly NULL) that is being processed.

    ParseTableId is the reference code for the parse table that is to be
    used to parse ElementInfo.

    ParseResult is a list of data references that were generated while
    parsing ElementInfo with ParseTableId.  The dynamically allocated
    results list is returned to the caller where they are appended to the
    inprogress list.

    DataReferenceIdPrefix is the most recent value of the DataReferenceId.
    It is an array of bytes (allowing 255 values per level).  Initially
    NULL, it is extended with the tag for the element whose value is being
    added to the parse results list, and passed to AddParseResults.
    AddParseResults includes the parameter in the parse results record. 
    If the rule allows multiple occurances of the element, then the same
    reference is used for each parse result added to the list.  The
    extraction routine will know to retrieve all occurances of the
    reference from the parse result table.  If the part to be added to the 
    DataReferenceId is 0, then don't append anything.  This behavior may
    Cascade somewhat. (see SignatureAlgorithm Parse Table Definition)
*/
int OSPPASN1ElementParse(
    OSPTASN1ELEMENTINFO *ospvElementInfo,
    OSPEASN1PARSETABLEID ospvParseTableId,
    OSPTASN1PARSERULE *ospvParentParseRule, 
    OSPTASN1PARSERESULT **ospvParseResult, 
    unsigned char ospvDataRef)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1PARSERULE *parseRule = OSPC_OSNULL;
    /* Pointer to the parse table to use */
    unsigned nextRule = 0;
    char msg[100];
    unsigned maxCount = 0;

    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfoContent = OSPC_OSNULL;

    OSPTASN1PARSERESULT *newResults = OSPC_OSNULL;
    OSPTASN1PARSERESULT *lastResult = OSPC_OSNULL;

    OSPEASN1PARSETABLEID parseTableId = OSPEPTID_NOTDEFINED;

    OSPM_ARGUSED(ospvParentParseRule);

    /* It is ok to have a null ElementInfo pointer.  Might be processing an
       rule for OPTIONAL data and there isn't any, i.e. Extensions, etc. */

    eInfo = ospvElementInfo;

    /* Get the first rule from the specified table */
    nextRule = 0;
    errorcode = PTPTableGetRule(ospvParseTableId, &parseRule, &nextRule);

    maxCount = parseRule->MaximumCount;

    if ((errorcode == OSPC_ERR_NO_ERROR) && (parseRule == OSPC_OSNULL)) {
        errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
        OSPM_DBGERRORLOG(errorcode, "No rules, but there are elements to process.");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Now we have a rule to process. */

        /* Compare the PT entry tag against the current element tag. */
        if ((eInfo == OSPC_OSNULL) || (OSPM_BASE_TAG(parseRule->Tag) != OSPM_BASE_TAG(eInfo->Tag))) {
            if (PTPRuleIsDerived(parseRule) || PTPRuleIsDERFormat(parseRule)) {
                /* Falls through, handled below */
                errorcode = OSPC_ERR_NO_ERROR;
            } else if (PTPRuleIsOptional(parseRule)) {
                /* Element is optional, not a parse error - YET, we are
                   done parseing this RULE - bye */
                errorcode = OSPC_ERR_ASN1_PARSE_DEFAULTED;
            } else if (parseRule->HasDefault) {
                /* Parse Results Added */
                errorcode = PTPAddParseResults(parseRule, OSPC_OSNULL, ospvParseResult, ospvDataRef);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* We are done with this element */
                    errorcode = OSPC_ERR_ASN1_PARSE_DEFAULTED;
                }
            } else {
                /* Tag mismatch and element is not optional - parse error */
                errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
                if (eInfo != OSPC_OSNULL) {
                    sprintf(msg, "Tag mismatch, pt = %02x, e = %02x", parseRule->Tag, eInfo->Tag);
                } else {
                    sprintf(msg, "Tag mismatch, pt = %02x, eInfo=NULL", parseRule->Tag);
                }
                OSPM_DBGERRORLOG(errorcode, msg);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Parse rule matches element, Process it. Change Tables */

        /* Parse Results Added for all elements */
        errorcode = PTPAddParseResults(parseRule, eInfo, ospvParseResult, ospvDataRef);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        lastResult = *ospvParseResult;
        newResults = OSPC_OSNULL;

        /* If rule is derived from another set of rules, then need to parse
           the element with the new rule. */
        if (PTPRuleIsDerived(parseRule)) {
            errorcode = OSPPASN1ElementParse(eInfo, parseRule->ParseTableId, parseRule, &newResults, parseRule->DataReference);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* Prefix generated DataRef's with this elements data ref */
                PTPResultUpdateDataRef(ospvDataRef, newResults);
                lastResult->NextResult = newResults;
            }
        } else if (PTPRuleIsPrimitive(parseRule)) {
            /* If the parseTable says this element is a primitive, 
               then the child pointers in the
               element Infos can be ignored.  THERE MIGHT BE children, but
               the parsetable is not interested in the children, this is
               the case for an algorithm id - all we might be interested
               in an algorithm id for is for it's DER value for the entire
               structure.
             */

            /* If primitive can be a list (max >= 1), then all peers of
               this element should be the same element and should all be
               added to the list. Add Parse Result assumes this and adds a
               result for the element passed, and then traverses the
               element's "next" pointer to add entries for each of the
               elements it finds.  It returns when an element's
               "next" pointer is NULL.  
             */

            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
        } else {
            /* The rule has sub-rules, process each of them */

            /* Parse rule is for a constructed data type.  These types are
               SEQUENCE, SEQUENCE-OF, SET, SET-OF, or CONSTRUCTED
               (contruction is derived from another parse table). */

            /*  Set an pointer to the element's child pointer.  Follow
               the child pointer to find first element in the
               construction.  Parse this element using the next parse
               rule in the table.  Continue parsing elements,
               following NEXT pointers until NEXT is null.  Use the
               next parse rule in the table for each jump following
               the next pointer.  

               Always follow each rule left in the parse table until 
               you run out, even if the NEXT pointer to be
               passed into the parse routine is NULL.  If the rule
               REQUIRES a value, then an error will occur and the
               parse will fail.  If the values are optional, then all
               is well.

               There should NOT be any elements left (following NEXT
               pointers) when the rules run out.  
               If there is, then it is a parse error.
             */

            /* Set pointer to first child component, other children are 
               found by following this child's NEXT pointer. */

            eInfoContent = eInfo->ContentElementInfo;

            /* Loop until all children (following first child's NEXT
               pointers, are consumed */
            while (errorcode == OSPC_ERR_NO_ERROR) {
                /* Get the next rule to follow */
                errorcode = PTPTableGetRule(ospvParseTableId, &parseRule, &nextRule);

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* Rule might have been found.  If not, then the
                       eInfoContent pointer MUST be NULL */

                    if (parseRule == OSPC_OSNULL) {
                        if (eInfoContent != OSPC_OSNULL) {
                            /* No rule, and there are still more elements
                               left to process - oh no! Maybe there are more
                               than one of these allowed?  Try that. */
                            if ((--maxCount) > 0) {
                                /* How about that, more allowed */
                                nextRule = 1;
                                errorcode = PTPTableGetRule(ospvParseTableId, &parseRule, &nextRule);
                            } else {
                                errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
                                OSPM_DBGERRORLOG(errorcode, "Rules ran out before elements did.");
                            }
                        } else {
                            /* No rules, no elements, Success */
                            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
                        }
                    }
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* We have at least a Rule and an ElementInfo (possible
                       NULL).  Let's try to parse it */

                    newResults = OSPC_OSNULL;

                    if (PTPRuleIsPrimitive(parseRule)) {

                        /* The element is primitive, save it on results */
                        if (errorcode == OSPC_ERR_NO_ERROR) {
                            /* Parse Results Added */
                            errorcode = PTPAddParseResults(parseRule, eInfoContent, &newResults, parseRule->DataReference);

                        }
                    } else {
                        errorcode = PTPRuleGetParseTableId(parseRule, &parseTableId);

                        if (errorcode == OSPC_ERR_NO_ERROR) {
                            /* Try to parse the element */
                            errorcode = OSPPASN1ElementParse(eInfoContent, parseTableId, parseRule, &newResults, parseRule->DataReference);
                        }
                    }

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* Get ready to do next element */
                        if (eInfoContent != OSPC_OSNULL) {
                            eInfoContent = eInfoContent->NextElementInfo;
                        }
                    }

                    if (errorcode == OSPC_ERR_ASN1_PARSE_DEFAULTED) {
                        /* Current element was not processed - Keep going
                           through the loop with the same content - Result was
                           created - indicating default value is used */
                        errorcode = OSPC_ERR_NO_ERROR;
                    }

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* Prefix this element's DataRef value to results */
                        if (newResults != OSPC_OSNULL) {
                            PTPResultUpdateDataRef(ospvDataRef, newResults);
                        }
                    }

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* If return parse results value is null, set it
                         */
                        if (*ospvParseResult == OSPC_OSNULL) {
                            /* First time for this element */
                            *ospvParseResult = newResults;
                            lastResult = newResults;
                        } else {
                            /* Add another batch of results */
                            lastResult->NextResult = newResults;
                        }

                        /* Find the end of the current result list */
                        for (; lastResult->NextResult != OSPC_OSNULL; lastResult = lastResult->NextResult);
                    }
                }
            }

            /* If successfully terminated parsing, then fix errorcode */
            if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE) {
                errorcode = OSPC_ERR_NO_ERROR;
            }
        }
    }

    /* errorcode will be a real error or OSP_ERR_ASN1_PARSE_COMPLETE */
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE) {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    return errorcode;
}

void OSPPASN1ElementParseDelete(
    OSPTASN1PARSERESULT **ospvParseResults)
{
    PTPResultsDelete(ospvParseResults);
}

int OSPPASN1ElementCopy(
    OSPTASN1ELEMENTINFO **ospvDstElement, 
    OSPTASN1ELEMENTINFO *ospvSrcElement)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    /* NOTE:  This function does not copy an entire element TREE.  It only
       copies the the data for the element. */

    /* Create a new element. */
    errorcode = OSPPASN1ElementCreate(&eInfo);

    /* Copy the contents of the src element into the dst element. */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MEMCPY(eInfo, ospvSrcElement, sizeof(OSPTASN1ELEMENTINFO));

        /* Create A buffer for the element data */
        errorcode = OSPPASN1ElementCopyElementData(ospvSrcElement, &(eInfo->Element), &(eInfo->ElementLength));
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        /* Get rid of allocated space */
        if (eInfo->Element) {
            OSPM_FREE(eInfo->Element);
        }

        if (eInfo) {
            OSPM_FREE(eInfo);
        }
    } else {
        /* Finish off the new element */

        /* Create a pointer to the content buffer */
        eInfo->ElementSpaceAllocated = 1;
        eInfo->Content = eInfo->Element + (eInfo->ElementLength - eInfo->ContentLength);

        /* Null pointers:ContentElementInfo, NextElementInfo, NextElement */
        eInfo->ContentElementInfo = OSPC_OSNULL;
        eInfo->NextElementInfo = OSPC_OSNULL;

        *ospvDstElement = eInfo;
    }

    return errorcode;
}

int OSPPASN1ElementDeparse(
    OSPTASN1ELEMENTINFO **ospvElementInfo,
    OSPTASN1PARSERESULT **ospvParseResults, 
    OSPEASN1PARSETABLEID ospvParseTableId, 
    unsigned char *ospvDataReference)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    int dataRefsMatch = 0;
    unsigned nextRule = 0;
    OSPTASN1ELEMENTINFO *elementInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *lastContentElement = OSPC_OSNULL;
    OSPTASN1PARSERULE *parseRule = OSPC_OSNULL;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *newElements = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *foundElement = OSPC_OSNULL;
    unsigned char dataReference[OSPC_ASN1_DATAREF_MAXLENGTH];
    unsigned foundCount = 0;

    if (ospvElementInfo != OSPC_OSNULL) {
        elementInfo = *ospvElementInfo;
    }

    if (ospvParseResults != OSPC_OSNULL) {
        parseResults = *ospvParseResults;
    }

    /* Loop through the parse table entries */
    for (nextRule = 0; errorcode == OSPC_ERR_NO_ERROR;) {
        /* Get the next rule - done if it returns PARSE_COMPLETE */
        errorcode = PTPTableGetRule(ospvParseTableId, &parseRule, &nextRule);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_MEMCPY(dataReference, ospvDataReference, OSPC_ASN1_DATAREF_MAXLENGTH);
            dataRefsMatch = PTPDataReferencesMatch(ospvDataReference, parseResults->DataReference);
            /* Figure out what to do with this type of rule */
            if (PTPRuleIsDerived(parseRule) && !dataRefsMatch) {
                /* Need to process these through another table.  Won't add
                   anything to element list for this rule right now */

                /* Copy and update datareference with the value of the next
                   element, start with a fresh copy - these guys are brothers */
                errorcode = PTPDataRefAddRef(dataReference, parseRule->DataReference);

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* Deparse this section of the element list */
                    errorcode = OSPPASN1ElementDeparse(&newElements, &parseResults, parseRule->ParseTableId, dataReference);
                }

            } else if (PTPRuleIsPrimitive(parseRule)) {
                /* Rule is a primitive or a DER format element (one that
                   would break down further if the parse rules called for
                   it).  There should be an entry in the result table for data
                   reference for this element.  

                   If the result is not found, and it is optional, then skip
                   it and process the next rule.  If it is not optional,
                   report an error.

                   If the result is found, there may be more than one. If the
                   rules allow for this, then add each one in order.  They
                   should follow each other in the result list.  If more than
                   one is not allowed, then report an error.  

                   If a result is found, then link it into the current
                   position in the element list.
                 */

                foundCount = 0;

                /* If there are any parse results left, try to find the
                   primitive described in the rule. */

                if (parseResults != OSPC_OSNULL) {
                    /* Get the next element from the results list.  */
                    OSPM_MEMCPY(dataReference, ospvDataReference, OSPC_ASN1_DATAREF_MAXLENGTH);
                    errorcode = PTPDataRefAddRef(dataReference, parseRule->DataReference);

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* First parse result in the list (remainder of list
                           really) should have a matching dataReference */ ;
                        if (!PTPDataReferencesMatch(dataReference, parseResults->DataReference)) {
                            /* ParseResult not found. Ok if it is optional */
                            if (!PTPRuleIsOptional(parseRule)) {
                                /* So it isn't optional.  Still ok if the
                                   element has a default value */
                                if (!parseRule->HasDefault) {
                                    /* Ok, so we have a problem */
                                    errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
                                    OSPM_DBGERRORLOG(errorcode, "DataReference mismatch");
                                } else {
                                    /* Rule has default value, so treat as
                                       optional */
                                    foundElement = OSPC_OSNULL;
                                }

                            } else {
                                /* Rule is optional, Null thing out */
                                foundElement = OSPC_OSNULL;
                            }
                        } else {
                            /* Found at least one match. Capture it as root */
                            foundElement = parseResults->ElementInfo;
                            foundCount++;
                        }
                    }

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* Hey we found something we were looking for. Capture
                           the first one to the as the head of the list */
                        newElements = foundElement;

                        /* Update the parse results ptr to consume the found 
                           element */
                        parseResults = parseResults->NextResult;

                        while ((errorcode == OSPC_ERR_NO_ERROR) && (parseResults != OSPC_OSNULL)) {
                            /* Is this another match */
                            if (PTPDataReferencesMatch(dataReference, parseResults->DataReference)) {
                                /* Found another match. Is that ok? */
                                foundCount++;

                                /* Add the element to the current list */
                                foundElement->NextElementInfo = parseResults->ElementInfo;

                                /* Update the pointers */
                                parseResults = parseResults->NextResult;
                                foundElement = foundElement->NextElementInfo;
                            } else {
                                break;
                            }
                        }
                    }
                }

                /* Test # of occurances */
                if (foundCount < parseRule->MinimumCount) {
                    errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
                    OSPM_DBGERRORLOG(errorcode, "Found less than " "the minimum number of an element");
                } else if (foundCount > parseRule->MaximumCount) {
                    errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
                    OSPM_DBGERRORLOG(errorcode, "Found more than " "the maximum number of an element");
                }

                foundCount = 0;
                foundElement = OSPC_OSNULL;
            } else {
                /* If not derived and not primitive, then it must be a
                   constructed rule.  */
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* Check for a match with rule and consume the parse
                       result. */

                    if (dataRefsMatch) {
                        /* Rule is constructed, but we want to capture the
                           completed encoding */
                        elementInfo = parseResults->ElementInfo;
                        parseResults = parseResults->NextResult;

                        if (elementInfo->Element != OSPC_OSNULL) {
                            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
                        }
                    } else {
                        if (PTPResultIsRuleComponent(ospvDataReference, parseResults->DataReference)) {
                            /* Wasn't a match, so assume that the construct
                               needs to be added to the element list, leave the
                               parseResults alone. */

                        } else {
                            if (!PTPRuleIsOptional(parseRule)) {
                                errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
                                OSPM_DBGERRORLOG(errorcode, "Required Rule has no results on list");
                            } else {
                                errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
                            }
                        }
                    }
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    if (elementInfo == OSPC_OSNULL) {
                        errorcode = OSPPASN1ElementCreate(&elementInfo);
                    }
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* Update the element tag with the tag from the 
                       parse rule, this element is constructed, so 
                       apply the constructed flags */
                    if (elementInfo->Tag == 0) {
                        elementInfo->Tag = (unsigned char) OSPM_CONSTRUCTED_TAG(parseRule->Tag);
                    }
                }

                /* Can't flesh out the rest of the fields until something
                   gets built. That will happen when the rest of the rules are
                   processed. */
                newElements = OSPC_OSNULL;
            }

            /* If no errors, and there is new data, then add it to the list */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* Setup for next iteration */
                if (newElements != OSPC_OSNULL) {
                    if (elementInfo == OSPC_OSNULL) {
                        elementInfo = newElements;
                    } else {
                        /* Add the new element(s) to the element list */
                        if (elementInfo->ContentElementInfo == OSPC_OSNULL) {
                            /* Add it to the beginning of the list */
                            elementInfo->ContentElementInfo = newElements;
                        } else {
                            /* Add it as a subsequent element */
                            lastContentElement->NextElementInfo = newElements;
                        }
                    }
                    lastContentElement = newElements;
                    newElements = OSPC_OSNULL;
                }
            }
        }
    }

    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE) {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvElementInfo = elementInfo;
        *ospvParseResults = parseResults;

        errorcode = OSPPASN1ElementEncode(*ospvElementInfo);
    }

    return errorcode;
}

int OSPPASN1ElementFormat(
    OSPTASN1ELEMENTINFO **ospvElement,
    unsigned char *ospvTag, 
    unsigned char ospvTagFlags, 
    unsigned ospvTagLength, 
    unsigned char *ospvData, 
    unsigned ospvDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    unsigned char *lengthBuffer = OSPC_OSNULL;
    unsigned char *dataBuffer = OSPC_OSNULL;
    unsigned int dataBufferLength = 0;
    unsigned lengthLength = 0;
    unsigned i = 0;

    /* NOTE:  For what we are doing, high order tags are not used.  They
       are not supported. */

    errorcode = OSPPASN1SmallInt2UnsignedChar(ospvDataLength, 256, &lengthBuffer, &lengthLength);
    dataBufferLength = ospvDataLength + ospvTagLength + lengthLength;
    if (ospvDataLength > 127) {
        dataBufferLength++;        /* Add a byte for length mode 
                                   indicator if length is >127 */
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementCreate(&eInfo);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(dataBuffer, unsigned char, dataBufferLength);

        if (dataBuffer == OSPC_OSNULL) {
            errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Error allocating encoded element buffer");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Initialize the element Info */
        /* Set the tag */
        if (ospvTagLength != 1) {
            errorcode = OSPC_ERR_ASN1_UNSUPPORTED_TAG;
            OSPM_DBGERRORLOG(errorcode, "Unsupported, high-order tag (size != 1) encountered");
        } else {
            eInfo->Tag = (unsigned char) (ospvTag[0] | ospvTagFlags);
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            i = 0;
            dataBuffer[i++] = (unsigned char) (ospvTag[0] | ospvTagFlags);

            /* Set the length */
            if (ospvDataLength > 127) {
                dataBuffer[i++] = (unsigned char) (lengthLength | OSPC_BER_LENGTH_MODE_LONG);
            }
            /* Copy length buffer to the databuffer */
            OSPM_MEMCPY(&(dataBuffer[i]), lengthBuffer, lengthLength);
            i += lengthLength;

            /* Copy the data to the databuffer */
            OSPM_MEMCPY(&(dataBuffer[i]), ospvData, ospvDataLength);

            /* Set element pointers appropriately */
            eInfo->ContentLength = ospvDataLength;
            eInfo->Content = (ospvDataLength) ? &(dataBuffer[i]) : OSPC_OSNULL;

            eInfo->Element = dataBuffer;
            eInfo->ElementLength = (dataBufferLength);
            eInfo->ElementSpaceAllocated = 1;    /* Mark for deletion */
        }
    }

    if (lengthBuffer != OSPC_OSNULL) {
        OSPM_FREE(lengthBuffer);
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPM_FREE(eInfo);
        OSPM_FREE(dataBuffer);
    } else {
        *ospvElement = eInfo;
    }

    return errorcode;
}
