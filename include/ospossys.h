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
 * ospossys.h - OS dependent system macros.
 */
#ifndef _OSPOSSYS_H
#define _OSPOSSYS_H

#include "ospdebug.h"

/*
 * =============================================
 * Memory Management Macros
 * =============================================
 */
#define OSPM_MALLOC(p,t,z) { \
    p = (t *)malloc(z); \
    OSPM_DBG(p == (t *)OSPC_OSNULL, \
        ("%s failed. %s\n", "malloc", strerror(errno))); \
}

#define OSPM_REALLOC(n,p,t,z) { \
    n = (t *)realloc(p, z); \
    OSPM_DBG(p == (t *)OSPC_OSNULL, \
        ("%s failed. %s\n", "realloc", strerror(errno))); \
}

#define OSPM_FREE(p)  if(p!=NULL) { free(p); p=NULL;  }

#define OSPM_MEMCPY(p,s,z) (void)memcpy((void *)p,s,z)

#define OSPM_MEMSET(p,c,z) (void)memset((void *)p,c,z)

#define OSPM_MEMCMP(p1,p2,z) memcmp(p1,p2,z) 

#define OSPM_MEMMOVE(p,s,z) memmove(p,s,z)

#define OSPM_SSCANF(s,f,r) sscanf((const char*)s,f,&r[0])

/*
 * =============================================
 * Thread Macros
 * =============================================
 */

#define OSPPOSEnterCriticalSection()    /* null for now */
#define OSPPOSExitCriticalSection()     /* null for now */


#ifdef _POSIX_THREADS
/* POSIX thread typedefs, applies to UNIX and WIN32 */

#define OSPM_CREATE_THREAD(t,a,f,p,e) { \
    e = pthread_create(&t, a, f, p); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_create", strerror(errno))); \
}

#define OSPM_THRATTR_INIT(a,e) { \
    e = pthread_attr_init(&a); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_attr_init", strerror(errno))); \
}

#define OSPM_THRATTR_DESTROY(a) { \
    int e = 0; \
    e = pthread_attr_destroy(&a); \
    OSPM_DBG(e, ("%s failed. %s\n", "pthread_attr_destroy", strerror(errno))); \
}

#define OSPM_SETDETACHED_STATE(a,e) { \
    e = pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_attr_setdetachstate", strerror(errno))); \
}

/* Mutexes (POSIX) */
#define OSPM_MUTEX_INIT(m,a,e) { \
    e = pthread_mutex_init(&m,a); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_mutex_init", strerror(errno))); \
}

#define OSPM_MUTEX_DESTROY(m,e) { \
    e = pthread_mutex_destroy(&m); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_mutex_destroy", strerror(errno))); \
}

#define OSPM_MUTEX_LOCK(m,e) { \
    e = pthread_mutex_lock(&m); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_mutex_lock", strerror(errno))); \
}

#define OSPM_MUTEX_UNLOCK(m,e) { \
    e = pthread_mutex_unlock(&m); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_mutex_unlock", strerror(errno))); \
}

/* Conditional Variables (POSIX) */
#define OSPM_CONDVAR_INIT(c,a,e) { \
    e = pthread_cond_init(&c,a); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_cond_init", strerror(errno))); \
}

#define OSPM_CONDVAR_DESTROY(c,e) { \
    e = pthread_cond_destroy(&c); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_cond_destroy", strerror(errno))); \
}

#define OSPM_CONDVAR_WAIT(c,m,e) { \
    e = pthread_cond_wait(&c,&m); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_cond_wait", strerror(errno))); \
}

#define OSPM_CONDVAR_TIMEDWAIT(c,m,t,e) { \
    struct timespec abstime; \
    abstime.tv_sec = time((time_t *)0) + t; abstime.tv_nsec = 0; \
    e = pthread_cond_timedwait(&c,&m,&abstime); \
    e = (e == ETIMEDOUT ? OSPC_ERR_OS_CONDVAR_TIMEOUT : e); \
}

#define OSPM_CONDVAR_SIGNAL(c,e) { \
    e = pthread_cond_signal(&c); \
    OSPM_DBG(e,("%s failed. %s\n", "pthread_cond_signal", strerror(errno))); \
}
#endif /* POSIX THREADS */


