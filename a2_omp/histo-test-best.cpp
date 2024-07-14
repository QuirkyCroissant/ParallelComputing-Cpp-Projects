#include <vector>
#include <iostream>
#include <chrono>
#include <random>

using namespace std;

struct generator {
private:
	int max;
	std::minstd_rand engine; 
	std::uniform_int_distribution<int> dist;

public:
	// let's generate random number like this for consistent tests
	generator(const int& max) : dist(0, max-1) {	}
	int operator()() {
		return dist(engine);
	}

};

struct histogram {
	int bins, *data;

	histogram(int count) : bins(count) {
		// allocate memory for histogram
		data = (int*) malloc(sizeof(int) * count);

		// initialize histogram with 0's
		for (int b = 0; b < count; b++) {
			data[b] = 0;
		}
	}

	~histogram() {free(data); }

	void populate(int sample_size) {
		// initialize random number generator
		generator number_generator(bins);

		vector<int> rand_num(bins, 0);

		#pragma omp parallel for reduction(+:rand_num) 
		for (int i = 0; i < sample_size; i++) {
			// generate random number
			//int random_number = number_generator();
			rand_num[number_generator()]++;

			// update corresponding bin
			//data[random_number]++;
		}

		for(int i = 0; i < bins; i++){
			data[i] += rand_num[i];
		}

	}

	void print() {
		int total = 0;
		for (int b = 0; b < bins; b++) {
			std::cout << b << ": " << data[b] << std::endl;
			total += data[b];
		}
		std::cout << "total: " << total << std::endl;
	}
};

int main(int argc, char **argv)
{
	int num_bins = 10;
	int sample_count = 30000000;

	std::cout << "Bins: " << num_bins << ", sample size: " << sample_count << std::endl;
	
	// initialize and empty histogram with 'num_bins' bins
	histogram h(num_bins);

	auto t1 = chrono::high_resolution_clock::now();

	// populate the histogram that was just created
	h.populate(sample_count);
	
	auto t2 = chrono::high_resolution_clock::now();

	// print the contents of the histogram and the time it took populate it
	h.print();
	std::cout << "\ntime elapsed: " << chrono::duration<double>(t2 - t1).count() << " seconds." << std::endl;
}