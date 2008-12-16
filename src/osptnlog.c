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
 * Copyright (c) 1993 Addison-Wesley Publishing Company.
 * All rights reserved.  This source code is provided
 * for its instructional value, and is not guaranteed
 * for any particular purpose.
 */
#include "osp/osptnlog.h"
#ifndef _WIN32
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <grp.h>
#include <stdlib.h>


#ifdef OSPTNLOGDEBUG
static char buf[BUFSZ];
static char tmpbuf[BUFSZ];
static int logfd = -1;

void get_log_time(char *timebuf)
{
    time_t t;
    char *tm;

    time(&t);
    tm = ctime(&t);
    tm[19] = '\0';
    sprintf(timebuf, "%s %ld", &tm[4], getpid());
    timebuf[15] = '\0';
}


void
tnlog(char *fmt, ...)
{
    va_list ap;

    get_log_time(buf);

    /*
     * Append the caller's string to the buffer.
     */
    va_start(ap, fmt);
    vsprintf(tmpbuf, fmt, ap);
    va_end(ap);

    strcat(buf, tmpbuf);

    if (!isspace(tmpbuf[0])) 
    {
        sprintf( tmpbuf, "\n%s", buf);
        OSPM_STRCPY(buf, tmpbuf);
    }

    /*
     * Add a newline.
     */
    strcat(buf, "\n");

    /*
     * Write the log message.
     */
    write(logfd, buf, OSPM_STRLEN(buf));
}

int
tnlogdump(unsigned char *data, int len, char *msg)
{
    int i ;
    int cnt=0;
    unsigned char c;

    int j=0;
    char hexbuf[(DUMPLEN*3)+5];
    char txtbuf[(DUMPLEN) + 5];

    /*
     * Preprocessing before new dump
     */

    memset(hexbuf, (char) 0, sizeof(hexbuf));
    memset(txtbuf, (char) 0, sizeof(hexbuf));

    if (len) 
    {
        /*
         * Change newline to space.  24 is location
         * of newline.  25 is end of string.  Then
         * copy the string to the buffer.
         */
        get_log_time((char *)tmpbuf);
        sprintf( buf, "\n%s", tmpbuf);

        sprintf(tmpbuf, "LOGDUMP - %s\n", msg);
        strcat(buf, tmpbuf);
        sprintf(tmpbuf, "LENGTH = %6d =========================================================", len);
        strcat(buf, tmpbuf);    
        /*
         * Add a newline.
         */
        strcat(buf, "\n");

        /*
        * Write the log message.
        */
        write(logfd, buf, OSPM_STRLEN(buf));
    } 

    /* 
     * Dump the available data
     */
    for (i=0 ; ; i++, cnt++ ) 
    {
        if (i && !(i%DUMPLEN)) 
        {
            sprintf(buf, "%05d: %s   %s\n", (((i-1)/DUMPLEN)*DUMPLEN), hexbuf, txtbuf);
            write(logfd, buf, OSPM_STRLEN(buf));
            OSPM_STRCPY(hexbuf, "");
            j=0;
            txtbuf[j] = '\0';
            if (i >= len) break;
        }

        c = (i < len)? data[i] : ' ';
        if (isalnum(c) || (c == ' ') || ispunct(c)) 
        {
            txtbuf[j++] = c;
        } 
        else 
        {
            txtbuf[j++] = '.';
        }
        txtbuf[j] = '\0';

        if (i < len) 
        {
            sprintf( tmpbuf, "%02x ", (unsigned int) c );
        } 
        else 
        {
            sprintf( tmpbuf, "   ");
        }
        strcat(hexbuf, tmpbuf);
    }

    /* Flush the log buffer. */
    sprintf(buf, "%s    %s\n", hexbuf, txtbuf);
    strcat(buf, "=========================================================================\n\n");

    write(logfd, buf, OSPM_STRLEN(buf));
    OSPM_STRCPY(hexbuf, "");
    j=0;
    txtbuf[j] = '\0';

    return(cnt);
}


int
tninitlog(char *filename)
{
    struct stat sbuf;
    struct group *gp;
    char logfile[100];
    int fl;

    /* Point to stderr for output if no logfile specification */
    if ((filename == (char *)NULL) || (filename[0] == '\0'))
    {
        OSPM_STRCPY(logfile, "/dev/stdout");
    } 
    else 
    {
        OSPM_STRCPY(logfile, filename);
    }

    /*
     * Open the log file, creating it if it isn't
     * present.
     */

    logfd = open(logfile, O_WRONLY|O_APPEND|O_CREAT,
        LOGPERM);
    if (logfd < 0)
        return(-1);


    /*
     * If the log file is too large, truncate it to 0.
     */
    if (fstat(logfd, &sbuf) == 0) 
    {
        if (sbuf.st_size > MAXLOGSZ)
            ftruncate(logfd, 0);
    }

    /*
     * Set the close-on-exec flag.
     */
    fl = fcntl(logfd, F_GETFD, 0);
    if ((fl < 0) ||
        (fcntl(logfd, F_SETFD, fl|FD_CLOEXEC) < 0)) 
    {
        close(logfd);
        logfd = -1;
        return(-1);
    }

    /*
     * Get the group ID for group "daemon."
     */
    gp = getgrnam("daemon");
    if (gp == NULL) 
    {
        close(logfd);
        logfd = -1;
        errno = EINVAL;
        return(-1);
    }

    /*
     * If the file isn't owned by the super-user or group
     * daemon, change the file's user and group IDs.
     */
    if (fstat(logfd, &sbuf) < 0) 
    {
        close(logfd);
        logfd = -1;
        return(-1);
    }
    if ((sbuf.st_uid != 0) || (sbuf.st_gid != gp->gr_gid))
        fchown(logfd, 0, gp->gr_gid);
    return(0);
}


void tnlogmemuse(char *string)
{
    struct mallinfo sMallInfo; 
    static struct mallinfo sOldMallInfo;
    int total;
    int oldtotal;
    static int gtotal = 0;
    static int testcount = 0;

    sMallInfo = mallinfo(); 

    if (!testcount) 
    {
        memset(&sOldMallInfo, 0, sizeof(sOldMallInfo));
    }   

    testcount ++;
    total = sMallInfo.usmblks + sMallInfo.uordblks;
    gtotal += total;
    oldtotal = sOldMallInfo.usmblks + sOldMallInfo.uordblks;

    tnlog("MEMUSE: (avg: %d)(chg: %7d)(%7ld + %7d = %7d) %-30.30s" ,  
        gtotal/testcount, total - oldtotal, 
        sMallInfo.usmblks, sMallInfo.uordblks, total, string); 

    sOldMallInfo = sMallInfo;

    return;
}
#endif
#endif /* WIN32 */
