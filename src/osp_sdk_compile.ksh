#!/bin/ksh
#
#########################################################################
#########################################################################
#########################################################################
#                                                               
#   COPYRIGHT (c) 1998, 1999, 2000 by TransNexus, LLC                          
#                                                                    
#   This software contains proprietary and confidential information  
#   of TransNexus, LLC. Except as may be set forth in the license    
#   agreement under which this software is supplied, use, disclosure, 
#   or reproduction is prohibited without the prior, express, written
#   consent of TransNexus, LLC.                                      
#                                     
#
# $Date: 2003/02/26 15:45:41 $
#
# $Id: osp_sdk_compile.ksh,v 1.4 2003/02/26 15:45:41 vmathur Exp $
#
# $RCSfile: osp_sdk_compile.ksh,v $
#
# $Revision: 1.4 $
#
# $Source: /tmp/OSPclient/osptoolkit/src/Attic/osp_sdk_compile.ksh,v $
#
##########################################################################
##########################################################################
##########################################################################
#
#
# Compile the various aromas and flavors of OSP.
#
#  ~ Build Flags ~ 
#
# Modes:
#  -o client   => Build an OSP Client Library (default)
#
# Token Security:
#  -o secure_token     => Issue signed <TokenInfo> tokens (default)
#  -o non_secure_token => Issue clear text <TokenInfo> tokens
#
# S/MIME Message Support:
#  -o smime   => Produce S/MIME Requests and Verify Responses
#
# SSL Client Authentication
#  -o ssl_client_auth => Enable SSL Client Authentication
#
# Token Encoding:
#  -o b64_encode => Base64 encode tokens (default)
#  -o cdata_only => CDATA encode tokens
#
# Performance:
#  -o optimized      => Use -O3 level compiler optimization (default)
#  -o hardware_accel => Enable hardware crypto acceleration (default)
#
# Diagnostics:
#  -o debug             => Enable full OSP debugging
#  -o with_symbols      => Use -g (w/gcc) compiler debug option 
#  -o no_hardware_accel => Disable hardware crypto acceleration
#  -o gprof             => Enable gprof profiling
#
# Compiler:
#  -o spro  => Use SparcWorks compiler
#  -o gcc   => Use GNU gcc (default)
#  -o g++   => Use GNU g++
#
# Other:
#  -o dest_dir=dir => make library in destination dir
#  -o clean_only   => do not make library, clean only
#
#  ~ Command Line Arguments ~ 
#
# The libraries can be built for either client, server, gm_sim and/or all. In all cases, 
# the destination library directory names are based on the build mode.
# 
# all is the default mode
#
# Production: 
#   -p [client] => Build a production target for client. 
#      Enables the following flags:
#      -o gcc -o optimized -o hardware_accel -o secure_token
#
# Development:
#   -d [client] => Build a development target for client. 
#      Enables the following flags:
#      -o gcc -o debug -o with_symbols -o secure_token
#
# Non-Secure:
#   -n [client] => Build a non-secure target for client. 
#      Enables the following flags:
#      -o gcc -o debug -o with_symbols -o cdata_only
#

# command line options
export CC=${CC:-gcc}
OPTFLAGS="p:d:n:o:t:"
#TARGET_OS=solar_26
RELEASE_TAG=DEV

#
# compiler flags
# 
# GNU gcc/g++
GNU_COMMON_FLAGS="-Wall -D_GNU_SOURCE"
GNU_OPTIMIZED="-O3 $GNU_COMMON_FLAGS"
GNU_DEBUG="-g $GNU_COMMON_FLAGS"
GNU_GPROF="-pg $GNU_DEBUG"
#
# Sun WorkShop
SPRO_COMMON_FLAGS="-v"
SPRO_OPTIMIZED="-xO3 $SPRO_COMMON_FLAGS"
SPRO_DEBUG="-g $SPRO_COMMON_FLAGS"

#
# --------------------------------------------------------------------------------
#  A      - Activate Option
#  Tag    - Tag which appears in OSPVBuildVersion
#  Params - Parameters passed to Makefile
#
# 
# Modes
# 
MODES="BLD_CLIENT"
#
#      Variable       A Tag  C Params
set -A BLD_CLIENT     1 clnt 1 OSP_MODE=-DOSP_SDK 

