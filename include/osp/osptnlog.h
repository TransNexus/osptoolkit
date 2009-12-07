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

#ifndef _OSPTNLOG_H
#define _OSPTNLOG_H

#define LOGPERM     (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)
#define MAXLOGSZ    100000
#define BUFSZ       8192
#define DUMPLEN     16

#ifndef _WIN32
#ifdef OSPTNLOGDEBUG
#define OSPTNINITLOG(a)         tninitlog(a)
#define OSPTNLOG(a)             tnlog(a)
#define OSPTNLOG1(a,b)          tnlog(a,b)
#define OSPTNLOG2(a,b,c)        tnlog(a,b,c)
#define OSPTNLOG3(a,b,c,d)      tnlog(a,b,c,d)
#define OSPTNLOG4(a,b,c,d,e)    tnlog(a,b,c,d,e)
#define OSPTNLOGDUMP(a,b,c)     tnlogdump(a,b,c)
#define OSPTNLOGMEMUSE(a)       tnlogmemuse(a)
#else
#define OSPTNINITLOG(a)
#define OSPTNLOG(a)
#define OSPTNLOG1(a,b)
#define OSPTNLOG2(a,b,c)
#define OSPTNLOG3(a,b,c,d)
#define OSPTNLOG4(a,b,c,d,e)
#define OSPTNLOGDUMP(a,b,c)
#define OSPTNLOGMEMUSE(a)
#endif

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void get_log_time(char *timebuf);
    void tnlog(char *fmt, ...);
    int tnlogdump(unsigned char *data, int len, char *msg);
    int tninitlog(char *filename);
    void tnlogmemuse(char *string);

#ifdef __cplusplus
}
#endif
#else /* WIN32 defined */
#define OSPTNINITLOG(a)
#define OSPTNLOG(a)
#define OSPTNLOG1(a,b)
#define OSPTNLOG2(a,b,c)
#define OSPTNLOG3(a,b,c,d)
#define OSPTNLOG4(a,b,c,d,e)
#define OSPTNLOGDUMP(a,b,c)
#define OSPTNLOGMEMUSE(a)
#endif /* WIN32 */

#endif /* _OSPTNLOG_H */
