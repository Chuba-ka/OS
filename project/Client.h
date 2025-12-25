#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include <string>
#include <pthread.h>
#include <signal.h>

class Client
{
public:
    Client();
    ~Client();

    void connect();
    bool registerLogin(const std::string &login);
    void run();

    static Client *instance;

    volatile sig_atomic_t stop_requested;

private:
    static void *receiver_loop(void *arg);
    void send_message(const std::string &to, const std::string &text);

    SharedData *data;
    int shm_fd;
    std::string login;
    pthread_t recv_thread;
};

void client_signal_handler(int sig);

#endif