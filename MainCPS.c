/*

Author: Arun Karki

Student ID: 101219923

Date: Oct 14, 2023

*/

#include "shared_lib.h"
#include <signal.h>

// --[ img stuff below ]--

// makes the image client fifo -> reads + blocked; check in header file
int init_img_fifo(char *img_client_name){

    pid_t pid = getpid();
    int img_client_fd;

    // create fifo first
    if(mkfifo(img_client_name, PERMISSIONS) == -1){
        perror("Error: Unable to create ECG client FIFO.");
        exit(EXIT_FAILURE);
    }

    // open IMAGE client channel
    img_client_fd = open(img_client_name, CLIENT_READ_FLAGS); // file descriptor

    if(img_client_fd == -1){
        perror("Error: Unable to open Image client channel.");
        exit(EXIT_FAILURE);
    }

    printf("[Image Client]: Client FIFO initialized with name: %s...\n", img_client_name);
    return img_client_fd;
}

// open a write channel to server -> check flags in header file
int connect_to_img_server()
{

    int server_fd = open(IMG_SERVER_NAME, CLIENT_WRITE_FLAGS);
    if(server_fd == -1){
        perror("Error: A server does not exist. No connection established.");
        exit(EXIT_FAILURE);
    }

    printf("[Image Client]: Connection to %s established\n", IMG_SERVER_NAME);
    return server_fd;
}

// image client writes to server
void transfer_img_data(int *img_server_fd, struct data_content *transfer_data){

    // add some data to transfer to the server
    strcpy(transfer_data->data, MESSAGE_TRANSMITTED);
    ssize_t bytes_transfered = write(*img_server_fd, transfer_data, sizeof(*transfer_data));

    if(bytes_transfered > 0)
    {
        printf("[Image Client]: Transfering data to %s, bytes transfered: %d\n", IMG_SERVER_NAME, bytes_transfered);
    }
}

// read from the client fifo
int retrieve_img_data(int *img_client_fd, struct data_content *data_retrieved)
{

    // this is how much data was sent from the server, store this to comapare later for signals
    ssize_t bytes_retrieved = read(*img_client_fd, data_retrieved, sizeof(*data_retrieved));

    printf("[Image Client]: Bytes retrieved from %s: %d\n", IMG_SERVER_NAME, data_retrieved->bytes_transfered);

    if(bytes_retrieved > 0){
        printf("[Image Client]: Data retrieved from %s: %s\n", IMG_SERVER_NAME, data_retrieved->data);
    }

    return bytes_retrieved;
}

// --[ ecg stuff below ]--
/*
    NOTE: these functions are copy pasted from the Image functions above; same functionality, check comments for above
*/

// makes the ecg client fifo -> reads + blocked
int init_ecg_fifo(char *ecg_client_name){

    pid_t pid = getpid();
    int ecg_client_fd;

    // create fifo first
    if(mkfifo(ecg_client_name, PERMISSIONS) == -1){
        perror("Error: Unable to create ECG client FIFO.");
        exit(EXIT_FAILURE);
    }

    // open ECG client channel
    ecg_client_fd = open(ecg_client_name, CLIENT_READ_FLAGS); // file descriptor

    if(ecg_client_fd == -1){
        perror("Error: Unable to open ECG client channel.");
        exit(EXIT_FAILURE);
    }

    printf("[ECG Client]: Client FIFO initialized with name: %s...\n", ecg_client_name);
    return ecg_client_fd;
}

// open a write channel to server
int connect_to_ecg_server(){

    int server_fd = open(ECG_SERVER_NAME, CLIENT_WRITE_FLAGS);
    if(server_fd == -1){
        perror("Error: A server does not exist. No connection established.");
        exit(EXIT_FAILURE);
    }

    printf("[ECG Client]: Connection to %s established\n", ECG_SERVER_NAME);
    return server_fd;
}

// ecg client writes to server
void transfer_ecg_data(int *ecg_server_fd, struct data_content *transfer_data){

    strcpy(transfer_data->data, MESSAGE_TRANSMITTED);
    ssize_t bytes_transfered = write(*ecg_server_fd, transfer_data, sizeof(*transfer_data));

    if(bytes_transfered > 0){
        printf("[ECG Client]: Transfering data to %s, bytes transfered: %d\n", ECG_SERVER_NAME, bytes_transfered);
    }
}

// read from the client fifo
int retrieve_ecg_data(int *ecg_client_fd, struct data_content *data_retrieved){

    ssize_t bytes_retrieved = read(*ecg_client_fd, data_retrieved, sizeof(*data_retrieved));

    printf("[ECG Client]: Bytes retrieved from %s: %d\n", ECG_SERVER_NAME, data_retrieved->bytes_transfered);

    if(bytes_retrieved > 0){
        printf("[ECG Client]: Data retrieved from %s: %s\n", ECG_SERVER_NAME, data_retrieved->data);
    }

    return bytes_retrieved;
}

