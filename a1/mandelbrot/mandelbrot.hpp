
#include <vector>
#include <complex>
#include <numeric>
#include <mutex>
#include <thread>
#include "pixel.hpp"
#include "image.hpp"

class Mandelbrot {
    Image image;

    int max_iterations = 2048;
    int pixels_inside = 0;
    std::mutex mutex;
    

public:
    Mandelbrot(int rows, int cols, int max_iterations): image(rows, cols, {255, 255, 255}), max_iterations(max_iterations) { }

    int compute(int num_threads = 1) {

        std::vector<std::thread> thrds;
        std::vector<std::vector<std::pair<size_t, size_t>>> thrd_procs_vec(num_threads);

        // tasks will be distributed by associating pixels to given threads in an
        // even manner by the modulo operator 
        for(size_t y = 0; y < image.height; y++){
            for(size_t x = 0; x < image.width; x++){
                thrd_procs_vec[(y * image.width + x) % num_threads]
                    .push_back({x, y});
            }
        }
        // each thread gets created and starts its task of assigned part of 
        // the image by calling the worker function
        for (size_t i = 0; i < num_threads; i++){
            thrds.push_back(std::thread(&Mandelbrot::worker, this, thrd_procs_vec[i], num_threads, i));
        }

        // waiting for all the threads to be completed
        for (auto& t : thrds) 
            t.join();
        
        return pixels_inside;

        //return worker(num_threads);
    }

    int worker(std::vector<std::pair<size_t, size_t>> process, int num_threads, int thread_id=0)
    {
        size_t thread_px_cnt = 0;
        
        unsigned char color = (255*(thread_id+1))/num_threads; // comment this out if you want a single color

        for (const auto& proc : process){
            size_t x = proc.first;
            size_t y = proc.second;

            double dx = ((double)x / image.width - 0.75) * 2.0;
            double dy = ((double)y / image.height - 0.5) * 2.0;

            std::complex<double> c(dx, dy);

            if (check_pixel(c)) { 
                image[y][x] = {color, color, color}; // {0, 0, 0} - black for a pixel inside of the mandelbrot set
                thread_px_cnt++;
            }

        }
        
        // safely override the shared memory by mutex
        std::lock_guard<std::mutex> guard(mutex);
        pixels_inside += thread_px_cnt;

        /*
        for (int y = 0; y < image.height; ++y) 
        {
            for (int x = 0; x < image.width; ++x) 
            {
                double dx = ((double)x / image.width - 0.75) * 2.0;
                double dy = ((double)y / image.height - 0.5) * 2.0;

                std::complex<double> c(dx, dy);

                if (check_pixel(c)) { 
                    image[y][x] = {color, color, color}; // {0, 0, 0} - black for a pixel inside of the mandelbrot set
                    pixels_inside++;
                }
            }
        }
        
        return pixels_inside;
        
        */


        return thread_px_cnt;
    }

    // Test if point c belongs to the Mandelbrot set
    bool check_pixel(std::complex<double> c)
    {
        std::complex<double> z(0, 0);
        for (int i=0; i<max_iterations; ++i) {
            z = z * z + c;
            if ( std::abs(z) > 4 ) return false;
        }

        return true;
    };

    void save_to_ppm(std::string filename){
        image.save_to_ppm(filename);
    }
};