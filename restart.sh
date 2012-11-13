#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/libmemcached/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

bin='Logic_Server'

rm core.*

bin_pid=`ps -u winkt -f | grep ${bin} | awk '/17093/{print $2}'`

if test -n "${bin_pid}"; then
	kill -9 ${bin_pid}
fi

datetag=`date '+%m_%d_%k_%M'`

if test -f ./*.trace
	then
		rm -fr *.trace
fi

if test -f ./core.*
	then
		tar -czf "CORE.${datetag}.tar.gz" core.*
		rm -fr core.*
fi

#rm -r *.log
rm -r Log/Log.*

nohup ./${bin} -p 17093 -l 7 -c config.xml -n LogicServer &
#./${bin} -p 8090 -l 7 &


