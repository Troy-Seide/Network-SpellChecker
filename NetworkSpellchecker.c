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
//command line arguments
//readme 
// printing to logfile word not found
//run on multiple servers

#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#define WORKERSIZE 6

/*
 * 
 */
pthread_cond_t cnotFull, lnotFull, cnotEmpty, lnotEmpty;
int cbuffer[250];
char *lbuffer[512];
pthread_t worker_threads[WORKERSIZE]; 
pthread_t log_thread;
pthread_mutex_t cmutex;
pthread_mutex_t lmutex;
const int size=200000;
int csize=0;
int lsize=0;
char *dictionwords[200000];
void *wordChecker(void* t);
void *logfile(void* logWords);
int chead=0,ctail=0, lhead=0, ltail=0;
int new_socket;
int main(int argc, char** argv) {
    pthread_cond_init(&cnotFull, NULL);
    pthread_cond_init(&lnotFull, NULL);
    pthread_cond_init(&cnotEmpty, NULL);
    pthread_cond_init(&lnotEmpty, NULL);
    int length=0;
    FILE *dictwords;
    char *words;
    dictwords=fopen("dictionary.txt","r");
    
    if(dictwords!=EOF){
        for(int i=0; i<size; i++){
	
	dictionwords[i]=calloc(20, 1);
            fgets(dictionwords[i], 20, dictwords);
	//printf("%s", dictionwords[i]);
        }
    }
  
    for(int i=0; i<WORKERSIZE; i++){
        pthread_create(&worker_threads[i], NULL, wordChecker, NULL);
    }
    pthread_create(&log_thread, NULL, logfile, NULL);    
    int portNumber=8888;
    int socket_desc, c;
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
//puts("new");

        //malloc on words
        
        int Maxwordsize=20;
char *message=calloc(Maxwordsize+10, 1);
        char *words=calloc(Maxwordsize, 1);
        while(recv(socket, words, Maxwordsize, 0)){
//puts("other");
            strcpy(message, words);
            bool found=false;
            //comparison to check between words in the dictionary file and words
//printf("%d", strlen("hello"));
//printf("%d", strlen(words));
            for(int i=0; i<size; i++){

                //assuming you` put into the logfile
                if(strncmp(dictionwords[i],words,strlen(words)-2)==0){
                    strcat(message, " found\n");
                    found=true;
//printf("correct");
                    break;
                    
                }
		
                
            }
            if(found==false){
                write(socket, "The word was not found\n", strlen("The word was not found\n"));
               	strcat(message, " not found\n");
                   
            }
            else{
                write(socket, "The word was found\n", strlen("The word was found\n"));
            }
            //free on words at end of inner while loop
            //putting into the logfile;
                pthread_mutex_lock(&lmutex);
                if(lsize==250){
                    pthread_cond_wait(&lnotFull, &lmutex);
                }
                lbuffer[lhead]=message;
                lsize++;
                if(lhead<249){
                    lhead++;
                }
                else{
                    lhead=0;
                }
                pthread_mutex_unlock(&lmutex);
                pthread_cond_signal(&lnotEmpty);
                free(words);
	char* words=calloc(20, 1);

            }
close(socket);
        }
    }
    
void *logfile(void* logWords){
    //taking out of logfile
    pthread_mutex_lock(&lmutex);
    if(lsize==0){
        pthread_cond_wait(&lnotEmpty, &lmutex);

    } 
    char *phrase=lbuffer[ltail];
    FILE* logfile;
    logfile=fopen("logfile.txt","a+");
    fprintf(logfile, "%s",  phrase);
    fclose(logfile);
    lsize--;
    if(ltail<249){
        ltail++;
    }
    else{
        lhead=0;
    }
    pthread_mutex_unlock(&lmutex);
    pthread_cond_signal(&lnotFull);
}
