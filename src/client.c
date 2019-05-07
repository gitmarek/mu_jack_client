#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "main.h"

client_t _client;

void
client_on_shutdown(void *arg)
{
    fprintf(stderr, "error: jack server closed the connection.\n");
    proc_deactivate();
    exit(1);
}


void
client_termination_handler (int signum)
{
    jack_deactivate(_client.jack_client);
    proc_deactivate();
    /* fprintf(stderr, CLIENT_NAME ": client_termination_handler()\n"); */
    exit(0);
}


client_t*
client_initialize()
{
    client_t *c = &_client;

    c->jack_client = jack_client_open(CLIENT_NAME, c->options, &c->status);
    if (c->jack_client == NULL)  {
        fprintf(stderr, CLIENT_NAME ": jack_client_open() failed\n");
        return NULL;
    }

    c->sr = jack_get_sample_rate(c->jack_client);
    c->buffer_size = jack_get_buffer_size(c->jack_client);

    if (jack_set_process_callback(c->jack_client, proc_callback, c) != 0) {
        fprintf(stderr, CLIENT_NAME ": jack_set_process_callback() failed");
        return NULL;
    }

    jack_on_shutdown(c->jack_client, client_on_shutdown, NULL);


    c->inport1 = jack_port_register(c->jack_client, "in1",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    c->inport2 = jack_port_register(c->jack_client, "in2",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    c->outport1 = jack_port_register(c->jack_client, "out1",
                                     JACK_DEFAULT_AUDIO_TYPE,
                                     JackPortIsOutput, 0);
    c->outport2 = jack_port_register(c->jack_client, "out2",
                                     JACK_DEFAULT_AUDIO_TYPE,
                                     JackPortIsOutput, 0);
    if (! (c->inport1 && c->inport2 &&
           c->outport1 && c->outport2)) {
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
    if (jack_activate(c->jack_client) != 0) {
        fprintf(stderr, CLIENT_NAME ": error: jack_activate() failed\n");
        return NULL;
    }

    return c;
}
