CC       = $(COMPILER)
OSP_NO_DELETE_CHECK = -DOSP_NO_DELETE_CHECK

CFLAGS   = $(COMPFLAGS) \
           $(OSPC_DEBUG) \
           $(ENABLETESTVERIFY) \
           $(ENABLESMIME) \
           $(OSPTNLOGDEBUG) \
           $(OSPC_VERIFY_BUILDTOKEN) \
           $(OSP_NO_DELETE_CHECK)  \
           $(DISABLESECLOCK) \
           $(IGNOREOUTOFDATERRR) \
           $(OSP_MODE) \
           $(OSP_TOKEN) \
           $(OSPC_USE_CDATA_ONLY) \
           $(OSPC_ENABLE_SSL_CLIENT_AUTHENTICATION) \
           $(OSPC_HW_ACCEL)  \
           $(OSP_OTHER)  \
           -D_POSIX_THREADS \
           -D_REENTRANT \
           -I$(INCLDIR) 
