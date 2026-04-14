#include "ft_ping.h"

void handle_sigint(int sig) {
    (void)sig;
    g_ping.stop = 1;
}

void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
}
