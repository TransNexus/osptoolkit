# Microsoft Developer Studio Project File - Name="osp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=osp - Win32 Debug_Non_Secure
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "osp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "osp.mak" CFG="osp - Win32 Debug_Non_Secure"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "osp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "osp - Win32 Debug_Secure" (based on "Win32 (x86) Static Library")
!MESSAGE "osp - Win32 Debug_Non_Secure" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "osp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release_Secure"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\crypto" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "OSPC_USE_SIGNED_TOKEN" /D "OSPC_HW_ACCEL" /D "OSP_SDK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "osp - Win32 Debug_Secure"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Secure"
# PROP BASE Intermediate_Dir "Debug_Secure"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug_Secure"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "OSP_SDK" /D "BSAFE" /D "OSPC_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\crypto" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "OSP_SDK" /D "OSPC_DEBUG" /D "OSPC_USE_SIGNED_TOKEN" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "osp - Win32 Debug_Non_Secure"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Non_Secure"
# PROP BASE Intermediate_Dir "Debug_Non_Secure"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug_Non_Secure"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\crypto" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "OSP_SDK" /D "BSAFE" /D "OSPC_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../include" /I "../crypto" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "OSP_SDK" /D "OSPC_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "osp - Win32 Release"
# Name "osp - Win32 Debug_Secure"
# Name "osp - Win32 Debug_Non_Secure"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ospaltinfo.c
# End Source File
# Begin Source File

SOURCE=.\ospasn1.c
# End Source File
# Begin Source File

SOURCE=.\ospasn1ids.c
# End Source File
# Begin Source File

SOURCE=.\ospasn1object.c
# End Source File
# Begin Source File

SOURCE=.\ospasn1parse.c
# End Source File
# Begin Source File

SOURCE=.\ospasn1primitives.c
# End Source File
# Begin Source File

SOURCE=.\ospaudit.c
# End Source File
# Begin Source File

SOURCE=.\ospauthcnf.c
# End Source File
# Begin Source File

SOURCE=.\ospauthind.c
# End Source File
# Begin Source File

SOURCE=.\ospauthreq.c
# End Source File
# Begin Source File

SOURCE=.\ospauthrsp.c
# End Source File
# Begin Source File

SOURCE=.\ospb64.c
# End Source File
# Begin Source File

SOURCE=.\ospbfr.c
# End Source File
# Begin Source File

SOURCE=.\ospcallid.c
# End Source File
# Begin Source File

SOURCE=.\ospciscoext.c
# End Source File
# Begin Source File

SOURCE=.\ospcomm.c
# End Source File
# Begin Source File

SOURCE=.\ospcrypto.c
# End Source File
# Begin Source File

SOURCE=.\ospcryptowrap.c
# End Source File
# Begin Source File

SOURCE=.\ospdest.c
# End Source File
# Begin Source File

SOURCE=.\ospfail.c
# End Source File
# Begin Source File

SOURCE=.\osphttp.c
# End Source File
# Begin Source File

SOURCE=.\ospinit.c
# End Source File
# Begin Source File

SOURCE=.\osplist.c
# End Source File
# Begin Source File

SOURCE=.\ospmime.c
# End Source File
# Begin Source File

SOURCE=.\ospmsgattr.c
# End Source File
# Begin Source File

SOURCE=.\ospmsgdesc.c
# End Source File
# Begin Source File

SOURCE=.\ospmsgelem.c
# End Source File
# Begin Source File

SOURCE=.\ospmsginfo.c
# End Source File
# Begin Source File

SOURCE=.\ospmsgque.c
# End Source File
# Begin Source File

SOURCE=.\ospmsgutil.c
# End Source File
# Begin Source File

SOURCE=.\ospopenssl.c
# End Source File
# Begin Source File

SOURCE=.\ospostime.c
# End Source File
# Begin Source File

SOURCE=.\osppkcs1.c
# End Source File
# Begin Source File

SOURCE=.\osppkcs7.c
# End Source File
# Begin Source File

SOURCE=.\osppkcs8.c
# End Source File
# Begin Source File

SOURCE=.\ospprovider.c
# End Source File
# Begin Source File

SOURCE=.\ospproviderapi.c
# End Source File
# Begin Source File

SOURCE=.\ospreauthreq.c
# End Source File
# Begin Source File

SOURCE=.\ospreauthrsp.c
# End Source File
# Begin Source File

SOURCE=.\ospsecssl.c
# End Source File
# Begin Source File

SOURCE=.\ospsecurity.c
# End Source File
# Begin Source File

SOURCE=.\ospsocket.c
# End Source File
# Begin Source File

SOURCE=.\ospssl.c
# End Source File
# Begin Source File

SOURCE=.\ospstatistics.c
# End Source File
# Begin Source File

SOURCE=.\ospstatus.c
# End Source File
# Begin Source File

SOURCE=.\osptnaudit.c
# End Source File
# Begin Source File

SOURCE=.\osptnlog.c
# End Source File
# Begin Source File

SOURCE=.\osptnprobe.c
# End Source File
# Begin Source File

SOURCE=.\osptoken.c
# End Source File
# Begin Source File

SOURCE=.\osptokeninfo.c
# End Source File
# Begin Source File

SOURCE=.\osptrans.c
# End Source File
# Begin Source File

SOURCE=.\osptransapi.c
# End Source File
# Begin Source File

SOURCE=.\osptransids.c
# End Source File
# Begin Source File

SOURCE=.\ospusage.c
# End Source File
# Begin Source File

SOURCE=.\ospusagecnf.c
# End Source File
# Begin Source File

SOURCE=.\ospusageind.c
# End Source File
# Begin Source File

SOURCE=.\osputils.c
# End Source File
# Begin Source File

