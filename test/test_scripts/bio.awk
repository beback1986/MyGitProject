#!/usr/bin/awk -F,
BEGIN {
	time=0;
	count=0;
	last_time=0;
}
{
	if (last_time == 0) {
		last_time = $1;
	} else {
		new_time = $1;
		if (new_time - last_time > 30000) {
			print "line:"count" begin_time:"last_time" cost_time:" new_time-last_time;
		}
		last_time = new_time;
	}
	time=time + $8;
	count++;
}
END {
	if (count != 0) {
		print "total count:"count" time:"time" avg:"time/count
	} else {
		print "total count:"count
	}
}
