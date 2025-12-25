#include "Client.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <signal.h>

size_t SHM_SIZE = sizeof(SharedData);

Client *Client::instance = nullptr;

void client_signal_handler(int sig)
{
    if (Client::instance)
    {
        Client::instance->stop_requested = 1;
    }
}

Client::Client() : data(nullptr), shm_fd(-1), stop_requested(0)
{
    instance = this;
}

Client::~Client()
{
    if (data)
    {
        munmap(data, SHM_SIZE);
    }
    if (shm_fd >= 0)
    {
        close(shm_fd);
    }
}

void Client::connect()
{
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0600);
    if (shm_fd < 0)
    {
        perror("shm_open (client) - make sure server is running");
        exit(1);
    }

    void *ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap (client)");
        exit(1);
    }
    data = reinterpret_cast<SharedData *>(ptr);
}

bool Client::registerLogin(const std::string &l)
{
    pthread_mutex_lock(&data->mutex);
    if (data->server_stopping)
    {
        pthread_mutex_unlock(&data->mutex);
        std::cerr << "Server is shutting down. Cannot register.\n";
        return false;
    }

    for (size_t i = 0; i < MAX_CLIENTS; ++i)
    {
        if (std::string(data->clients[i]) == l)
        {
            pthread_mutex_unlock(&data->mutex);
            login = l;
            return true;
        }
    }

    for (size_t i = 0; i < MAX_CLIENTS; ++i)
    {
        if (data->clients[i][0] == '\0')
        {
            strncpy(data->clients[i], l.c_str(), LOGIN_LEN - 1);
            data->clients[i][LOGIN_LEN - 1] = '\0';
            pthread_mutex_unlock(&data->mutex);
            login = l;
            return true;
        }
    }

    pthread_mutex_unlock(&data->mutex);
    return false;
}

void *Client::receiver_loop(void *arg)
{
    Client *self = static_cast<Client *>(arg);
    while (!self->stop_requested)
    {
        pthread_mutex_lock(&self->data->mutex);
        pthread_cond_wait(&self->data->cond, &self->data->mutex);

        if (self->data->server_stopping)
        {
            pthread_mutex_unlock(&self->data->mutex);
            std::cout << "\nServer is shutting down. Exiting client receiver.\n";
            break;
        }

        for (int i = 0; i < (int)MAX_MESSAGES; ++i)
        {
            if (self->data->messages[i].timestamp != 0 && self->data->messages[i].delivered == 0)
            {
                if (self->login == std::string(self->data->messages[i].recipient))
                {
                    std::time_t t = (time_t)self->data->messages[i].timestamp;
                    char buf[64];
                    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
                    std::cout << "\n[" << buf << "] " << self->data->messages[i].sender
                              << ": " << self->data->messages[i].text << "\n> ";
                    self->data->messages[i].delivered = 1;
                }
            }
        }
        pthread_mutex_unlock(&self->data->mutex);
    }
    return nullptr;
}

void Client::send_message(const std::string &to, const std::string &text)
{
    pthread_mutex_lock(&data->mutex);
    if (data->server_stopping)
    {
        pthread_mutex_unlock(&data->mutex);
        std::cout << "Server is shutting down; cannot send message.\n";
        return;
    }

    int idx = data->msg_write_index;
    strncpy(data->messages[idx].sender, login.c_str(), LOGIN_LEN - 1);
    data->messages[idx].sender[LOGIN_LEN - 1] = '\0';
    strncpy(data->messages[idx].recipient, to.c_str(), LOGIN_LEN - 1);
    data->messages[idx].recipient[LOGIN_LEN - 1] = '\0';
    strncpy(data->messages[idx].text, text.c_str(), MESSAGE_TEXT_LEN - 1);
    data->messages[idx].text[MESSAGE_TEXT_LEN - 1] = '\0';
    data->messages[idx].timestamp = (uint64_t)time(nullptr);
    data->messages[idx].saved = 0;
    data->messages[idx].delivered = 0;

    data->msg_write_index = (idx + 1) % MAX_MESSAGES;
    pthread_cond_broadcast(&data->cond);
    pthread_mutex_unlock(&data->mutex);
}

void Client::run()
{
    if (pthread_create(&recv_thread, nullptr, &Client::receiver_loop, this) != 0)
    {
        perror("pthread_create");
        return;
    }

    std::string line;
    std::cout << "> ";
    while (!stop_requested && std::getline(std::cin, line))
    {
        if (data->server_stopping)
        {
            std::cout << "Server is shutting down. Exiting client.\n";
            break;
        }
        if (stop_requested)
            break;

        if (line == "quit" || line == "exit")
            break;

        if (line.rfind("send ", 0) == 0)
        {
            size_t pos = line.find(' ', 5);
            if (pos == std::string::npos)
            {
                std::cout << "Usage: send <recipient> <message>\n";
            }
            else
            {
                std::string recipient = line.substr(5, pos - 5);
                std::string text = line.substr(pos + 1);
                send_message(recipient, text);
            }
        }
        else if (line == "help")
        {
            std::cout << "Commands:\n  send <recipient> <message>\n  exit\n";
        }
        else
        {
            std::cout << "Unknown command. Type help.\n";
        }
        std::cout << "> ";
    }

    stop_requested = 1;
    pthread_mutex_lock(&data->mutex);
    pthread_cond_broadcast(&data->cond);
    pthread_mutex_unlock(&data->mutex);

    pthread_join(recv_thread, nullptr);
    std::cout << "Client exiting...\n";
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = client_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);

    std::cout << "Client starting. Connect to server's shared memory...\n";

    Client c;
    c.connect();

    std::string login;
    std::cout << "Enter login: ";
    std::getline(std::cin, login);
    if (login.empty())
    {
        std::cerr << "Login cannot be empty.\n";
        return 1;
    }

    if (!c.registerLogin(login))
    {
        std::cerr << "Cannot register login. Exiting.\n";
        return 1;
    }

    std::cout << "Registered as " << login << "\n";
    c.run();

    return 0;
}