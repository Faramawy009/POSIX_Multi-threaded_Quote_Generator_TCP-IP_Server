# Multi-Threaded quote generator server using POSIX threads and TCP/IP

This project is part of the Operating Systems Class at the University of Minnesota CSCI 4061 to fully understand its purpose it is highly recommended to read the Handout.pdf file

## Getting Started

* To run:
  1) run the command make
  2) run the command "./server.out" to start the server (recommended: localhost)
  3) run the command "./client.out <IP address> or localhost"to start a client


### Important Notes

* To clean up the directory from object and executable files, run the command: make clean

* The program assumes the following limitations:
   1) maximum message length: 1024 characters
   2) maximum number of categories: 1000
   3) max pending connections queue will hold is 10
   4) each quotes file contains an even number of lines (Non empty)
