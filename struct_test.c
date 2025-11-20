#include <stdio.h>
#include <stdlib.h>

struct s {
    struct s *next;
    int k;
};

int main() {
    // 1. Instantiate a new object of type struct s and print out its size
    struct s structure;
    printf("Size of struct s: %lu bytes\n", sizeof(structure));

    // 2. Instantiate a pointer to struct s
    struct s *s_ptr;

    // 3. Call malloc to allocate some memory for struct *s_ptr
    s_ptr = malloc(sizeof(struct s));
    if (s_ptr == NULL) {
        printf("malloc failed!\n");
        return 1;
    }

    // 4. Print the address of the memory region that malloc returns
    printf("Address returned by malloc for s_ptr: %p\n", (void *)s_ptr);

    // 5. Set the values of the elements in the struct to 0
    s_ptr->next = NULL;
    s_ptr->k = 0;

    // 6. malloc() a second pointer to struct s, and point its next to the first
    struct s *s_ptr2 = malloc(sizeof(struct s));
    if (s_ptr2 == NULL) {
        printf("malloc failed!\n");
        return 1;
    }

    s_ptr2->next = s_ptr;  // link second to first
    s_ptr2->k = 0;

    printf("Address of second struct: %p\n", (void *)s_ptr2);
    printf("Second->next points to: %p\n", (void *)s_ptr2->next);

    // free allocations (good practice)
    free(s_ptr2);
    free(s_ptr);

    return 0;
}

