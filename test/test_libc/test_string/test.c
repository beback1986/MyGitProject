#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	char *source = "/sys/devices/pci0000:00/0000:00:07.1/host0/target0:0:0/0:0:0:0/block/sda";
	char *block = NULL;
	char *first = NULL;
	char *last = NULL;

	block = strstr(source, "block");
	if (!block)
		goto out;
	printf("block:%s\n", block);

	first = index(block, '/');
	if (!first)
		goto out;
	printf("first '/:'%s\n", first);

	last = rindex(block, '/');
	if (!last)
		goto out;
	printf("last '/':%s\n", last);

	if (first != last)
		goto out;
	printf("find device:%s\n", last+1);

out:
	return 0;
}
