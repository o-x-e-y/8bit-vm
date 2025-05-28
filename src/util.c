#include "headers/util.h"

#include <assert.h>
#include <stdio.h>

string_t read_file_to_str(const char* path) {
    assert(path != NULL);

    FILE* fp = fopen(path, "r");

    if (fp) {
        string_t res = new_str(100);

        char c;
        while ((c = fgetc(fp)) != -1) {
            push_str(&res, c);
        }

        fclose(fp);

        return res;
    }

    return new_str(0);
}

void write_file(const char* path, slice_t content) {
    assert(path != NULL);
    assert(content.str != NULL);

    FILE* fp = fopen(path, "w");

    fprintf(fp, "%.*s", (int)content.len, content.str);

    fclose(fp);
}
