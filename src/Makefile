#
# OSP Library Makefile
#
INCLDIR = ../include

#
# ***** Platform specific flags ******
#
# Solaris (POSIX threads)

include ospcflags.inc

#TARGET_OS should be set in release builder's environment.!!!!

#
LFLAGS   = -lsocket -lnsl -lpthread
SOFLAGS  = -shared 
SOOBJ    = libosp.so
AR       = /usr/ccs/bin/ar
ARLIB    = $(TARGETDIR)/libosp.a
#LINT     = /opt/SUNWspro/bin/lint
MKDEP    = /usr/openwin/bin/makedepend

#
#
# SSL library selection
#

# for OpenSSL
SSLOBJ    = ospopenssl.o
SSLLIB    = ../lib/libssl.a
SSLINC    = ../crypto
SSLCFLAGS = -I$(SSLINC)

#
# for No SSL
#
#SSLOBJ = ospnossl.o

#
# How to lint
#
LINT=OSPLINT -s -Xc -I./ $(SSLCFLAGS) $(CRYPTOCFLAGS) -D_POSIX_THREADS -D_REENTRANT -DOSP_SDK 
#
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
            ospaudit.o osptnaudit.o osptransids.o ospciscoext.o $(SSLOBJ) $(CRYPTOADDOBJ) ospversion.o

.SUFFIXES: .o .c 

.c.o:
	$(CC) $(CFLAGS) $(CRYPTOCFLAGS) $(SSLCFLAGS) $(<) -c

#.c.o:
# $(LINT) $(<) ; $(CC) $(CFLAGS) $(CRYPTOCFLAGS) $(SSLCFLAGS) $(<) -c

libosp  : $(OBJS)
	$(AR) -r $(ARLIB) $(OBJS)

clean   :
	rm -f $(OBJS) $(ARLIB)

depend  :
	$(MKDEP) -I$(INCLDIR) *.c

# DO NOT DELETE

