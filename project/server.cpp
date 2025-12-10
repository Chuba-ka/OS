#include "common.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <pthread.h>
#include <sstream>
#include <signal.h>

size_t SHM_SIZE = sizeof(SharedData);

static SharedData *data = nullptr;
static int shm_fd = -1;
static volatile sig_atomic_t stop_requested = 0;

void signal_handler(int)
{
    stop_requested = 1;
}

void init_shared_memory(bool create_new)
{
    if (create_new)
    {
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
        if (shm_fd < 0)
        {
            perror("shm_open create");
            exit(1);
        }
        if (ftruncate(shm_fd, SHM_SIZE) == -1)
        {
            perror("ftruncate");
            exit(1);
        }
    }
    else
    {
        shm_fd = shm_open(SHM_NAME, O_RDWR, 0600);
        if (shm_fd < 0)
        {
            perror("shm_open open");
            exit(1);
        }
    }

    void *ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    data = reinterpret_cast<SharedData *>(ptr);

    if (create_new)
    {
        memset(data, 0, SHM_SIZE);
        pthread_mutexattr_t mattr;
        pthread_condattr_t cattr;
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&data->mutex, &mattr);
        pthread_mutexattr_destroy(&mattr);

        pthread_condattr_init(&cattr);
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&data->cond, &cattr);
        pthread_condattr_destroy(&cattr);

        data->msg_write_index = 0;
        data->server_stopping = 0;
    }
}

void append_history(const Message &m)
{
    std::ofstream ofs("history.txt", std::ios::app);
    if (!ofs)
        return;
    std::time_t t = (time_t)m.timestamp;
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    ofs << "[" << buf << "] " << m.sender << " -> " << m.recipient << ": " << m.text << "\n";
}

void *monitor_and_save(void *)
{
    while (!stop_requested)
    {
        pthread_mutex_lock(&data->mutex);
        pthread_cond_wait(&data->cond, &data->mutex);

        if (stop_requested || data->server_stopping)
        {
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        for (int i = 0; i < (int)MAX_MESSAGES; ++i)
        {
            if (data->messages[i].timestamp != 0 && data->messages[i].saved == 0)
            {
                append_history(data->messages[i]);
                data->messages[i].saved = 1;
            }
        }
        pthread_mutex_unlock(&data->mutex);
    }
    return nullptr;
}

void cli_loop()
{
    std::string cmd;
    while (!stop_requested)
    {
        std::cout << "server> ";
        if (!std::getline(std::cin, cmd))
        {
            stop_requested = 1;
            break;
        }
        if (stop_requested)
            break;
        if (cmd == "quit" || cmd == "exit")
        {
            break;
        }
        if (cmd == "list_clients")
        {
            pthread_mutex_lock(&data->mutex);
            std::cout << "Clients:\n";
            for (size_t i = 0; i < MAX_CLIENTS; ++i)
            {
                if (data->clients[i][0] != '\0')
                {
                    std::cout << "- " << data->clients[i] << "\n";
                }
            }
            pthread_mutex_unlock(&data->mutex);
        }
        else if (cmd.rfind("search ", 0) == 0)
        {
            std::istringstream iss(cmd);
            std::string tok, a, b, keyword;
            iss >> tok >> a >> b;
            std::getline(iss, keyword);
            if (!keyword.empty() && keyword[0] == ' ')
                keyword.erase(0, 1);
            std::ifstream ifs("history.txt");
            if (!ifs)
            {
                std::cout << "No history file.\n";
                continue;
            }
            std::string line;
            while (std::getline(ifs, line))
            {
                bool users_ok = (line.find(" " + a + " -> " + b + ":") != std::string::npos) || (line.find(" " + b + " -> " + a + ":") != std::string::npos);
                bool keyword_ok = keyword.empty() || (line.find(keyword) != std::string::npos);
                if (users_ok && keyword_ok)
                {
                    std::cout << line << "\n";
                }
            }
        }
        else if (cmd == "help")
        {
            std::cout << "Commands:\n  list_clients\n  search <user1> <user2> [keyword]\n  exit\n";
        }
        else
        {
            std::cout << "Unknown command. Type help.\n";
        }
    }
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);

    std::cout << "Starting server...\n";
    bool created = false;
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0600);
    if (shm_fd < 0)
    {
        created = true;
    }
    else
    {
        close(shm_fd);
    }
    init_shared_memory(created);

    pthread_t monitor_thread;
    if (pthread_create(&monitor_thread, nullptr, monitor_and_save, nullptr) != 0)
    {
        perror("pthread_create");
        return 1;
    }

    cli_loop();

    stop_requested = 1;

    pthread_mutex_lock(&data->mutex);
    data->server_stopping = 1;
    pthread_cond_broadcast(&data->cond);
    pthread_mutex_unlock(&data->mutex);

    pthread_join(monitor_thread, nullptr);

    munmap(data, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    std::cout << "Server shutting down...\n";
    return 0;
}