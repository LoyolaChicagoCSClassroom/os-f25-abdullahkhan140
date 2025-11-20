#ifndef IDE_H
#define IDE_H

#define SECTOR_SIZE 512

// ATA disk reading function (implemented in ide.s)
// Parameters: (unsigned int lba, unsigned char *buffer, unsigned int numsectors)
extern int ata_lba_read(unsigned int lba, unsigned char *buffer, unsigned int numsectors);

// Alias for compatibility
#define ide_readblock ata_lba_read

#endif
