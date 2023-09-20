#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "../../src/debug.h"
#include "../../src/vector.h"

int main() {
    #ifdef DEBUG
        printf("Hello from example_test: debug\n");
    #else
        printf("Hello from example_test: not debug\n");
    #endif

    // Test the vector
    vector_t vec = init_vector();
    char *cool = malloc(6);
    strcpy(cool, "hello");
    vec.callbacks.push(&vec, (void *)cool);
    printf("Vec first element: %s\n", (char *)vec.callbacks.get(&vec, 0));
    free_vector(vec);
    
    return 0;
}
