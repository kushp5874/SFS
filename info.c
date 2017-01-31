//Name: Kushal Patel    CSC 360 Assignment 3    ID:V00733023
//info.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

int read_num(unsigned char *data, size_t off, size_t size){
	int i, ret;
	for(i = 0, ret = 0; i < size; i++){
		ret += data[off+i] << (8*i);
	}
	return ret;
}

void write_num(unsigned char *data, int num, size_t off, size_t size){
	int i = 0;
	unsigned int mask = 0xFF;
	while(i<size){
		data[off+i] = (num & (mask << 8*i)) >> (i*8);
		i++;
	}
}

int physical_entry(int x){
	return (x + 31) * 512;
}

void read_str(char *buff, unsigned char *data, size_t off, size_t len){
	strncpy(buff, ((char*)data + off), len);
	buff[len] = '\0';
}

int total_sectors(unsigned char *data){
	int tot = read_num(data, 19, 2);
	if(tot == 0){
		tot = read_num(data, 32, 4);
	}
	return tot;
}

int get_label(unsigned char *data, char *buff, int sectSize){
	int i;
	for(i = 19*sectSize; i < (33*sectSize); i+=32){
		if((int)data[i] == 0xE5 || (int)data[i+11] == 0x0F) continue;
		if(!data[i]){
			printf("ERROR");
			exit(EXIT_FAILURE);
		}
		if(data[i+11] & 0x08){
			read_str(buff, data, i, 8);
			return 0;
		}
	}
	return 1;
}

int files_in_root(unsigned char *data, int sectSize){
	int i;
	int count = 0;
	for(i = 19*sectSize; i < (33*sectSize); i+=32){
   		if ((int)data[i] == 0xE5 || (int)data[i+11] == 0x0F || data[i+11] & 0x08 || data[i+11] & 0x10){
		 	continue;
		}
    	if (!data[i]){
	 		return count;
		}
    	++count;
  	}
  return count;
}

int free_space(unsigned char *data, int tot, int sectSize){
	int sp = 0;
	int i, ent, in1, in2;
	for(i = 2; i < tot-33+2; i++){
		in1 = sectSize + (3*i/2);
		in2 = sectSize + 1 + (3*i/2);
		if(i % 2 == 0){
			ent = (data[in1]) + ((data[in2] & 0x0F) << 8);
		}else{
			ent = ((data[in1] & 0xF0) >> 4) + (data[in2] << 4);
		}
		if(ent == 0x00){
			sp = sp + sectSize;
		}
	}
	return sp;
}

void process_disk(unsigned char *data, disk_info *inf){
	int sectSize = read_num(data, 11, 2);
	read_str(inf->os_name, data, 3, 8);
	get_label(data, inf->volume_label, sectSize);
	inf->sector_size = sectSize;
	inf->total_sectors = total_sectors(data);
	inf->total_size = inf->total_sectors * sectSize;
	inf->files_in_root = files_in_root(data, sectSize);
	inf->free_space = free_space(data, inf->total_sectors, sectSize);
	inf->num_fats = read_num(data, 16, 1);
	inf->sectors_per_fat = read_num(data, 22, 2);
}

void nullify(char *buff, size_t len){
	int i;
	for(i=0; i<len; i++){
		if(buff[i] == ' '){
			buff[i] = '\0';
		}				
	}
}