# --------------------------------------------------------------------------------
# build options (Production default)
#
#
# All Build Options (modes excluded)
#
BLD_VARS="BLD_SEC_TOK BLD_CFLAGS BLD_HW_ACCEL"
BLD_VARS="${BLD_VARS} BLD_DEBUG BLD_CDATA_ONLY BLD_GCC BLD_GPLUSPLUS"
BLD_VARS="${BLD_VARS} BLD_SSL_CL_ATH BLD_SPRO BLD_SMIME"

set -A BLD_SEC_TOK    1 stok  1 OSP_TOKEN=-DOSPC_USE_SIGNED_TOKEN
set -A BLD_HW_ACCEL   1 hwe   1 OSPC_HW_ACCEL=-DOSPC_HW_ACCEL
set -A BLD_DEBUG      0 dbg   1 OSPC_DEBUG=-DOSPC_DEBUG
set -A BLD_CDATA_ONLY 0 cdat  1 OSPC_USE_CDATA_ONLY=-DOSPC_USE_CDATA_ONLY
set -A BLD_SSL_CL_ATH 0 sslc  1 OSPC_ENABLE_SSL_CLIENT_AUTHENTICATION=-DOSPC_ENABLE_SSL_CLIENT_AUTHENTICATION
set -A BLD_OPTIMIZED  1 opt   0
set -A BLD_CC_DEBUG   1 cdbg  0 
set -A BLD_CC_GPROF   1 gprf  0 
set -A BLD_GCC        1 gcc   1 COMPILER=gcc
set -A BLD_GPLUSPLUS  0 gpp   1 COMPILER=g++
set -A BLD_SPRO       0 spro  1 COMPILER=/usr/vws/bin/cc
set -A BLD_CFLAGS     1 opt   0 
set -A BLD_SMIME      0 smime 0 
# ---------------------------------------------------------------------------------

function Usage
{
    echo "ospcompile [options]\n"
    cat <<-[] 
where options are: 
   -p [client] => Build a production target for client.
   Enables the following flags: -o gcc -o optimized -o hardware_accel -o secure_token

   -d [client] => Build a development target for client.
   Enables the following flags: -o gcc -o debug -o with_symbols -o secure_token

   -n [client] => Build a non-secure target for client.
   Enables the following flags: -o gcc -o debug -o with_symbols -o cdata_only

   -o option -o option .... => Override options
   Option flags are: secure_token non_secure_token b64_encode cdata_only smime nodebug
                     optimized hardware_accel no_hardware_accel debug with_symbols
                     use_gcc use_gplusplus use_spro clean_only dest_dir=dir ssl_client_auth
	
   -t release_tag => CVS Release Tag.  Added to ospversion.c to identify CVS tag in code.

[]
    exit 1
}

function ShowOptions
{
    echo "BLD_CLIENT     = $BLD_CLIENT"
    echo "BLD_SEC_TOK    = $BLD_SEC_TOK"
    echo "BLD_CFLAGS     = $BLD_CFLAGS"
    echo "BLD_HW_ACCEL   = $BLD_HW_ACCEL"
    echo "BLD_DEBUG      = $BLD_DEBUG"
    echo "BLD_CDATA_ONLY = $BLD_CDATA_ONLY"
    echo "BLD_GCC        = $BLD_GCC"
    echo "BLD_GPLUSPLUS  = $BLD_GPLUSPLUS"
    echo "BLD_SPRO       = $BLD_SPRO"
}

