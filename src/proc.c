#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "main.h"


struct proc {
    uint32_t    idx;
    uint32_t    bufsize;
    float*      bufin;
    float*      bufout;
};


proc_t   _proc;


proc_t*
proc_initialize(client_t *client)
{
    proc_t* p = &_proc;

    p->idx = 0;
    p->bufsize = PROC_NUMCHANNELS * PROC_BUFSIZE;

    p->bufin  = (float*) malloc(sizeof(float) * p->bufsize);
    p->bufout = (float*) malloc(sizeof(float) * p->bufsize);
    if (! (p->bufin && p->bufout) ) {
        fprintf(stderr, CLIENT_NAME ": malloc failed with NULL\n");
        return NULL;
    }

    for (uint32_t i=0; i<p->bufsize; i++) {
        *(p->bufin  + i) = 0.0;
        *(p->bufout + i) = 0.0;
    }

    return p;
}


int
proc_deactivate(void)
{
    free(_proc.bufin);
    free(_proc.bufout);

    /* fprintf(stderr, CLIENT_NAME ": proc_deactivate()\n"); */

    return 0;
}


void
proc_procbufs_control(uint32_t idx)
{

    return;
}


void
proc_procbufs_audio(uint32_t idx)
{
    for (int i=0; i<PROC_NUMCHANNELS; i++) {
        *(_proc.bufout + idx + i) = *(_proc.bufin + idx + i);
    }

    return;
}


int
proc_callback(jack_nframes_t nframes, void *arg)
{
    static jack_nframes_t gidx;
    client_t *client = (client_t*) arg;

    float *restrict in1  = (float*) jack_port_get_buffer(client->inport1, nframes);
    float *restrict in2  = (float*) jack_port_get_buffer(client->inport2, nframes);
    float *restrict out1 = (float*) jack_port_get_buffer(client->outport1, nframes);
    float *restrict out2 = (float*) jack_port_get_buffer(client->outport2, nframes);


    proc_procbufs_control(_proc.idx);

    /* to/from jack ports */
    for (jack_nframes_t i=0; i<nframes; i++) {

        /* write first 2 channels back to jack ports */
        *(_proc.bufin + _proc.idx)   = *(in1  + i);
        *(_proc.bufin + _proc.idx+1) = *(in2  + i);

        proc_procbufs_audio(_proc.idx);

        *(out1 + i) = *(_proc.bufout + _proc.idx);
        *(out2 + i) = *(_proc.bufout + _proc.idx+1);

        if (_proc.idx + PROC_NUMCHANNELS >= _proc.bufsize)
            _proc.idx = 0;
        else
            _proc.idx += PROC_NUMCHANNELS;

        gidx++;
    }

    return 0;
}
