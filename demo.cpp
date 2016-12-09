//
// Created by Pietro Saccardi on 10/12/2016.
//

#include <cstdio>
#include "zorder.h"

int main() {
    unsigned n = 0;
    scanf("%u", &n);
    printf("%u", deinterleave_one(n));
    return 0;
}
