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
 * ospsecssl.cpp - Functions for security object.
 */
#include "osp/osp.h"
#include "osp/ospsecurity.h"
#include "osp/ospssl.h"

OSPTBOOL
    OSPPSecSSLLifetimeHasExpired(
    OSPTSEC          *ospvSecurity,
    OSPTSSLSESSIONID *ospvSessionId)
{
    OSPTBOOL          hasexpired   = OSPC_FALSE;
    unsigned          lifetime     = 0;

    OSPM_DBGENTER(("ENTER: OSPPSecSSLLifetimeHasExpired()\n"));

    if (ospvSessionId != OSPC_OSNULL)
    {
        lifetime = OSPPSecGetSSLLifetime(ospvSecurity);

        if ((ospvSessionId->IssuedTime + (OSPTTIME)lifetime) < 
            (OSPTTIME)OSPM_TIME((OSPTTIME *)0) || ospvSessionId->Length == 0)
        {
            hasexpired = OSPC_TRUE;
            OSPM_DBGSEC(("SSL  : OSPPSecSSLLifetimeHasExpired() session expired\n"));
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSecSSLLifetimeHasExpired() (%d)\n", hasexpired));
    return hasexpired;
}

int
OSPPSecSSLSessionIdNew(
    OSPTSEC        *ospvSecurity,
    unsigned long   ospvLength, 
    unsigned char  *ospvData,
    char           *ospvHostName,
    short           ospvPort)
{
    int               errorcode    = OSPC_ERR_NO_ERROR;
    OSPTSSLSESSIONID *sslsessionid = OSPC_OSNULL,
        *item   = OSPC_OSNULL;
    int               result = 1;

    OSPM_DBGENTER(("ENTER: OSPPSecSSLSessionIdNew()\n"));
    if (ospvSecurity == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_SSL_INIT_SESSION_FAILED;
    }
    else
    {
        OSPM_MALLOC(sslsessionid, OSPTSSLSESSIONID, 
            sizeof(OSPTSSLSESSIONID));
        if (sslsessionid != OSPC_OSNULL)
        {
            OSPM_MEMSET(sslsessionid, 0, sizeof(OSPTSSLSESSIONID));
            OSPM_MALLOC(sslsessionid->Value, unsigned char,
                ospvLength);
            if (sslsessionid->Value != OSPC_OSNULL)
            {
                /*
                 * set the new session id variables
                 */
                sslsessionid->Length = ospvLength; 
                OSPM_MEMCPY(sslsessionid->Value, ospvData, 
                    ospvLength);
                sslsessionid->IssuedTime = OSPM_TIME((OSPTTIME *)0);
                OSPM_MALLOC(sslsessionid->HostName, char, OSPM_STRLEN(ospvHostName));
                if (sslsessionid->HostName != OSPC_OSNULL)
                {
                    OSPM_MEMCPY(sslsessionid->HostName, ospvHostName, 
                        OSPM_STRLEN(ospvHostName));
                    sslsessionid->Port = ospvPort;

                    if (ospvSecurity->SSLSessionHasMutex == OSPC_TRUE)
                    {
                        OSPM_MUTEX_LOCK(ospvSecurity->SSLSessionMutex, errorcode);
                        if (errorcode == OSPC_ERR_NO_ERROR)
                        {

                            /*
                            fprintf(stderr, "Session Id %lx Length = %ld Value = %lx\n",  
                                   (unsigned long)sslsessionid, sslsessionid->Length, 
                                   (unsigned long)sslsessionid->Value);
                            */
                            for (item = (OSPTSSLSESSIONID *)OSPPListFirst((OSPTLIST *)&(ospvSecurity->SSLSessionIdList)); 
                                item != OSPC_OSNULL; 
                                item = (OSPTSSLSESSIONID *)OSPPListNext((OSPTLIST *)&(ospvSecurity->SSLSessionIdList), item))
                            {
                                /*
                                    fprintf(stderr, "Comparing sess %lx val %lx with sess %lx val %lx sz %d\n", 
                                         (unsigned long)sslsessionid,(unsigned long)sslsessionid->Value, 
                                         (unsigned long)item, (unsigned long)item->Value, sslsessionid->Length); 
                                */
                                if (sslsessionid->Length == item->Length)
                                {
                                    result = OSPM_MEMCMP(sslsessionid->Value, item->Value, sslsessionid->Length); 
                                    if (result == 0) 
                                        break; 
                                }
                            } 

                            if (result != 0)
                            {
                                /*
                                fprintf(stderr, "Session Id Added Value = %lx\n", (unsigned long)sslsessionid->Value);
                                */
                                /* add sessionid to current list */
                                OSPPListAppend((OSPTLIST *)&(ospvSecurity->SSLSessionIdList), 
                                    (void *)sslsessionid);
                                OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdNew() session add to list\n"));
                            }
                            else
                            {
                                /* it's a duplicate, SSLREF will make 'Add' callback even on reuse */
                                OSPPSecSSLSessionIdDelete(ospvSecurity, &sslsessionid, OSPC_FALSE);
                                OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdNew() session dup\n"));
                            }
                            OSPM_MUTEX_UNLOCK(ospvSecurity->SSLSessionMutex, errorcode);
                        }
                    }
                }
            }
            else
            {
                errorcode = OSPC_ERR_SSL_INIT_SESSION_FAILED;
            }

            /* if any error should occur above, cleanup */
            if (errorcode != OSPC_ERR_NO_ERROR || 
                (sslsessionid != OSPC_OSNULL && (sslsessionid->Value == OSPC_OSNULL ||
                sslsessionid->HostName == OSPC_OSNULL)))
            {
                OSPPSecSSLSessionIdDelete(ospvSecurity, &sslsessionid, OSPC_FALSE);
                errorcode = OSPC_ERR_SSL_INIT_SESSION_FAILED;
            }
        }
        else
        {
            errorcode = OSPC_ERR_SSL_INIT_SESSION_FAILED;
        }
    }
    OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdNew() db count = <%u>\n", 
        OSPPListCount((OSPTLIST *)&(ospvSecurity->SSLSessionIdList))));

    OSPM_DBGEXIT(("EXIT : OSPPSecSSLSessionIdNew() (%d)\n", errorcode));
    return errorcode; 
}

