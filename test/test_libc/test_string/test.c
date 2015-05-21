#include <string.h>
#include <stdint.h>
#include <stdio.h>


static const char *bwlc_scan_keyword[][3] = { 
        {"sd", "ssd", "hd"}, /* For BWLC_SCAN_NORMAL. */
        {"md", "dm"}, /* For BWLC_SCAN_MULTIPATH. */
};

void test_string_array(void)
{
	int i, j;

	for (i=0; i<2; i++) {
		for (j=0; j<3; j++) {
			if (bwlc_scan_keyword[i][j] != NULL)
				printf("%s\t", bwlc_scan_keyword[i][j]);
		}
		printf("\n");
	}
	printf("%llu\n", (uint64_t)-1);

	return ;
}

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

	test_string_array();

out:
	return 0;
}
