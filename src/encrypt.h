#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

string exec(const char* cmd);

string decrypt_aes_key(string key)
{
    return exec(("echo '"+key+"' | base64 -d | openssl rsautl -decrypt -inkey keys/private.key | tr -d '\n'").c_str());
}

//string connection_decryptstring(string msg)
//{
//    system(("base64 -d "+msg+" | openssl aes-256-cbc -a -A -e -salt -md sha256 -k '"+aes_key+"' | tr -d '\n' > "+to_string(fd+1)).c_str());
//    fstream msg_file;
//    msg_file.open(to_string(fd+1),ios::in);
//    string decr_msg;
//    getline(msg_file,decr_msg);
//    msg_file.close();
//    remove(to_string(fd+1).c_str());
//    return decr_msg;
//}
//
//string connection_encryptstring(string msg)
//{
//    fstream msg_file;
//    msg_file.open(to_string(fd),ios::out);
//    msg_file << msg;
//    msg_file.close();
//    string encrypted_msg_file = exec(("base64 "+to_string(fd)+" | openssl aes-256-cbc -a -A -e -salt -md sha256 -k '"+aes_key+"' | tr -d '\n'").c_str());
//    //exit(0);
//    remove(to_string(fd).c_str());
//    return encrypted_msg_file;
//}

string decryptstring(string msg, string key)
{
    return exec(("echo '"+msg+"' | openssl aes-256-cbc -a -A -d -salt -md sha256 -k '"+key+"' | base64 -d ").c_str());
}

string encryptstring(string msg, string key)
{
    fstream msg_file;
    msg_file.open(to_string(1),ios::out);
    msg_file << msg;
    msg_file.close();
    string encrypted_msg_file = exec(("base64 "+to_string(1)+" | openssl aes-256-cbc -a -A -e -salt -md sha256 -k '"+key+"' | tr -d '\n'").c_str());
    //exit(0);
    remove(to_string(1).c_str());
    return encrypted_msg_file;
}
//
//string encryptstring(string msg, string key)
//{
//    return exec(("echo '"+msg+"' | base64 | openssl aes-256-cbc -a -A -e -salt -md sha256 -k '"+key+"' | tr -d '\n'").c_str());
//}

//Created by waqas and edited by Curious on stackoverflow.com
//http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
string exec(const char* cmd)
{
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}
