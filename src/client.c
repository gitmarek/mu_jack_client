#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "main.h"

client_t _client;

void
client_on_shutdown(void *arg)
{
    fprintf(stderr, "error: jack server closed the connection.\n");
    exit(1);
}


void
client_termination_handler (int signum)
{
    jack_deactivate(_client.jack_client);
    exit(0);
}


client_t*
client_initialize()
{
    client_t *d = &_client;

    d->jack_client = jack_client_open(CLIENT_NAME, d->options, &d->status);
    if (d->jack_client == NULL)  {
        fprintf(stderr, CLIENT_NAME ": jack_client_open() failed\n");
        return NULL;
    }

    d->sr = jack_get_sample_rate(d->jack_client);
    d->buffer_size = jack_get_buffer_size(d->jack_client);

    if (jack_set_process_callback(d->jack_client, process_callback, d) != 0) {
        fprintf(stderr, CLIENT_NAME ": jack_set_process_callback() failed");
        return NULL;
    }

    jack_on_shutdown(d->jack_client, client_on_shutdown, NULL);


    d->inport1 = jack_port_register(d->jack_client, "in1",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    d->inport2 = jack_port_register(d->jack_client, "in2",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    d->outport1 = jack_port_register(d->jack_client, "out1",
                                     JACK_DEFAULT_AUDIO_TYPE,
                                     JackPortIsOutput, 0);
    d->outport2 = jack_port_register(d->jack_client, "out2",
                                     JACK_DEFAULT_AUDIO_TYPE,
                                     JackPortIsOutput, 0);
    if (! (d->inport1 && d->inport2 &&
           d->outport1 && d->outport2)) {
        fprintf(stderr, CLIENT_NAME ": error: cannot register ports\n");
        return NULL;
    }

    if (signal (SIGINT, client_termination_handler) == SIG_IGN)
        signal (SIGINT, SIG_IGN);
    if (signal (SIGHUP, client_termination_handler) == SIG_IGN)
        signal (SIGHUP, SIG_IGN);
    if (signal (SIGTERM, client_termination_handler) == SIG_IGN)
        signal (SIGTERM, SIG_IGN);

    /* fire! */
    if (jack_activate(d->jack_client) != 0) {
        fprintf(stderr, CLIENT_NAME ": error: jack_activate() failed\n");
        return NULL;
    }

    return d;
}
