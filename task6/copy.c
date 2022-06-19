#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void print_help()
{
	printf("copy [-m] <file_name> <new_file_name>\ncopy [-h]\n");
}

void copy_read_write(int fd_from, int fd_to);
void copy_mmap(int fd_from, int fd_to);

const int BUFFER_SIZE = 100;

int main(int argc, char *argv[])
{
	int opt;
	int m_option = 0;

	if (argc == 1)
	{
		print_help();
		return 0;
	}

	while ((opt = getopt(argc, argv, "mh")) != -1)
	{
		switch (opt)
		{
		case 'm':
			m_option = 1;
			break;
		case 'h':
			print_help();
			return 0;
		case '?':
			printf("Unknown parameter");
			return 1;
		}
	}

	if (optind != argc - 2)
	{
		printf("You need to provide exactly two filenames\n");
		return 1;
	}
	char *file_name = argv[optind];
	char *new_file_name = argv[optind + 1];

	if (strcmp(file_name, new_file_name) == 0)
	{
		printf("Output file should have a different name\n");
		return 1;
	}

	int fd_from;
	fd_from = open(file_name, O_RDONLY);
	if (fd_from <= 0)
	{
		printf("File \"%s\" doesn't exist\n", file_name);
		return 1;
	}

	int fd_to;
	fd_to = open(new_file_name, 0);
	if (fd_to > 0)
	{
		printf("File \"%s\" already exists\n", new_file_name);
		close(fd_from);
		close(fd_to);
		return 1;
	}
	fd_to = open(new_file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd_to <= 0)
	{
		printf("Cannot create output file\n");
		close(fd_from);
		return 1;
	}

	if (m_option == 1)
	{
		printf("Using mmap...\n");
		copy_mmap(fd_from, fd_to);
	}
	else
	{
		printf("Using read, write...\n");
		copy_read_write(fd_from, fd_to);
	}

	close(fd_to);
	close(fd_from);
	return 0;
}

void copy_read_write(int fd_from, int fd_to)
{
	void *buf = malloc(BUFFER_SIZE);
	if (buf == NULL)
	{
		printf("Cannot allocate memory for a buffer\n");
		return;
	}

	ssize_t bytes_read;
	while ((bytes_read = read(fd_from, buf, BUFFER_SIZE)) != 0)
	{
		if (write(fd_to, buf, bytes_read) == -1)
		{
			printf("Unable to write to a file\n");
			free(buf);
			return;
		}
	}
	free(buf);
}

void copy_mmap(int fd_from, int fd_to)
{
	struct stat s;
	fstat(fd_from, &s);
	long filesize = s.st_size;

	void *addr_from = NULL;
	addr_from = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd_from, 0);
	if (addr_from == MAP_FAILED)
	{
		printf("Failed to map a from file\n");
		return;
	}

	if (ftruncate(fd_to, filesize) != 0)
	{
		printf("Cannot truncate the output file\n");
		return;
	}

	void *addr_to = NULL;
	addr_to = mmap(NULL, filesize, PROT_WRITE | PROT_READ, MAP_SHARED, fd_to, 0);
	if (addr_to == MAP_FAILED)
	{
		printf("Failed to map a to file: %d\n", errno);
		return;
	}

	memcpy(addr_to, addr_from, filesize);
}