SOURCE=.\ospx509.c
# End Source File
# Begin Source File

SOURCE=.\ospxml.c
# End Source File
# Begin Source File

SOURCE=.\ospxmlattr.c
# End Source File
# Begin Source File

SOURCE=.\ospxmlelem.c
# End Source File
# Begin Source File

SOURCE=.\ospxmlenc.c
# End Source File
# Begin Source File

SOURCE=.\ospxmlparse.c
# End Source File
# Begin Source File

SOURCE=.\ospxmltype.c
# End Source File
# Begin Source File

SOURCE=.\ospxmlutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\osp.h
# End Source File
# Begin Source File

SOURCE=..\include\ospaltinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\ospasn1.h
# End Source File
# Begin Source File

SOURCE=..\include\ospasn1ids.h
# End Source File
# Begin Source File

SOURCE=..\include\ospaudit.h
# End Source File
# Begin Source File

SOURCE=..\include\ospauthcnf.h
# End Source File
# Begin Source File

SOURCE=..\include\ospauthind.h
# End Source File
# Begin Source File

SOURCE=..\include\ospauthreq.h
# End Source File
# Begin Source File

SOURCE=..\include\ospauthrsp.h
# End Source File
# Begin Source File

SOURCE=..\include\ospb64.h
# End Source File
# Begin Source File

SOURCE=..\include\ospbfr.h
# End Source File
# Begin Source File

SOURCE=..\include\ospcallid.h
# End Source File
# Begin Source File

SOURCE=..\include\ospciscoext.h
# End Source File
# Begin Source File

SOURCE=..\include\ospcode.h
# End Source File
# Begin Source File

SOURCE=..\include\ospcomm.h
# End Source File
# Begin Source File

SOURCE=..\include\ospconfig.h
# End Source File
# Begin Source File

SOURCE=..\include\ospcrypto.h
# End Source File
# Begin Source File

SOURCE=..\include\ospcryptowrap.h
# End Source File
# Begin Source File

SOURCE=..\include\ospcustomdebug.h
# End Source File
# Begin Source File

SOURCE=..\include\ospdatatypes.h
# End Source File
# Begin Source File

SOURCE=..\include\ospdebug.h
# End Source File
# Begin Source File

SOURCE=..\include\ospdest.h
# End Source File
# Begin Source File

SOURCE=..\include\osperrno.h
# End Source File
# Begin Source File

SOURCE=..\include\ospfail.h
# End Source File
# Begin Source File

SOURCE=..\include\osphttp.h
# End Source File
# Begin Source File

SOURCE=..\include\ospinit.h
# End Source File
# Begin Source File

SOURCE=..\include\ospkeys.h
# End Source File
# Begin Source File

SOURCE=..\include\osplist.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmime.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsg.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsgattr.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsgdesc.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsgelem.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsginfo.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsgpart.h
# End Source File
# Begin Source File

SOURCE=..\include\ospmsgque.h
# End Source File
# Begin Source File

SOURCE=..\include\osposincl.h
# End Source File
# Begin Source File

SOURCE=..\include\ospossys.h
# End Source File
# Begin Source File

SOURCE=..\include\ospostime.h
# End Source File
# Begin Source File

SOURCE=..\include\osppkcs1.h
# End Source File
# Begin Source File

SOURCE=..\include\osppkcs7.h
# End Source File
# Begin Source File

SOURCE=..\include\osppkcs8.h
# End Source File
# Begin Source File

SOURCE=..\include\ospprovider.h
# End Source File
# Begin Source File

SOURCE=..\include\ospproviderapi.h
# End Source File
# Begin Source File

SOURCE=..\include\ospreauthreq.h
# End Source File
# Begin Source File

SOURCE=..\include\ospreauthrsp.h
# End Source File
# Begin Source File

SOURCE=..\include\ospsecurity.h
# End Source File
# Begin Source File

SOURCE=..\include\ospsocket.h
# End Source File
# Begin Source File

SOURCE=..\include\ospssl.h
# End Source File
# Begin Source File

SOURCE=..\include\ospsslsess.h
# End Source File
# Begin Source File

SOURCE=..\include\ospstatistics.h
# End Source File
# Begin Source File

SOURCE=..\include\ospstatus.h
# End Source File
# Begin Source File

SOURCE=..\include\osptnaudit.h
# End Source File
# Begin Source File

SOURCE=..\include\osptnlog.h
# End Source File
# Begin Source File

SOURCE=..\include\osptnprobe.h
# End Source File
# Begin Source File

SOURCE=..\include\osptoken.h
# End Source File
# Begin Source File

SOURCE=..\include\osptokeninfo.h
# End Source File
# Begin Source File

SOURCE=..\include\osptrans.h
# End Source File
# Begin Source File

SOURCE=..\include\osptransapi.h
# End Source File
# Begin Source File

SOURCE=..\include\osptransids.h
# End Source File
# Begin Source File

SOURCE=..\include\ospusage.h
# End Source File
# Begin Source File

SOURCE=..\include\ospusagecnf.h
# End Source File
# Begin Source File

SOURCE=..\include\ospusageind.h
# End Source File
# Begin Source File

SOURCE=..\include\osputils.h
# End Source File
# Begin Source File

SOURCE=..\include\ospx500.h
# End Source File
# Begin Source File

SOURCE=..\include\ospx509.h
# End Source File
# Begin Source File

SOURCE=..\include\ospxml.h
# End Source File
# Begin Source File

SOURCE=..\include\ospxmlattr.h
# End Source File
# Begin Source File

SOURCE=..\include\ospxmldoc.h
# End Source File
# Begin Source File

SOURCE=..\include\ospxmlelem.h
# End Source File
# Begin Source File

SOURCE=..\include\ospxmltype.h
# End Source File
# End Group
# End Target
# End Project
