#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

struct opts {
	int 	 get_size_way; /* Get size by(0:stat, 1:lseek. Default is stat) */
	off_t 	 gap_size;   /* Gap between write blocks. */
	size_t 	 data_size;
	int 	 iter;
	char 	*file_path;
	char 	*data;
	off_t 	 trunc_size;
	int 	 trunc_iter;
};
static struct opts opt = {
	.get_size_way 	= 0,
	.gap_size	= 0,
	.data_size	= 5000,
	.iter		= 100,
	.file_path	= NULL,
	.data		= NULL,
	.trunc_size 	= 49900,
	.trunc_iter	= 10,
};

char *prepare_data(size_t data_size)
{
	char *buf;
	int i, index;

	buf = calloc(1, data_size);
	if (buf) {
		for (i=0; i<data_size; i++) {
			index = i%53;
			if (index < 26)
				buf[i] = index+97;
			else if (index < 52)
				buf[i] = index+65-26;
			else
				buf[i] = '\n';
		}
	}

	return buf;
}

off_t file_size(int fd)
{
	struct stat stbuff;

	if (fstat(fd, &stbuff) < 0)
		return -1;

	return stbuff.st_size;
}

int __file_check_gap(int fd, off_t start, off_t len)
{
	char *buf = NULL;
	int i;

	buf = (char *)calloc(1, len);
	if (!buf) {
		printf("Can not alloc mem while check file.\n");
		goto failed;
	}

	if (lseek(fd, start, SEEK_SET) < 0) {
		printf("lseek failed while check file.\n");
		goto failed;
	}

	if (read(fd, buf, len) != len) {
		printf("read size does't match or operation failed.\n");
		goto failed;
	}

	for (i=0; i<len; i++) {
		if (buf[i] != 0) {
			printf("Gap in file not zero!\n");
			goto failed;
		}
	}

	free(buf);
	return 0;
failed:
	if (buf)
		free(buf);
	return 1;
}

int __file_check_data(int fd, off_t start, off_t len, char *data)
{
	char *buf = NULL;
	int i;

	buf = (char *)calloc(1, len);
	if (!buf) {
		printf("can not alloc mem while check file.\n");
		goto failed;
	}

	if (lseek(fd, start, SEEK_SET) < 0) {
		printf("lseek failed while check file data.\n");
		goto failed;
	}

	if (read(fd, buf, len) != len) {
		printf("read size does't match or operation failed\n");
		goto failed;
	}

	for (i=0; i<len; i++) {
		if (buf[i] != data[i]) {
			printf("data does't match\n");
			goto failed;
		}
	}

	free(buf);
	return 0;
failed:
	if (buf)
		free(buf);
	return 1;
}

int file_check(int fd, off_t pos1, off_t pos2, off_t pos3, char *data)
{
	int ret = 1;

	if (pos2 > pos1)
		if (__file_check_gap(fd, pos1, pos2-pos1))
			goto out;

	ret = __file_check_data(fd, pos2, pos3-pos2, data);

out:
	return ret;
}

int grow_file(int fd, int iter)
{
	off_t fsize;
	ssize_t write_len;
	off_t pos1, pos2, pos3;
	off_t trunk_pos;

	fsize = file_size(fd);
	pos1 = pos2 = lseek(fd, 0, SEEK_END);
	if (pos1 < 0 || fsize != pos1) {
		printf("file end & file size mismatch or operation failed\n");
		goto failed;
	}

	if (opt.gap_size) {
		pos2 = lseek(fd, fsize + opt.gap_size, SEEK_SET);
		if (pos2 < 0 || (pos2 - pos1) != opt.gap_size) {
			printf("lseek do not reach require size or operation failed\n");
			goto failed;
		}
	}

	write_len = write(fd, opt.data, opt.data_size);
	if (write_len < 0 || write_len != opt.data_size) {
		printf("write len does't not match or operation failed\n");
		goto failed;
	}

	pos3 = lseek(fd, 0, SEEK_CUR);
	fsize = file_size(fd);
	if (pos3 < 0 || fsize != pos3 || (pos3 - pos2) != opt.data_size) {
		printf("write data len mismatch or operation failed\n");
		goto failed;
	}

	if (file_check(fd, pos1, pos2, pos3, opt.data))
		goto failed;

	if (((iter+1)%opt.trunc_iter) == 0) {
		trunk_pos = lseek(fd, 0, SEEK_CUR);
		if (trunk_pos < 0 || trunk_pos != pos3) {
			printf("trunk_pos does't match\n");
			goto failed;
		}
		printf("truncate file to  %jd trunk_pos=%jd\n", trunk_pos-opt.trunc_size, trunk_pos);
		printf("refrence position %lu, iter=%d, pos3=%jd, fsize=%jd, write_len=%jd\n", 
				((iter+1)/opt.trunc_iter)*(opt.data_size*opt.trunc_iter-opt.trunc_size),
				iter+1,
				pos3,
				fsize, 
				write_len);
		if (((iter+1)/opt.trunc_iter)*(opt.data_size*opt.trunc_iter-opt.trunc_size) 
				!= trunk_pos-opt.trunc_size) {
			printf("wrong truncate position\n");
			goto failed;
		}
		if (ftruncate(fd, trunk_pos - opt.trunc_size) < 0) {
			printf("can not truncate file\n");
			goto failed;
		}
	}

	return 0;
failed:
	return 1;
}

int get_opt(int argc, char *argv[])
{
	int i;

	for (i=1; i< argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			i++;
			if (i == argc)
				goto failed;
			opt.get_size_way = atoi(argv[i]);
		} else if (strcmp(argv[i], "-g") == 0) {
			i++;
			if (i == argc)
				goto failed;
			opt.gap_size = atoi(argv[i]);
		} else if (strcmp(argv[i], "-d") == 0) {
			i++;
			if (i == argc)
				goto failed;
			opt.data_size = atoi(argv[i]);
		} else if (strcmp(argv[i], "-i") == 0) {
			i++;
			if (i == argc)
				goto failed;
			opt.iter = atoi(argv[i]);
		} else {
			opt.file_path = strdup(argv[i]);
		}
	}

	return 0;
failed:
	printf("options parse failed!\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int fd, i;

	if (get_opt(argc, argv) || !opt.file_path)
		goto failed;

	opt.data = prepare_data(opt.data_size);
	if (!opt.data) {
		printf("memory alloc failed!\n");
		goto failed;
	}

	for (i=0; i<opt.iter; i++) {
		fd = open(opt.file_path, O_RDWR|O_CREAT, 0777);
		if (fd < 0) {
			printf("can not open file\n");
			goto failed;
		}
		if (grow_file(fd, i))
			goto failed;
		close(fd);
	}

	return 0;
failed:
	printf("Got error(%d):%s\n", errno, strerror(errno));
	return 1;
}
