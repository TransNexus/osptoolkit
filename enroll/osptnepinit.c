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







#include "osp/ospcomm.h"
#include "osp/ospssl.h"
#include "osp/ospsecurity.h"
#include "osp/ospb64.h"

#include "osptnep.h"
#include "osptnepinit.h"
#include "osp/ospcryptowrap.h"

/* 
 * Initialize the Enrollment Parameter object sent in. All of the
 * structures will have to be malloced in this case.
 *
 * Input: Pointer to an OSPTENROLLPARAMS object
 *
 * Errors:
 *     o the pointer to the OSPTENROLLPARAMS is null;
 *     o malloc or initialization fails on any member of the OSPTENROLLPARAMS.
 */
int OSPPInitEnrollParams (
    OSPTENROLLPARAMS* ospvEnrollParamsIn
)
{
    int retVal = OSPC_ERR_NO_ERROR;


    OSPM_DBGENTER(( "ENTER: OSPPInitEnrollParams\n" ));

    /* If ( the pointer to the enrollment parameter list is ok ) then
     *  o allocate memory for every string in the enrollment parameter list
     *    using default lengths. If a string is too short, too long, or needs
     *    to be allocated in another function for some other reason, then
     *    just take it out here.
     *  o if ( all of the pointers are ok ) then
     *      - initialize all of their values.
     */
    if ( ospvEnrollParamsIn == OSPC_OSNULL )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal, 
            "Unable to initialize a null enrollment parameter set.\n" );
    }

    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        ospvEnrollParamsIn->Verbose    = 0;
        ospvEnrollParamsIn->Nonce      = OSPC_OSNULL;
        ospvEnrollParamsIn->NonceLen   = 0;

        ospvEnrollParamsIn->CACert = OSPC_OSNULL;
        ospvEnrollParamsIn->CACertLen = 0;
        ospvEnrollParamsIn->CACertB64 = OSPC_OSNULL;
        ospvEnrollParamsIn->CACertB64Len = 0;

        ospvEnrollParamsIn->Function = OSPC_OSNULL;
        ospvEnrollParamsIn->Username = OSPC_OSNULL;
        ospvEnrollParamsIn->Password = OSPC_OSNULL;
        ospvEnrollParamsIn->DeviceId = OSPC_OSNULL;
        ospvEnrollParamsIn->CustomerId = OSPC_OSNULL;
        ospvEnrollParamsIn->CertReq = OSPC_OSNULL;
        ospvEnrollParamsIn->SSLUrl = OSPC_OSNULL;
        ospvEnrollParamsIn->CAUrl = OSPC_OSNULL;
        ospvEnrollParamsIn->CAFprint = OSPC_OSNULL;
    }

    OSPM_DBGEXIT(( "EXIT: OSPPFreeEnrollParams\n" ));

    return retVal;

}

/* 
 * This function is called for freeing up all the memory taken up
 * by an enrollment parameter object.
 *
 * Input: 
 *     o pointer to an OSPTENROLLPARAMS;
 *
 * Errors:
 *     o if the input variable is null ( OSPC_ERR_ENROLL_ENROLL_PARAMS_FREE ) 
 */
int OSPPFreeEnrollParams (
    OSPTENROLLPARAMS* ospvEnrollParamsIn
)
{
    int retVal = OSPC_ERR_NO_ERROR;


    OSPM_DBGENTER(( "ENTER: OSPPFreeEnrollParams\n" ));

    if ( ospvEnrollParamsIn == OSPC_OSNULL )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "The enrollment parameter list being freed was null.\n" );
    }

    /* Free all of the parameters in the enrollment list, just so long as
     * they're valid ( and the enrollment list itself isn't null, either ):
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        if ( ospvEnrollParamsIn->Function  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->Function  );
        }

        if ( ospvEnrollParamsIn->Username  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->Username  );
        }

        if ( ospvEnrollParamsIn->Password  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->Password  );
        }

        if ( ospvEnrollParamsIn->DeviceId  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->DeviceId  );
        }

        if ( ospvEnrollParamsIn->CustomerId  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->CustomerId  );
        }

        if ( ospvEnrollParamsIn->CertReq  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->CertReq  );
        }

        if ( ospvEnrollParamsIn->CAFprint != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->CAFprint );
        }

        if ( ospvEnrollParamsIn->SSLUrl  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->SSLUrl  );
        }

        if ( ospvEnrollParamsIn->CAUrl  != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->CAUrl  );
        }

        if ( ospvEnrollParamsIn->CACertB64 != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->CACertB64 );
        }

        if ( ospvEnrollParamsIn->CACert != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->CACert );
        }

        if ( ospvEnrollParamsIn->Nonce != OSPC_OSNULL )
        {
            OSPM_FREE( ospvEnrollParamsIn->Nonce );
        }

    }

    OSPM_DBGEXIT(( "EXIT: OSPPFreeEnrollParams\n" ));

    return retVal;
}

/* 
 * This function will take a list of enrollment parameters and communications
 * parameters that specify how a communications manager will be set up.
 * The communications manager will first be created and then initialized
 * with all of the non-SSL-related parameters, followed by all of the
 * SSL-related parameters.
 *
 * Input: a pointer to the enrollment server parameters ( for the enrollment
 *        server's url ); a pointer to the communication parameters; and a 
 *        pointer to the reference to the communication manager to be initialized.
 *        *ospvCommOut should be null ( but ospvCommOut should not. )
 *
 * Output: *ospvCommMgrOut should be initialized as a communications manager.
 *         If it can't be initialized, then it should be null and the 
 *         return value will be non-zero. Otherwise, the return value should
 *         be zero.
 */
int OSPPInitSecureCommMgr(
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM**        ospvCommOut 
)
{
    int retVal = OSPC_ERR_NO_ERROR;


    OSPM_DBGENTER(( "ENTER: OSPPInitSecureCommMgr\n" ));

    /* If ( any of the parameters are null or 
     *      if the url for the CA is null ) then
     *  o set an error code and complain.
     */
    if ( ( ospvEnrollParamsIn == OSPC_OSNULL ) ||
         ( ospvCommParamsIn == OSPC_OSNULL ) ||
         ( ospvCommOut == OSPC_OSNULL ) ||
         ( ospvEnrollParamsIn->SSLUrl == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "The parameters for initializing the secure comm manager are invalid.\n" );
    }

    /* Now create the communications manager. We should check the
     * pointer returned for the communications manager even if the return
     * value says otherwise.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = OSPPCommNew( ospvCommOut );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to allocate the space for the communications manager.\n" );
        }

        else if ( *ospvCommOut == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_COMM_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal,
                 "The communications manager returned was invalid.\n" );
        }
    }

    /* Initialize the communications manager, which includes allocating
     * its memory.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPInitNonSSLCommMgrParams( 
                ospvEnrollParamsIn->SSLUrl,
                ospvCommParamsIn, 
                *ospvCommOut );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to initialize the communication manager's non-SSL related parameters.\n" ); 
        }
    }

    /* Now set up all of the communications parameters specific to SSL: */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal =
            OSPPInitSSLCommMgrParams(
                ospvEnrollParamsIn,
                ospvCommParamsIn,
                *ospvCommOut );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to initialize the communication manager's SSL related parameters.\n" ); 
        }
    }

    OSPM_DBGEXIT(( "EXIT: OSPPInitSecureCommMgr\n" ));

    return retVal;

}
    

