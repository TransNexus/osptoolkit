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
DESTRIBUTION_NAME="BUILDs/OSPToolkit-""$VERSION"".tar"

##
##	Remove old files
##
rm -f *.tar *.gz

##
##	Build a list of files excluded from destribution
##
LIST_OF_EXCLUDED_FILES="exclude_while_taring"

# CVS dirs
find . -name CVS -type dir 		 > $LIST_OF_EXCLUDED_FILES

# This script
echo $0                   		>> $LIST_OF_EXCLUDED_FILES

# The list itself
echo $LIST_OF_EXCLUDED_FILES	>> $LIST_OF_EXCLUDED_FILES
echo "BUILDs"									>> $LIST_OF_EXCLUDED_FILES



##
##	Tar it
##
$TAR $TAR_OPTIONS $DESTRIBUTION_NAME $LIST_OF_EXCLUDED_FILES *

##
##	Zip it
##
$ZIP $ZIP_OPTIONS $DESTRIBUTION_NAME

##
##	Clean Up
##
rm $LIST_OF_EXCLUDED_FILES
