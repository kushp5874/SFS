//Name: Kushal Patel	CSC360 Assignment 3	ID:V00733023
//disklist

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "info.h"

struct date{ int d, m, y; };
struct time{ int hr, min, sec; };
struct disk{ char type, name[9], ext[4]; int size; struct date d; struct time t; };

void setDate(struct date *dd, unsigned char *data, int off);
void setTime(struct time *tt, unsigned char *data, int off);
void printD(struct disk dsk);
void root(unsigned char *data, int sectSize);

void setDate(struct date *dd, unsigned char *data, int off){
	int date = (data[off+17] << 8) + data[off+16];
	dd->y = 1980 + ((date & 0xFE00) >> 9);
	dd->m = (date & 0x01E0) >> 5;
	dd->d = (date & 0x001F);
}

void setTime(struct time *tt, unsigned char *data, int off){
	int time = (data[off+15] << 8) + data[off+14];
	tt->hr = (time & 0xF800) >> 11;
	tt->min = (time & 0x07E0) >> 5;
	tt->sec = (time & 0x001F);
}

void printD(struct disk dsk){
	char n[20];
	strcpy(n, dsk.name);
	strcat(n, ".");	
	strcat(n, dsk.ext);
	printf("%c", dsk.type);
	printf("%10d ", dsk.size);
	printf("%-20s", n);
	printf("%4d-%02d-%02d", dsk.d.y, dsk.d.m, dsk.d.d);
	printf("%02d:%02d\n", dsk.t.hr, dsk.t.min);
}

void root(unsigned char *data, int sectSize){
	struct disk dsk;
	int i;
	for(i = (19*sectSize); i<(33*sectSize); i+=32){
		if((int)data[i] == 0xE5 || (int)data[i+11] == 0x0F || data[i+11] & 0x08){
			continue;
		}
		if(!data[i]){
			return;
		}
		dsk.type = (data[i+11] & 0x10) ? 'D':'F';
		dsk.size = read_num(data, (i+28), 4);
		read_str(dsk.name, data, i, 8);
		nullify(dsk.name, 8);
		read_str(dsk.ext, data, i+8, 3);
		setDate(&dsk.d, data, i);
		setTime(&dsk.t, data, i);
		printD(dsk);
	}
}

int main(int argc, char **argv){
	int disk, sectSize;
	unsigned char *data;
	struct stat s;

	if(argc != 2){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}
	
	if((disk = open(argv[1], O_RDONLY))){
		fstat(disk, &s);
		data = mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, disk, 0);
		sectSize = read_num(data, 11, 2);
		root(data, sectSize);
	}else{
		printf("FILE NOT FOUND\n");
	}
	return 0;
}