/* This is just a minimal wrapper for calling OSPPInitNonSecureCommMgrParams;
 * it just extracts the service point for the CA from the enrollment 
 * parameters and passes it along as the service point to be explicitly 
 * contacted by the communications manager.
 *
 * Input: a pointer to the enrollment server parameters ( for the CA's
 *        url ); a pointer to the communication parameters; and a pointer
 *        to the reference to the communication manager to be initialized.
 *
 * Output: *ospvCommMgrOut should be initialized as a communications manager.
 *         If it can't be initialized, then it should be null and the 
 *         return value will be non-zero. Otherwise, the return value should
 *         be zero.
 */
int OSPPInitNonSecureCommMgr(
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*  ospvCommParamsIn,
    OSPTCOMM**       ospvCommOut 
)
{
    int retVal = OSPC_ERR_NO_ERROR;


    OSPM_DBGENTER(( "OSPPInitNonSecureCommMgr\n" ));

    /* If ( any of the parameters are null or 
     *      if the url for the CA is null ) then
     *  o set an error code and complain.
     */
    if ( ( ospvEnrollParamsIn == OSPC_OSNULL ) ||
         ( ospvCommParamsIn == OSPC_OSNULL ) ||
         ( ospvCommOut == OSPC_OSNULL ) ||
         ( ospvEnrollParamsIn->CAUrl == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "The parameters for initializing the CA's communications manager are invalid.\n" );
    }

    /* Create the communications manager before we initialize it: */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = OSPPCommNew( ospvCommOut );

        if ( retVal != OSPC_ERR_NO_ERROR ) 
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to create communications manager\n" );
        }

        /* For some reason we got a valid return value but an invalid 
         * communications manager; set an error code and complain:
         */
        else if ( *ospvCommOut == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_COMM_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal,
                 "The communications manager returned was invalid.\n" );
        }
    }

    /* Now that the communications manager has been created, initialize
     * its basic parameters. We'll use the CAUrl from the enrollment
     * parameters as the service point:
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPInitNonSSLCommMgrParams( 
                ospvEnrollParamsIn->CAUrl,
                ospvCommParamsIn, 
                *ospvCommOut );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to initialize the communication manager's parameters for the CA.\n" );
        }
    }

    OSPM_DBGEXIT(( "OSPPInitNonSecureCommMgr\n" ));

    return retVal;
}
    

/*
 * Initialize the communications manager's SSL-related parameters. In this
 * case, we'll need: the CA certificate ( given in the CACert field of
 * the enrollment parameters ); the service point to be accessed
 * ( given in the SSLUrl field of the enrollment parameters ); the
 * lifetime of an SSL session ( given in the SSLLifetime field of the
 * communications parameters ); and the communications manager itself.
 *
 * Input: a list of enrollment parameters ( for the CACert and SSLUrl fields of
 *        the OSPTENROLLPARAMS* ); a list of communications parameters
 *        ( for the SSLLifetime field of the OSPTCOMMPARAMS* ); and a
 *        reference to a communications manager.
 *
 * Output: the communications manager will have its parameters set, if
 *         possible. A return value of OSPC_ERR_NO_ERROR should indicate that
 *         there have been no problems. Any other return value will indicate
 *         an error.
 */
int OSPPInitSSLCommMgrParams(
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM*         ospvCommOut
)
{
    int retVal = OSPC_ERR_NO_ERROR;

    OSPTCERT caCert;
    const OSPTCERT *caCerts[1];

    caCerts[0] = &caCert;


    OSPM_DBGENTER(( "ENTER: OSPPInitSSLCommMgrParams\n" ));

    /* Check the references to the parameters first. We'll be using
     * the SSLUrl field of the enrollment parameters list for the
     * service point, so we'll need to check that as well:
     */
    if ( ( ospvEnrollParamsIn == OSPC_OSNULL ) || 
         ( ospvEnrollParamsIn->SSLUrl == OSPC_OSNULL ) ||
         ( ospvCommParamsIn == OSPC_OSNULL ) || 
         ( ospvCommOut == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "The parameters for initializing the enrollment server communications were invalid.\n" );
    }

    /* If ( there were no problems ) then
     *  o set all of the communication manager's parameters, including
     *    connectivity parameters and the CA certificate. 
     *    The CA certificate will be retrieved if necessary.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        /* Create a new security object. Note that we need the address
         * of the communication's manager's security object pointer, so
         * we need to dereference the ospvCommOut, access its security
         * object, and then take its address. Not pretty.
         */
        retVal = OSPPSecNew( &(ospvCommOut->Security) );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, "Unable to create security object for comm mgr." );
        }
    }

    /* If ( there weren't any problems ) then
     *  o set the SSL lifetime and max number of connections
     *  o record any errors. 
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPPSecSetSSLLifetime( 
            ospvCommOut->Security, ospvCommParamsIn->SSLLifetime );

        OSPM_DBGMISC(( "CA cert: \n" ));
        OSPPDumpHex( ospvEnrollParamsIn->CACert, ospvEnrollParamsIn->CACertLen );

				caCert.CertData				= ospvEnrollParamsIn->CACert;
				caCert.CertDataLength = ospvEnrollParamsIn->CACertLen;

        retVal = 
            OSPPSecSetAuthorityCertificates(
                ospvCommOut->Security, 
                1,
                caCerts);

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set the authority certs for the communications manager.\n" );
        }
    }

	/* Initialize the SSL context for the communication manager.
	 * If we fail, then report the error and set an error code.
	 */
	if ( retVal == OSPC_ERR_NO_ERROR )
	{
		retVal = OSPPSSLWrapInit( (void*)(ospvCommOut->Security) );

		if ( retVal != OSPC_ERR_NO_ERROR )
		{
			OSPM_DBGERRORLOG( retVal, "Unable to initialize SSL context.\n" );
		}
	}

    OSPM_DBGMISC(( "Return value: <%d>\n", retVal ));
    OSPM_DBGEXIT(( "EXIT: OSPPInitSSLCommMgrParams\n" ));

    return retVal;

}


/* Initialize all of the non-SSL related parameters; we'll set the
 * retry delay, http timeout, persistence values, and so on. All of
 * the SSL-related parameters are set in the OSPPInitSSLCommMgrParams,
 * which won't be necessary for certain functions ( like retrieving the
 * CA certificate, which is currently done without SSL. )
 *
 * We'll be using this function for establishing basic parameters that
 * aren't dependent on the use of the communications manager, so we'll
 * need a generic service point specification, instead of assuming that
 * it will be in the CAUrl or SSLUrl fields of an OSPTENROLLPARAM* 
 * structure. Doing the service point initialization here just eliminates
 * duplication of effort.
 *
 * Input: pointers to the communications parameters and the service point's
 *        url, as well as the outgoing communications manager. 
 *
 * Output: The communication manager's parameters should be set up.
 *         A return value of OSPC_ERR_NO_ERROR is returned if everything
 *         worked out; otherwise, an error code is returned.
 */
