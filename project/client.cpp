#include "common.h"
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
static SharedData *data = nullptr;
static int shm_fd = -1;
static volatile sig_atomic_t stop_requested = 0;

void signal_handler(int)
{
    stop_requested = 1;
}

void connect_shared_memory()
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

bool register_login(const std::string &login)
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
        if (std::string(data->clients[i]) == login)
        {
            pthread_mutex_unlock(&data->mutex);
            return true;
        }
    }
    for (size_t i = 0; i < MAX_CLIENTS; ++i)
    {
        if (data->clients[i][0] == '\0')
        {
            strncpy(data->clients[i], login.c_str(), LOGIN_LEN - 1);
            data->clients[i][LOGIN_LEN - 1] = '\0';
            pthread_mutex_unlock(&data->mutex);
            return true;
        }
    }
    pthread_mutex_unlock(&data->mutex);
    return false;
}

struct ReceiverArg
{
    char login[LOGIN_LEN];
};

void *receiver_loop(void *arg)
{
    ReceiverArg *rarg = (ReceiverArg *)arg;
    std::string login(rarg->login);
    free(rarg);

    while (!stop_requested)
    {
        pthread_mutex_lock(&data->mutex);
        pthread_cond_wait(&data->cond, &data->mutex);

        if (data->server_stopping)
        {
            pthread_mutex_unlock(&data->mutex);
            std::cout << "\nServer is shutting down. Exiting client receiver.\n";
            break;
        }

        for (int i = 0; i < (int)MAX_MESSAGES; ++i)
        {
            if (data->messages[i].timestamp != 0 && data->messages[i].delivered == 0)
            {
                if (login == std::string(data->messages[i].recipient))
                {
                    std::time_t t = (time_t)data->messages[i].timestamp;
                    char buf[64];
                    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
                    std::cout << "\n[" << buf << "] " << data->messages[i].sender << ": " << data->messages[i].text << "\n> ";
                    data->messages[i].delivered = 1;
                }
            }
        }
        pthread_mutex_unlock(&data->mutex);
    }
    return nullptr;
}

void send_message(const std::string &from, const std::string &to, const std::string &text)
{
    pthread_mutex_lock(&data->mutex);
    if (data->server_stopping)
    {
        pthread_mutex_unlock(&data->mutex);
        std::cout << "Server is shutting down; cannot send message.\n";
        return;
    }
    int idx = data->msg_write_index;
    strncpy(data->messages[idx].sender, from.c_str(), LOGIN_LEN - 1);
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

int main()
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);

    std::cout << "Client starting. Connect to server's shared memory...\n";
    connect_shared_memory();
    std::string login;
    std::cout << "Enter login: ";
    std::getline(std::cin, login);
    if (login.empty())
    {
        std::cerr << "Login cannot be empty.\n";
        return 1;
    }
    if (!register_login(login))
    {
        std::cerr << "Cannot register login. Exiting.\n";
        return 1;
    }
    std::cout << "Registered as " << login << "\n";

    ReceiverArg *rarg = (ReceiverArg *)malloc(sizeof(ReceiverArg));
    strncpy(rarg->login, login.c_str(), LOGIN_LEN - 1);
    rarg->login[LOGIN_LEN - 1] = '\0';

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, nullptr, receiver_loop, rarg) != 0)
    {
        perror("pthread_create");
        free(rarg);
        return 1;
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
            size_t pos1 = line.find(' ', 5);
            if (pos1 == std::string::npos)
            {
                std::cout << "Usage: send <recipient> <message>\n";
            }
            else
            {
                std::string recipient = line.substr(5, pos1 - 5);
                std::string text = line.substr(pos1 + 1);
                send_message(login, recipient, text);
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
    return 0;
}