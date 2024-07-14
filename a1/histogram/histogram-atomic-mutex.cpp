#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <numeric>
#include <atomic>
#include <thread>

#include "helpers.hpp"

using namespace std;

struct histogram {
	std::vector<std::atomic<int>> data;
	
	histogram(int count) : data(count){ 
		for (auto& atom : data) {
            atom.store(0, std::memory_order_relaxed);
        }
	}

	void add(int i) {
		data[i].fetch_add(1, std::memory_order_relaxed);
	}

	int get(int i)	{
		return data[i].load(std::memory_order_relaxed);
	}

	void print(std::ostream& str) {
		size_t total = 0;
		for (size_t i = 0; i < data.size(); ++i){ 
			int cnt = data[i].load(std::memory_order_relaxed);

			str << i << ":" << cnt << "\n";
			total += cnt;
		}

		str << "total:" << total << "\n";
	}
};

void worker(int begin, int end, histogram& h, int num_bins) 
{

	generator gen(num_bins);

	for (size_t i = begin; i < end; i++) {
		int next = gen();
		h.add(next);
		
	}
	
}

int main(int argc, char **argv)
{
	int num_bins = 10;
	int sample_count = 30000000;

	int num_threads = std::thread::hardware_concurrency();
	int print_level = 2; // 0 exec time only, 1 histogram, 2 histogram + config
	parse_args(argc, argv, num_threads, num_bins, sample_count, print_level);
	
	histogram h(num_bins);
	std::vector<std::thread> thrds;

	// this calculates how many samples per thread will be evenly distributed
	// alongside all used threads
	int samp_in_thrd = sample_count / num_threads;

	auto t1 = chrono::high_resolution_clock::now();

	//worker(sample_count, h, num_bins);

	// distributing the subtasks to each future thread 
	for (size_t i = 0; i < num_threads; i++) {
		int begin = i * samp_in_thrd;
		int end = (i + 1) * samp_in_thrd;

		if (i == num_threads-1)
			end =sample_count;
		

		thrds.emplace_back(worker, begin, end, std::ref(h), num_bins);	
	}

	// creating and waiting for the completion of all the threads
	for (auto& t : thrds)
		t.join();

	auto t2 = chrono::high_resolution_clock::now();

	if ( print_level >= 2 ) std::cout << "Bins: " << num_bins << ", sample size: " << sample_count << ", threads: " << num_threads << endl;
	if ( print_level >= 1 ) h.print(std::cout);
	std::cout << chrono::duration<double>(t2 - t1).count() << endl;
}