int OSPPInitNonSSLCommMgrParams(
    unsigned char*    ospvServicePtUrlIn,
    OSPTCOMMPARAMS*   ospvCommParamsIn,
    OSPTCOMM*         ospvCommOut 
)
{
    int retVal = OSPC_ERR_NO_ERROR;


    OSPM_DBGENTER(( "ENTER: OSPPInitNonSSLCommMgrParams\n" ));

    /* set an error code and complain if we can't use the input parameters: */
    if ( ( ospvCommParamsIn == OSPC_OSNULL ) ||
         ( ospvCommOut == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "At least one parameter is invalid (null)\n");
    }

    /* If ( there weren't any problems ) then
     *  o set the HTTP persistence 
     *  o record any errors. 
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal =
            OSPPCommSetPersistence( 
                ospvCommOut, 
                ospvCommParamsIn->HTTPPersistence );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set HTTP persistence" );
        }
    }

    /* If ( there weren't any problems ) then
     *  o set the http retry delay 
     *  o record any errors. 
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPCommSetRetryDelay( 
                ospvCommOut, 
                ospvCommParamsIn->HTTPRetryDelay );
        
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set HTTP retry delay." );
        }
    }

    /* If ( there weren't any problems ) then
     *  o set the http retry limit 
     *  o record any errors. 
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPCommSetRetryLimit( 
                ospvCommOut, 
                ospvCommParamsIn->HTTPRetryLimit );
        
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set HTTP Retry Limit." );
        }
    }

    /* If ( there weren't any problems ) then
     *  o set the http max connections 
     *  o record any errors. 
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPCommSetMaxConnections( 
                ospvCommOut, 
                ospvCommParamsIn->HTTPMaxConnections );
        
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set HTTP Max connections." );
        }
    }
        
    /* If ( there weren't any problems ) then
     *  o set the http timeout
     *  o record any errors. 
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPCommSetTimeout( 
                ospvCommOut, 
                ospvCommParamsIn->HTTPTimeout );
        
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set HTTP timeout." );
        }
        retVal = 
            OSPPCommSetConnSelectionTimeout( 
                ospvCommOut, 
                ospvCommParamsIn->HTTPTimeout );
        
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to set Connection Selection timeout." );
        }
    }

    /* Now set the service point and record any errors: */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_DBGMISC(( "enrollment url: <%s>\n", ospvServicePtUrlIn )); 

        retVal = 
            OSPPCommSetServicePoints( 
                ospvCommOut, 
                1, 
                (const char**)&(ospvServicePtUrlIn) );

        retVal = OSPPCommMgrSetSPMessageCount(
                ospvCommOut,
                NULL);

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGMISC(( "Unable to set service points: <%d>\n", retVal ));
        }
    }

    OSPM_DBGEXIT(( "EXIT: OSPPInitNonSSLCommMgrParams\n" ));

    return retVal;
}

/*
 * This function will: initialize a communications manager; construct
 * a message to be sent to a service point; parse the results; and validate
 * the contents of the certificate. We'll need the enrollment parameters
 * so that we know which CA to contact for  a certificate, and we'll
 * need the communications parameters for initializing the communications
 * manager. The enrollment parameters will also be used to store the
 * CA certificate that is returned ( accessed by 
 * ospvEnrollParamsInOut->CACert ).
 *
 * Input: a pointer to the enrollment parameter list and a pointer to the
 *        communications parameter list.
 *
 * Output: the CA certificate ( if it was found ) in the enrollment parameter
 *         list ( under the CACert field ). OSPC_ERR_NO_ERROR is returned
 *         if everything went ok; otherwise, some other error code is returned.
 */