// call this when a process is about to terminate -> signal handler
void termination_notice(int sig, siginfo_t *info, void *ucontext_t){

    if(sig == SIGALRM){
        printf("[SYSTEM]: Received SIGALRM signal. Bytes sent and received did not match.\n");
    }
    else if(sig == SIGUSR1){
        printf("[SYSTEM]: Received SIGUSR1 signal. Bytes sent and received matched.\n");
    }
}

int main(){

    pid_t ecg_client_pid, img_client_pid;
    int ecg_client_fd, img_client_fd, ecg_server_fd, img_server_fd;
    struct sigaction sig_act;

    // client fifo names
    char ecg_client_name[FILE_NAME_SIZE];
    char img_client_name[FILE_NAME_SIZE];

    // fork client fifo
    ecg_client_pid = fork();

    switch(ecg_client_pid){

    // ecg failure case
    case -1:
        perror("Error: Unable to fork ECG client process.");
        exit(EXIT_FAILURE);

    // ecg client created
    case 0:

        // display
        printf("-------------------------------");
        printf("\nMedical CPS System Simulation\n");
        printf("-------------------------------\n\n");
        printf("[SYSTEM]: ECG Client online...\n\n");

        // give ecg fifo its name
        sprintf(ecg_client_name, ECG_CLIENT_NAME, getpid());

        // connect to shared ecg server
        ecg_server_fd = connect_to_ecg_server();
        struct data_content transfer_data;
        transfer_data.client_pid = getpid();

        // write data on the serve fifo
        transfer_ecg_data(&ecg_server_fd, &transfer_data);

        // create unique client fifo to read data
        ecg_client_fd = init_ecg_fifo(&ecg_client_name);

        // receive data from the server
        int bytes_read = retrieve_ecg_data(&ecg_client_fd, &transfer_data);

        // clean up...
        close(ecg_client_fd);
        close(ecg_server_fd);
        unlink(ecg_client_name);

        printf("[ECG Client]: Transmission complete. Terminating process...\n");

        // compare if bytes_read is equal to bytes sent by the server
        // signaling time -> child sends signal to parent process
        if(bytes_read == transfer_data.bytes_transfered){
            kill(getppid(), SIGUSR1);
            exit(EXIT_SUCCESS);
        }
        else{
            // trigger an alarm
            raise(SIGALRM);
            exit(EXIT_FAILURE);
        }
        break;

    // still in the parent process so fork the next child (img process)
    default:

        // fork image client process
        img_client_pid = fork();

        switch(img_client_pid){

        // img failure case
        case -1:
            perror("Error: Unable to fork Image client process.");
            exit(EXIT_FAILURE);

        // img client created
        case 0:

            // display
            printf("-------------------------------");
            printf("\nMedical CPS System Simulation\n");
            printf("-------------------------------\n\n");
            printf("[SYSTEM]: Clients online...\n\n");

            // give img fifo its name
            sprintf(img_client_name, IMG_CLIENT_NAME, getpid());

            // connect to shared img server
            img_server_fd = connect_to_img_server();
            struct data_content transfer_data;
            transfer_data.client_pid = getpid();

            // write data on the serve fifo
            transfer_img_data(&img_server_fd, &transfer_data);

            // create unique client fifo to read data
            img_client_fd = init_img_fifo(&img_client_name);

            // receive data from the server
            int bytes_read = retrieve_img_data(&img_client_fd, &transfer_data);

            // clean up...
            close(img_client_fd);
            close(img_server_fd);
            unlink(img_client_name);

            printf("[Image Client]: Transmission complete. Terminating process...\n");

            // compare if bytes_read is equal to bytes sent by the server
            // signaling time -> child sends signal to parent process
            if(bytes_read == transfer_data.bytes_transfered){
                kill(getppid(), SIGUSR1);
                exit(EXIT_SUCCESS);
            }
            else{
                // trigger an alarm
                raise(SIGALRM);
                exit(EXIT_FAILURE);
            }
            break;

        // still in parent process; do program termination and signal handling
        default:

            // handle signal
            sig_act.sa_flags = 0;
            sig_act.sa_handler = termination_notice;
            sigaction(SIGUSR1, &sig_act, 0);
            sigaction(SIGALRM, &sig_act, 0);

            pause(); // wait for ecg signal
            pause(); // wait for img signal

            printf("[SYSTEM]: Clean-up complete...\n$$$$$\n");
            exit(EXIT_SUCCESS);
        }
        return 0;
    }
}