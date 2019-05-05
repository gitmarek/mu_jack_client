#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <jack/jack.h>

#define CLIENT_NAME "mu"

#define INPORTS 2
#define OUTPORTS 2


struct proc_dat {
    jack_client_t *client;
    jack_port_t *outport[OUTPORTS], *inport[INPORTS];
    jack_nframes_t sr; /* sample rate */
    jack_nframes_t i;  /* generic index */

    void *data;
};


int
process_callback(jack_nframes_t nframes, void *arg);

void
on_shutdown(void *arg);


int
main(int argc, char **argv) {

    jack_client_t    *client;
    jack_options_t    options = JackNullOption;
    jack_status_t     status;
    jack_nframes_t    sr;
    jack_port_t      *outport[OUTPORTS], *inport[INPORTS];

    struct proc_dat   dat;
    char* name;

    /* initialize client */
    client = jack_client_open(CLIENT_NAME, options, &status);
    if (client == NULL)  {
        printf("jack_client_open failed.\n");
        exit(1);
    }
    dat.client   = client;

    sr = jack_get_sample_rate(client);
    dat.sr       = sr;

    printf("Sucessfully opened a new client: " CLIENT_NAME "\n"
            "Sample rate: %d\n", sr);

    if (jack_set_process_callback(client, process_callback, &dat) != 0) {
        printf("jack_set_process_callback() failed");
        exit(1);
    }

    jack_on_shutdown(client, on_shutdown, NULL);

    /* Register ports */
    for (int i=0; i<INPORTS; i++) {

        name = (char*) malloc(sizeof(char) * 5);
        snprintf(name, 5, "in%0*d", 2, i);

        inport[i] = jack_port_register(client, name,
                JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);


        if (inport[i] == NULL) {
            printf("error: cannot register port.\n");
            exit(1);
        }

        dat.inport[i] = inport[i];
        free(name);
    }

    for (int i=0; i<OUTPORTS; i++) {

        name = (char*) malloc(sizeof(char) * 6);
        snprintf(name, 6, "out%0*d", 2, i);

        outport[i] = jack_port_register(client, name,
                JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

        if (outport[i] == NULL) {
            printf("error: cannot register port.\n");
            exit(1);
        }

        dat.outport[i] = outport[i];
        free(name);
    }


    /* fire! */
    if (jack_activate(client) != 0) {
        printf("error: jack_activate() failed.\n");
        exit(1);
    }

    sleep(-1);

    return 0;
}



void on_shutdown(void *arg) {

    printf("error: jack server closed the connection.\n");
    exit(1);
}



int
process_callback(jack_nframes_t nframes, void *arg) {

    struct proc_dat  *dat;
    float            *out[OUTPORTS];
    float            *in[INPORTS];
    jack_nframes_t    i;


    dat = (struct proc_dat*) arg;

    for (int i=0; i<INPORTS; i++)
        in[i] = (float*) jack_port_get_buffer(dat->inport[i], nframes);
    for (int i=0; i<OUTPORTS; i++)
        out[i] = (float*) jack_port_get_buffer(dat->outport[i], nframes);

    for (i=0; i<nframes; i++) {

        *(out[0] + i) = *(in[1] + i);
        *(out[1] + i) = *(in[0] + i);

        dat->i++;
    }

    if (dat->i >= dat->sr) dat->i -= dat->sr;

    return 0;
}

