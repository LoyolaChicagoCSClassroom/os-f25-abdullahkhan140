#ifndef PAGE_H
#define PAGE_H

struct ppage {
    void *physical_addr;
    struct ppage *next;
    struct ppage *prev;
};

extern struct ppage physical_page_array[];
extern struct ppage *free_physical_pages;

void init_pfa_list(void);
struct ppage *allocate_physical_pages(unsigned int npages);
void free_physical_pages(struct ppage *ppage_list);

#endif

