#pragma once
#include <vector>
#include <numeric>

template<typename T>
T computeAverage(const std::vector<T>& values) {
    if (values.empty()) {
        return T{};
    }
    T sum = std::accumulate(values.begin(), values.end(), T{});
    return sum / static_cast<T>(values.size());
}