int
OSPPSecSSLSessionIdGet(
    OSPTSEC        *ospvSecurity,
    unsigned long  *ospvLength, 
    unsigned char **ospvData,
    char           *ospvHostName,
    short           ospvPort)
{
    int               errorcode    = OSPC_ERR_NO_ERROR,
                      result       = 0;
    OSPTSSLSESSIONID *sslsessionid = OSPC_OSNULL;
    OSPTBOOL          reuse        = OSPC_FALSE;

    OSPM_DBGENTER(("ENTER: OSPPSecSSLSessionIdGet()\n"));
    if (ospvSecurity == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_SSL_INIT_SESSION_FAILED;
    }
    else
    {
        if (ospvSecurity->SSLSessionHasMutex == OSPC_TRUE)
        {
            OSPM_MUTEX_LOCK(ospvSecurity->SSLSessionMutex, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                for (sslsessionid = (OSPTSSLSESSIONID *)OSPPListFirst(
                    (OSPTLIST *)&(ospvSecurity->SSLSessionIdList));
                    sslsessionid != OSPC_OSNULL;
                    sslsessionid = (OSPTSSLSESSIONID *)OSPPListNext(
                    (OSPTLIST *)&(ospvSecurity->SSLSessionIdList),
                    sslsessionid))
                {
                    result = OSPM_MEMCMP(ospvHostName, sslsessionid->HostName,
                        OSPM_STRLEN(ospvHostName));

                    if (result == 0 && sslsessionid->Port == ospvPort)
                    {
                        if (OSPPSecSSLLifetimeHasExpired(ospvSecurity, sslsessionid)
                            == OSPC_FALSE)
                        {
                            OSPM_MALLOC(*ospvData, unsigned char, sslsessionid->Length);
                            if (*ospvData != OSPC_OSNULL)
                            {
                                OSPM_MEMCPY(*ospvData, sslsessionid->Value, sslsessionid->Length);
                                *ospvLength = sslsessionid->Length;
                                reuse = OSPC_TRUE;
                            }

                            OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdGet() addr <%lx>\n",
                                (unsigned long)sslsessionid->Value));
                        }
                        else
                        {
                            OSPPSecSSLSessionIdDelete(ospvSecurity, &sslsessionid, OSPC_FALSE);
                        }
                    }
                }
                OSPM_MUTEX_UNLOCK(ospvSecurity->SSLSessionMutex, errorcode);
            }
        }
        if (errorcode != OSPC_ERR_NO_ERROR || reuse == OSPC_FALSE)
        {
            errorcode = OSPC_ERR_SSL_INIT_SESSION_FAILED;
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSecSSLSessionIdGet() (%d)\n", errorcode));
    return errorcode;
}

