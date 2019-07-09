#!/bin/sh
# Add flock to avoid critical section.

ME=`basename "$0"`
LOCK="/tmp/${ME}.LOCK"
exec 8>$LOCK

flock -x 8
if [ "$1" = "on" ]; then
	echo "1" > /tmp/av.status
elif [ "$1" = "off" ] ; then
	echo "0" > /tmp/av.status
else
echo "-1" > /tmp/av.status
fi
