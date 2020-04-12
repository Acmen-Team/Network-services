FROM gcc AS mybuildstage
COPY test_server.cpp .
RUN g++ -o server test_server.cpp
FROM centos:7.6.1810
COPY --from=mybuildstage server .
ENTRYPOINT  ["./server"]