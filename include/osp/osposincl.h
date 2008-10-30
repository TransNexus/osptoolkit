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
 * osposincl.h - OS common includes.
 */

#ifndef _OSPOSINCL_H
#define _OSPOSINCL_H

/* OS common headers */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>

/* Applies to both UNIX and WIN32 */
#ifdef _POSIX_THREADS
#include <pthread.h>
#endif

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <winsock.h>
#include <process.h>

#else /* UNIX */

#include <unistd.h>
#include <sys/signal.h>

#ifndef _POSIX_THREADS
#include <synch.h>
#include <thread.h>
#endif

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#endif

#endif /* _OSPOSINCL_H */
