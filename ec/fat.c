#include "fat.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

char sector_buf[512];
char rde_region[16384];
int fd = 0;

int read_sector_from_disk_image(unsigned int sector_num, char *buf, unsigned int nsectors) {
    lseek(fd, sector_num * SECTOR_SIZE, SEEK_SET);
    return read(fd, buf, SECTOR_SIZE * nsectors);
}

void extract_filename(struct root_directory_entry *rde, char *fname){
    int k = 0;
    while(rde->file_name[k] != ' ' && k < 8) {
        fname[k] = rde->file_name[k];
        k++;
    }
    fname[k] = '\0';
    if(rde->file_extension[0] == ' ') return;
    fname[k++] = '.';
    int n = 0;
    while(rde->file_extension[n] != ' ' && n < 3) {
        fname[k] = rde->file_extension[n];
        k++; n++;
    }
    fname[k] = '\0';
}

int fatInit(void) {
    fd = open("disk.img", O_RDONLY);
    if (fd < 0) return -1;
    if(read_sector_from_disk_image(0, sector_buf, 1) != SECTOR_SIZE) return -1;
    return 0;
}

struct file* fatOpen(const char* filename) {
    struct root_directory_entry *rde_tbl = (struct root_directory_entry*)rde_region;
    read_sector_from_disk_image(19, rde_region, 32); // example sector
    static struct file f;
    for(int k=0; k<512; k++) {
        char temp[16];
        extract_filename(&rde_tbl[k], temp);
        if(strcmp(temp, filename) == 0) {
            f.rde = rde_tbl[k];
            f.start_cluster = rde_tbl[k].cluster;
            return &f;
        }
    }
    return NULL;
}

int fatRead(struct file* f, char* buf, int size) {
    if (!f) return 0;
    int bytes = pread(fd, buf, size, (f->start_cluster-2)*CLUSTER_SIZE);
    return bytes;
}

