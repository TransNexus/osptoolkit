#
# TEST_APP Makefile
#
INCLDIR = ../include
#
# ***** Platform specific flags ******
#
CC         = g++
CFLAGS     = -g $(PROFILE) -I. -I$(INCLDIR) $(OSPTNLOGDEBUG) -D_POSIX_THREADS -D_REENTRANT -DOSP_SDK
#CC         = /usr/vws/bin/cc
#CFLAGS     =  -g -v -xCC -I. -I$(INCLDIR) $(OSPTNLOGDEBUG) -D_POSIX_THREADS -D_REENTRANT -DOSP_SDK
LFLAGS	   = $(PROFILELIB) -lsocket -lnsl -lpthread -lmalloc  -ldl -lm
LFLAGS_LINUX	   = $(PROFILELIB) -lpthread -lm
SOFLAGS    = -shared 
SOOBJ      = libosp.so
AR         = /usr/ccs/bin/ar
ARLIBDIR   = ../lib
ARLIB      = -losp

#LINT	   = /usr/ccs/bin/ucblint
MKDEP      = /usr/openwin/bin/makedepend
SSLINC     = -I../crypto
SSLLIBDIR  = $../lib
SSLLIB     = -lssl -lcrypto
SSL        = -L$(SSLLIBDIR) $(SSLLIB)

#
####################################################################
OBJS	= nonblocking.o syncque.o test_app.o

.SUFFIXES: .o .c

.c.o:
	$(CC) $(CFLAGS) $(SSLINC) $(<) -c

all: 	$(OBJS)
	$(PURE) $(CC) $(PROFILE) $(OBJS) -o ../bin/test_app \
	$(ARLIBDIR)/libosp.a $(LFLAGS) -L$(ARLIBDIR) $(ARLIB) $(SSL)

linux: 	$(OBJS)
	$(PURE) $(CC) $(PROFILE) $(OBJS) -o ../bin/test_app \
	$(ARLIBDIR)/libosp.a $(LFLAGS_LINUX) -L$(ARLIBDIR) $(ARLIB) $(SSL)

clean :
	rm -f *.o ../bin/test_app
