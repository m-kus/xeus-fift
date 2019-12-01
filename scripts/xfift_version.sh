#!/bin/sh
#
# This script extracts the Xeus-Fift version from ../src/xfift.hpp (working dir is build/ or scriots/), which is the master
# location for this information.
#
if [ ! -f ../src/version.h ]; then
    echo "version.sh: error: ../src/version.h does not exist" 1>&2
    exit 1
fi
MAJOR=`egrep '^#define +XEUS_FIFT_VERSION_MAJOR +[0-9]+$' ../src/version.h`
MINOR=`egrep '^#define +XEUS_FIFT_VERSION_MINOR +[0-9]+$' ../src/version.h`
PATCH=`egrep '^#define +XEUS_FIFT_VERSION_PATCH +[0-9]+$' ../src/version.h`
if [ -z "$MAJOR" -o -z "$MINOR" -o -z "$PATCH" ]; then
    echo "xfift_version.sh: error: could not extract version from ../src/version.h" 1>&2
    exit 1
fi
MAJOR=`echo $MAJOR | awk '{ print $3 }'`
MINOR=`echo $MINOR | awk '{ print $3 }'`
PATCH=`echo $PATCH | awk '{ print $3 }'`
echo $MAJOR.$MINOR.$PATCH | tr -d '\n'
