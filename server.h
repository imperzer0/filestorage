// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#ifndef FILESTORAGE_SERVER_H
#define FILESTORAGE_SERVER_H

#include "mongoose.h"


extern const char* address;
extern const char* log_level;
extern const char* directory;
extern const char* pattern;
extern int hexdump;

extern void server_initialize(const char* database_user_password);

extern void server_run(const char* database_user_password);

extern void server_destroy_database(const char* database_user_password);

#endif //FILESTORAGE_SERVER_H
