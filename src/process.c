#include "main.h"


int
process_callback(jack_nframes_t nframes, void *arg)
{
    static jack_nframes_t idx;
    client_t *d = (client_t*) arg;

    float *restrict in1  = (float*) jack_port_get_buffer(d->inport1, nframes);
    float *restrict in2  = (float*) jack_port_get_buffer(d->inport2, nframes);
    float *restrict out1 = (float*) jack_port_get_buffer(d->outport1, nframes);
    float *restrict out2 = (float*) jack_port_get_buffer(d->outport2, nframes);

    jack_nframes_t    i;

    for (i=0; i<nframes; i++) {
        *(out1 + i) = *(in2 + i);
        *(out2 + i) = *(in1 + i);
        idx++;
    }

    if (idx >= d->sr)
        idx -= d->sr;

    return 0;
}
