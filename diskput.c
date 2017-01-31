//Name: Kushal Patel    CSC 360 Assignment 3    ID:V00733023
//diskput

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "info.h"

int n_free(unsigned char *data, disk_info inf, int curr);
int c_time();
int c_date();
int newRoot(unsigned char *data, disk_info inf, char *dName, int size, int clu);
int newFAT(unsigned char *data, int in, int val, int sectSize);
void f_write(unsigned char *data, disk_info inf, char *dName);

int n_free(unsigned char *data, disk_info inf, int curr){
	int ent, in1, in2;
	int i = 2;
	while(i < inf.total_sectors-33+2){
		if(i == curr){
			continue;
		}
		in1 = inf.sector_size + (3*i/2);
		in2 = inf.sector_size + (3*i/2) + 1;
		if(i%2 == 0){
			ent = (data[in1] + ((data[in2] & 0x0F) << 8));
		}else{
			ent = ((data[in1] & 0xF0) >> 4) + (data[in2] <<4);
		}
		if(ent == 0x00){
			return i;
		}
		i++;
	}
	printf("ERROR: Entry not found\n");
	exit(EXIT_FAILURE);
}

int c_time(){
	time_t raw;
	struct tm *info;
	time(&raw);
	info = localtime(&raw);
	int hr = info->tm_hour;
	int min = info->tm_min;
	int sec = info->tm_sec;
	int time = ((hr & 0x1f) << 11) | ((min & 0x3f) << 5) | (sec & 0x1f);
	return time;
}

int c_date(){
	time_t raw;
	struct tm *info;
	time(&raw);
	info = localtime(&raw);
	int y = info->tm_year - 80;
	int m = info->tm_mon + 1;
	int d = info->tm_mday;
	int date = ((y & 0x7f) << 9) | ((m & 0xf) << 5) | (d & 0x1f);
	return date;
}

int newRoot(unsigned char *data, disk_info inf, char *dName, int size, int clu){
	int j, b, ex, t, d;
	char *n = strdup(dName);
	char *base = strtok(n, ".");
	char *ext = strtok(NULL, ".");
	b = (int)strlen(base);
	ex = (int)strlen(ext);

	int i;
	for(i = 19 * (inf.sector_size);i < (33*inf.sector_size); i+=32){
		if((int)data[i] == 0xE5 || (int)data[i] == 0x00){
			for(j = 0; j < 8; j++) data[i+j] = (j<b) ? base[j]:' ';
			for(j = 0; j < 3; j++) data[i+j+8] = (j<ex) ? ext[j]:' ';
			data[i+11] = 0x00;
			write_num(data, size, i+28, 4);
			write_num(data, clu, i+26, 2);
			t = c_time();
			d = c_date();
			write_num(data, t, i+14, 2);
			write_num(data, d, i+16, 2);
			write_num(data, d, i+18, 2);
			write_num(data, t, i+22, 2);
			write_num(data, d, i+24, 2);
			return 0;
		}
	}
	printf("ERROR: NO ROOT DIR\n");
	exit(EXIT_FAILURE);
}

int newFAT(unsigned char *data, int in, int val, int sectSize){
	int in1 = sectSize + (3*in/2);
	int in2 = sectSize + (3*in/2) + 1;
	if(in % 2 == 0){
		data[in1] = val & 0xFF;
		data[in2] = (data[in2] & 0xF0) | ((val & 0xF00) >> 8);
	}else{
		data[in1] = (data[in1] & 0xF) | ((val &0xF) << 4);
		data[in2] = (val & 0xFF0) >> 4;
	}
}

void f_write(unsigned char *data, disk_info inf, char *dName){
	int done, rem, i, next, pEnt, toWri, last, temp;
	unsigned char *in;
	struct stat s;
	int clu = n_free(data, inf, 0);
	if((done = open(dName, O_RDONLY))){
		printf("NOT ENOUGH SPACE\n");
	}else{
		newRoot(data, inf, dName, (int)s.st_size, clu);
		rem = (int)s.st_size;
		for(next = clu, i = 0; rem > 0; rem -= 512, i++){
			pEnt = physical_entry(next);
			last = rem > 512 ? 0:1;
			toWri = last ? 512 : rem;
			memcpy(data+pEnt, in+(i*512), toWri);
			if(last){
				newFAT(data, next, 0xFFF, inf.sector_size);
			}else{
				temp = next;
				next = n_free(data, inf, temp);
				newFAT(data, temp, next, inf.sector_size);
			}
		}
	}
	printf("ERROR: FNF\n");
    	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int disk;
	unsigned char *data;
	struct stat s;
	disk_info inf;

	if(argc != 3){
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}
	
	if((disk = open(argv[1], O_RDWR))){
		fstat(disk, &s);
		data = mmap(NULL, s.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, disk, 0);
		process_disk(data, &inf);
		f_write(data, inf, argv[2]);
	}else{
		printf("FILE NOT FOUND\n");
	}
	return 0;
}

