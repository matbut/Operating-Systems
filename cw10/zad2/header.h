#ifndef HEADER_H
#define HEADER_H

#define CLIENT_MAX 10
#define CONTENT_MAX 64

typedef enum message_t {
    REGISTER,
    UNREGISTER,
    SERVER_OK,
    SERVER_REJECT,
    PING,
    PONG,
    CALC,
    RESULT,
    SERVER_ERR
} message_t;

#define TO_STRING(type) ( \
    type == REGISTER ? "REGISTER" : ( \
    type == UNREGISTER ? "UNMIRROR" : ( \
    type == SERVER_OK ? "SERVER_OK" : ( \
    type == SERVER_REJECT ? "SERVER_REJECT" : ( \
    type == PING ? "PING" : ( \
    type == PONG ? "PONG" : ( \
    type == CALC ? "CALC" : ( \
    type == RESULT ? "RESULT" : ( \
    type == SERVER_ERR ? "SERVER_ERR" : "UNKNOWN" \
    )))))))))

typedef struct client{
    struct sockaddr *addr;
    int addr_size;
    int sock;
	int id;
	char* name;
	int active;
} client;

typedef struct message {
    int msg_num;
    int id;
    message_t msg_type;
    char content[CONTENT_MAX];
}message;

#endif

