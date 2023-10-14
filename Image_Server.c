/*

Author: Arun Karki

Student ID: 101219923

Date: Oct 14, 2023

*/

#include "shared_lib.h"

// create server READ img FIFO
int init_server()
{
    int server_fd;
    // create fifo first
    if (mkfifo(IMG_SERVER_NAME, PERMISSIONS) == -1){
        perror("Error: Unable to create server FIFO.");
        exit(EXIT_FAILURE);
    }

    // open server channel

    // SERVER_READ_FLAGS (O_RDONLY | O_NONBLOCK) // server can only read and is non-blocked
    server_fd = open(IMG_SERVER_NAME, SERVER_READ_FLAGS); // file descriptor
    if (server_fd == -1){
        perror("Error: Unable to open server channel.");
        exit(EXIT_FAILURE);
    }

    printf("[Image Server]: Server FIFO initialized with name: %s...\n", IMG_SERVER_NAME);
    return server_fd;
}

// read from the server fifo
int retrieve_data(int *server_fd, struct data_content *data_retrieved, char* client_name)
{
    ssize_t bytes_retrieved = read(*server_fd, data_retrieved, sizeof(*data_retrieved));

    if (bytes_retrieved > 0){
        // store the bytes retrieved during the transfer
        data_retrieved->bytes_transfered = bytes_retrieved;

        // store the client's fifo name to write back
        sprintf(client_name, IMG_CLIENT_NAME, data_retrieved->client_pid); // this name will be used to identify from the server
        printf("[Image Server]: Bytes retrieved from %s: %d\n", client_name, data_retrieved->bytes_transfered);
        printf("[Image Server]: Data retrieved from %s: %s\n", client_name, data_retrieved->data);

        return 1;
    }

    // error occured in reading -> no bytes available or failed to read
    return 0;
}

// connect to the client im about to write to -> return the client_fd basically
int connect_to_client(char* client_name){
  
    // open channel using client name previously stored in struct
    int client_fd = open(client_name, SERVER_WRITE_FLAGS);

    if(client_fd == -1){
        perror("Error: This client FIFO does not exist. No connection established.");
        exit(EXIT_FAILURE);
    }

    printf("[Image Server]: Connection to %s established\n", client_name);
    return client_fd;
}

// write to client fifo
void transfer_data(char* client_name, struct data_content* transfer_data){

    // open channel to client first
    int client_pid = connect_to_client(client_name);
    
    // update the transfer data
    strcpy(transfer_data->data, MESSAGE_TRANSMITTED);

    ssize_t bytes_transfered = write(client_pid, transfer_data, sizeof(*transfer_data));

    if(bytes_transfered > 0){
        printf("[Image Server]: Transfering data to %s, bytes transfered: %d\n", client_name, bytes_transfered);
        printf("[Image Server]: Data successfully transfered.\n");
    }
}

int main(){

    // display
    printf("-------------------------------");
    printf("\nMedical CPS System Simulation\n");
    printf("-------------------------------\n\n");
    printf("[SYSTEM]: Image Server online...\n\n");

    int server_fd;
    char client_name[256];
    struct data_content data_retrieved;

    // initlaize the server FIFO and open channel; see function
    server_fd = init_server();

    while (1){
        if(retrieve_data(&server_fd, &data_retrieved, &client_name) == 1){ // client_name is written here now, return 1 means success
            // little delay to avoid potential race conditions
            usleep(50);
            // send data back to client
            transfer_data(&client_name, &data_retrieved);
        }
    }

    return 0;
}
