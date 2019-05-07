#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "main.h"


int
main(int argc, char **argv)
{
    client_t *client;
    proc_t *proc;

    client = client_initialize();
    if ( client == NULL ) {
        fprintf(stderr, CLIENT_NAME ": client_initialize() returned NULL\n");
        exit(1);
    }
    printf(CLIENT_NAME ": connected to jack server\n"
           "sample rate: %d\n"
           "buffer size: %d\n", client->sr, client->buffer_size);


    proc = proc_initialize(client);
    if ( proc == NULL ) {
        fprintf(stderr, CLIENT_NAME ": proc_initialize() returned NULL\n");
        exit(1);
    }

    /* enter interactive loop? */
    sleep(-1);

    return 0;
}


