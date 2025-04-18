#include "tests.h"
#include "../src/headers/flags.h"
#include "greatest.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    Flags f = 0;
    f = set_zf(f);
    
    assert(f != 0);
    
    printf("all tests passed!\n");
    
    return 0;
}