int OSPPRetrieveCACert (
    OSPTENROLLPARAMS* ospvEnrollParamsInOut,
    OSPTCOMMPARAMS*   ospvCommParamsIn
)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* The structure used for encapsulating the request and response: */
    OSPTMSGINFO*   certReqMsg  = OSPC_OSNULL;

    /* The request that we'll be building and sending to the CA via the
     * certReqMsg ( which will also hold the response. )
     */
    unsigned char* caRequest      = OSPC_OSNULL;

    /* The communications manager used for contacting the enrollment server
     * ( or its CA ) and requesting the CA certificate:
     */
    OSPTCOMM*      caCommMgr      = OSPC_OSNULL;

    /* The CA certificate response that will be null-terminated: */
    unsigned char* caCertResponse = OSPC_OSNULL;


    OSPM_DBGENTER(( "ENTER: OSPPRetrieveCACert\n" ));

    /* Check the input parameters for null values first: */
    if ( ( ospvCommParamsIn == OSPC_OSNULL ) || 
         ( ospvEnrollParamsInOut == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal, 
            "Parameters were invalid; at least one was null.\n" );
    }

    /* Now initialize the communications manager: */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPInitNonSecureCommMgr( 
                ospvEnrollParamsInOut, 
                ospvCommParamsIn, 
                &caCommMgr );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to init communication with the CA.\n" );
        }
    }

    /* If ( the parameters were ok ) then
     *  o initialize the memory for the outbound message.
     *  o if ( we coldn't initialize it ) then
     *      - set an error code and complain.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = OSPPMsgInfoNew( &certReqMsg );
        OSPM_MALLOC( 
            certReqMsg->ContentType, 
            unsigned char, 
            OSPM_STRLEN( OSPC_COMM_TEXT_MSG ) + 1 );

        if ( certReqMsg->ContentType == OSPC_OSNULL ) 
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal, 
                "No memory is available for the CA certificate request.\n" );
        }
    }

    /* If ( we could create the message info structure ) then
     *  o create the retrieval request to be sent
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPCreateCARetrievalRequest( 
                ospvEnrollParamsInOut, &caRequest );

        /* Print out an error if there was something wrong: */
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to create CA retrieval request." );
        }
    }

    /* If ( the request could be created ) then
     *  o set the request message and its size for the MessageInfo:
     *  o add the MessageInfo to the queue; the request should be 
     *    transmitted to the enrollment server and a request received.
     *  o if ( sending the request failed ) then write an error message.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        certReqMsg->RequestMsg = caRequest;
        certReqMsg->RequestSz = OSPM_STRLEN( (const char *)caRequest );
        OSPM_STRCPY( (char *)(certReqMsg->ContentType) , OSPC_COMM_TEXT_MSG ); 

        OSPM_DBGMISC(( 
            "request msg: <%s>, <%d>\n", 
            certReqMsg->RequestMsg, certReqMsg->RequestSz ));

        retVal = OSPPCommAddTransaction( caCommMgr, certReqMsg ); 

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to retrieve CA certificate." );
        }
    }

    /* If ( sending the request was successful ) then
     *  o Get the CA certificate out of the response.
     *  o if ( there were problems getting it out ) then
     *      - print an error.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        if ( certReqMsg->ResponseMsg == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_INVALID_RESPONSE;
            OSPM_DBGERRORLOG( 
                retVal, 
                "The response received for the CA certificate was empty.\n" );
        }
    }

    /* If ( the return value was ok ) then
     *  o check the error code returned as well; it's possible that the
     *    error code from the enrollment server ( at the HTTP level; this
     *    is not equivalent to the status of the enrollment request )
     *    didn't result in a bad return value. In this case, report an
     *    error.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        if ( certReqMsg->ErrorCode != OSPC_ERR_NO_ERROR )
        {
            retVal = OSPC_ERR_ENROLL_INVALID_RESPONSE;
            OSPM_DBGERRORLOG( 
                retVal, 
                "The error code received invalidates the CA's response.\n" );
            OSPM_DBGMISC(( 
                "Error code received from server: <%d>\n", 
                certReqMsg->ErrorCode ));
        }
    }

    /*
     * Copy the response into something that's null-terminated; that way,
     * we won't have to worry about overwriting or reading other memory:
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MALLOC( caCertResponse, unsigned char, certReqMsg->ResponseSz + 1 );
        if ( caCertResponse == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG(
                retVal,
                "No memory for the null-terminated response.\n" );
        }
    }

    /* 
     * Everything looks alright with the response so far, so now let's get
     * the CA certificate ( base64 decoded ) from the response:
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MEMSET( caCertResponse, 0, certReqMsg->ResponseSz + 1 );
        OSPM_STRNCPY( 
            caCertResponse, 
            certReqMsg->ResponseMsg, 
            certReqMsg->ResponseSz );

        retVal = 
            OSPPExtractCACertFromResponse( 
                caCertResponse,
                certReqMsg->ResponseSz,
                &(ospvEnrollParamsInOut->CACert),
                &(ospvEnrollParamsInOut->CACertLen),
                &(ospvEnrollParamsInOut->CACertB64),
                &(ospvEnrollParamsInOut->CACertB64Len) );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to extract CA certificate from response." );
        }
    }

    /*
     * If we got the binary string for the CA certificate successfully,
     * then we'll have to validate it. Here we'll turn it into an ASN.1
     * object, check the structure, check the signature ( the CA certificate
     * should be self-signed ), and check the fingerprint. The enrollment
     * parameters structure should contain the CA certificate and the acceptable
     * fingerprint for the certificate prior to the validation stage.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = OSPPValidateCACertificate( ospvEnrollParamsInOut );
    }

    /* Now it's time to shutdown the communications manager: */
    if ( caCommMgr != OSPC_OSNULL )
    {
        OSPM_DBGMISC(( "Cleaning up communications manager...\n" ));
        OSPPEnrollCleanupCommMgr( &caCommMgr );
    }

    /* Destroy the memory for the enrollment request: */
    OSPPMsgInfoDelete( &certReqMsg );

    /* Destroy the null-terminated CA certificate response if necessary: */
    if ( caCertResponse != OSPC_OSNULL )
    {
        OSPM_FREE( caCertResponse );
    }

    OSPM_DBGEXIT(( "EXIT: OSPPRetrieveCACert\n" ));

    return retVal;
}

/*
 * Create the CA retrieval request. This function should create that
 * request and put it into the ospvCARequest that gets passed in. Note
 * that the memory alloced here for ospvCARequest will not be unmalloced
 * by this function; it will have to be freed by the calling function.
 *
 * Input: ospvCARequest ( also written to; should be empty initially. )
 *
 */
int OSPPCreateCARetrievalRequest (
    OSPTENROLLPARAMS* ospvEnrollParamsIn,
    unsigned char**   ospvCARequestOut
)
{
    int retVal        = OSPC_ERR_NO_ERROR;

    /* Length of the request being built: */
    int requestStrLen = -1;

    /* The request being built: */
    unsigned char* requestStr = OSPC_OSNULL;


    OSPM_DBGENTER(( "ENTER: OSPPCreateCARetrievalRequest\n" ));

    /* If ( the output request is null ) then
     *  o set an error code 
     */
    if ( ospvCARequestOut == OSPC_OSNULL )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
    }

    if ( retVal == OSPC_ERR_NO_ERROR ) 

    /* If ( everything has gone ok so far ) then
     *  o calculate the length of the request: the length of 
     *    "operation=GetCACert".
     *  o allocate that much memory, plus one byte ( for null )
     *  o if ( we couldn't malloc memory ) then
     *      o something really bad happened, so set the return value to
     *        an error code and get out.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        requestStrLen = OSPM_STRLEN( OSPC_ENROLL_OPERATION_REQ_PARAM ) +
                        OSPM_STRLEN( OSPC_ENROLL_NAME_VALUE_DELIMITER ) +
                        OSPM_STRLEN( OSPC_ENROLL_OP_CA_CERT_REQ_PARAM );

        OSPM_MALLOC( requestStr, unsigned char, requestStrLen + 1 ); 
        if ( requestStr == OSPC_OSNULL )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to allocate space for request\n" );
        }
    }

    /* If ( the parameters and the memory setting were ok ) then
    *   o copy the request parameters into the request
    *   o set the CA certificate request to be the string that gets built.
    */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MEMSET( requestStr, 0, requestStrLen + 1 );
        OSPM_SPRINTF( 
            (char *)requestStr, 
            "%s%s%s", 
            OSPC_ENROLL_OPERATION_REQ_PARAM,
            OSPC_ENROLL_NAME_VALUE_DELIMITER,
            OSPC_ENROLL_OP_CA_CERT_REQ_PARAM );

        *ospvCARequestOut = requestStr;
        OSPM_DBGMISC(( "request string: <%s>\n", requestStr ));
    }
        
    OSPM_DBGEXIT(( "EXIT: OSPPCreateCARetrievalRequest\n" ));

    return retVal;
}

/* Extract the CA certificate from the response and base64-decode it.
 * We'll also use the base64-encoded certificate to be passed back to
 * the user so that they can use it again on input ( instead of having
 * to redo the base64 encoding. )
 *
 * Input: a character string representing the response to be scanned, 
 *        along with outgoing parameters for the binary BER-encoding
 *        of the CA certificate, its length, the base64 encoding of the
 *        certificate, and its length as well.
 *
 * Output: Along with the values of the outgoing parameters, a return value
 *         of OSPC_ERR_NO_ERROR is returned if the variables could be
 *         allocated and assigned properly. Otherwise, a return value of
 *         something other than OSPC_ERR_NO_ERROR is returned.
 */
