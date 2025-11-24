#include "fat.h"
#include "rprintf.h"
#include "terminal.h"
#include "ide.h"

// CRITICAL: The FAT partition starts at sector 2048 (1MB offset)
#define FAT_PARTITION_OFFSET 2048

// Global variables
char bootSector[512];
char fat_table[8 * SECTOR_SIZE];
char rde_region[4096];  // Space for root directory entries
struct boot_sector *bs;
unsigned int root_sector;
unsigned int data_region_start;

// String comparison function
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// String length function
int strlen_fat(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

// Convert character to uppercase
char toupper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}

// String copy function
void strcpy_fat(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

/*
 * extract_filename
 *
 * Extract a readable filename from a root directory entry
 */
void extract_filename(struct root_directory_entry *rde, char *fname) {
    int k = 0;
    
    // Copy filename (up to 8 characters)
    while (k < 8 && rde->file_name[k] != ' ') {
        fname[k] = rde->file_name[k];
        k++;
    }
    
    // Check if there's a file extension
    if (rde->file_extension[0] != ' ') {
        fname[k++] = '.';
        int n = 0;
        while (n < 3 && rde->file_extension[n] != ' ') {
            fname[k++] = rde->file_extension[n++];
        }
    }
    
    fname[k] = '\0';
}

/*
 * fatInit
 *
 * Initialize the FAT filesystem driver by reading the boot sector and FAT
 */
int fatInit(void) {
    esp_printf(putc, "[HW5] Initializing FAT filesystem...\r\n");
    
    // Read boot sector - ADD PARTITION OFFSET!
    if (ide_readblock(FAT_PARTITION_OFFSET, bootSector, 1) != 0) {
        esp_printf(putc, "[HW5] ERROR: Failed to read boot sector\r\n");
        return -1;
    }
   
    bs = (struct boot_sector*)bootSector;
    
    // Validate boot signature
    if (bs->boot_signature != 0xAA55) {
        esp_printf(putc, "[HW5] ERROR: Invalid boot signature: 0x%x\r\n", bs->boot_signature);
        return -1;
    }
    
    // Validate filesystem type
    if (bs->fs_type[0] != 'F' || bs->fs_type[1] != 'A' || bs->fs_type[2] != 'T') {
        esp_printf(putc, "[HW5] ERROR: Not a FAT filesystem\r\n");
        return -1;
    }
    
    esp_printf(putc, "[HW5] Boot sector info:\r\n");
    esp_printf(putc, "  Bytes per sector: %d\r\n", bs->bytes_per_sector);
    esp_printf(putc, "  Sectors per cluster: %d\r\n", bs->num_sectors_per_cluster);
    esp_printf(putc, "  Reserved sectors: %d\r\n", bs->num_reserved_sectors);
    esp_printf(putc, "  Number of FATs: %d\r\n", bs->num_fat_tables);
    esp_printf(putc, "  Root directory entries: %d\r\n", bs->num_root_dir_entries);
    esp_printf(putc, "  Sectors per FAT: %d\r\n", bs->num_sectors_per_fat);
    
    // Calculate root directory sector (relative to partition start)
    root_sector = bs->num_fat_tables * bs->num_sectors_per_fat + 
                  bs->num_reserved_sectors;
    
    esp_printf(putc, "  Root directory sector: %d\r\n", root_sector);
    
    // Calculate data region start
    unsigned int root_dir_sectors = (bs->num_root_dir_entries * 32 + bs->bytes_per_sector - 1) / bs->bytes_per_sector;
    data_region_start = root_sector + root_dir_sectors;
    
    esp_printf(putc, "  Data region starts at sector: %d\r\n", data_region_start);
    
    // Read FAT table - ADD PARTITION OFFSET!
    int fat_start_sector = FAT_PARTITION_OFFSET + bs->num_reserved_sectors;
    if (ide_readblock(fat_start_sector, fat_table, bs->num_sectors_per_fat) != 0) {
        esp_printf(putc, "[HW5] ERROR: Failed to read FAT table\r\n");
        return -1;
    }
    
    esp_printf(putc, "[HW5] FAT filesystem initialized successfully\r\n\r\n");
    return 0;
}

/*
 * fatOpen
 *
 * Open a file by searching for it in the root directory
 * Returns a file handle, or NULL if not found
 */
struct file *fatOpen(const char *filename) {
    char upper_filename[16];
    char rde_filename[16];
    int k;
    
    // Convert filename to uppercase
    for (k = 0; k < 15 && filename[k]; k++) {
        upper_filename[k] = toupper(filename[k]);
    }
    upper_filename[k] = '\0';
    
    esp_printf(putc, "[HW5] Opening file: %s\r\n", upper_filename);
    
    // Read root directory entries - ADD PARTITION OFFSET!
    unsigned int rde_sectors = (bs->num_root_dir_entries * 32 + bs->bytes_per_sector - 1) / bs->bytes_per_sector;
    if (ide_readblock(FAT_PARTITION_OFFSET + root_sector, rde_region, rde_sectors) != 0) {
        esp_printf(putc, "[HW5] ERROR: Failed to read root directory\r\n");
        return 0;
    }
    
    struct root_directory_entry *rde_table = (struct root_directory_entry*)rde_region;
    
    // Search for the file in root directory
    for (k = 0; k < bs->num_root_dir_entries; k++) {
        // Skip empty entries
        if (rde_table[k].file_name[0] == 0x00 || rde_table[k].file_name[0] == 0xE5) {
            continue;
        }
        
        // Extract filename from RDE
        extract_filename(&rde_table[k], rde_filename);
        
        // Compare with requested filename
        if (strcmp(rde_filename, upper_filename) == 0) {
            esp_printf(putc, "[HW5] Found file: %s\r\n", rde_filename);
            esp_printf(putc, "  Cluster: %d\r\n", rde_table[k].cluster);
            esp_printf(putc, "  Size: %d bytes\r\n", rde_table[k].file_size);
            
            // Allocate file handle (simplified - using static allocation)
            static struct file file_handle;
            file_handle.rde = rde_table[k];
            file_handle.start_cluster = rde_table[k].cluster;
            file_handle.current_position = 0;
            file_handle.next = 0;
            file_handle.prev = 0;
            
            return &file_handle;
        }
    }
    
    esp_printf(putc, "[HW5] ERROR: File not found: %s\r\n", upper_filename);
    return 0;
}

/*
 * fatRead
 *
 * Read data from an open file into a buffer
 */
int fatRead(struct file *file, char *buffer, unsigned int size) {
    if (file == 0) {
        return -1;
    }
    
    unsigned int bytes_to_read = size;
    if (bytes_to_read > file->rde.file_size) {
        bytes_to_read = file->rde.file_size;
    }
    
    esp_printf(putc, "[HW5] Reading %d bytes from file...\r\n", bytes_to_read);
    
    // Calculate the sector for this cluster
    // Cluster 2 is the first data cluster (clusters 0 and 1 are reserved)
    unsigned int first_sector = data_region_start + (file->start_cluster - 2) * bs->num_sectors_per_cluster;
    
    // Calculate how many sectors we need to read
    unsigned int sectors_to_read = (bytes_to_read + bs->bytes_per_sector - 1) / bs->bytes_per_sector;
    
    esp_printf(putc, "[HW5] Reading %d sectors starting at sector %d\r\n", sectors_to_read, first_sector);
    
    // Read the data - ADD PARTITION OFFSET!
    if (ide_readblock(FAT_PARTITION_OFFSET + first_sector, buffer, sectors_to_read) != 0) {
        esp_printf(putc, "[HW5] ERROR: Failed to read file data\r\n");
        return -1;
    }
    
    file->current_position += bytes_to_read;
    
    esp_printf(putc, "[HW5] Successfully read %d bytes\r\n", bytes_to_read);
    return bytes_to_read;
}
