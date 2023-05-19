# CU Boulder Operating Systems (CSCI3753) Programming Assignments 
This is a repository for some of the programming assignments I have completed for the CSCI3753 Operating Systems class at CU Boulder.
# Simple Character Device Driver
Implementation of a simple character device driver using Loadable Kernel Modules (LKMs). Also has a user test program where a user can use commands to write, seek, and read from a file using the character device driver. 

# Multi-Threading 
The purpose of this assignment was to use multi-threading to resolve domain names to IP addresses. It reproduces the classic producer-consumer problem by having requesters, which read from files that list domain names, and resolvers which resolve the given domain name to an IP address. 

The requester threads read from multiple files that have domain names and place these names in a shared buffer, which is implemented as a circular queue. There are semaphores and mutexes used to handle access to the buffer as well as the checking whether the buffer is full or not. If the buffer if full, a requester will wait until a space has been found. Once a thread has added a domain name to the buffer, it will write the domain name to a shared requester thread log file.

The resolver threads are the consumers. They will wait until there is an item in the shared array and will remove the domain name from the buffer before resolving it to an IP address using a given utility function. The resolver threads also write the domain name and its respective IP address in a shared resolver log file. If a domain name cannot be resolved, the thread simply writes NOT_RESOLVED instead. Access to all shared variables (buffer, log files, input files, etc) is controlled with synchronization variables such as mutexes and semaphores. 