int OSPPExtractCACertFromResponse( 
    unsigned char*  ospvResponseIn,
    unsigned        ospvResponseLenIn,
    unsigned char** ospvCACertOut,
    unsigned*       ospvCACertLenOut,
    unsigned char** ospvCACertB64Out,
    unsigned*       ospvCACertB64LenOut
)
{
    int            retVal       = OSPC_ERR_NO_ERROR;
    int            caCertB64Len = 0;
    unsigned char* caCertB64    = OSPC_OSNULL;


    OSPM_DBGENTER(( "ENTER: OSPPExtractCACertFromResponse\n" ));

    /* Check the input parameters first: */
    if ( ( ospvResponseIn == OSPC_OSNULL ) ||
         ( ospvCACertOut == OSPC_OSNULL ) ||
         ( ospvCACertLenOut == OSPC_OSNULL ) ||
         ( ospvCACertB64Out == OSPC_OSNULL ) ||
         ( ospvCACertB64LenOut == OSPC_OSNULL ) ) 
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal, 
            "The arguments for extracting a CA certificate are invalid.\n" );
    }

    /* Retrieve the base64-encoded CA certificate from the response;
     * the memory should be allocated for the caCertB64 inside of 
     * OSPPExtractFieldFromResponse.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = 
            OSPPExtractFieldFromResponse( 
                ospvResponseIn, 
                ospvResponseLenIn,
                OSPC_ENROLL_CA_CERT_RSP_PARAM, 
                &caCertB64 ); 

        /* Complain if we couldn't find the CA certificate: */
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to find the base64-encoded CA certificate in the response.\n" );
        }

        /* Set an error code and complain if the return value was null: */
        else if ( caCertB64 == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal,
                 "The base64-encoded CA certificate returned was null.\n" );
        }
    }

    /* If ( we could find the CA certificate ) then
     *  o get its length
     *  o set an error code and complain if it's empty
     */
    if ( retVal == OSPC_ERR_NO_ERROR ) 
    {
        caCertB64Len = OSPM_STRLEN( caCertB64 );
        if ( caCertB64Len <= 0 )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "The CA certificate returned was empty.\n" );
        }
    }

    /* IF ( it looks like the CA certificate might be ok ) then
     *  o allocate memory for the BER-encoded CA certificate ( that is, the
     *    one that's been base64-decoded ); we'll use the pointer returned
     *    to caCertB64 for the outgoing base64-encoding of the cert, so we
     *    don't need any space for it here
     *  o complain if the space couldn't be allocated.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MALLOC( *ospvCACertOut, unsigned char, 2 * caCertB64Len + 1 ); 

        if ( *ospvCACertOut == OSPC_OSNULL )
             
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to allocate memory for the certificate buffers.\n" );
        }
    }

    /* If ( the memory could be allocated ) then
     *  o initialize it
     *  o set the maximum size of the base64-decoded cert to be twice the
     *    length of the base64-encoded cert. This isn't a very tight 
     *    space allocation ( in fact, the decoding of a base64-encoded message
     *    is 4 * ceil( caCertB64Len/3 ) ), but it's required for the 
     *    base64 decoding mechanism.
     *  o complain if we couldn't get it
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {    
        OSPM_MEMSET( *ospvCACertOut, 0, 2 * caCertB64Len + 1 );
        *ospvCACertLenOut = 2 * caCertB64Len;

        retVal = 
            OSPPBase64Decode( 
                caCertB64, 
                caCertB64Len, 
                *ospvCACertOut, 
                ospvCACertLenOut );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal,
                 "Bad base 64 encoding for CA certificate\n" );
        }

    }

    /* If ( we could successfully base64-decode the certificate ) then
     *  o copy the base64-encoded CA certificate and set its length.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        *ospvCACertB64Out = caCertB64;
        *ospvCACertB64LenOut = caCertB64Len;
    }

    OSPM_DBGEXIT(( "EXIT: OSPPExtractCACertFromResponse\n" ));

    return retVal;
}

/* Now take the enrollment parameter's CA certificate fingerprint in hex
 * ( which should have been taken from a secure channel, such as paper 
 * [ if ubiquity can be considered security ] or an SSL session with the
 * enrollment server's accompanying web site ( if available. ) If the 
 * fingerprint is missing, then an error will not be generated; we'll assume
 * for now ( although this is debatable ) that the fingerprint isn't 
 * necessary. If any of the other parameters are otherwise null, then
 * this function will fail. If the CA certificate is missing, then this
 * function will fail regardless of whether or not we care about checking
 * for the fingerprint. This may be changed too ( just rearrange the ordering
 * of the statements for checking the null values. )
 *
 * Input: a pointer to the enrollment parameters, which contains the CA 
 *        certificate and the CA's intended fingerprint.
 *
 * Output: an error code if anything is null or if the fingerprint cannot be
 *         taken or doesn't match up to what's expected.
 */
