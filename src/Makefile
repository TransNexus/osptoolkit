#
# OSP Library Makefile
#
INCLDIR = ../include
INSTALL_PATH=/usr/local

LFLAGS   = -lsocket -lnsl -lm -lpthread
ARLIB    = ../lib/libosp.a

# WARNING !!
# If not using gcc for compiling the toolkit
# and instead using the Sun Compiler, please comment the line below for proper 
# compilation.
#
GCCFLAGS = -Wall -D_GNU_SOURCE -fPIC

#
# CFLAGS definition
# This is for DEBUG mode of compilation
#CFLAGS	= -g -DOSP_ALLOW_DUP_TXN -DOSP_NO_DELETE_CHECK -DOSP_SDK -D_POSIX_THREADS -D_REENTRANT -DOPENSSL_NO_KRB5 $(GCCFLAGS) -DOSPC_DEBUG

# This is for PRODUCTION mode of compilation
CFLAGS	= -O -DOSP_ALLOW_DUP_TXN -DOSP_NO_DELETE_CHECK -DOSP_SDK -D_POSIX_THREADS -D_REENTRANT -DOPENSSL_NO_KRB5 $(GCCFLAGS)


#
#
# SSL library selection
# for OpenSSL
SSLOBJ    = ospopenssl.o
SSLINC    = ../crypto
INCL = -I$(SSLINC) -I$(INCLDIR)

####################################################################
OBJS    =   osppkcs1.o osppkcs8.o osppkcs7.o ospcryptowrap.o ospasn1ids.o \
            ospasn1object.o ospx509.o ospasn1.o ospasn1primitives.o \
            ospasn1parse.o ospcrypto.o osptnlog.o ospsecssl.o  ospsecurity.o \
            osplist.o osphttp.o ospxml.o ospmime.o ospprovider.o \
            ospproviderapi.o ospsocket.o ospcomm.o osputils.o ospmsgque.o \
            ospmsginfo.o osptransapi.o osptrans.o ospinit.o \
            ospmsgelem.o ospdest.o ospusage.o ospmsgattr.o ospcallid.o \
            osptoken.o ospmsgutil.o ospmsgdesc.o ospostime.o ospxmltype.o \
            ospxmlparse.o ospxmlattr.o ospxmlutil.o ospxmlenc.o ospxmlelem.o \
            ospusageind.o ospstatus.o ospauthreq.o ospauthrsp.o ospauthind.o \
            ospauthcnf.o ospreauthreq.o ospreauthrsp.o ospusagecnf.o ospb64.o \
            ospbfr.o osptokeninfo.o  ospfail.o ospaltinfo.o ospssl.o  ospstatistics.o osptnprobe.o \
            ospaudit.o osptnaudit.o osptransids.o ospciscoext.o ospcapind.o ospcapcnf.o $(SSLOBJ)

.SUFFIXES: .o .c 

.c.o:
	cc $(CFLAGS) $(CRYPTOCFLAGS) $(INCL) $(<) -c

default:
	@ echo 'Use - make build, to build the osp library'
	@ echo ""
	@ echo 'Use - make install, to install the osp library'
	@ echo '      make install will install the files in: $(INSTALL_PATH)'
	@ echo '      Modify the INSTALL_PATH variable to install the files at a different location'
	@ echo '      make install might require root previliges'


build  : $(OBJS)
	ar -r $(ARLIB) $(OBJS)

install: build
	cp -r ../include/osp/ $(INSTALL_PATH)/include/
	cp ../lib/libosp.a $(INSTALL_PATH)/lib

clean   :
	rm -f $(OBJS) $(ARLIB)

# DO NOT DELETE

