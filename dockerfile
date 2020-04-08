FROM centos:7.6.1810

RUN yum install -y gcc gcc-c++
ADD test_server.cpp /server_test/
WORKDIR /server_test/
RUN g++ test_server.cpp -o server
ENTRYPOINT  ["./server"]