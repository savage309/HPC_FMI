# HPC Course

## Contents

1. Intro (2)
2. Hardware single threaded (2)
   * registers
   * memory hierarchy
   * pipelines,super scalar
   * instruction level paralism
   * out of order execution
   * register renaming
   * branch prediction
   * tlb cache
   * float vs double
3. Single threaded 
   * How the Program runs (1)
   * Linked list vs vector
   * Soa vs aos
   * reductions
   * Memory dependencies
   * Pointer aliasing
   * Loop unrolling
   * Naive App
   * Double float
   * Cmp with 0
   * Data-oriented design (2)
   * Garbage collection
4. SIMD (2)
   * what is SIMD and why is it important
   * vertical vs horizontal operations
   * x86_64 SIMD
   * ARMv8 SIMD
   * SIMD intrinsics
   * generic vectors
   * auto-vectorization
   * SIMD-ification of some classic problems
5. Roof line (1)
6. Multithreading
   * Hardware 
       * Moore
       * Hyperthreading
       * Amdahl's law
       * OS
       * Context switching
       * User and system mode - fibers
       * Numa, uma, smp
   * Mutex
       * atomic
       * false sharing
       * critical sections
       * contention
       * Conditional variables
       * deadlocks
       * read / write locks

   * Parallel programs
   * patterns - singleton
   * data structures - queue, 
   * algorithms
       * producer consumer
       * map reduce
   * Task based 
   * APIs
       * openmp
       * cilk
       * tbb
       * fibers
       * Languages - Go, 
7.  Mac CPU performance - generate code based on data
8. GPU
   * hardware (2)
   * CUDA (1)
   * OpenCL (1)
   * GPU performance considerations (1)
   * GPU design patterns (1)
   * Developing & debugging GPU programs
   * Optimizing gpu apps (2 )
9. Tools - 3

## Homeworks

* Vector vs list
* Image processing
* Multiprocessing 
* multithread - algorithm with API by choice
* Opencl compute

## Project ideas

* Simple Ray tracer
* image editor
* audio/video encoder
* Monte Carlo
* Cryptography cracking
* Passwords cracking
* Data compression
