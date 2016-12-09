#include <iostream>
#include <random>
#include <chrono>
#include "zorder.h"

int main() {
    static constexpr unsigned ub = 10000000u;
    std::random_device r;
    std::mt19937 gen(r());
    std::uniform_int_distribution<int> dist;
    long long ns_interleaving = 0;
    long long ns_dividing = 0;
    for (unsigned j = 0; j < 20u; ++j) {
        {
            const auto time_begin = std::chrono::high_resolution_clock::now();
            for (unsigned i = 0; i < ub; ++i) {
                (void)deinterleave_all(static_cast<unsigned>(dist(gen)));
            }
            const auto time_end = std::chrono::high_resolution_clock::now();
            const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_begin).count();
            std::cout << ns << "ns for " << ub << " attempts at deinterleaving." << std::endl;
            ns_interleaving += ns;
        }
        {
            const auto time_begin = std::chrono::high_resolution_clock::now();
            const int divisor = dist(gen);
            for (unsigned i = 0; i < ub; ++i) {
                (void)std::div(dist(gen), divisor);
            }
            const auto time_end = std::chrono::high_resolution_clock::now();
            const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_begin).count();
            std::cout << ns << "ns for " << ub << " attempts at dividing." << std::endl;
            ns_dividing += ns;
        }
        if (j == 0) {
            std::cout << "Discarding initial loop." << std::endl;
            ns_dividing = ns_interleaving = 0;
        }
    }
    if (ns_interleaving < ns_dividing) {
        std::cout << "Interleaving";
    } else if (ns_interleaving > ns_dividing) {
        std::cout << "Dividing";
    }
    const double ratio = double(std::max(ns_interleaving, ns_dividing)) / double(std::min(ns_interleaving, ns_dividing));
    std::cout << " is faster by a factor " << ratio << std::endl;
    return 0;
}
