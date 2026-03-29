#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "functions.h"

#define X(name) name##_func name = NULL;
FUNCTION_LIST
#undef X

// -------------------- FILE UTILS --------------------

time_t get_mod_time(const char *path);

void copy_file(const char *src, const char *dst);

// -------------------- LOADER FUNCTION --------------------

int load_functions(void *handle);

// -------------------- MAIN --------------------

int main() {
    void *handle = NULL;
    time_t last_mod = 0;

    while (1) {
        time_t mod = get_mod_time("build/plugin.so");

        if (mod != last_mod) {

            if (handle) {
                dlclose(handle);
                handle = NULL;
            }

            copy_file("build/plugin.so", "build/plugin_temp.so");

            handle = dlopen("./build/plugin_temp.so", RTLD_NOW);
            if (!handle) {
                printf("dlopen error: %s\n", dlerror());
                continue;
            }

            if (!load_functions(handle)) {
                dlclose(handle);
                handle = NULL;
                continue;
            }

            if (init) {
                init();
            }

            last_mod = mod;
        }

        if (update) {
            update();
        }

        sleep(1);
    }

    return 0;
}

time_t get_mod_time(const char *path) {
    struct stat attr;
    if (stat(path, &attr) == 0) {
        return attr.st_mtime;
    }
    return 0;
}

void copy_file(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0755);

    char buffer[4096];
    ssize_t n;

    while ((n = read(in, buffer, sizeof(buffer))) > 0) {
        write(out, buffer, n);
    }

    close(in);
    close(out);
}

int load_functions(void *handle) {
    int failed = 0;

    #define X(name) \
        name = (name##_func)dlsym(handle, #name); \
        if (!name) { \
            printf("Missing: %s\n", #name); \
            failed = 1; \
        }

    FUNCTION_LIST
    #undef X

    return !failed;
}