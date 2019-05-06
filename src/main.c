#include <stdio.h>
#include <unistd.h>

#include "main.h"


int
main(int argc, char **argv)
{

    client_t *client = client_initialize();

    printf( CLIENT_NAME ": jack connection opened\n"
            "sample rate: %d\n"
            "buffer size: %d\n", client->sr, client->buffer_size);

    /* enter interactive loop? */
    sleep(-1);

    return 0;
}


