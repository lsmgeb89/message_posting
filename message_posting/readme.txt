--src
   |
   --client
   |   |
   |   client.cc client.h: implementation of class Client
   |   |
   |   client_main.cc: main entrance of client
   |
   --server
   |   |
   |   server.cc server.h: implementation of class Server
   |   |
   |   server_main.cc: main entrance of server
   |
   --utils
   |   |
   |   log_util.h: logging utility
   |   |
   |   message_util.h: implementation of class MessageUtil for handling communication
   |   |
   |   socket_util.h: a simple wrapper of system socket operations
   |
   CMakeLists.txt: a CMake file for building project

summary.pdf: summary document

design.pdf: design document

How to compile (only support on csgrads1.utdallas.edu by using Linux shell command):
1. create a build folder outside the src folder
   (eg: mkdir build_minsizerel)
2. change directory to the build folder
   (eg: cd build_minsizerel)
2. cmake 'path_to_source_root' -DCMAKE_BUILD_TYPE=MINSIZEREL
   (eg: cmake ../src -DCMAKE_BUILD_TYPE=MINSIZEREL)
3. make

How to run:
'path_to_message_server' 'port'
(eg: ./message_server 2016)
'path_to_message_client' 'host' 'port'
(eg: ./message_client 127.0.0.1 2016)