int OSPPValidateCACertificate(
    OSPTENROLLPARAMS* ospvEnrollParams
)
{
    int             retVal           = OSPC_ERR_NO_ERROR;
    unsigned char*  caFprint         = OSPC_OSNULL;
    OSPTASN1OBJECT* caCertASN1Object = OSPC_OSNULL;
    int             caFprintLen      = -1;

    /* The binary version of the input fingerprint: */
    unsigned char* inputFprintBin    = OSPC_OSNULL;
    int            inputFprintBinLen = 0;

    /* This is the index of the CA certificate's parent ( itself ) within the
     * given list. It's ignored because it should be self-signed.
     */
    int            ignoreIndex       = -1;


    OSPM_DBGENTER(( "ENTER: OSPPValidateCACertificate\n" ));

    /* If ( the input parameters are bad ) then
     *  o there's nothing we can do; set the errorcode and complain.
     */
    if ( ospvEnrollParams == OSPC_OSNULL )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "Invalid enrollment parameters passed in.\n" );
    }

    /* Now check the CA certificate's value; if the CA certificate is null
     * or empty, then we can't check it. In this case, set an errorcode
     * and complain.
     */
    if ( ( ospvEnrollParams->CACert == OSPC_OSNULL )  ||
         ( ospvEnrollParams->CACertLen <= 0 ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal, 
            "The CA certificate being validated is empty.\n" );
    }

    /* Create an ASN1 object out of the CA certificate received. If we cannot
     * form a certificate, then we know that there's something wrong with the
     * general format of the certificate.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = OSPPX509CertCreate( ospvEnrollParams->CACert, &caCertASN1Object );    
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "The CA certificate received is not a valid X.509 cert.\n" );
        }
    }

    /* Then we successfully parsed the CA certificate's structure; now let's
     * check the signature. The signature must be self-signed; if it isn't,
     * then we'll need some other way of authenticating the certificate
     * ( such as assuming that the CA certificate entered by hand is valid.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
		OSPM_DBGMISC(( "CA cert:\n" ));
		OSPPDumpHex( 
			caCertASN1Object->ElementInfo->Content, 
			caCertASN1Object->ElementInfo->ContentLength );
        retVal = 
            OSPPX509CertValidateCertificate( 
                caCertASN1Object, &caCertASN1Object, 1, &ignoreIndex );

        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "The CA certificate chain is not self-signed.\n" );
        }
    }

    /* If the fingerprint passed in was null, then we're done; we don't 
     * care about the CA fingerprint. The security policy here may be 
     * changed if we absolutely require the CA's fingerprint. We won't 
     * return right away if there's no other processing to be done, 
     * simply for the sake of having a single entry/exit point.
     */
    if ( ( ospvEnrollParams->CAFprint != OSPC_OSNULL ) &&
         ( OSPM_STRLEN( ospvEnrollParams->CAFprint ) > 0 ) ) 
    {
        OSPM_DBGMISC(( "fingerprint on input: \n" ));
        OSPPDumpHex( ospvEnrollParams->CAFprint, 40 );

        if ( ospvEnrollParams->CACert == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG( 
                retVal,
                 "The CA certificate is missing.\n" );
        }

        /* Create the memory for the hash: */
        if ( retVal == OSPC_ERR_NO_ERROR )
        {
            OSPM_MALLOC( 
                caFprint, 
                unsigned char, 
                OSPC_ENROLL_MAX_FPRINT_SIZE + 1 );

            OSPM_MALLOC(
                inputFprintBin, 
                unsigned char, 
                2 * OSPC_ENROLL_MAX_FPRINT_SIZE + 1 );    

            /* If ( the memory allocation didn't work ) then
             *  o set an error code and complain; we'll clean up any
             *    memory allocated at the end of this block anyway.
             */
            if ( ( caFprint == OSPC_OSNULL ) || 
                 ( inputFprintBin == OSPC_OSNULL ) )
            {
                retVal = OSPC_ERR_ENROLL_NO_MEMORY;
                OSPM_DBGERRORLOG( 
                    retVal, 
                    "Unable to create memory for the fingerprint.\n" );
            }
        }

        /* Now turn the hexadecimal ASCII into binary; this is better than
         * comparing two hex encodings that may have different capitalization
         * schemes.
         */
        if ( retVal == OSPC_ERR_NO_ERROR )
        {
            inputFprintBinLen = 2 * OSPC_ENROLL_MAX_FPRINT_SIZE;
            retVal = 
                OSPPHexToBinary( 
                    ospvEnrollParams->CAFprint, 
                    OSPM_STRLEN( ospvEnrollParams->CAFprint ),
                    inputFprintBin,
                    &inputFprintBinLen ); 

            if ( retVal != OSPC_ERR_NO_ERROR )
            {
                OSPM_DBGERRORLOG( 
                    retVal, 
                    "The fingerprint passed in could not be decoded to binary.\n" ); 
                OSPM_DBGMISC(( 
                    "fingerprint passed in: <%s>\n", 
                    ospvEnrollParams->CAFprint ));
            }
        }

        /* If ( we could initialize for the hash ) then
         *  o initialize the memory;
         *  o take the hash;
         *  o check the return value for calculating the fingerprint.
         */
        if ( retVal == OSPC_ERR_NO_ERROR )
        {
            OSPM_MEMSET( caFprint, 0, OSPC_ENROLL_MAX_FPRINT_SIZE + 1 );
            OSPPDumpHex( 
                caCertASN1Object->ElementInfo->Content,
                caCertASN1Object->ElementInfo->ContentLength );

            /* Use the CA certificate and its length as the parameters for
             * the fingerprint/digest; put the digest in caFprint and its
             * length in caFprintLen. We'll use SHA 1 for the digesting 
             * algorithm; the other two paramters( OSPC_OSNULL and 0 ) are
             * superfluous values; ignore them and their green curtain.
             */
            retVal = 
                OSPPCryptoWrapDigest( 
                    caFprint,
                    &caFprintLen,
                    OSPC_OSNULL,
                    0,
                    ospvEnrollParams->CACert,
                    ospvEnrollParams->CACertLen,
                    OSPC_CRYPTO_FLAG_USE_SHA1_DIGEST );

            OSPM_DBGMISC(( "retVal for digesting: <%d>\n", retVal ));
            if ( retVal == OSPC_ERR_NO_ERROR )
            {
                OSPM_DBGMISC(( "CA fingerprint:\n" ));
                OSPPDumpHex( caFprint, caFprintLen );
            }
        }

        /* Now check the fingerprint against what we were given. Translate
         * the fingerprint that was taken into hex and compare it against the
         * hex fingerprint given on input.
         */
        if ( retVal == OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGMISC(( "input fingerprint: \n" ));
            OSPPDumpHex( inputFprintBin, caFprintLen );
            if ( caFprintLen != inputFprintBinLen )
            {
                retVal = OSPC_ERR_ENROLL_BAD_FPRINT;
            }

            else if ( OSPM_MEMCMP( inputFprintBin, caFprint, caFprintLen ) != 0 )
            {
                retVal = OSPC_ERR_ENROLL_BAD_FPRINT;
            }

            if ( retVal == OSPC_ERR_NO_ERROR )
            {
                OSPM_DBGMISC((
                    "The CA fingerprint matches the one passed in.\n" ));
            }

            else
            {
                OSPM_DBGERRORLOG( 
                    retVal, 
                    "The CA certificate received had a bad fingerprint.\n" );
            }
        }

        /* Free up everything that we've allocated, but only if the
         * space exists:
         */
        if ( caFprint != OSPC_OSNULL )
        {
            OSPM_FREE( caFprint );
        }

        if ( inputFprintBin != OSPC_OSNULL )
        {
            OSPM_FREE( inputFprintBin );
        }
    }

    /* This object is allocated outside the scope of when we check
     * the CA certificate's fingerpring, so save the freeing of this
     * object until the end.
     */
    if ( caCertASN1Object != OSPC_OSNULL )
    {
        OSPPASN1ObjectDelete( &caCertASN1Object );
    }

    OSPM_DBGEXIT(( "EXIT: OSPPValidateCACertificate\n" ));

    return retVal;

}

/* Translate the given hex string into binary. We'll be given the
 * length of the hex string and the length of the binary string as
 * well. We'll expect that the binary string already has enough
 * memory allocated ( at least twice as much as the hex encoding. )
 *
 * The length of the hex string dictates how much memory will be copied.
 * For example, if the length of the hex string is 200 bytes ( or 100 bytes
 * in its full binary representation ), then the number of octets to
 * be copied from the hex string can be anywhere from 2 to 200. 
 *
 * The length of the binary string initially refers to the amount of
 * space that the binary string can contain. It will be set to the
 * number of bytes that were copied into the binary string.
 *
 * Input: the hex string, the length of the hex string, the binary
 *        representation, and the length of the binary representation
 *
 * Output: the binary representation and its length will be output.
 *         A return code of OSPC_ERR_NO_ERRRO ( 0 ) will be returned 
 *         if everything went alright; otherwise, a non-zero code
 *         will be returned.
 */