#ifdef  _WIN32 
#ifndef _POSIX_THREADS
/* MS Threads */
#define OSPM_CREATE_THREAD(t,a,f,p,e) { \
    t = _beginthread(f,0,(void *)p); \
    OSPM_DBG((t < 0),("%s failed. %d\n", "_beginthread", GetLastError())); \
    if (t < 0) \
        e = GetLastError(); \
}

/* currently unused in Win32 Threads */
#define OSPM_THRATTR_INIT(a,e) a = 0, e = OSPC_ERR_NO_ERROR; 
#define OSPM_THRATTR_DESTROY(a) a = 0;
#define OSPM_SETDETACHED_STATE(a,e) a = a, e = OSPC_ERR_NO_ERROR;

/* Mutexes (WIN32) */
#define OSPM_MUTEX_INIT(m,a,e) { \
    m = CreateMutex(NULL, FALSE, NULL); \
    OSPM_DBG(m == NULL,("%s failed. %d\n", "CreateMutex", GetLastError())); \
    e = GetLastError(); \
}

#define OSPM_MUTEX_DESTROY(m,e) { \
    e = CloseHandle(m); \
    if(e) \
        e = OSPC_ERR_NO_ERROR; \
    else \
        OSPM_DBG(e == 0,("%s failed. %d\n", "CloseHandle", GetLastError())); \
}

#define OSPM_MUTEX_LOCK(m,e) { \
    e = WaitForSingleObject(m,INFINITE); \
    OSPM_DBG(e == WAIT_FAILED,("%s failed. %d\n", "WaitForSingleObject", GetLastError())); \
}

#define OSPM_MUTEX_UNLOCK(m,e) { \
    e = ReleaseMutex(m); \
    OSPM_DBG(e == 0,("%s failed. %d\n", "ReleaseMutex", GetLastError())); \
    e = (e == 0 ? GetLastError() : OSPC_ERR_NO_ERROR); \
}

/* Conditional Variables (not natively supported in WIN32) */
#define OSPM_CONDVAR_INIT(c,a,e) { \
    c.Semaphore = CreateSemaphore(NULL,0,1,NULL); \
    e = GetLastError(); \
    OSPM_DBG(e,("%s failed. %d\n", "CreateSemaphore", GetLastError())); \
    if (e == 0) { \
        c.SemaCount = 0; \
        c.WaitLimit = 5000; \
    } \
}

#define OSPM_CONDVAR_DESTROY(c,e) { \
    e = CloseHandle(c.Semaphore); \
    OSPM_DBG(e == 0,("%s failed. %d\n", "CloseHandle", GetLastError())); \
}

#define OSPM_CONDVAR_WAIT(c,m,e) { \
    c.SemaCount++; \
    ReleaseMutex(m); \
    e = WaitForSingleObject(c.Semaphore, INFINITE); \
    if (e != WAIT_FAILED) \
        e = WaitForSingleObject(m, INFINITE); \
    c.SemaCount--; \
    OSPM_DBG(e == WAIT_FAILED,("%s failed. %d\n", \
        "WaitForSingleObject", GetLastError())); \
}

#define OSPM_CONDVAR_TIMEDWAIT(c,m,t,e) { \
    c.SemaCount++; \
    ReleaseMutex(m); \
    e = WaitForSingleObject(c.Semaphore, t * 1000); \
    if (e != WAIT_TIMEOUT && e != WAIT_FAILED) { \
        e = WaitForSingleObject(m, t * 1000); \
     } \
    c.SemaCount--; \
    OSPM_DBG(e == WAIT_FAILED,("%s failed. %s\n", \
        "WaitForSingleObject", GetLastError())); \
    e = (e == WAIT_TIMEOUT ? OSPC_ERR_OS_CONDVAR_TIMEOUT : e); \
}

#define OSPM_CONDVAR_SIGNAL(c,e) { \
    if (c.SemaCount > 0) \
        e = (ReleaseSemaphore(c.Semaphore, 1, NULL) ? \
            OSPC_ERR_NO_ERROR : OSPC_ERR_OS_FAILURE); \
}

#endif /* !_POSIX_THREADS */
#endif /*  _WIN32 */

/*
 * =============================================
 * Socket Macros
 * =============================================
 */

/* -----------------------------*/
/* WIN32 Winsock Macros         */
/* -----------------------------*/
#ifdef _WIN32

