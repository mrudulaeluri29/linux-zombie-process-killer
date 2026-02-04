# Linux Zombie Process Killer

## Project Overview
This project implements a Linux kernel module that detects and cleans zombie processes using a bounded buffer producer consumer model. The module operates entirely in kernel space and demonstrates core operating systems concepts including process lifecycle management, kernel threads, synchronization, and safe interaction with kernel data structures.

The module continuously scans the Linux process table to identify zombie processes belonging to a specified user ID. Identified zombie processes are placed into a bounded buffer by a producer thread. Multiple consumer threads remove entries from the buffer and terminate the corresponding parent processes to allow proper cleanup of the zombie processes by the operating system.

This project was developed as part of an advanced operating systems assignment and is intended for educational and demonstration purposes.

## Problem Addressed
Zombie processes occur when a child process terminates but its parent fails to collect the exit status. Over time, zombie processes can exhaust system resources such as process IDs. This project demonstrates a controlled and synchronized approach to identifying and eliminating zombie processes in a Linux environment.

## Key Features
- Linux kernel module written in C  
- Producer consumer design implemented inside the kernel  
- Kernel threads for concurrent execution  
- Bounded buffer shared between producer and consumers  
- Semaphore based synchronization for buffer access  
- Mutex protection for critical sections  
- Configurable module parameters for flexibility  
- Structured kernel logging for traceability and testing  

## Design Overview

### Producer Thread
The producer thread periodically scans the Linux process list using kernel task structures. When a zombie process matching the specified user ID is found, its task structure pointer is added to the bounded buffer. The producer pauses between scans to prevent excessive CPU usage.

### Consumer Threads
Multiple consumer threads retrieve zombie process entries from the bounded buffer. Each consumer identifies the parent process of the zombie and sends a termination signal to the parent. This allows the operating system to properly reap the zombie process.

### Synchronization Mechanism
The bounded buffer is protected using semaphores to manage available slots and filled slots. A mutex is used to ensure mutual exclusion when accessing shared buffer data. This guarantees thread safety and prevents race conditions inside the kernel.

## Module Parameters
The module behavior can be customized at load time using parameters:
- Number of producer threads  
- Number of consumer threads  
- Size of the bounded buffer  
- Target user ID whose zombie processes should be monitored  

These parameters allow controlled experimentation and scalability testing.

## How to Build and Use the Module
The module should be built and tested on a Linux system with kernel headers installed. Testing is strongly recommended in a virtual machine or isolated environment.

First, navigate to the directory containing the module source and build the module using the system kernel build tools.

After building, load the module into the kernel while specifying the desired parameters such as the number of consumer threads, buffer size, and target user ID.

Once loaded, the producer thread begins scanning for zombie processes and consumer threads process them concurrently. Kernel logs can be monitored to observe producer and consumer activity and verify correct behavior.

When testing is complete, the module can be safely unloaded from the kernel.

## Logging and Observability
Each producer and consumer action is logged using kernel logging facilities. Log messages clearly indicate which thread produced or consumed a zombie process along with the process ID and parent process ID. This logging format supports automated testing and manual verification.

## Testing Environment
The module is intended to be tested in a controlled Linux environment such as a virtual machine. Testing should be performed using a dedicated test user account to avoid impacting critical system processes. Kernel logs are monitored to validate expected behavior.

## Limitations and Safety Notes
This module terminates parent processes to eliminate zombie processes. It should never be used on production systems. Testing must be limited to isolated environments where process termination is safe and expected.

## Skills Demonstrated
- Linux kernel programming  
- Operating systems internals  
- Process lifecycle management  
- Concurrency and synchronization  
- Semaphore and mutex usage  
- Systems level debugging and logging  

## What This Project Demonstrates
This project demonstrates strong foundational knowledge of operating systems and practical experience with kernel level development. It highlights the ability to design concurrent systems, reason about synchronization in constrained environments, and implement safe process management techniques within the Linux kernel.