int OSPPHexToBinary (
    unsigned char* ospvHexStr,
    unsigned       ospvHexStrLen,
    unsigned char* ospvBinaryStr,
    unsigned*      ospvBinaryStrLen )
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* Index for looping through the hex string contents: */
    unsigned       hexStrIndex = 0;

    /* The next two bytes from the hex string that will compose the
     * next byte in the binary string:
     */
    unsigned char* nextByte = OSPC_OSNULL; 

    /* The next word of data to be placed in the binary string; this
     * will be the output of a strtol on nextByte:
     */
    long           nextWord    = 0;

    /* If ( any of the values input are null ) then 
     *  o complain and set an error code.
     */
    if ( ( ospvHexStr == OSPC_OSNULL ) || 
         ( ospvBinaryStr == OSPC_OSNULL ) ||
         ( ospvBinaryStrLen == OSPC_OSNULL ) ) 
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal,
             "A null pointer was passed in.\n" );
    }

    /* If ( the hex string is not as long as what the caller wants
     *      to copy from it ) then
     *  o set an error code and complain.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        if ( OSPM_STRLEN( ospvHexStr ) < ospvHexStrLen )
        {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG( 
                retVal,
                 "The requested length of the hex string to be translated is longer than the string itself.\n" );
        }
    }

    /* If ( the caller wants more bytes from the binary string than are
     *      allocated to it or if the binary string is empty ) then
     *  o set an error code and complain.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        if ( ( *ospvBinaryStrLen <= 0 ) ||
             ( *ospvBinaryStrLen < 2 * ospvHexStrLen ) )
        {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG( 
                retVal, 
                "The given binary string is too small.\n" );
            OSPM_DBGMISC(( "%d < 2 * %d\n", *ospvBinaryStrLen, ospvHexStrLen ));
        }
    }

    /* For ( each character ) 
     *  o if ( the character isn't a hex digit ) then
     *      - set an error code and return.
     */
    for ( hexStrIndex = 0; hexStrIndex < ospvHexStrLen; hexStrIndex++ )
    {
        if ( !OSPM_ISXDIGIT( ospvHexStr[ hexStrIndex ] ) )
        {
            retVal = OSPC_ERR_ENROLL_INVALID_ARG;
            OSPM_DBGERRORLOG( 
                retVal, 
                "The hex string on input is not entirely hex characters.\n" );

            OSPM_DBGMISC(( 
                "character %c at offset %d of %s is not a hex character\n",
                ospvHexStr[ hexStrIndex ],
                hexStrIndex,
                ospvHexStr ));
        }
    }

    /* Try to allocate memory for the chunk of data that will be used for
     * translating the hex to binary. Set an error code and complain 
     * if we can't.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MALLOC( nextByte, unsigned char, 3 );
        if ( nextByte == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to allocate memory for translating hex ascii strings to binary.\n" );
        }
    }

    /* If ( all of the parameters are ok ) then
     *  o reset the length of the binary output string.
     *  o initialize the memory for the binary string.
     *  o for ( every octet in the hex  string that we want to copy ) do
     *      - copy the next octet into some other space
     *      - turn it into a long int.
     *      - take the last 8 bits ( wherever they may be ) and put them
     *        at the end of the binary string; increment the length of the
     *        binary string by one.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        *ospvBinaryStrLen = 0;
        OSPM_MEMSET( nextByte, 0, 3 );

        for ( hexStrIndex = 0; ( hexStrIndex < ospvHexStrLen ); hexStrIndex+=2 )
        {
            OSPM_SPRINTF( nextByte, "%.2s", ospvHexStr + hexStrIndex ); 
            nextWord = OSPM_STRTOL( nextByte, NULL, 16 );

            OSPM_DBGMISC(( 
                "nextWord: %ld, %d\n", 
                nextWord, 
                ( nextWord  && OSPC_LONG_TO_BYTE_MASK ) ));

            ospvBinaryStr[ (*ospvBinaryStrLen)++ ] = 
                (unsigned char)( nextWord & OSPC_LONG_TO_BYTE_MASK );
        }
        OSPM_DBGMISC(( "final binary encoding:\n" ));
        OSPPDumpHex( ospvBinaryStr, *ospvBinaryStrLen );
    }

    /* Free up any memory we allocated: */
    if ( nextByte != OSPC_OSNULL )
    {
        OSPM_FREE( nextByte );
    }

    return retVal;

}


/* Given the response to scan and the field to search for, extract the
 * contents of that field into the outgoing value. We'll be careful
 * not to accidentally use names that look similar but which have different
 * lengths. For example, if we're looking for the field "def" and we're
 * given "abcdef=ghi&def=jkl", then we want to return "jkl" and not
 * "ghi". If we find an appropriate field, then we'll save it in 
 * the outgoing pointer ( which we'll also allocate. )
 *
 * Input: the response to be scanned, the field to look for, and a pointer
 *        to a string that will contain the field's value ( if we find it. )
 *
 * Output: *ospvValueOut will be non-null and will contain the contents of
 *         the field if the field can be found and is non-empty. Otherwise,
 *         if the field is empty or can't be found, then an error code
 *         other than OSPC_ERR_NO_ERROR will be returned and *ospvValueOut
 *         will be untouched.
 */
