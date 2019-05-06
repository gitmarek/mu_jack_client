#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include <jack/jack.h>

#define CLIENT_NAME "mu"


jack_client_t    *client;
jack_options_t    options = JackNullOption;
jack_status_t     status;
jack_nframes_t    sr; /* sample rate */
jack_port_t      *inport1,*inport2;
jack_port_t      *outport1,*outport2;

struct proc_dat {
    jack_nframes_t idx; /* generic index */
    void *data;
};

int  process_callback(jack_nframes_t nframes, void *arg);
void on_shutdown(void *arg);
void termination_handler (int signum);


int
main(int argc, char **argv)
{

    struct proc_dat dat;

    /* initialize client */
    client = jack_client_open(CLIENT_NAME, options, &status);
    if (client == NULL)  {
        printf("jack_client_open failed.\n");
        exit(1);
    }

    sr = jack_get_sample_rate(client);

    printf("Sucessfully opened a new client: " CLIENT_NAME "\n"
           "Sample rate: %d\n", sr);

    if (jack_set_process_callback(client, process_callback, &dat) != 0) {
        printf("jack_set_process_callback() failed");
        exit(1);
    }

    jack_on_shutdown(client, on_shutdown, NULL);


    inport1 = jack_port_register(client, "in1",
                                 JACK_DEFAULT_AUDIO_TYPE,
                                 JackPortIsInput, 0);
    inport2 = jack_port_register(client, "in2",
                                 JACK_DEFAULT_AUDIO_TYPE,
                                 JackPortIsInput, 0);
    outport1 = jack_port_register(client, "out1",
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsOutput, 0);
    outport2 = jack_port_register(client, "out2",
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsOutput, 0);
    if (! (inport1 && inport2 && outport1 && outport2)) {
        printf("error: cannot register ports.\n");
        exit(1);
    }


    if (signal (SIGINT, termination_handler) == SIG_IGN)
        signal (SIGINT, SIG_IGN);
    if (signal (SIGHUP, termination_handler) == SIG_IGN)
        signal (SIGHUP, SIG_IGN);
    if (signal (SIGTERM, termination_handler) == SIG_IGN)
        signal (SIGTERM, SIG_IGN);


    /* fire! */
    if (jack_activate(client) != 0) {
        printf("error: jack_activate() failed.\n");
        exit(1);
    }

    sleep(-1);

    return 0;
}


int
process_callback(jack_nframes_t nframes, void *arg)
{
    float *restrict in1  = (float*) jack_port_get_buffer(inport1, nframes);
    float *restrict in2  = (float*) jack_port_get_buffer(inport2, nframes);
    float *restrict out1 = (float*) jack_port_get_buffer(outport1, nframes);
    float *restrict out2 = (float*) jack_port_get_buffer(outport2, nframes);

    struct proc_dat *dat = (struct proc_dat*) arg;
    jack_nframes_t    i;

    for (i=0; i<nframes; i++) {
        *(out1 + i) = *(in2 + i);
        *(out2 + i) = *(in1 + i);
        dat->idx++;
    }

    if (dat->idx >= sr)
        dat->idx -= sr;

    return 0;
}


void
on_shutdown(void *arg)
{
    printf("error: jack server closed the connection.\n");
    exit(1);
}


void
termination_handler (int signum)
{
    jack_deactivate(client);
    exit(0);
}
