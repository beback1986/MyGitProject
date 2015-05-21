#!/usr/bin/awk -F,
BEGIN {
	line_num=0;
	last_pid=000
	last_time=000
	total_switch_count=0;
	total_switch_time=0;
}
{
	pid=$1;
	time=$2;
	line_num++;
	if (pid!=last_pid) {
		last_pid=pid;
#		print "line:"line_num" pid:"last_pid" time:"(time-last_time);
		total_switch_time+=time-last_time;
		total_switch_count++;
	}
	last_time=time;
}
END {
	print "total switch count:"total_switch_count" time:"total_switch_time
}