#define OSPM_INITWINSOCK(e) { \
    WORD wVersionRequested; \
    WSADATA wsaData; \
    wVersionRequested = MAKEWORD( 2, 0 ); \
    e = WSAStartup( wVersionRequested, &wsaData ); \
    OSPM_DBG(e,("%s failed. %s\n", "initwinsock", strerror(errno))); \
}

#define OSPM_CLEANUPWINSOCK()   WSACleanup()

#define OSPM_BLOCKIO(s,f,e) { \
    if ( f ) \
    { \
        /* set to non-blocking */ \
        e = ioctlsocket(s, FIONBIO, (unsigned long *)&f); \
        OSPM_DBG(e,("%s failed. %s\n", "ioctlsocket", strerror(errno))); \
    } \
    else \
    { \
        /* set to blocking, default behavior */ \
    } \
}

#define OSPM_GETHOSTBYNAME(h,ip,e) { \
    struct hostent *pHost; \
    struct sockaddr_in sSockAddr; \
    if ((pHost = gethostbyname((const char *)h)) == (struct hostent *)NULL) \
    { \
        e = GetLastError(); \
        OSPM_DBG(e,("%s failed for %s. %s\n", "gethostbyname", h, strerror(errno))); \
        ip = 0; \
    } \
    else \
    { \
        OSPM_MEMSET(&sSockAddr, 0, sizeof(sSockAddr)); \
            OSPM_MEMCPY((char *)&sSockAddr.sin_addr, \
            pHost->h_addr, pHost->h_length); \
        ip = sSockAddr.sin_addr.s_addr; \
    } \
}

#define OSPM_GETSOCKERR(h,e) { \
    int err = 0, err_sz = sizeof(int); \
    e = getsockopt(h->SockFd, SOL_SOCKET, SO_ERROR, (char *)&err, &err_sz); \
    if (err != 0) e = OSPC_ERR_SOCK_RESET; \
}

#define OSPM_SEND(fd,n,b,l,e) { \
    e = 0; \
        errno=0; \
    n = send(fd, (char *)b, l, 0); \
    e = WSAGetLastError(); \
    if (e == (int)WSAENOTCONN || e == (int)WSAECONNABORTED || e == (int)WSAECONNRESET) \
        e = OSPC_ERR_SOCK_RESET; \
    OSPM_DBG(e || (OSPTSSIZE_T)n != (OSPTSSIZE_T)l,("%s failed. %s\n", "send", strerror(errno))); \
}

#define OSPM_RECV(h,b,l,e) { \
    e = 0; \
        errno=0; \
    h->ByteCount = recv(h->SockFd, (char *)b, l, 0); \
    e = WSAGetLastError(); \
    OSPM_DBG(e,("%s failed. %s\n", "recv", strerror(errno))); \
}

#define OSPM_SELECT(s,fr,fw,fe,t,e) { \
    e = select(s,fr,fw,fe,t); \
    OSPM_DBG((e == SOCKET_ERROR),("%s failed. %s\n", "select", strerror(errno))); \
}

#define OSPM_CONNECT(s,ip,pt,e) { \
    struct sockaddr_in sin; \
    sin.sin_family      = AF_INET; \
    sin.sin_addr.s_addr = ip; \
    sin.sin_port        = pt; \
    e = connect(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)); \
    if(e != 0) { \
      if(WSAGetLastError() == WSAEWOULDBLOCK){ \
        e = OSPC_ERR_SOCK_CONN_IN_PROGRESS; \
      } \
      else { \
        OSPM_DBG(e,("Warning: %s to %s failed. %d\n", "connect", inet_ntoa(sin.sin_addr), WSAGetLastError())); \
      } \
    } \
}

#define OSPM_INET_NTOA(ip, s) { \
    struct sockaddr_in sin; \
    sin.sin_family          = AF_INET; \
    sin.sin_addr.s_addr     = ip; \
    s = inet_ntoa(sin.sin_addr); \
}

#define OSPM_CLOSE(s,e) { \
    e = closesocket(s); \
    if (e) \
        OSPM_DBG(WSAGetLastError(),("%s failed. %d\n", "closesocket", WSAGetLastError())); \
}

#define OSPM_SOCKET(s,e) { \
    s = socket(AF_INET, SOCK_STREAM, 0); \
    if (s == INVALID_SOCKET) \
        e = OSPC_ERR_SOCK_CREATE_FAILED; \
    OSPM_DBG(e,("%s failed. %d\n", "socket", WSAGetLastError())); \
}

