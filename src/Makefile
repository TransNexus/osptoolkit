#
# OSP Library Makefile
#

# Default install path
INSTALL_PATH = /usr/local

# OSP directory
INCDIR = ../include
LIBDIR = ../lib

# SSL library selection 
# For OpenSSL
# Use case, in order to use openssl11 on CentOS export SSLINCDIR=-I/usr/include/openssl11
SSLOBJ = ospopenssl.o

# Compiler tools
CC = gcc
AR = ar

# WARNING !!
# If not using gcc for compiling the toolkit and instead using the Sun Compiler,
# please comment the line below for proper compilation.
GCCFLAGS = -Wall -D_GNU_SOURCE -fPIC

# Macro definition
MFLAGS = -DOPENSSL_NO_KRB5 -D_POSIX_THREADS -D_REENTRANT -DOSP_SDK -DOSP_ALLOW_DUP_TXN -DOSP_NO_DELETE_CHECK -DOSP_SSL_VERIFY_NONE

# Debug flags
# This is for DEBUG mode of compilation
# DFLAGS = -g -DOSPC_DEBUG
# This is for PRODUCTION mode of compilation
DFLAGS = -O

# Compile flags
CFLAGS = $(MFLAGS) $(DFLAGS) $(GCCFLAGS)

# Include flags
IFLAGS = $(SSLINCDIR) -I$(INCDIR)

################################################################################

OSPOBJS = osppkcs1.o osppkcs8.o osppkcs7.o ospcryptowrap.o ospasn1ids.o \
          ospasn1object.o ospx509.o ospasn1.o ospasn1primitives.o \
          ospasn1parse.o ospcrypto.o osptnlog.o ospsecssl.o ospsecurity.o \
          osplist.o osphttp.o ospxml.o ospmime.o ospprovider.o \
          ospproviderapi.o ospsocket.o ospcomm.o osputils.o ospmsgque.o \
          ospmsginfo.o osptransapi.o osptrans.o ospinit.o \
          ospmsgelem.o ospdest.o ospusage.o ospmsgattr.o ospcallid.o \
          osptoken.o ospmsgutil.o ospmsgdesc.o ospostime.o ospxmltype.o \
          ospxmlparse.o ospxmlattr.o ospxmlutil.o ospxmlenc.o ospxmlelem.o \
          ospusageind.o ospstatus.o ospauthreq.o ospauthrsp.o ospauthind.o \
          ospauthcnf.o ospreauthreq.o ospreauthrsp.o ospusagecnf.o ospb64.o \
          ospbfr.o osptokeninfo.o ospfail.o ospaltinfo.o ospssl.o \
          ospstatistics.o osptnprobe.o ospaudit.o osptnaudit.o osptransids.o \
          ospciscoext.o ospcapind.o ospcapcnf.o ospstir.o $(SSLOBJ)
          
OSPLIB = $(LIBDIR)/libosptk.a

.SUFFIXES: .o .c 

.c.o:
	$(CC) $(CFLAGS) $(CRYPTOCFLAGS) $(IFLAGS) -c $(<)

default:
	@ echo 'Use - make build, to build the osp library'
	@ echo ""
	@ echo 'Use - make install, to install the osp library'
	@ echo '      make install will install the files in: $(INSTALL_PATH)'
	@ echo '      Modify the INSTALL_PATH variable to install the files at a different location'
	@ echo '      make install might require root previliges'

build: $(OSPLIB)

$(OSPLIB): $(OSPOBJS)
	$(AR) -r $(OSPLIB) $(OSPOBJS)

install: build
	install -d $(INSTALL_PATH)/include/osp
	cp $(INCDIR)/osp/*.h $(INSTALL_PATH)/include/osp/
	install -d $(INSTALL_PATH)/lib
	cp $(OSPLIB) $(INSTALL_PATH)/lib/

clean:
	rm -f $(OSPLIB) $(OSPOBJS)

# DO NOT DELETE

