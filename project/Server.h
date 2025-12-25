#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <pthread.h>
#include <signal.h>

class Server
{
public:
    Server();
    ~Server();
    void run();

    static Server *instance;

    volatile sig_atomic_t stop_requested; 

private:
    static void *monitor_and_save(void *arg);
    void init_shared_memory(bool create_new);
    void append_history(const Message &m);
    void cli_loop();

    SharedData *data;
    int shm_fd;
    pthread_t monitor_thread;
};

void server_signal_handler(int sig);

#endif