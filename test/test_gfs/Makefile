

all:
	gcc lic_rpc_ops.c lic.c lic_test.c -g -O0 -Wall -I. -I/usr/include/glusterfs/ -lglusterfs -lgfrpc -lgfxdr -DGF_LINUX_HOST_OS -DDATADIR=\"/var/\"
