#This make file is for developer use. The Makefile will
#be shipped with the toolkit.

CC            = gcc
INCLDIRS      = -I./ -I$(CVSENVROOT)/src/include/osp \
                -I/usr/local/ssl/include

WARNING_FLAGS = -Wall
CFLAGS        = -g $(WARNING_FLAGS)

MACRO_FLAGS   = -D_POSIX_THREADS -DSOLARIS -DOSPD_DEBUG -DOSP_SDK -DOSPC_DEBUG=1

#SSL_LIBS      = -lsslref-bsafe
#SSL_OBJS      = ../ospsslref.o

SSL_LIBS      = -lssl -lcrypto
SSL_OBJS      = ../ospopenssl.o
SSL_LIBDIRS   = -L/usr/local/ssl/lib
#
LIBS          = -lnsl -lsocket -ldl -lm $(SSL_LIBS) -losp -lpthread 
OSP_LIBDIR    = $(CVSENVROOT)/lib/client/$(TARGET_OS)
LIBDIRS       = -L$(CVSENVROOT)/lib -L$(OSP_LIBDIR) $(SSL_LIBDIRS) 

EXEC          = $(CVSENVROOT)/bin/enroll

CREATED_OBJS  = osptnepinit.o osptnepenroll.o osptnep.o osptneputil.o
#OSP_OBJS      = ../ospbsafetstd.o
OBJS          = $(CREATED_OBJS) $(OSP_OBJS) $(SSL_OBJS)


	$(PURIFY) $(CC) -c $(CFLAGS) -c $(MACRO_FLAGS) $(INCLDIRS) $(<) 

	$(PURIFY) $(CC) -o $(EXEC) $(OBJS) $(LIBDIRS) $(LIBS) $(INCLDIRS)

	rm -f $(CREATED_OBJS) $(EXEC)
