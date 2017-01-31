//Name: Kushal Patel	CSC360 Assignment 3	ID:V00733023
//diskinfo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "info.h"

void printInf(disk_info inf);

void printInf(disk_info inf){
	printf("OS Name: %s\n", inf.os_name);
    printf("Label of the disk: %s\n", inf.volume_label);
    printf("Total size of the disk: %d\n", inf.total_size);
    printf("Free space: %d\n", inf.free_space);
    printf("==============\n");
    printf("The number of files in the root directory"
        " (not including subdirectories): %d\n", inf.files_in_root);
    printf("==============\n");
   	printf("Number of FAT copies: %d\n", inf.num_fats);
	printf("Sectors per FAT: %d\n", inf.sectors_per_fat);
}

int main(int argc, char **argv){
	int disk;
	unsigned char *data;
	struct stat s;
	disk_info inf;

	if(argc != 2){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}
	
	if((disk = open(argv[1], O_RDONLY))){
		fstat(disk, &s);
		data = mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, disk, 0);
		process_disk(data, &inf);
		printInf(inf);
	}else{
		printf("FILE NOT FOUND\n");
	}
	return 0;
}
