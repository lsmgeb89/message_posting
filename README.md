# Message Posting (Network Communication Using Sockets)

## Summary
  * Implemented a message posting system composed of a server and clients - each client or the server is a process running on a machine
  * Utilized [sockets][socket] to communicate between a client with the server while handling partial messages and [endianness][endian] problem
  * Utilized [`std::thread`][std_thread] to create a detached thread for serving each incoming client
  * Utilized [`std::mutex`][std_mutex] to maintain data consistency while serving multiple clients concurrently

## Project Information
  * Course: [Operating Systems Concepts (CS 5348)][os]
  * Professor: [Greg Ozbirn][ozbirn]
  * Semester: Spring 2016
  * Programming Language: C++
  * Build Tool: [CMake][cmake]

[socket]: https://en.wikipedia.org/wiki/Network_socket
[endian]: https://en.wikipedia.org/wiki/Endianness
[std_thread]: http://en.cppreference.com/w/cpp/thread/thread
[std_mutex]: http://en.cppreference.com/w/cpp/thread/mutex
[os]: https://catalog.utdallas.edu/2016/graduate/courses/cs5348
[cmake]: https://cmake.org/
[ozbirn]: http://cs.utdallas.edu/people/faculty/ozbirn-greg/
