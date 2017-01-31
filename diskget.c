//Name: Kushal Patel    CSC 360 Assignment 3    ID:V00733023
//diskget

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "info.h"

typedef struct diskInf{ int size, clu; } dInf;

int entry(unsigned char *data, int clu, size_t sectSize);
void find(unsigned char *data, dInf *d, size_t sectSize, char *disk);
void copy(unsigned char *data, dInf d, size_t sectSize, char *out);

int entry(unsigned char *data, int clu, size_t sectSize){
	int in1, in2, entry;
    
    in1 = sectSize + (3*clu/2);
   	in2 = sectSize + 1 + (3*clu/2);
    if(clu % 2 == 0){
        	entry = (data[in1]) + ((data[in2] & 0x0F) << 8);
    }else{
        	entry = ((data[in1] & 0x0F) >> 4) + (data[in2] << 4);
    }
    return entry;
}

void find(unsigned char *data, dInf *d, size_t sectSize, char *disk){
    	char *inDname, *inDext;
    	char dName[9], ext[4];
    	inDname = strtok(disk, ".");
    	inDext = strtok(NULL, ".");
    	int i;
    	for(i = 19 * sectSize; i < (33*sectSize); i +=32){
        	read_str(dName, data, i, 8);
        	nullify(dName, 8);
        	read_str(ext, data, i+8, 3);
        	if(!strcmp(inDname, dName) && !strcmp(inDext, ext)){
            		d->size = read_num(data, i+28, 4);
            		d->clu = read_num(data, i+26, 2);
            		return;
        	}
    	}
    	printf("ERROR: FNF\n");
    	exit(EXIT_FAILURE);
}

void copy(unsigned char *data, dInf d, size_t sectSize, char *out){
	int ent, pEnt, toRead;
	int rem = d.size;
	FILE *fout = fopen(out, "w");
	for(ent = d.clu; ent < 0xFF8; ent = entry(data, ent, sectSize), rem -= 512){
		pEnt = physical_entry(ent);
		toRead = rem >= 512 ? 512:rem;
		fwrite(data+pEnt, toRead, 1, fout);
	}
	fclose(fout);
}

int main(int argc, char **argv){
	int disk;
	unsigned char *data;
	char *n = strdup(argv[2]);
	struct stat s;
	dInf d;
	disk_info inf;

	if(argc != 3){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}
	
	if((disk = open(argv[1], O_RDONLY))){
		fstat(disk, &s);
		data = mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, disk, 0);
		process_disk(data, &inf);
		find(data, &d, inf.sector_size, n);
		copy(data, d, inf.sector_size, argv[2]);
	}else{
		printf("FILE NOT FOUND\n");
	}
	return 0;
}
		
