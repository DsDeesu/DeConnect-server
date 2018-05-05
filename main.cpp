#include "server.h"


struct user
{
    int fd;
    string key;
    string nick;
};

vector <user> ConnectedUsers;

vector <thread> ConnectionsVector;

string message_add_date(string msg)
{
    //define needed variables
    char date[15];

    //get system date
    time_t actual_time = time(0);
    struct tm tstruct;
    tstruct = *localtime(&actual_time);

    //convert it to YY MM DD HH mm
    strftime(date, 15, "%Y%m%d%H%M", &tstruct);

    cout << "Sending: " << date << msg << endl;

    return date + msg;
}

void signalHandler(int signum)
{
    cout << endl << "Received siginit, terminating.." << endl;

    for(int i=ConnectedUsers.size()-1; i>=0; i--)
    {
        close(ConnectedUsers[i].fd);
    }
    cout << "Waiting for threads.." << endl;
    for(int i=0; i<ConnectionsVector.size(); i++)
    {
        cout << "Thread nr." << i+1 << endl;
        ConnectionsVector[i].join();
    }
    close(server_fd);
    exit(0);
}

void error_network(string msg)
{
    cout << endl << msg << endl;
    close(server_fd);
    exit(0);
}

void send_to_all(string msg)
{
    for(int i=0; i<ConnectedUsers.size(); i++)
    {
        string encrypted_message = encryptstring(msg,ConnectedUsers[i].key);
        send(ConnectedUsers[i].fd,encrypted_message.c_str(),encrypted_message.length(),0);
    }
}

void welcome_message(string nick)
{
    for(int i=0; i<ConnectedUsers.size(); i++)
    {
        if(ConnectedUsers[i].nick == nick)
        {
            string welcome_message = "00000000000006SERVERWelcome on DeConnect";
            welcome_message = encryptstring(welcome_message,ConnectedUsers[i].key);
            write(ConnectedUsers[i].fd,welcome_message.c_str(),welcome_message.length());
        }
        else
        {
            string msg;
            msg = encryptstring(("00000000000006SERVER"+nick+" has joined channel").c_str(),ConnectedUsers[i].key);
            write(ConnectedUsers[i].fd,msg.c_str(),msg.length());
        }
    }
}

void user_handler(int client_fd)
{
    char buffer[4096];
    bzero(buffer,4096);
    recv(client_fd,buffer,4096,0);
    string client_key = decrypt_aes_key(buffer);
    cout << "Received key: " << client_key << endl;

    string nick="";

    if(!checkCredentials(client_fd, client_key, &nick))
    {
        cout << "Bad credentials\n";
        send(client_fd,"0",1,0); // error bad credentials
        close(client_fd);
    }
    else
    {
        bool islogged=false;
        for(int i=0; i<ConnectedUsers.size(); i++)
        {
            if(ConnectedUsers[i].nick == nick)
            {
                islogged=true;
                break;
            }
        }
        if(islogged)
        {
            cout << "User: " << nick << " is already logged!" << endl;
            send(client_fd,"2",1,0);
            close(client_fd);
        }
        else
        {
            cout << "Good credentials\n";
            send(client_fd,"1",1,0); // good, correct credentials

            user actual_user;
            actual_user.fd = client_fd;
            actual_user.key = client_key;
            actual_user.nick = nick;

            ConnectedUsers.push_back(actual_user);

            cout << "Sending welcome message to " << client_fd << endl;
            welcome_message(nick);

            while(true)
            {
                cout << "Waiting for message.." << endl;
                bzero(buffer,4096);
                if(recv(client_fd,buffer,4096,0) > 0)
                {
                    string msg = decryptstring(buffer,client_key);
                    cout << "Get message: " << msg << endl;

                    int len = atoi(msg.substr(0,2).c_str());
                    if(msg[len+2] == '/')
                    {
                        cout << "Command.." << endl;
                        string cmd = msg.substr(len+2);
                        if((cmd == "/list") || (cmd == "/l"))
                        {
                            cout << "Command /list" << endl;
                            string client_list="00000000000006SERVERConnected Users: ";
                            for(int i=0; i<ConnectedUsers.size(); i++)
                            {
                                client_list += ConnectedUsers[i].nick;
                                if(i+1 < ConnectedUsers.size())
                                    client_list += ", ";
                            }
                            cout << "Users list: " << client_list << endl;

                            client_list = encryptstring(client_list,client_key);

                            send(client_fd,client_list.c_str(),client_list.length(),0);
                        }
                    }
                    else
                    {
                        send_to_all(message_add_date(msg));
                    }
                }
                else
                    break;
            }
            cout << "User: " << nick << " is disconnecting.." << endl;
            for(int i=0; i<ConnectedUsers.size(); i++)
            {
                if(ConnectedUsers[i].fd == client_fd)
                {
                    ConnectedUsers.erase(ConnectedUsers.begin()+i);
                    break;
                }
            }

            cout << "Closing user: " << nick << " socket.." << endl;
            close(client_fd);
        }
    }


}


int main(int argc, char *argv[])
{
    signal(SIGINT,signalHandler);

    for(int i=1; i<argc; i++)
    {
        string argument = argv[i];
        if((argument == "-p") || (argument == "--port"))
        {
            PORT = atoi(argv[i+1]);
            i++;
        }
        else if((argument == "-i") || (argument == "--ip"))
        {
            SERVERIP = argv[i+1];
            i++;
        }
        else
        {
            cout << "Unknown option " << argument << ". Use -h/--help to get more informations." << endl;
            exit(0);
        }
    }

    struct sockaddr_in serv_struct, cli_struct;
    serv_struct.sin_family = AF_INET;
    serv_struct.sin_addr.s_addr = inet_addr(SERVERIP);
    serv_struct.sin_port = htons(PORT);
    socklen_t cli_size = sizeof(cli_struct);

    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1)
        error_network("Error when creating socket.");
    if(bind(server_fd,(struct sockaddr *) &serv_struct, sizeof(serv_struct)) == -1)
        error_network("Error when binding");
    listen(server_fd,5);

    int client_fd=0;

    cout << "Waiting for connections.." << endl;
    while(true)
    {
        client_fd = accept(server_fd,(struct sockaddr *) &cli_struct, &cli_size);
        if(client_fd > 0)
        {
            cout    << "Connection from: " << inet_ntoa(cli_struct.sin_addr)
                    << " on port: " << ntohs(cli_struct.sin_port)
                    << endl;
            cout << "Connection status: " << ConnectedUsers.size() << endl;
            cout << "Creating thread.." << endl;
            ConnectionsVector.push_back(thread(user_handler,client_fd));
            client_fd = 0;
        }
    }

    return 0;
}
