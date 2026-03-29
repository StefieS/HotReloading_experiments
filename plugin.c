#include <stdio.h>

#define VERSION "v1"

void init(void) {
    printf("Plugin " VERSION " init\n");
}

void update(void) {
    printf("Update " VERSION "\n");
}