/*

Author: Arun Karki

Student ID: 101219923

Date: Oct 14, 2023

*/

#ifndef SHARED_LIB_H
#define SHARED_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define ECG_SERVER_NAME "./ECG_SERVER_FIFO" // name of server fifo
#define ECG_CLIENT_NAME "./ECG_CLIENT%d_FIFO" // name of the ecg client, there will be multiple copies

#define IMG_SERVER_NAME "./IMG_SERVER_FIFO" // name of server fifo
#define IMG_CLIENT_NAME "./IMG_CLIENT%d_FIFO" // name of the img client, there will be multiple copies

#define PERMISSIONS 0777 // give all permission
#define DATA_SIZE 512 // max string capacity
#define FILE_NAME_SIZE 256 // max name for FIFO file names

#define SERVER_READ_FLAGS (O_RDONLY | O_NONBLOCK) // server can only read and is non-blocked
#define CLIENT_WRITE_FLAGS (O_WRONLY) // client can only write to server

#define SERVER_WRITE_FLAGS (O_WRONLY) // server can only write back and is blocked
#define CLIENT_READ_FLAGS (O_RDONLY) // client can read only and is blocked

#define MESSAGE_TRANSMITTED "[ARBITRARY_DATA]"

// this is the structure data transfered
struct data_content{
    pid_t client_pid; // store client pid to change names and find FIFO files
    char data[DATA_SIZE-1]; // "\0" occupies so -1
    int bytes_transfered; // ill use this to keep track of string bytes sent to check from server or client
};

#endif