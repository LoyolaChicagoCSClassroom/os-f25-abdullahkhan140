#ifndef __SD_H__
#define __SD_H__

#define SECTOR_SIZE 512

// Read sectors from disk using ATA PIO mode
// lba: Logical Block Address (sector number)
// buffer: Buffer to store data
// numsectors: Number of sectors to read
int ata_lba_read(unsigned int lba, unsigned char *buffer, unsigned int numsectors);

// Wrapper function for easier use
static inline int sd_readblock(unsigned int lba, char *buffer, unsigned int numsectors) {
    return ata_lba_read(lba, (unsigned char*)buffer, numsectors);
}

#endif
