
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <tuple>
#include <time.h>
#include <cmath>
#include <complex>
#include <chrono>
#include <future>
#include <thread>

#include "helper.hpp"
#include "mandelbrot.hpp"

using namespace std;

int main(int argc, char **argv)
{
    // arguments 
    int num_threads = 1;
    int max_iterations = 2048;
    int print_level = 1; // 0 - no exec time, 1 - exec time and pixel count
    
    // height and width of the output image
    int width = 512, height = 384;

    helper::parse_args(argc, argv, num_threads, height, width, max_iterations, print_level);

    std::cout << "Generating Mandelbrot for " << width << "x" << height << " image (max iterations: " << max_iterations << ") with " << num_threads << " threads.\n";
    
    int pixels_inside = 0;

    // Generate Mandelbrot set in this image
    Mandelbrot mb(height, width, max_iterations); 
    
    std::vector<std::thread> threads;

    auto t1 = chrono::high_resolution_clock::now();

    pixels_inside = mb.compute(num_threads);

    auto t2 = chrono::high_resolution_clock::now();

    // save image
    mb.save_to_ppm("mandelbrot.ppm");

    cout << "Total Mandelbrot pixels: " << pixels_inside << endl;
    if ( print_level >= 1 ) cout << chrono::duration<double>(t2 - t1).count() << endl;

    return 0;
}