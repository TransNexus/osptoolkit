#!/usr/bin/sh

##
##	TAR
##
TAR=/usr/bin/tar
TAR_OPTIONS=-cvfX

##
##	ZIP
##
ZIP=/usr/bin/gzip
ZIP_OPTIONS=-f


##
##	Destribution File Name
##
VERSION=$1
DISTRIBUTION_NAME="./TK-3_3_0-20041210/BUILDs/OSPToolkit-""$VERSION"".tar"

##
##	Remove old files
##
rm -f *.tar *.gz

##
##	Build a list of files excluded from destribution
##
LIST_OF_EXCLUDED_FILES="exclude_while_taring"

# Copy openssl.cnf file
#cp $CVSENVROOT/vendor_libs/openssl/apps/openssl.cnf bin/

# CVS dirs
cd ..
find ./TK-3_3_0-20041210 -name ".svn" -type d | cut -b 1-   > $LIST_OF_EXCLUDED_FILES

# This script
echo "TK*/"$0                   		>> $LIST_OF_EXCLUDED_FILES

# The list itself
echo $LIST_OF_EXCLUDED_FILES	>> $LIST_OF_EXCLUDED_FILES
echo "./TK-3_3_0-20041210/BUILDs"									>> $LIST_OF_EXCLUDED_FILES

# Do NOT include documentation folder
echo "./TK-3_3_0-20041210/doc"										>> $LIST_OF_EXCLUDED_FILES



##
##	Tar it
##
$TAR $TAR_OPTIONS $DISTRIBUTION_NAME $LIST_OF_EXCLUDED_FILES ./TK-3_3_0-20041210/*

##
##	Zip it
##
$ZIP $ZIP_OPTIONS $DISTRIBUTION_NAME

##
##	Clean Up
##
rm $LIST_OF_EXCLUDED_FILES
