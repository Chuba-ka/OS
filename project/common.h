#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <cstdint>

const char *SHM_NAME = "/simple_chat_shm_v1";
const size_t MAX_CLIENTS = 16;
const size_t LOGIN_LEN = 32;
const size_t MAX_MESSAGES = 1024;
const size_t MESSAGE_TEXT_LEN = 256;

struct Message
{
    char sender[LOGIN_LEN];
    char recipient[LOGIN_LEN];
    char text[MESSAGE_TEXT_LEN];
    uint64_t timestamp;
    int saved;
    int delivered;
};

struct SharedData
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char clients[MAX_CLIENTS][LOGIN_LEN];
    Message messages[MAX_MESSAGES];
    int msg_write_index;
    int server_stopping;
};

#endif