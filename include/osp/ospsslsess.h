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
 * ospsslsess.h - SSL Session definitions
 */
#ifndef _OSPSSLSESS_H
#define _OSPSSLSESS_H

/*---------------------------------------*/
/* SSL Session Id typedef                */
/*---------------------------------------*/ 
typedef struct _OSPTSSLSESSIONID
{
    OSPTLISTLINK      Link;
    unsigned long     Length;
    unsigned char    *Value;
    char             *HostName;
    OSPTTIME          IssuedTime;
    short             Port;
    short             pad;
} OSPTSSLSESSIONID;

/*---------------------------------------*/
/* SSL Session typedef                   */
/*---------------------------------------*/ 
typedef struct _OSPTSSLSESSION
{
    OSPTLISTLINK      Link;
    void             *Context;
    void             *RandomRef;
    OSPTSSLSESSIONID *SessionId;
} OSPTSSLSESSION;

#endif