function SetAdditionalOptions
{
    case "$1" in
    secure_token)       BLD_SEC_TOK=1
                        ;;
    non_secure_token)   BLD_SEC_TOK=0
                        ;;
    b64_encode)         BLD_CDATA_ONLY=0
                        ;;
    cdata_only)         BLD_CDATA_ONLY=1
                        ;;
    ssl_client_auth)    BLD_SSL_CL_ATH=1
                        ;;
    optimized)          BLD_OPTIMIZED=1
                        BLD_CC_DEBUG=0
                        BLD_CFLAGS[1]=${BLD_OPTIMIZED[1]}
                        ;;
    hardware_accel)     BLD_HW_ACCEL=1
                        ;;
    no_hardware_accel)  BLD_HW_ACCEL=0
                        ;;
    debug)              BLD_DEBUG=1
                        ;;
    nodebug)            BLD_DEBUG=0
                        ;;
    gprof)              BLD_DEBUG=1
                        BLD_OPTIMIZED=0
                        BLD_CFLAGS[1]=${BLD_CC_GPROF[1]}
                        export COMPFLAGS=$GNU_GPROF
                        ;;
    with_symbols)       BLD_CC_DEBUG=1
                        BLD_OPTIMIZED=0
                        BLD_CFLAGS[1]=${BLD_CC_DEBUG[1]}
                        ;;
    trx_del)            BLD_GK_SIM=1
                        ;;
    use_gcc)            BLD_GCC=1
                        BLD_GPLUSPLUS=0
                        BLD_SPRO=0
                        if [ $BLD_OPTIMIZED -eq 1 ]; then
                            export COMPFLAGS=$GNU_OPTIMIZED
                        else
                            export COMPFLAGS=$GNU_DEBUG
                        fi
                        ;;
    use_gplusplus)      BLD_GCC=0
                        BLD_GPLUSPLUS=1
                        BLD_SPRO=0
                        if [ $BLD_OPTIMIZED -eq 1 ]; then
                            export COMPFLAGS=$GNU_OPTIMIZED
                        else
                            export COMPFLAGS=$GNU_DEBUG
                        fi
                        ;;
    use_spro)           BLD_GCC=0
                        BLD_GPLUSPLUS=0
                        BLD_SPRO=1
                        if [ $BLD_OPTIMIZED -eq 1 ]; then
                            export COMPFLAGS=$SPRO_OPTIMIZED
                        else
                            export COMPFLAGS=$SPRO_DEBUG
                        fi
                        ;;
    dest_dir\=*)        BLD_DEST_DIR=`echo $1 | cut -d\= -f2`
                        ;; 
    clean_only)         CLEAN_ONLY=1
                        ;;
    smime)              BLD_SMIME=1
                        export ENABLESMIME=-DENABLESMIME
                        ;;
    *)                  Usage
                        ;;
    esac
}

function SetDestDir
{
   	case "$1" in
   	BLD_CLIENT) BLD_DEST_DIR=../lib
			;;
   	*)      echo "Invalid mode \[$i\] - can't create dest dir"
			exit
           	;;
   	esac
}

function SetMode
{
    case "$1" in

    client) BLD_CLIENT=1
            BLD_GK_SIM=0
            BLD_SERVER=0
            ;;

    *)      Usage
            ;;
    esac
}

function SetOptions
{
    while getopts $OPTFLAGS opt
    do
        case $opt in

        p)  # production #
            SetAdditionalOptions "optimized"
            SetAdditionalOptions "use_gcc"
            SetMode "$OPTARG"
            ;;

        d)  # development #
            SetAdditionalOptions "with_symbols"
            SetAdditionalOptions "use_gcc"
            SetAdditionalOptions "debug"
            SetAdditionalOptions "no_hardware_accel"
            SetMode "$OPTARG"
            ;;
        
        n)  # non-secure #
            SetAdditionalOptions "with_symbols"
            SetAdditionalOptions "use_gcc"
            SetAdditionalOptions "debug"
            SetAdditionalOptions "no_hardware_accel"
            SetAdditionalOptions "non_secure_token"
            SetMode "$OPTARG"
            ;;

        o)  SetAdditionalOptions "$OPTARG"
            ;;

        t) # release tag
	    RELEASE_TAG="$OPTARG"
            ;;

        *)  # ub screwed #
            Usage
            ;;
        esac
    done    
}

