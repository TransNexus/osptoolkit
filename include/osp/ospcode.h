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
 * ospcode.h - Status codes and strings for status element
 */

#ifndef _OSPCODE_H
#define _OSPCODE_H

typedef struct statuscode {
    unsigned ospmCode;
    unsigned char *ospmDesc;
} OSPTSTATLIST;

#define MAX_STATS   23

OSPTSTATLIST statlist[MAX_STATS] = {
    /* 2xx  =  operation successful */
    {200, "success"},
    {201, "information created"},
    {210, "updated information accepted"},
    /* 4xx  =  client error */
    {400, "bad request"},
    {401, "unauthorised"},
    {410, "character encoding not supported"},
    {411, "parsing unsuccessful"},
    {412, "critical element not supported"},
    {420, "generic security problem"},
    {421, "signature invalid"},
    {422, "cryptographic algorithm not supported"},
    {423, "certificate invalid"},
    {424, "certificate revoked"},
    {425, "encryption required"},
    /* 5xx  =  server error */
    {500, "internal server error"},
    {501, "not implemented"},
    {503, "service not available"},
    {510, "transient problem in server"},
    {520, "long term problem in server"},
    {530, "time problem"},
    {531, "valid time too soon"},
    {532, "time interval too small"},
    {999, "generic failure"}
};

#endif /* _OSPCODE_H */
