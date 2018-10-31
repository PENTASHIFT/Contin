#!/bin/bash

CONTIN_MAIN="/usr/bin"

case "$1" in
	start)
		sudo -u root bash -c "cd $CONTIN_MAIN && ./contin"
		;;
	stop)
		:
		;;
esac
exit 0
