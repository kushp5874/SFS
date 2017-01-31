//Name: Kushal Patel    CSC 360 Assignment 3    ID:V00733023
//info.h

#ifndef INFO_H_
#define INFO_H_

typedef struct disk_info {
  char os_name[9];
  char volume_label[9];
  int sector_size;
  int total_sectors;
  int total_size;
  int free_space;
  int files_in_root;
  int num_fats;
  int sectors_per_fat;
} disk_info;

int read_num(unsigned char*, size_t, size_t);
void write_num(unsigned char*, int, size_t, size_t);
void read_str(char*, unsigned char*, size_t, size_t);
int physical_entry(int);
void process_disk(unsigned char*, disk_info*);
void nullify(char*, size_t);

#endif