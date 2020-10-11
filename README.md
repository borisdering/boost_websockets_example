# Basic Websockets Implementation in C++ with Boost::Asio

## Requirements 
You need at least those dependencies installed
* build-essential
* cmake
* libboost-all-dev // system should be enough


## Build this Project
```asm
$ cmake -H. -Bbuild
$ cd build
$ make 
```

## NodeJS Server for Testing
The folder node_ws_example contains a basic server implementation 
with websockets it sends a keep alive signal every second that passed.