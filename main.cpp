#include <cstdio>
#include "zorder.h"

int main() {
    unsigned n = 0;
//    std::cin >> n;
//    auto coords = deinterleave_all<unsigned, 2>(n);
//    std::cout << coords[0] << "\t" << coords[1] << std::endl;
    scanf("%u", &n);
    printf("%u", deinterleave_one(n));
    return 0;
}