function SetTagsAndFlags
{
    ATTR=$1
    eval ACTIVE=`echo \$\{${ATTR}[0]\}`
    eval FLAGCNT=`echo \$\{${ATTR}[2]\}`
    if [ "$ACTIVE" -eq 1 ];
    then
        # Set Version Tags
        eval VFLAG=`echo \$\{${ATTR}[1]\}`
        VERSIONTAGS="${VERSIONTAGS} ${VFLAG}"

        let j=0
        while [ $j -lt "$FLAGCNT" ]
        do
            # Set Macro Flags
            eval MFLAG=`echo \$\{${ATTR}[$j+3]\}`
            MACROFLAGS="${MACROFLAGS} ${MFLAG}"
            let j=$j+1
        done
    fi
}

function CreateTagsAndFlagsString
{
    for x in $BLD_VARS
    do
        eval ACTIVE=`echo \$\{${x}[0]\}`
        eval FLAGCNT=`echo \$\{${x}[2]\}`
        SetTagsAndFlags $x
    done
    echo

    # remove any leading spaces
    MACROFLAGS=`echo $MACROFLAGS | sed -e 's/^ *//'`
    VERSIONTAGS=`echo $VERSIONTAGS | sed -e 's/^ *//'`

    # add destination directory
    if [ ! -d "${BLD_DEST_DIR}" ]; then
        mkdir -p ${BLD_DEST_DIR} || {
           echo "Error: Cannot make directory ${BLD_DEST_DIR}"
           exit 1
        }
    fi
    TARGETDIR="TARGETDIR=${BLD_DEST_DIR}"
    MACROFLAGS="${MACROFLAGS} $TARGETDIR"

    echo "MACROS  = $MACROFLAGS"
    echo "VERSION = v${RELEASE_TAG} $VERSIONTAGS"
}

function CreateVersion
{
    BLD_DATE=`date '+%Y-%m-%d %T'`
    BLD_MACHINE=`uname -a`
    cat <<-[] > ospversion.c

char *OSPVBuildVersion = "OSP v${RELEASE_TAG} $BLD_DATE $BLD_MACHINE $LOGNAME $VERSIONTAGS";

[]
}

function DoMake
{
    # Check each mode to see if a library needs to be built
    for i in $MODES
    do
        eval ACTIVE=`echo \$\{${i}[0]\}`
        if [ "$ACTIVE" -eq 1 ];
        then
            unset MACROFLAGS VERSIONTAGS
            SetTagsAndFlags $i
            SetDestDir $i
            CreateTagsAndFlagsString
            CreateVersion

            # CLEAN
            [ ! -z "$CLEAN_ONLY" -o ! -f .osp -o "$VERSIONTAGS" != "`cat -s .osp`" ] && {
                echo "\nCleaning...."
                make $TARGETDIR clean

            }
            
            # BUILD (if not clean_only)
            [ -z "$CLEAN_ONLY" ] && {
                echo "\nBuilding...."
                echo "MACROFLAGS=$MACROFLAGS"
                make -f Makefile $MACROFLAGS
                RETURNCODE=$?
                [ $RETURNCODE -ne 0 ] && exit $RETURNCODE

            }

            echo $VERSIONTAGS > .osp
        fi
   done
}

function DoMenu
{
    while :
    do
        # reset any optional flags
        unset ENABLESMIME
        BLD_SMIME=0

        cat <<-XXEOFXX
    

                OSP Build Menu
        -----------------------------
        1. Client Production Library
        2. Client Development Library
        3. Client Non-Secure Library

        q. Quit
        - - - - - - - - - - - - - - -
XXEOFXX
        echo "        Enter Selection => \c"
        read optn
    
        case $optn in

        1) BFLAG=-p
           BMODE=client
           ;; 
        2) BFLAG=-d
           BMODE=client
           ;;
        3) BFLAG=-n
           BMODE=client
           ;;
        q|Quit) BMODE=quit
                ;;
        esac
    
        [ "$BMODE" == "quit" ] && break;

        [ ! -z "$BFLAG" -a ! -z "$BMODE" ] && {
            # execute this script with the correct options
            echo "\nStart: osp_sdk_compile.ksh $BFLAG $BMODE"
            osp_sdk_compile.ksh $BFLAG $BMODE
    
            RC=$?
            [ $RC -ne 0 ] && exit $RC
        }

    done
}


## MAIN ##

if [ -z "$1" ]; then
    DoMenu
else
    SetOptions $*
    DoMake
fi
exit 0