#define OSPM_UDP_SOCKET(s,e) { \
    s = socket(AF_INET, SOCK_DGRAM, 0); \
    if (s == INVALID_SOCKET) \
        e = OSPC_ERR_SOCK_CREATE_FAILED; \
    OSPM_DBG(e,("%s failed. %d\n", "socket", WSAGetLastError())); \
}
#else

/* -----------------------------*/
/* UNIX Sockets Macros          */
/* -----------------------------*/
#define OSPM_INITWINSOCK(e)      { }

#define OSPM_CLEANUPWINSOCK()    { }

#define OSPM_BLOCKIO(s,f,e) { \
    if (f) { \
        e = fcntl(s, F_SETFD, O_NONBLOCK); \
        OSPM_DBG(e < 0,("%s failed. %s\n", "fcntl", strerror(errno))); \
    } \
    else { \
        e = fcntl(s, F_SETFD, 0); \
        OSPM_DBG(e < 0,("%s failed. %s\n", "fcntl", strerror(errno))); \
    } \
}

#define OSPM_GETHOSTBYNAME(h,ip,e) { \
    struct hostent *pHost, sHostResult; \
    struct sockaddr_in sSockAddr; \
    char cBuf[512]; \
    int Error; \
    if ((pHost = gethostbyname_r((const char *)h, \
          &sHostResult, cBuf, 512, &Error)) == (struct hostent *)NULL) \
    { \
        e = Error; \
        OSPM_DBG(e,("%s failed. %s\n", "gethostbyname_r", strerror(errno))); \
        ip = 0; \
    } \
    else { \
        OSPM_MEMSET(&sSockAddr, 0, sizeof(sSockAddr)); \
            OSPM_MEMCPY((char *)&sSockAddr.sin_addr, \
            pHost->h_addr, pHost->h_length); \
        ip = sSockAddr.sin_addr.s_addr; \
    } \
}

#define OSPM_GETSOCKERR(h,e) { \
    int err = 0, err_sz = sizeof(int); \
    e = getsockopt(h->SockFd, SOL_SOCKET, SO_ERROR, (void *)&err, &err_sz); \
    if (err != 0) e = OSPC_ERR_SOCK_RESET; \
}

#define OSPM_SEND(fd,n,b,l,e) { \
    e = 0; \
    errno = 0; \
    n = send(fd, (char *)b, l, 0); \
    e = errno; \
    if (e == ENOTCONN || e == EPIPE || e == ECONNRESET) \
        e = OSPC_ERR_SOCK_RESET; \
    OSPM_DBG(e,("%s failed. %s\n", "send", strerror(errno))); \
}

#define OSPM_RECV(h,b,l,e) { \
    e = 0; \
    errno = 0; \
    h->ByteCount = recv(h->SockFd, (char *)b, l, 0); \
    e = errno; \
    OSPM_DBG(e,("%s failed. %s\n", "recv", strerror(errno))); \
}

#define OSPM_SELECT(s,fr,fw,fe,t,e) { \
    e = select(s,fr,fw,fe,t); \
}

#define OSPM_CONNECT(s,ip,pt,e) { \
    struct sockaddr_in sin; \
    sin.sin_family      = AF_INET; \
    sin.sin_addr.s_addr = ip; \
    sin.sin_port        = pt; \
    e = connect(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)); \
    if(e != 0) { \
      if(errno == EINPROGRESS){ \
        e = OSPC_ERR_SOCK_CONN_IN_PROGRESS; \
      } \
      else { \
        OSPM_DBG(e,("%s failed. %s\n", "connect", strerror(errno))); \
      } \
    } \
}

#define OSPM_INET_NTOA(ip, s) { \
    struct in_addr sin; \
    sin.s_addr     = ip; \
    s = inet_ntoa(sin); \
}

#define OSPM_CLOSE(s,e) { \
    e = close(s); \
    OSPM_DBG(e,("%s failed. %s\n", "close", strerror(errno))); \
}

#define OSPM_SOCKET(s,e) { \
    s = socket(AF_INET, SOCK_STREAM, 0); \
    if (s < 0) \
        e = OSPC_ERR_SOCK_CREATE_FAILED; \
    OSPM_DBG(e,("%s failed. %s\n", "socket", strerror(errno))); \
}

#define OSPM_UDP_SOCKET(s,e) { \
    s = socket(AF_INET, SOCK_DGRAM, 0); \
    if (s < 0) \
        e = OSPC_ERR_SOCK_CREATE_FAILED; \
    OSPM_DBG(e,("%s failed. %s\n", "socket", strerror(errno))); \
}
#endif

