// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#ifndef FILESTORAGE_CONFIG_SCRIPT_H
#define FILESTORAGE_CONFIG_SCRIPT_H

#include <string>


typedef struct
{
	int response_code;
	std::string page;
} extension_response;


extern void config_initialization_thread();

extern void init_config_script();

extern void refresh_config_script();

extern void close_config_script();

typedef struct
{
	std::string login;
	uint64_t session_cookie;
	bool valid;
	std::string name;
	std::string argument;
} extension_data;

extern extension_response call_lua_extension(const extension_data& data);

extern bool http_response_is_null(const extension_response& response);

#endif //FILESTORAGE_CONFIG_SCRIPT_H
