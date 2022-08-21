// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.

#ifndef FILESTORAGE_DATABASE_H
#define FILESTORAGE_DATABASE_H

#include <mariadb/conncpp.hpp>
#include "constants.hpp"
#include "mongoose.h"


void mariadb_create_db(const char* db_user_password);

void mariadb_drop_db(const char* db_user_password);

std::unique_ptr<sql::Connection> mariadb_connect_to_db(const char* db_user_password);

void mariadb_create_table(sql::Connection* connection);

int mariadb_user_insert(sql::Connection* connection, const char* login, const char* password);

void mariadb_user_update(sql::Connection* connection, const char* login, const char* password);

const char* mariadb_user_get_password(sql::Connection* connection, const char* login);


#endif //FILESTORAGE_DATABASE_H
