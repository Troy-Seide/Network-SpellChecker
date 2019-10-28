/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NetworkSpellchecker.cpp
 * Author: Owner
 *
 * Created on October 22, 2019, 2:29 PM
 */

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#define WORKERSIZE 6
using namespace std;

/*
 * 
 */
pthread_cond_t cFull, lFull, cEmpty, lEmpty;
char cbuffer1[250], lbuffer[1024];
pthread_t worker_threads[WORKERSIZE]; 
//pthread_create(&name, NULL, methodName, NULL);
//thread_pool pool(8);
pthread_t log_thread;
pthread_mutex_t cmutex;
pthread_mutex_t lmutex;
string dictionwords[200000];
void *wordChecker(void* t);
void *logfile(void* i);
int main(int argc, char** argv) {
    pthread_cond_init(&cFull, NULL);
    pthread_cond_init(&lFull, NULL);
    pthread_cond_init(&cEmpty, NULL);
    pthread_cond_init(&lEmpty, NULL);
    int length=0;
    ifstream dictwords;
    string words;
    dictwords.open("dictionary.txt");
 
    int i=0;
    cout<<"before while";
    while(!dictwords.eof()){
        getline(dictwords, words);
        //cout<<words;
        dictionwords[i]=words;
        //cout<<words<<endl;
        cout<<words[1];
        i++;
    }
    
    for(int i=0; i<WORKERSIZE; i++){
        pthread_create(&worker_threads[i], NULL, wordChecker, NULL);
    }
    pthread_create(&log_thread, NULL, logfile, NULL);
    //(Shakeel's Work from lines 31 to 57)
    int portNumber=8888;
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;
    char *message;
    socket_desc=socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc==-1){
        puts("Error creating socket!");
    }
    server.sin_family= AF_INET;
    server.sin_addr.s_addr= INADDR_ANY;
    server.sin_port= htons(portNumber);
    if(bind(socket_desc,(struct sockaddr*)&server, sizeof(server))<0){
        puts("Error: Bind Failed");
        return 1;
    }
    puts("Bind done.");
    listen(socket_desc, 3);
    puts("Waiting for incoming connections...");
    while(1){
       c=sizeof(struct sockaddr_in);
       new_socket=accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c); //somehow add new socket to the client buffer
       if(new_socket<0){
           perror("Error: Accept Failed");
           return 1;
       }
        puts("Connection accepted.");
    }
       return 0;
}

void *wordChecker(void* t){
    //code for processing client
    cout<<"hello";
}
void *logfile(void* i){
    
    cout<<"hello";
}
