#include "greatest.h"
#include "tests.h"

GREATEST_MAIN_DEFS();
int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(INSTRUCTION_PARITY_SUITE);
    RUN_SUITE(INSTRUCTION_FUNCTIONALITY_SUITE);

    GREATEST_MAIN_END();
}