void
OSPPSecSSLSessionIdDelete(
    OSPTSEC           *ospvSecurity,
    OSPTSSLSESSIONID **ospvSSLSessionId,
    OSPTBOOL           ospvUseMutex)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSecSSLSessionIdDelete()\n"));

    if (ospvSSLSessionId != OSPC_OSNULL &&
        (*ospvSSLSessionId) != OSPC_OSNULL)
    {
        if (ospvSecurity->SSLSessionHasMutex == OSPC_TRUE)
        {
            if (ospvUseMutex)
            {
                OSPM_MUTEX_LOCK(ospvSecurity->SSLSessionMutex, errorcode); 
            }
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                (void)OSPPListRemoveSpecificItem(
                    (OSPTLIST *)&(ospvSecurity->SSLSessionIdList), 
                    (void *)(*ospvSSLSessionId));

                if ((*ospvSSLSessionId)->Value != OSPC_OSNULL)
                {
                    OSPM_FREE((*ospvSSLSessionId)->Value);
                    (*ospvSSLSessionId)->Value = OSPC_OSNULL;
                    OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdDelete() session id value delete\n"));
                }
                if ((*ospvSSLSessionId)->HostName != OSPC_OSNULL)
                {
                    OSPM_FREE((*ospvSSLSessionId)->HostName);
                    (*ospvSSLSessionId)->HostName = OSPC_OSNULL;
                    OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdDelete() session id hostname delete\n"));
                }

                OSPM_FREE(*ospvSSLSessionId);
                *ospvSSLSessionId = OSPC_OSNULL;
                if (ospvUseMutex)
                {
                    OSPM_MUTEX_UNLOCK(ospvSecurity->SSLSessionMutex, errorcode); 
                }
            }
        }
        OSPM_DBGSEC(("SSL  : OSPPSecSSLSessionIdDelete() session id ctx delete\n"));
    }
    OSPM_DBGEXIT(("EXIT : OSPPSecSSLSessionIdDelete()\n"));
    return;
}

int
OSPPSecSSLSessionIdInitDB(
    OSPTSEC *ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvSecurity != OSPC_OSNULL)
    {
        if (ospvSecurity->SSLSessionHasMutex == OSPC_FALSE)
        {
            OSPM_MUTEX_INIT(ospvSecurity->SSLSessionMutex, OSPC_OSNULL, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                ospvSecurity->SSLSessionHasMutex = OSPC_TRUE;
            }
        }
        OSPM_MUTEX_LOCK(ospvSecurity->SSLSessionMutex, errorcode);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPPListNew((OSPTLIST *)&(ospvSecurity->SSLSessionIdList));
            OSPM_MUTEX_UNLOCK(ospvSecurity->SSLSessionMutex, errorcode);
        }
    }
    return errorcode;
}

void
OSPPSecSSLSessionIdDeleteDB(
    OSPTSEC *ospvSecurity)
{
    OSPTSSLSESSIONID *sslsessionid = OSPC_OSNULL;
    int               errorcode    = OSPC_ERR_NO_ERROR;

    if (ospvSecurity != OSPC_OSNULL)
    {
        if (ospvSecurity->SSLSessionHasMutex == OSPC_TRUE)
        {
            OSPM_MUTEX_LOCK(ospvSecurity->SSLSessionMutex, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR && 
                ospvSecurity->SSLSessionIdList != OSPC_OSNULL)
            {

                while (!OSPPListEmpty((OSPTLIST *)&(ospvSecurity->SSLSessionIdList)))
                {
                    sslsessionid = (OSPTSSLSESSIONID *)OSPPListRemove(
                        (OSPTLIST *)&(ospvSecurity->SSLSessionIdList));
                    OSPPSecSSLSessionIdDelete(ospvSecurity, &sslsessionid, OSPC_FALSE);
                }
            }
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                ospvSecurity->SSLSessionHasMutex = OSPC_FALSE;
                OSPM_MUTEX_UNLOCK(ospvSecurity->SSLSessionMutex, errorcode);
                OSPM_MUTEX_DESTROY(ospvSecurity->SSLSessionMutex, errorcode);
            }
        }
    }
    return;
}

void
OSPPSecSetSSLLifetime(
    OSPTSEC *ospvSecurity,
    unsigned ospvSSLLifetime)
{
    ospvSecurity->SSLLifetime = ospvSSLLifetime;
    return;
}

unsigned
OSPPSecGetSSLLifetime(
    OSPTSEC *ospvSecurity)
{
    return ospvSecurity->SSLLifetime;
}

