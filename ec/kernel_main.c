#include "page.h"
#include "rprintf.h"
#include "fat.h"

int main(void) {
    rprintf("Initializing Page Frame Allocator...\n");
    init_pfa_list();

    struct ppage *alloc = allocate_physical_pages(5);
    free_physical_pages(alloc);

    rprintf("Initializing FAT filesystem...\n");
    if (fatInit() == 0) {
        rprintf("FAT initialized successfully!\n");
        struct file *f = fatOpen("TEST.TXT");
        if (f) {
            char buf[512];
            int bytes = fatRead(f, buf, 512);
            rprintf("Read %d bytes from TEST.TXT: %s\n", bytes, buf);
        } else {
            rprintf("Failed to open TEST.TXT\n");
        }
    } else {
        rprintf("FAT initialization failed.\n");
    }

    return 0;
}

