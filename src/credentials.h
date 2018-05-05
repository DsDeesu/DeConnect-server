bool checkUser(string login, string password)
{
    fstream file;
    file.open(("users/"+login).c_str(),ios::in);
    if(!file.good())
        return false;
    string password_from_file;
    file >> password_from_file;
    file.close();
    if(password_from_file == password)
        return true;
    return false;
}

bool checkCredentials(int client_fd, string key, string *nick)
{
    char buffer[2048];
    bzero(buffer,2048);
    string login = "";
    string password = "";

    if(recv(client_fd,buffer,2048,0) > 0)
    {
        login=buffer;

        send(client_fd,"+",1,0); // for noerror connection!
        bzero(buffer,2048);

        if(recv(client_fd,buffer,2048,0) > 0)
        {
            password=buffer;
        }
    }
    printf("Enc Login: A|%s|A\nEnc Pass: A|%s|A\n",login.c_str(),password.c_str());
    if((login != "") && (password != ""))
    {
        if(checkUser(decryptstring(login,key),decryptstring(password,key)))
        {
            printf("Dec Login: A|%s|A\nDec Pass: A|%s|A\n",login.c_str(),password.c_str());
            *nick = decryptstring(login,key);
            return true;
        }
    }
    return false;
}

