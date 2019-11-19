#!/bin/sh
#
# This script extracts the Xeus-Fift version from src/xfift.hpp (working dir is the project dir), which is the master
# location for this information.
#
if [ ! -f src/xfift.hpp ]; then
    echo "version.sh: error: src/xfift.hpp does not exist" 1>&2
    exit 1
fi
MAJOR=`egrep '^#define +XEUS_FIFT_VERSION_MAJOR +[0-9]+$' src/xfift.hpp`
MINOR=`egrep '^#define +XEUS_FIFT_VERSION_MINOR +[0-9]+$' src/xfift.hpp`
PATCH=`egrep '^#define +XEUS_FIFT_VERSION_PATCH +[0-9]+$' src/xfift.hpp`
if [ -z "$MAJOR" -o -z "$MINOR" -o -z "$PATCH" ]; then
    echo "xfift_version.sh: error: could not extract version from src/xfift.hpp" 1>&2
    exit 1
fi
MAJOR=`echo $MAJOR | awk '{ print $3 }'`
MINOR=`echo $MINOR | awk '{ print $3 }'`
PATCH=`echo $PATCH | awk '{ print $3 }'`
echo $MAJOR.$MINOR.$PATCH | tr -d '\n'
