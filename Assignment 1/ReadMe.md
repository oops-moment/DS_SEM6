## Overview

This MPI parallelized C++ program solves the N-Queens problem and distributes the workload among multiple processes.

### Time Complexity

- Total time complexity: O(n^2 * n!) where n is the number of queens.

### Message Complexity

- Total message complexity: O(size) due to MPI communication in the `MPI_Reduce` function.

### Space Requirements

- Space complexity: O(n), primarily occupied by the `perm` vector in the `nqueens` function.

### Performance Scaling Results

The N-Queens MPI parallel solver demonstrates effective performance scaling:

- **1 Process:**
  - Time taken: 0.0236754 seconds

- **2 Processes:**
  - Time taken: 0.014453 seconds
  - Speedup: ~1.64x compared to 1 process

- **4 Processes:**
  - Time taken: 0.0124854 seconds
  - Speedup: ~1.9x compared to 1 process

- **7 Processes:**
  - Time taken: 0.0088418 seconds
  - Speedup: ~2.68x compared to 1 process

- **8 Processes:**
  - Time taken: 0.0085037 seconds
  - Speedup: ~2.79x compared to 1 process

These results indicate a favorable performance improvement as the number of processes increases, showcasing the efficiency of parallelization in solving the N-Queens problem.
