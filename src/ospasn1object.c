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
 * ospasn1object.c  Functions to implement PKCS7 ASN1 Objects 
 */

#include "osp/osp.h"
#include "osp/ospasn1.h"

#define OSPC_ERR_PKCS7                  25500
#define OSPC_ERR_PKCS7_MALLOC_FAILED    (OSPC_ERR_PKCS7 + 1000)

#define OSPC_SIGNEDDATA_VERSION 1

/* FUNCTION PROTOTYPES */

/*
 * Member functions
 */

int OSPPASN1ObjectGetElementInfo(
    OSPTASN1OBJECT *ospvObject, 
    OSPTASN1ELEMENTINFO **ospvElementInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvObject == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_NULL_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Invalid pointer to Object");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvElementInfo = ospvObject->ElementInfo;
    }

    return errorcode;
}

int OSPPASN1ObjectGetParseResults(
    OSPTASN1OBJECT *ospvObject, 
    OSPTASN1PARSERESULT **ospvParseResults)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvObject == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_NULL_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Invalid pointer to Object");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvParseResults = ospvObject->ParseResults;
    }

    return errorcode;
}

int OSPPASN1ObjectNew(
    OSPTASN1OBJECT **ospvASN1Object, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *newElement = OSPC_OSNULL;
    OSPTASN1PARSERESULT *newResult = OSPC_OSNULL;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get a new Element Info */
        errorcode = OSPPASN1ElementCreate(&newElement);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = PTPResultsCreate(&newResult, newElement, ospvDataRefId);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectCreate(&newObject, newElement, newResult);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvASN1Object = newObject;
    } else {
        OSPPASN1ObjectDelete(&newObject);
        OSPPASN1ElementDelete(&newElement, 0);
        PTPResultsDelete(&newResult);
    }

    return errorcode;
}

int OSPPASN1ObjectCreate(
    OSPTASN1OBJECT **ospvASN1Object, 
    OSPTASN1ELEMENTINFO *ospvElementInfo, 
    OSPTASN1PARSERESULT *ospvParseResults)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;

    OSPM_MALLOC(newObject, OSPTASN1OBJECT, sizeof(OSPTASN1OBJECT));
    if (newObject == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to create an ASN1 Object");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Init the new object */
        OSPM_MEMSET(newObject, 0, sizeof(OSPTASN1OBJECT));

        newObject->ElementInfo = ospvElementInfo;
        newObject->ParseResults = ospvParseResults;
        *ospvASN1Object = newObject;
    } else {
        OSPPASN1ObjectDelete(&newObject);
    }

    return errorcode;
}

void OSPPASN1ObjectDelete(OSPTASN1OBJECT ** ospvASN1Object)
{
    OSPTASN1OBJECT *object = OSPC_OSNULL;

    if (ospvASN1Object != OSPC_OSNULL) {
        object = *ospvASN1Object;
        if (object != OSPC_OSNULL) {
            OSPPASN1ElementDelete(&(object->ElementInfo), 0);
            PTPResultsDelete(&(object->ParseResults));

            OSPM_FREE(*ospvASN1Object);
            *ospvASN1Object = OSPC_OSNULL;
        }
    }
}

int OSPPASN1ObjectCopyElementObject(
    OSPTASN1OBJECT **ospvFoundObject, 
    OSPTASN1OBJECT *ospvParentObject, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT foundObject;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    OSPM_MEMSET(&foundObject, 0, sizeof(OSPTASN1OBJECT));

    errorcode = OSPPASN1ObjectGetParseResults(ospvParentObject, &parseResults);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = PTPResultsGetElement(ospvDataRefId, parseResults, &eInfo);
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = PTPResultsCreate(&parseResults, eInfo, ospvDataRefId);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        foundObject.ElementInfo = eInfo;
        foundObject.ParseResults = parseResults;

        errorcode = OSPPASN1ObjectCopy(ospvFoundObject, &foundObject);

        PTPResultsDelete(&parseResults);
    }

    return errorcode;
}

int OSPPASN1ObjectGetElementByDataRef(
    OSPTASN1OBJECT *ospvObject, 
    OSPTASN1ELEMENTINFO **ospvElementInfo, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1OBJECT *foundObject = OSPC_OSNULL;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectCopyElementObject(&foundObject, ospvObject, ospvDataRefId);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectGetElementInfo(foundObject, ospvElementInfo);
    }

    if (foundObject != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&(foundObject->ParseResults->ElementInfo), 0);
        OSPM_FREE(foundObject->ParseResults);
        OSPM_FREE(foundObject);
    }

    return errorcode;
}

/* ObjectCopy 
This routine only copies the object and it's immediate children,i.e. the
element info and parse results that are pointed to by it's pointers.  It
DOES NOT recurse through the element info linked list(s), or grab all of
the results in the parse results linked list. */

