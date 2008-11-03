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

#include "osp/ospasn1.h"

/*************************************************************************
 * Structure definitions:
 *************************************************************************/

/*
 * This structure should be constructed so that it adequately describes
 * any parameters that may be taken from the command line. Other structures
 * can be used to describe the type of operation being performed by the
 * enrollment client. 
 */
typedef struct _OSPTENROLLPARAMS {
    unsigned char *Function;
    unsigned char *Username;
    unsigned char *Password;
    unsigned char *DeviceId;
    unsigned char *CustomerId;
    unsigned char *Nonce;
    unsigned NonceLen;
    unsigned char *CertReq;
    unsigned char *Subject;
    unsigned char *SSLUrl;
    unsigned char *CAUrl;
    unsigned char *CAFprint;
    unsigned char *CACert;
    unsigned CACertLen;
    unsigned char *CACertB64;
    unsigned CACertB64Len;
    unsigned Verbose;
} OSPTENROLLPARAMS;

/* 
 * This is for specifying what to use for the communication manager's
 * parameters. This will have to be set from outside the enrollment client,
 * preferably in some manner that also suits the OSP SDK. This list may
 * expand or contract, but it should only be used by the functions for
 * collecting this data and for the function that initializes the 
 * communications manager. After that, this list will not be used anywhere
 * else, so changes to function prototypes, variables, and so
 * on should be contained to the functions using this structure.
 *
 * The authority's certificates are included here for validating any
 * information ( e.g., certificates ) that come back.
 */
typedef struct _OSPTCOMMPARAMS {
    unsigned SSLLifetime;
    unsigned HTTPMaxConnections;
    unsigned HTTPPersistence;
    unsigned HTTPRetryDelay;
    unsigned HTTPRetryLimit;
    unsigned HTTPTimeout;
} OSPTCOMMPARAMS;
