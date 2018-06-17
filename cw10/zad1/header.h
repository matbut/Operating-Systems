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
	int fd;
	char* name;
	int active;
} client;

typedef struct message_info {
    int msg_num;
    message_t msg_type;
    int msg_size;
} message_info;

#endif

