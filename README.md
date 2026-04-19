# Parallel Word Frequency Analyzer

## 📖 Overview
This project explores parallel computing techniques to efficiently find the top 10 most frequent words in a large dataset.

Three approaches were implemented:
- Naïve (Sequential execution)
- Multithreading using POSIX Threads (pthreads)
- Multiprocessing using Unix processes

## ⚙️ Technologies Used
- C Programming Language
- POSIX Threads (pthreads)
- Unix System Calls (fork, wait)
- Shared Memory (mmap)
- Semaphores for synchronization

## 🚀 Key Features
- Parallel processing using threads and processes
- Synchronization using mutexes and semaphores
- Performance comparison between different approaches
- Scalability analysis using Amdahl’s Law

## 📊 Results
- Significant performance improvement using parallel approaches
- Multithreading and multiprocessing reduced execution time drastically compared to naïve approach
- Optimal performance achieved with 8 threads/processes (based on system cores)