int OSPPASN1ObjectCopy(
    OSPTASN1OBJECT **ospvDestObject, 
    OSPTASN1OBJECT *ospvSrcObject)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *newElement = OSPC_OSNULL;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;
    OSPTASN1PARSERESULT *newResult = OSPC_OSNULL;

    /* Create the new object */
    OSPM_MALLOC(newObject, OSPTASN1OBJECT, sizeof(OSPTASN1OBJECT));

    if (newObject == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to create an ASN1 Object for copy");
    } else {
        OSPM_MEMSET(newObject, 0, sizeof(newObject));
    }

    /* Get the element info from the source object */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectGetElementInfo(ospvSrcObject, &eInfo);
    }

    /* Get the parse results from the source object */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectGetParseResults(ospvSrcObject, &parseResults);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementCopy(&newElement, eInfo);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = PTPResultsCopy(&newResult, parseResults);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        newObject->ElementInfo = newElement;
        newObject->ParseResults = newResult;
        *ospvDestObject = newObject;
    } else {
        OSPM_FREE(newElement);
        OSPM_FREE(newResult);
        OSPM_FREE(newObject);
    }

    return errorcode;
}

int OSPPASN1ObjectAddChild(
    OSPTASN1OBJECT *ospvParent, 
    OSPTASN1OBJECT *ospvChild, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *parentElement = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *childElement = OSPC_OSNULL;
    OSPTASN1PARSERESULT *tmpResults = OSPC_OSNULL;
    OSPTASN1PARSERESULT *lastResult = OSPC_OSNULL;

    unsigned char *tmpDataRef = OSPC_OSNULL;

    /* Add the child's eInfo tree to the parent's eInfo tree on the end of
       the contentElementInfo chain. Then add the child's result list to the 
       end of the parents result list. In both cases, use the data provided
       without allocating new data. */

    errorcode = OSPPASN1ObjectGetElementInfo(ospvParent, &parentElement);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectGetElementInfo(ospvChild, &childElement);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get parents current parse results */
        errorcode = OSPPASN1ObjectGetParseResults(ospvParent, &tmpResults);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Go to the end of the list */
        lastResult = PTPResultsEndOfList(tmpResults);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the childs parse results */
        errorcode = OSPPASN1ObjectGetParseResults(ospvChild, &tmpResults);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Update the child's dataReference */
        errorcode = PTPDataRefIdGetValue(ospvDataRefId, &tmpDataRef);
        OSPM_MEMCPY(tmpResults->DataReference, tmpDataRef, OSPC_ASN1_DATAREF_MAXLENGTH);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Concatenate them to the list */
        lastResult->NextResult = tmpResults;
    }

    return errorcode;
}

/* OSPPASN1ObjectDeparse
    Walk through the parse tables using a depth first search.  Build an
    element list for the object by adding elements to handle Sequences and
    Sets, etc.  When a primitive element encountered in
    the tables, look for a matching result in the objects result list.  If
    found, add it to the parse table being built.  This sets you up to
    encode the parsed object by scanning the element list.
*/
int OSPPASN1ObjectDeparse(
    OSPTASN1OBJECT *ospvObject, 
    OSPEASN1PARSETABLEID ospvParseTableId, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1PARSERESULT *origParseResults;
    OSPTASN1PARSERESULT *parseResults;
    OSPTASN1ELEMENTINFO *elementInfo;
    unsigned char dataReference[OSPC_ASN1_DATAREF_MAXLENGTH];
    unsigned char *dataReferencePtr = OSPC_OSNULL;

    /* Get the process started. Call ElementDeparse to generate the element list */

    OSPM_MEMSET(dataReference, 0xff, sizeof(dataReference));
    errorcode = PTPDataRefIdGetValue(ospvDataRefId, &dataReferencePtr);

    /* Need to get the parse results from the object */
    errorcode = OSPPASN1ObjectGetParseResults(ospvObject, &origParseResults);
    parseResults = origParseResults;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        elementInfo = ospvObject->ElementInfo;

        /* Generate the element list first - this is a recursive call */
        errorcode = OSPPASN1ElementDeparse(&elementInfo, &parseResults, ospvParseTableId, dataReferencePtr);
    }

    return errorcode;
}

int OSPPASN1ObjectEncode(
    OSPTASN1OBJECT **ospvEncodedObject,
    unsigned char *ospvTag,
    unsigned ospvTagLength,
    unsigned char ospvTagFlags, 
    unsigned char *ospvContent, 
    unsigned ospvContentLength, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *encodedObject = OSPC_OSNULL;

    errorcode = OSPPASN1ElementFormat(&eInfo, ospvTag, ospvTagFlags, ospvTagLength, ospvContent, ospvContentLength);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectFormat(&encodedObject, eInfo, ospvDataRefId);
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        if (eInfo != OSPC_OSNULL) {
            OSPPASN1ElementDelete(&eInfo, 0);
        }
        if (encodedObject != OSPC_OSNULL) {
            OSPPASN1ObjectDelete(&encodedObject);
        }
    } else {
        *ospvEncodedObject = encodedObject;
    }

    return errorcode;
}

int OSPPASN1ObjectFormat(
    OSPTASN1OBJECT **ospvObject, 
    OSPTASN1ELEMENTINFO *ospvElement, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1OBJECT *object = OSPC_OSNULL;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;

    OSPM_MALLOC(object, OSPTASN1OBJECT, sizeof(OSPTASN1OBJECT));

    if (object == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for new object");
    } else {
        OSPM_MEMSET(object, 0, sizeof(object));
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = PTPResultsCreate(&parseResults, ospvElement, ospvDataRefId);
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPM_FREE(object);
        OSPM_FREE(parseResults);
    } else {
        object->ElementInfo = ospvElement;
        object->ParseResults = parseResults;
        *ospvObject = object;
    }

    return errorcode;
}
