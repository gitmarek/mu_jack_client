#ifndef _MAIN_H_
#define _MAIN_H_

#include <jack/jack.h>

/**
 * client interface (client.c)
 *
 */
#define CLIENT_NAME "mu"
#define CLIENT_JACK_OPTIONS JackNullOption

typedef struct {
    jack_client_t    *jack_client;
    jack_port_t      *inport1, *inport2;
    jack_port_t      *outport1, *outport2;
    jack_options_t    options;
    jack_status_t     status;
    jack_nframes_t    sr; /* sample rate */
    jack_nframes_t    buffer_size;

    void *data;
} client_t;

client_t* client_initialize(void);


/**
 * proc interface (proc.c)
 *
 */
#define PROC_NUMCHANNELS 4

/* The size of internal buffers does not increase latency.  */
#define PROC_BUFSIZE 65536 /* per channel */

typedef struct proc proc_t;

proc_t* proc_initialize(client_t *client);
int proc_deactivate(void);
int proc_callback(jack_nframes_t nframes, void *arg);


#endif /* _MAIN_H_ */