int OSPPExtractFieldFromResponse (
    unsigned char*  ospvResponse,
    unsigned        ospvResponseLen,
    unsigned char*  ospvSearchField,
    unsigned char** ospvValueOut 
)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* This will point to the value that we find, if we find it: */
    unsigned char* beginValuePtr = OSPC_OSNULL;

    /* This points to the name that we're trying to match on: */
    unsigned char* nextNamePtr   = OSPC_OSNULL;

    /* Flag for determining whether or not we found a value: */
    OSPTBOOL       valueFound    = OSPC_FALSE;


    OSPM_DBGENTER(( "ENTER: OSPPExtractFieldFromResponse\n" ));

    /* Check for any null or empty parameters passed in first.
     * We'll be allocating *ospvValueOut, so don't check its contents
     * ( just check that ospvValueOut isn't null. )
     */
    if ( ( ospvResponse == OSPC_OSNULL ) ||
         ( ospvResponseLen <= 0 ) ||
         ( ospvSearchField == OSPC_OSNULL ) ||
         ( ospvValueOut == OSPC_OSNULL ) ||
         ( OSPM_STRLEN( ospvSearchField ) <= 0 )
        )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal, 
            "Invalid arguments for searching for a field\n" );
    }

    /* We know that the parameters are valid, so allocate enough memory
     * to store the value that we find. The maximum possible size is
     * strlen( ospvResponse ) - 2, which would happen if the response
     * was a one-field value with a one-character name ( such as
     * "a=1073741824" .) So, go ahead and allocate the size of the string.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MALLOC( beginValuePtr, unsigned char, ospvResponseLen );

        if ( beginValuePtr == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to allocate memory for saving the field's value.\n" );
        }
    }

    /* If ( we could allocate the memory ) then
     *  o get the beginning of the string
     *  o do:
     *      - find the first occurrence of a name in the string.
     *      - if ( the occurrence is null ) then
     *          o we're done; we couldn't find a name-value pair for the
     *            given name.
     *      - otherwise:
     *          o check the name to see if it matches a name-value pair
     *            ( that is, we don't want to use "abcd=efgh" if we're looking
     *              for the name "bc" )
     *          o if ( we matched on one string ) then we must have found
     *            something for a name and a value.
     *              - if ( the value is null ) then
     *                  o the value we found here is empty; we can't use it.
     *              - else if ( the name was at the beginning of the response )
     *                     OR ( the previous character is the field delimiter )
     *                  o then we know that we've actually found the name
     *                    and hence the correct value. We know that we 
     *                    haven't matched on "abcd=efgh" if we're looking
     *                    for the name "bc", since we compare the name we
     *                    found to the beginning of the string. Likewise,
     *                    cases like "abcd=efgh&ijkl=mnop" will fall through
     *                    the cracks if we're looking for "jk" ( since 
     *                    'i' != '&' )
     *       - if ( we didn't find anything on this iteration ) then
     *           o increment the pointer so that we don't get the same case.
     *     while ( we haven't found anything ) and 
     *           ( we haven't run into any problems )
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        nextNamePtr = ospvResponse;

        do
        {
            nextNamePtr = OSPM_STRSTR( nextNamePtr, ospvSearchField );

            if ( nextNamePtr == OSPC_OSNULL )
            {
                retVal = OSPC_ERR_ENROLL_SEARCH_FIELD_NOT_FOUND;
            }

            if ( retVal == OSPC_ERR_NO_ERROR )
            {
				OSPM_DBGMISC(( "nextNamePtr: <%s>\n", nextNamePtr ));
                if ( OSPM_SSCANF( 
                         nextNamePtr, 
                         OSPC_ENROLL_SEARCH_PATTERN, 
                         beginValuePtr ) 
                    > 0 )
                {
                    if ( beginValuePtr == OSPC_OSNULL )
                    {
                        /* Do nothing; the field value was empty. */
						OSPM_DBGMISC(( "The value for this field is empty; ignonring current value.\n" ));
                    }    

                    else if ( ( nextNamePtr == ospvResponse ) ||
                              ( OSPM_STRCHR( OSPC_ENROLL_URL_FIELD_DELIMITERS, *(nextNamePtr-1) ) ) ) 
                    {
                        valueFound = OSPC_TRUE;
                    }

					else
					{
						OSPM_DBGMISC(( "False hit.\n" ));
					}
                }
				else
				{
					OSPM_DBGMISC(( "The given attribute was a false hit.\n" ));
				}
            }

            /* Make sure that we don't get the same case again: */
            if ( valueFound != OSPC_TRUE )
            {
                nextNamePtr++;
            }
        } while ( ( valueFound == OSPC_FALSE ) && 
                  ( retVal == OSPC_ERR_NO_ERROR ) );
    }

    if ( ( valueFound != OSPC_TRUE ) || 
         ( beginValuePtr == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_SEARCH_FIELD_NOT_FOUND;
        OSPM_DBGERRORLOG( 
            retVal,
             "Unable to find the field requested.\n" );
        OSPM_DBGMISC(( "Unable to find field <%s> in <%s>\n", ospvSearchField, ospvResponse ));
    }

    /* Copy the contents of the string found into the outgoing string if
     * we found something we can use:
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        retVal = OSPPCopyString( ospvValueOut, beginValuePtr );
        if ( retVal != OSPC_ERR_NO_ERROR )
        {
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to copy value found into the outgoing pointer.\n" );
        }
    }

    /* Now free up the pointer we found, regardless of whether or not
     * we were successful:
     */
    if ( beginValuePtr != OSPC_OSNULL )
    {
        OSPM_FREE( beginValuePtr );
    }

    OSPM_DBGEXIT(( "EXIT: OSPPExtractFieldFromResponse\n" ));

    return retVal;

}


/* copy the source string to the destination string; allocate and initialize
 * enough space in the destination string that we can fit everything in
 * the source string.
 *
 * Input: pointer to the destination string and the source string to be
 *        copied.
 *
 * Output: *ospvDestStr should be a replica of ospvSrcStr if everything goes
 *         well; in this case, OSPC_ERR_NO_ERROR is returned. Otherwise,
 *         an error code other than OSPC_ERR_NO_ERROR is returned.
 */
int OSPPCopyString (
    unsigned char** ospvDestStr,
    unsigned char*  ospvSrcStr
)
{
    int retVal = OSPC_ERR_NO_ERROR;

    /* Don't assign the source string to the destination string 
     * until the end; we'll perform all of our operations on this 
     * string instead: 
     */
    unsigned char* tmpDestStr = OSPC_OSNULL;

    /* This is the length of the source string: */
    int            srcStrLen  = 0;

    
    OSPM_DBGENTER(( "ENTER OSPPCopyString\n" ));

    /* Check for valid inputs first: */
    if ( ( ospvDestStr == OSPC_OSNULL ) ||
         ( ospvSrcStr == OSPC_OSNULL ) )
    {
        retVal = OSPC_ERR_ENROLL_INVALID_ARG;
        OSPM_DBGERRORLOG( 
            retVal, 
            "The parameters for copying a string were invalid.\n" );
    }

    /* if ( the parameters look ok ) then
     *  o get the length of the source string
     *  o if ( the source is empty ) then
     *      - set an error code and complain.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        srcStrLen = OSPM_STRLEN( ospvSrcStr );
        if ( srcStrLen <= 0 )
        {
            retVal = OSPC_ERR_ENROLL_SOURCE_STRING_EMPTY;
            OSPM_DBGERRORLOG( 
                retVal,
                 "Unable to copy empty source string.\n" );
        }
    }

    /* If ( the source string is ok ) then
     *  o allocate enough memory for the output string
     *  o if ( we couldn't do it ) then
     *      - complain
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MALLOC( tmpDestStr, unsigned char, srcStrLen + 1 );

        if ( tmpDestStr == OSPC_OSNULL )
        {
            retVal = OSPC_ERR_ENROLL_NO_MEMORY;
            OSPM_DBGERRORLOG( 
                retVal, 
                "Unable to allocate memory for destination string.\n" );
        }
    }

    /* If ( we could allocate the memory ) then
     *  o initialize the memory
     *  o copy the strings
     *  o assign the string created to the outgoing pointer.
     */
    if ( retVal == OSPC_ERR_NO_ERROR )
    {
        OSPM_MEMSET( tmpDestStr, 0, srcStrLen + 1 ); 
        OSPM_STRCPY( tmpDestStr, ospvSrcStr );
        *ospvDestStr = tmpDestStr;
    }

    /* Else we had some problem so
     *  o free up any memory we may have allocated:
     */
    else
    {
        if ( tmpDestStr != OSPC_OSNULL )
        {
            OSPM_FREE( tmpDestStr );
        }
    }

    OSPM_DBGEXIT(( "EXIT OSPPCopyString\n" ));

    return retVal;

}
