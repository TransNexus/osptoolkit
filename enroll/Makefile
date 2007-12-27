#
# OSP Enroll Makefile
#

# OSP direcory
INCDIR = ../include
LIBDIR = ../lib
BINDIR = ../bin

# OSP library
OSPLIB = -L$(LIBDIR) -losptk

# For OpenSSL:
SSLINCDIR = ../crypto
SSLLIBDIR = ../lib
SSLLIBS = -L$(SSLLIBDIR) -lssl -lcrypto

# Compiler tools
CC = gcc

# WARNING !!
# If not using gcc for compiling the toolkit and instead using the Sun Compiler,
# please comment the line below for proper compilation.
GCCFLAGS = -Wall

# Macro definition
MFLAGS = -DOPENSSL_NO_KRB5 -D_POSIX_THREADS -D_REENTRANT -DOSP_SDK -DSOLARIS

# Debug flags
# This is for DEBUG mode of compilation
#DFLAGS = -g -DOSPC_DEBUG=1
# This is for PRODUCTION mode of compilation
DFLAGS = -O

# Compile flags
CFLAGS = $(MFLAGS) $(DFLAGS) $(GCCFLAGS)

# Include flags
IFLAGS = -I./ -I$(INCDIR) -I$(SSLINCDIR)

# Library flags
LIBS = $(OSPLIB) $(SSLLIBS) -lpthread -ldl -lm -lnsl -lsocket
LIBS_LINUX = $(OSPLIB) $(SSLLIBS) -lpthread -ldl -lm

################################################################################

ENOBJS = osptnepinit.o osptnepenroll.o osptnep.o osptneputil.o

ENROLL = $(BINDIR)/enroll

.SUFFIXES: .o .c

.c.o:
	$(PURIFY) $(CC) $(CFLAGS) $(IFLAGS) -c $(<)

default: $(ENOBJS)
	$(PURIFY) $(CC) -o $(ENROLL) $(ENOBJS) $(LIBS)

linux: $(ENOBJS)
	$(PURIFY) $(CC) -o $(ENROLL) $(ENOBJS) $(LIBS_LINUX)

clean:
	rm -f $(ENROLL) $(ENOBJS)
