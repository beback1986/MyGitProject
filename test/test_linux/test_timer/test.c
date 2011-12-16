/*
 * Test for timer related system interfaces(NOT glibc interfaces).
 * TODO: finish this.
 */

int main(int argc, char *argv[])
{
	setitimer();
	alarm();
	nanosleep();
	timer_create();
}
