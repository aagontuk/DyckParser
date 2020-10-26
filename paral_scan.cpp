#include <iostream>
#include <math.h>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>

void print_vector(std::vector<unsigned long long> &v); 

void reduce(std::vector<unsigned long long> &v, size_t startIndex, size_t endIndex) {
    int limit = std::log2(endIndex - startIndex);
    for (int i = 0; i < limit; i++) {
        size_t x = pow(2, i + 1);
        size_t y = pow(2, i);
        for (size_t j = startIndex; j < endIndex; j += x) {
            v[j + x - 1] = v[j + x - 1] + v[j + y - 1]; 
        }
    }
}

void down_sweep(std::vector<unsigned long long> &v, size_t startIndex, size_t endIndex) {
    int limit = std::log2(endIndex - startIndex);
    for (int i = limit - 1; i >= 0; i--) {
        size_t x = pow(2, i + 1);
        size_t y = pow(2, i);
        unsigned long long tmp;
        for (size_t j = startIndex; j < endIndex; j += x) {
            tmp = v[j + y - 1]; 
            v[j + y - 1] = v[j + x - 1];
            v[j + x - 1] = tmp + v[j + x - 1];
        }    
    }
}

void blelloch_scan(std::vector<unsigned long long> &v) {
    reduce(v, 0, v.size());
    v[v.size() - 1] = 0;
    down_sweep(v, 0, v.size());
}

void seq_scan(std::vector<unsigned long long> &v, int startIndex, int endIndex) {
    unsigned long long tmp = v[startIndex];
    unsigned long long tmp2;
    v[startIndex] = 0;

    for (int i = startIndex + 1; i < endIndex; i++) {
        tmp2 = v[i];
        v[i] = v[i - 1] + tmp;
        tmp = tmp2;
    }
}

void par_scan(std::vector<unsigned long long> &v) {
    std::vector<unsigned long long> tails;
    std::vector<unsigned long long> sums;
    std::vector<std::thread> threadPool;
    int startIndex, endIndex;
    int nproc = std::thread::hardware_concurrency();

    std::cout << "Number of processors: " << nproc << "\n";
    
    for (int i = 0; i < nproc; i++) {
        startIndex = i * (v.size() / nproc); 
        endIndex = ((i + 1) * (v.size() / nproc));
        tails.push_back(v[endIndex - 1]);
        threadPool.push_back(std::thread(seq_scan, std::ref(v), startIndex, endIndex));
    }

    for (int i = 0; i < nproc; i++) {
        threadPool[i].join();
        startIndex = (i * (v.size() / nproc));
        endIndex = ((i + 1) * (v.size() / nproc));
        sums.push_back(v[endIndex - 1]);
    }

    for (int i = 0; i < sums.size(); i++) {
        sums[i] += tails[i];
    }

    seq_scan(std::ref(sums), 0, sums.size());

    for (int i = 0; i < nproc; i++) {
        std::cout << "sum: " << sums[i] << "\ttail: " << tails[i] << "\n";
    }

    for (int i = 1; i < nproc; i++) {
        startIndex = (i * (v.size() / nproc));
        endIndex = ((i + 1) * (v.size() / nproc));
        for (int j = startIndex; j < endIndex; j++) {
            v[j] += sums[i]; 
        }
    }
}

void print_vector(std::vector<unsigned long long> &v) {
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << " "; 
    }   
    std::cout << "\n";
}

int main() {
    std::vector<unsigned long long> v;
    std::vector<unsigned long long> v2;
    std::vector<unsigned long long> v3;
    size_t SIZE = std::pow(2, 24);

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    for (size_t i = 1; i <= SIZE; i++) {
        v2.push_back(i); 
    }
    
    for (size_t i = 1; i <= SIZE; i++) {
        v.push_back(i); 
    }

    /* sequential */
    start = std::chrono::high_resolution_clock::now();
    seq_scan(std::ref(v2), 0, v2.size());
    end = std::chrono::high_resolution_clock::now(); 
    double time_taken_seq = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << v2[v2.size() - 1] << "\n";
    std::cout << "Sequential time: " << time_taken_seq * 1e-9 << " sec\n";

    start = std::chrono::high_resolution_clock::now();
    blelloch_scan(std::ref(v));
    end = std::chrono::high_resolution_clock::now(); 
    double time_taken_par = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << v[v.size() - 1] << "\n";
    std::cout << "Blelloch time: " << time_taken_par * 1e-9 << " sec\n";


    /* parallel */
    start = std::chrono::high_resolution_clock::now();
    par_scan(std::ref(v));
    end = std::chrono::high_resolution_clock::now(); 
    time_taken_par = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << v3[SIZE-1] << "\n";
    std::cout << "Parallel time: " << time_taken_par * 1e-9 << " sec\n";
    std::cout << "Speedup: " << time_taken_seq/time_taken_par << "X\n";

    return 0;
}
