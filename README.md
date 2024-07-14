# Parallel Computing C++ Projects

This repository contains a collection of assignments from the 'Parallel Computing' course at the University of Vienna. The projects demonstrate various techniques and algorithms for parallel processing and optimization using the thread library, OpenMP, and MPI frameworks.

## Project Structure

### Directories

- **a1_thread_lib/**: Contains projects utilizing the thread library.
  - **a1 a.pdf**: Task description for thread library projects.
  - **a1 b.pdf**: Additional task description for thread library projects.
  - **histogram/**: Implementation files for histogram projects using the thread library.
    - **helpers.hpp**
    - **histogram-atomic-mutex.cpp**
    - **histogram-best.cpp**
    - **histogram.cpp**
  - **mandelbrot/**: Implementation files for Mandelbrot projects using the thread library.
    - **a.out**
    - **helper.hpp**
    - **image.hpp**
    - **main.cpp**
    - **mandelbrot.hpp**
    - **mandelbrot.ppm**
    - **pixel.hpp**
    - **speedtest.sh**

- **a2_omp/**: Contains projects utilizing OpenMP.
  - **histo-test-best.cpp**: Implementation file for the best histogram test using OpenMP.
  - **histogram/**: Implementation files for histogram projects using OpenMP.
    - **histogram-v1-best.cpp**
    - **histogram-v1-naive.cpp**

- **a3_mpi/**: Contains projects utilizing MPI.
  - **a3.pdf**: Task description for MPI projects.
  - **heat/**: Directory for heat distribution simulation projects using MPI.
    - **heat2d.cpp**
    - **heat2d.txt**
    - **helpers.hpp**
    - **jobscript.sh**: Script for running MPI jobs on a cluster with SLURM.
    - **sequential-heat2d.cpp**
    - **slurm-334592.out**
    - **slurm-334593.out**
    - **slurm-334594.out**

## Getting Started

### Prerequisites

- C++ Compiler (e.g., g++)
- CMake
- Make
- OpenMP
- MPI

### Building the Projects

1. Clone the repository:
   ```bash
   git clone https://github.com/QuirkyCroissant/ParallelComputing-Cpp-Projects.git
   cd ParallelComputing-Cpp-Projects
   ```
2. Create a build directory and navigate to it:
	```bash
	mkdir build
	cd build
	```
3. Generate the Makefile using CMake:
	```bash
	cmake ..
	```
4. Build the project:
	```bash
	make
	```

### Running the Projects

#### Thread Library Projects (a1)

Navigate to the `a1_thread_lib/histogram` or `a1_thread_lib/mandelbrot` directory and execute the built binaries. Example:

```bash
cd a1_thread_lib/histogram
./histogram-atomic-mutex
./histogram-best

cd ../mandelbrot
./a.out
```

#### OpenMP Projects (a2)

Navigate to the `a2_omp` directory and execute the built binaries. Example:

```bash
./histo-test-best
./histogram-v1-best
./histogram-v1-naive
```

#### MPI Projects (a3)

##### Running Locally

Navigate to the `a3_mpi` directory and use the `mpirun` command to execute the MPI binaries. Example:

```bash
cd a3_mpi
mpirun -np 4 ./heat2d
```

##### Running on a Cluster with SLURM

For running on a cluster that supports SLURM, you can use the provided `jobscript.sh` to submit jobs. Ensure you modify the script according to your cluster's configuration.

1. Modify `jobscript.sh` as necessary.
2. Submit the job script using SLURM:
   ```bash
   sbatch jobscript.sh
	```

## Project Details

### Thread Library Projects (a1)

- **histogram-atomic-mutex.cpp**: A histogram implementation using atomic operations and mutex for thread synchronization.
- **histogram-best.cpp**: An optimized version of the histogram using threads.
- **histogram.cpp**: A basic implementation of a histogram using threads.
- **mandelbrot/**: Implementation files for Mandelbrot projects using the thread library.

### OpenMP Projects (a2)

- **histo-test-best.cpp**: The efficient histogram implementation using OpenMP.
- **histogram-v1-best.cpp**: An optimized version of the histogram using only a single OpenMP directive.
- **histogram-v1-naive.cpp**: A naive implementation of a histogram using only a single OpenMP directive.

### MPI Projects (a3)

- **heat2d.cpp**: A 2D heat distribution simulation using MPI for parallel computation.
- **mandelbrot.hpp**: A Mandelbrot set computation using MPI and threads for parallel processing.

## Scripts

- **jobscript.sh**: A script for running MPI jobs on a cluster with SLURM.
- **speedtest.sh**: A script for running speed tests for the thread library and OpenMP projects.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


