#include <sys/socket.h> //socket(), bind() etc
#include <unistd.h> //close()
#include <netinet/in.h> //struct sockaddr_in
#include <arpa/inet.h> //inet_addr()

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <fstream>
#include <vector>
#include <thread>
#include <string.h>

using namespace std;
