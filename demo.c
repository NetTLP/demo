#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

/* From http://git.dpdk.org/dpdk/tree/lib/librte_eal/linux/eal/eal_memory.c#n96 */
static uintptr_t phy_addr(void *virt) {
	uintptr_t entry = 0;
	long pagesize;
	ssize_t rc;
	off_t ret;
	int fd;

	fd = open("/proc/self/pagemap", O_RDONLY);
	if (fd < 0)
		err(1, "open /proc/self/pagemap: %s\n", strerror(errno));

	pagesize = sysconf(_SC_PAGESIZE);

	ret = lseek(fd, (uintptr_t)virt / pagesize * sizeof(uintptr_t),
		    SEEK_SET);
	if (ret < 0)
		err(1, "lseek for /proc/self/pagemap: %s\n", strerror(errno));


	rc = read(fd, &entry, sizeof(entry));
	if (rc < 1 || entry == 0)
		err(1, "read for /proc/self/pagemap: %s\n", strerror(errno));

	close(fd);

	return (entry & 0x7fffffffffffffULL) * pagesize +
		   ((uintptr_t)virt) % pagesize;
}

int main(int argc, char **argv)
{
	char str[512] = "test_string";
	uintptr_t pa;

	pa = phy_addr(str);

	while (1) {
		printf("PHY ADDR %#lx %s\n", pa, str);
		sleep(1);
	}

	return 0;
}

