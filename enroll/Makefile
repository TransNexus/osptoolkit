#Compiler
CC            = gcc



# For OpenSSL:
SSLINC        = ../crypto
SSL_LIBDIRS   = -L../lib
SSL_LIBS      = -lssl -lcrypto
SSL_OBJS      = ../src/ospopenssl.o



INCLDIRS      = -I./ -I../include -I$(SSLINC)
#WARNING_FLAGS = -Wall
CFLAGS        = -g $(WARNING_FLAGS)

MACRO_FLAGS   = -D_POSIX_THREADS -DSOLARIS -DOSPD_DEBUG -DOSP_SDK -DOPENSSL_NO_KRB5 -DOSPC_DEBUG=1

LIBS          = -lnsl -lsocket -ldl -lm -losp $(SSL_LIBS) -lpthread 
LIBS_LINUX    = -losp $(SSL_LIBS) -lpthread -ldl -lm
OSP_LIBDIR    = -L../lib
LIBDIRS       = $(OSP_LIBDIR) $(SSL_LIBDIRS)

EXEC          =../bin/enroll

CREATED_OBJS  = osptnepinit.o osptnepenroll.o osptnep.o osptneputil.o
OBJS          = $(CREATED_OBJS) $(OSP_OBJS) $(SSL_OBJS)


# Rule for converting all of the source files into objects:
.c.o:
	$(PURIFY) cc -c $(CFLAGS) -c $(MACRO_FLAGS) $(INCLDIRS) $(<) 

# Rule for creating the output executable:
enroll: $(OBJS)
	$(PURIFY) cc -o $(EXEC) $(OBJS) $(LIBDIRS) $(LIBS) $(INCLDIRS)

linux: $(OBJS)
	$(PURIFY) cc -o $(EXEC) $(OBJS) $(LIBDIRS) $(LIBS_LINUX) $(INCLDIRS)

clean:
	rm -f $(CREATED_OBJS) $(EXEC)
