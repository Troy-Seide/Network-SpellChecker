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
pthread_cond_t cnotFull, lnotFull, cnotEmpty, lnotEmpty;
int cbuffer[250];
char lbuffer[2][512];
pthread_t worker_threads[WORKERSIZE]; 
pthread_t log_thread;
pthread_mutex_t cmutex;
pthread_mutex_t lmutex;
const int size=20000;
int csize=0;
int lsize=0;
string dictionwords[size];
void *wordChecker(void* t);
void *logfile(void* i);
int chead=0,ctail=0, lhead=0, ltail=0; 
int main(int argc, char** argv) {
    pthread_cond_init(&cnotFull, NULL);
    pthread_cond_init(&lnotFull, NULL);
    pthread_cond_init(&cnotEmpty, NULL);
    pthread_cond_init(&lnotEmpty, NULL);
    int length=0;
    ifstream dictwords;
    string words;
    dictwords.open("dictionary.txt");
    
    if(dictwords.is_open()){
        for(int i=0; i<size; i++){
            dictwords>>dictionwords[i];
            cout<<dictionwords[i];
            cout<<" ";
        }
    }
    /*
    //cout<<"before while";
    while(!dictwords.eof()){
        getline(dictwords, words);
        //cout<<words;
        dictionwords[i]=words;
        //cout<<words<<endl;
        i++;
        cout<<dictionwords[i];
        cout<<" "; 
    }
     */
    //cout<<words[1];
    for(int i=0; i<WORKERSIZE; i++){
        pthread_create(&worker_threads[i], NULL, wordChecker, NULL);
    }
    pthread_create(&log_thread, NULL, logfile, NULL);    
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
        pthread_mutex_lock(&cmutex);
        if(csize==250){
            pthread_cond_wait(&cnotFull, &cmutex);
  
        }
        cbuffer[chead]=new_socket;
        csize++;
        if(chead<249){
            chead++;
            
        }
        else{
            chead=0;
        }
        pthread_mutex_unlock(&cmutex);
        pthread_cond_signal(&cnotEmpty);
    }
       return 0;
}

void *wordChecker(void* t){
    pthread_mutex_lock(&cmutex);
    if(csize==0){
        pthread_cond_wait(&cnotEmpty, &cmutex);

    }
    
    int socket=cbuffer[ctail];
    csize--;
    if(ctail<249){
        ctail++;
    }
    else{
        chead=0;
    }
    pthread_mutex_unlock(&cmutex);
    pthread_cond_signal(&cnotFull);
    while(1){
        char *words; //malloc on words
        int Maxwordsize=20;
        while(recv(socket, words, Maxwordsize, 0)){
            //comparison to check between words in the dictionary file and words
            //free on words at end of inner while loop
            //putting into the logfile;
        }
    }
    
    
}
void *logfile(void* i){
    //taking out of logfile
}