/* -------------------------------*/
/* Common Winsock/Sockets Macros  */
/* -------------------------------*/

#define OSPM_DISABLE_NAGLE(s,e) { \
    int flag = 1; \
    OSPM_SETSOCKOPT(s,IPPROTO_TCP, TCP_NODELAY, \
        flag,sizeof(flag),e); \
}

#define OSPM_SETSOCKOPT(s,lev,opn,opv,opl,e) { \
    e = setsockopt(s, lev, opn, \
        (const char *)&opv, opl); \
    OSPM_DBG(e,("%s failed. %s\n", "setsockopt", strerror(errno))); \
}

#define OSPM_INET_ADDR(host,ip) { \
    if (!OSPPUtilIsDottedNumericIP(host)) \
        ip = 0; \
    else \
    { \
        ip = inet_addr(host); \
        if (ip < 0) ip = 0; \
    } \
}

#define OSPM_SHUTDOWN(s,h,e) { \
    e = shutdown(s,h); \
    OSPM_DBG(e,("%s failed. %s\n", "shutdown", strerror(errno))); \
}

/*
 * =============================================
 * Char/String Manipulation Macros
 * =============================================
 */
#define OSPM_TOLOWER(c)             tolower(c)

#define OSPM_STRCPY(s1,s2)          strcpy((s1), (s2))
#define OSPM_STRCMP(s1,s2)          strcmp((s1), (s2))
#define OSPM_STRNCMP(s1,s2,n)       strncmp((s1), (s2), (n))
#define OSPM_STRLEN(s)              ((s != OSPC_OSNULL)? strlen(s) : 0)
#define OSPM_STRNCAT(s1,s2,n)       strncat((s1), (s2), (n))
#define OSPM_STRNCPY(s1,s2,n)       strncpy((char *)(s1), (const char *)(s2), (n))
#define OSPM_STRNCASECMP(s1,s2,n)   strncasecmp((s1), (s2), (n))
#define OSPM_STRTOUL(s1,a1,b)       strtoul((s1), (a1), (b))
#define OSPM_STRSTR(s1,s2)          strstr((s1),(s2))

#define OSPM_ATOF(s)                atof(s)

#ifdef _WIN32

#define OSPM_STRTOK(s,c, s2)        strtok(s,c)

#else

#define OSPM_STRTOK(s, c, s2)       strtok_r(s,c,s2)

#endif

/*
 * =============================================
 * Miscellaneous Macros
 * =============================================
 */
#ifdef _WIN32
#define OSPM_SLEEP(sec) { \
    Sleep(sec * 1000); \
}

#define OSPM_POW(num, power, err){ \
    struct _exception e; \
    if(num == 1){ \
        err = num;} \
    else{ \
        err = ((unsigned)pow((double)num, power)); \
        if((_matherr(&e)) || (err == 1) || (err == 0)){ \
            err = OSPC_ERR_POW; \
        } \
    } \
}

#define OSPM_ISNAN(num, flag) { \
    flag = _isnan(num); \
    }

#define OSPM_ACCESS 	_access

#else

#define OSPM_POW(num, power, err){ \
    if(num == 1){ \
        err = num;} \
    else{ \
        err = pow(num, power); \
        if((isnan(err)) || (err == -HUGE_VAL) || (err == +HUGE_VAL) || (err == 1) || (err == 0)){ \
            err = OSPC_ERR_POW; \
        } \
    } \
}
#define OSPM_SLEEP(sec) sleep(sec)

#define OSPM_ISNAN(num, flag) { \
    flag = isnan(num); \
    }

#define OSPM_ACCESS 	access

#endif

#define OSPM_SET_FLAG_BIT(bitfield,bitmask,value) { \
    if (value) \
        bitfield |= bitmask; \
    else \
        bitfield &= ~(bitmask); \
}

#define OSPM_PRINTF     printf
#define OSPM_SPRINTF    sprintf
#define OSPM_CTIME      ctime
#define OSPM_TIME       time
#define OSPM_MODF       modf
#define OSPM_CEIL       ceil
#define OSPM_FLOOR      floor
#define OSPM_GETENV     getenv

/* 'uses' the argument */
#define OSPM_ARGUSED(x) (x=x)


#endif

