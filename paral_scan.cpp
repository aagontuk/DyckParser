#include <iostream>
#include <math.h>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>

void reduce(std::vector<int> &v, int startIndex, int endIndex) {
    int limit = std::log2(endIndex - startIndex);
    for (int i = 0; i < limit; i++) {
        int x = pow(2, i + 1);
        int y = pow(2, i);
        for (int j = startIndex; j < endIndex; j += x) {
            v[j + x - 1] = v[j + x - 1] + v[j + y - 1]; 
        }
    }
}

void down_sweep(std::vector<int> &v, int startIndex, int endIndex) {
    int limit = std::log2(endIndex - startIndex) - 1;
    for (int i = limit; i >= 0; i--) {
        int x = pow(2, i + 1);
        int y = pow(2, i);
        int tmp;
        for (int j = startIndex; j < endIndex; j += x) {
            tmp = v[j + y - 1]; 
            v[j + y - 1] = v[j + x - 1];
            v[j + x - 1] = tmp + v[j + x - 1];
        }    
    }
}

void blelloch_scan(std::vector<int> &v, int startIndex, int endIndex) {
    reduce(v, startIndex, endIndex / 2);
    reduce(v, endIndex / 2, endIndex);
    
    v[endIndex - 1] = v[(endIndex - 1) / 2];
    v[(endIndex - 1) / 2] = 0;
    
    down_sweep(v, startIndex, endIndex / 2);
    down_sweep(v, endIndex / 2, endIndex);
}

void seq_scan(std::vector<int> &v, int startIndex, int endIndex) {
    int tmp = v[startIndex];
    int tmp2;
    v[startIndex] = 0;

    for (int i = startIndex + 1; i < endIndex; i++) {
        tmp2 = v[i];
        v[i] = v[i - 1] + tmp;
        tmp = tmp2;
    }
}

int main() {
    std::vector<int> v;
    std::vector<int> v2;
    std::vector<int> tails;
    std::vector<int> sums;
    std::vector<std::thread> threadPool;
    int startIndex, endIndex;
    int nproc = std::thread::hardware_concurrency();

    for (int i = 1; i <= 1024*1024; i++) {
        v2.push_back(i); 
    }
    
    for (int i = 1; i <= 1024*1024; i++) {
        v.push_back(i); 
    }

    auto start = std::chrono::high_resolution_clock::now();

    /* sequential */
    seq_scan(std::ref(v2), 0, v2.size());

    auto end = std::chrono::high_resolution_clock::now(); 
    double time_taken_seq = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << "Seq: " << time_taken_seq * 1e-9 << " sec\n";
    
    start = std::chrono::high_resolution_clock::now();
    
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

    for (int i = 1; i < nproc; i++) {
        startIndex = (i * (v.size() / nproc));
        endIndex = ((i + 1) * (v.size() / nproc));
        for (int j = startIndex; j < endIndex; j++) {
            v[j] += sums[i]; 
        }
    }
    
    end = std::chrono::high_resolution_clock::now(); 
    double time_taken_par = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); 
    std::cout << "par: " << time_taken_par * 1e-9 << " sec\n";
    std::cout << "Speedup: " << ((time_taken_par/time_taken_seq) - 1) * 100 << "%\n";

    return 0;
}
