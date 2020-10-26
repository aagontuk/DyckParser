#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>

void sum(std::vector<unsigned long long> &v, size_t startIndex, size_t endIndex, size_t depth) {
    size_t x = pow(2, depth); 
    size_t y = pow(2, depth + 1); 
    
    for (size_t i = startIndex + x - 1; i < endIndex; i += y) {
        v[i + x] = v[i] + v[i + x];
    }
}

unsigned long long paral_sum(std::vector<unsigned long long> &v) {
    for (size_t i = 0; i < log2(v.size()); i++) {
        int x = pow(2, i);
        int y = pow(2, i + 1);
        for (size_t j = x - 1; j < v.size(); j += y) {
            v[j + x] = v[j] + v[j + x];
        }
    }

    return v[v.size() - 1];
}

unsigned long long paral_sum2(std::vector<unsigned long long> &v) {
    int nproc = std::thread::hardware_concurrency();
    std::vector<std::thread> threadPool(nproc);

    for (int i = 0; i < log2(v.size()); i++) {
        for (int j = 0; j < nproc; j++) {
            size_t startIndex = j * (v.size() / nproc);
            size_t endIndex = (j + 1) * (v.size() / nproc);
            threadPool[j] = std::thread(sum, std::ref(v), startIndex, endIndex, i); 
        }
        
        for (int j = 0; j < nproc; j++) {
            threadPool[j].join(); 
        }
    }

    return v[v.size() - 1];
}

unsigned long long seq_sum(std::vector<unsigned long long> &v) {
    unsigned long long sum = 0;
    for (size_t i = 0; i < v.size(); i++) {
        sum += v[i]; 
    }

    return sum;
}

int main() {
    std::vector<unsigned long long> v;
    std::vector<unsigned long long> v2;
    size_t size = 1024*1024;

    for (size_t i = 1; i <= size; i++) {
        v.push_back(i); 
    }
    
    for (size_t i = 1; i <= size; i++) {
        v2.push_back(i); 
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Sequential sum: " << seq_sum(std::ref(v)) << "\n";
    auto end = std::chrono::high_resolution_clock::now(); 
    double time_taken_seq = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << "Time: " << time_taken_seq * 1e-9 << " sec\n";
    
    start = std::chrono::high_resolution_clock::now();
    end = std::chrono::high_resolution_clock::now(); 
    std::cout << "Parallel sum2: " << paral_sum2(std::ref(v2)) << "\n";
    double time_taken_par = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << "Time: " << time_taken_par * 1e-9 << " sec\n";
    std::cout << "Speedup: " << time_taken_seq/time_taken_par << "X\n";

    return 0;
}
