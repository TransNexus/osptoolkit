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
DESTRIBUTION_NAME="osptoolkit/BUILDs/OSPToolkit-""$VERSION"".tar"

##
##	Remove old files
##
rm -f *.tar *.gz

##
##	Build a list of files excluded from destribution
##
LIST_OF_EXCLUDED_FILES="exclude_while_taring"

# Copy openssl.cnf file
cp $CVSENVROOT/vendor_libs/openssl/apps/openssl.cnf bin/

# CVS dirs
cd ..
find osptoolkit -name CVS -type dir | cut -b 1-   > $LIST_OF_EXCLUDED_FILES

# This script
echo "osptoolkit/"$0                   		>> $LIST_OF_EXCLUDED_FILES

# The list itself
echo $LIST_OF_EXCLUDED_FILES	>> $LIST_OF_EXCLUDED_FILES
echo "osptoolkit/BUILDs"									>> $LIST_OF_EXCLUDED_FILES

# Do NOT include documentation folder
echo "osptoolkit/doc"										>> $LIST_OF_EXCLUDED_FILES



##
##	Tar it
##
$TAR $TAR_OPTIONS $DESTRIBUTION_NAME $LIST_OF_EXCLUDED_FILES osptoolkit/*

##
##	Zip it
##
$ZIP $ZIP_OPTIONS $DESTRIBUTION_NAME

##
##	Clean Up
##
rm $LIST_OF_EXCLUDED_FILES
