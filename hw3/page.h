#ifndef PAGE_H
#define PAGE_H

struct ppage {
    struct ppage *next;
    struct ppage *prev;
    void *physical_addr;
};

void init_pfa_list(void);
void free_page_list(struct ppage *ppage_list);

struct ppage *allocate_physical_pages(unsigned int npages);
extern struct ppage *free_physical_pages;

#endif
