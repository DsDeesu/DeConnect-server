#include "include.h"

int PORT=12121;
char* SERVERIP = "127.0.0.1";
int server_fd;



string decrypt_aes_key(string key);
string encryptstring(string msg, string key);
string decryptstring(string msg, string key);

bool checkCredentials(int client_fd, string key);
#include "src/encrypt.h"
#include "src/credentials.h"
