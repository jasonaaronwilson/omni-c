///
/// The simplest test possible to make sure the C compiler doesn't
/// die parasing the library, etc.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

int main(int argc, char** argv) { exit(0); }
