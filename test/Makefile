#
# OSP Test Applicatopn Makefile
#

# OSP direcory
INCDIR = ../include
LIBDIR = ../lib
BINDIR = ../bin

# OSP library
OSPLIB = -L$(LIBDIR) -losptk

# For OpenSSL:
SSLINCDIR = ../crypto
SSLLIBDIR = /lib
SSLLIBS = -L$(SSLLIBDIR) -lssl -lcrypto

# Compiler tools
CC = g++

# WARNING !!
# If not using gcc for compiling the toolkit and instead using the Sun Compiler,
# please comment the line below for proper compilation.
GCCFLAGS = -Wall

# Macro definition
MFLAGS = -DOPENSSL_NO_KRB5 -D_POSIX_THREADS -D_REENTRANT -DOSP_SDK

# Debug flags
# This is for DEBUG mode of compilation
DFLAGS = $(OSPTNLOGDEBUG) -g -DOSPC_DEBUG=1
# This is for PRODUCTION mode of compilation
#DFLAGS = $(OSPTNLOGDEBUG) -O

# Compile flags
CFLAGS = $(MFLAGS) $(DFLAGS) $(GCCFLAGS) $(PROFILE)

# Include flags
IFLAGS = -I./ -I$(INCDIR) -I$(SSLINCDIR)

# Library flags
LIBS = $(OSPLIB) $(SSLLIBS) -lutf8proc -lpthread -ldl -lm -lnsl -lsocket -lmtmalloc $(PROFILELIB)
LIBS_LINUX = $(OSPLIB) $(SSLLIBS) -lutf8proc -lpthread -ldl -lm $(PROFILELIB)

################################################################################

TESTOBJS = nonblocking.o syncque.o test_app.o

TESTAPP = $(BINDIR)/test_app

.SUFFIXES: .o .c

.c.o:
	$(PURIFY) $(CC) $(CFLAGS) $(IFLAGS) -c $(<)

default: $(TESTOBJS)
	$(PURIFY) $(CC) -o $(TESTAPP) $(TESTOBJS) $(LFLAGS) $(LIBS) $(PROFILE)
	$(PURIFY) $(CC) -o connect-test connect-test.c $(LFLAGS) $(LIBS) $(PROFILE)

linux: $(TESTOBJS)
	$(PURIFY) $(CC) -o $(TESTAPP) $(TESTOBJS) $(LFLAGS) $(LIBS_LINUX) $(PROFILE)
	$(PURIFY) $(CC) -o connect-test connect-test.c $(LFLAGS) $(LIBS_LINUX) $(PROFILE)

clean :
	rm -f $(TESTAPP) $(TESTOBJS)
	rm -f connect-test
