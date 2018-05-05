# DeConnect-server
Server for DeConnect chat.
## SETUP
  1. cd ~
  1. git clone https://github.com/DsDeesu/DeConnect-server.git
  1. cd DeConnect-server
  1. mkdir bin
  1. g++ main.cpp -lpthread -o bin/server
  1. cd bin
  1. mkdir keys
  1. mkdir users

## Configure RSA keys
  1. Generate server private key insinde keys/private.key
      * openssl genrsa -out keys/private.key 4096
  1. Generate server public key for clients
      * openssl rsa -pubout -in private.key -out keys/public.key

## Add users
  1. For normal account
      * echo "password" | sha256sum > users/nick
  1. For server account (red nickname :P)
      * echo "password" | sha256sum > users/SERVER
      
## Configure server instane via cli
    * -i ip_on_which_server_will_listen
    * -p port_on_which_server_will_listen
    
## Troubleshooting
  Server is logging everything to stdout. But client in default not. If you want to see what's going on client instance just add "-d" as argument to client binary.
