// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include "config_script.h"
#include "constants.hpp"
#include "resources.hpp"


#ifndef EXTERNAL_TEST

#include "lua_config_libfunctions.h"
#include "mongoose.h"


#endif


#include <cstdio>
#include <sys/stat.h>
#include <string>
#include <pthread.h>

#include <lua.hpp>
#include <unistd.h>


#undef LUA_ASSERT
#define LUA_ASSERT(expr, ret_expr) if (assert(expr)) return ret_expr


static lua_State* config_script = nullptr;
static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

#ifndef EXTERNAL_TEST

inline static void print_lua_error() { MG_ERROR(("[Lua] reported an error: %s", lua_tostring(config_script, -1))); }

#else

inline static void print_lua_error() { fprintf(stderr, "[Lua] reported an error: %s", lua_tostring(config_script, -1)); }

#endif

inline static bool assert(int r)
{
	if (r != LUA_OK)
	{
		print_lua_error();
		return true;
	}
	return false;
}


inline static void overwrite_config()
{
	#ifndef EXTERNAL_TEST
	FILE* fp = fopen(CONFIG_SCRIPT_FILE, "wb");
	fwrite(filestorageconf_lua, sizeof filestorageconf_lua[0], LEN(filestorageconf_lua), fp);
	fclose(fp);
	#endif
}

void* config_initialization_thread_th(void*)
{
	while (true)
	{
		usleep(10'000'000); // every 10 seconds
		refresh_config_script();
	}
}

void config_initialization_thread()
{
	init_config_script();
	
	pthread_t thread;
	pthread_create(&thread, nullptr, &config_initialization_thread_th, nullptr);
	pthread_detach(thread);
}


class mutex_locker
{
public:
	mutex_locker() { pthread_mutex_lock(&mutex); }
	
	~mutex_locker() { pthread_mutex_unlock(&mutex); }
};

void init_config_script()
{
	mutex_locker locker;
	
	close_config_script();
	
	MG_INFO(("Initializing lua configuration script context..."));
	
	config_script = luaL_newstate();
	luaL_openlibs(config_script);
	
	struct stat st{ };
	if (stat(CONFIG_SCRIPT_FILE, &st) < 0)
	{
		overwrite_config();
		return;
	}
	
	if (!S_ISREG(st.st_mode) || !st.st_size)
	{
		system((std::string("rm -rf ") + CONFIG_SCRIPT_FILE).c_str());
		overwrite_config();
		return;
	}
	
	refresh_config_script();
}

void refresh_config_script()
{
	mutex_locker locker;
	
	#ifndef EXTERNAL_TEST
	MG_INFO(("Refreshing Lua configuration script context..."));
	#endif
	
	register_lib_functions(config_script);
	
	lua_newtable(config_script);
	lua_setglobal(config_script, "Extensions");
	
	LUA_ASSERT(luaL_dostring(config_script, reinterpret_cast<const char*>(config_lib_lua)),);
	
	LUA_ASSERT(luaL_dofile(config_script, CONFIG_SCRIPT_FILE),);
}

void close_config_script()
{
	mutex_locker locker;
	if (config_script)
	{
		MG_INFO(("Closing Lua configuration script context..."));
		lua_close(config_script);
		config_script = nullptr;
	}
}

extension_response call_lua_extension(const extension_data& data)
{
	mutex_locker locker;
	
	lua_getglobal(config_script, "Extensions");
	
	lua_getfield(config_script, -1, data.name.c_str());
	
	lua_createtable(config_script, 0, 4);
	
	lua_pushstring(config_script, data.login.c_str());
	lua_setfield(config_script, -2, "login");
	
	lua_pushstring(config_script, std::to_string(data.session_cookie).c_str());
	lua_setfield(config_script, -2, "password");
	
	lua_pushstring(config_script, data.name.c_str());
	lua_setfield(config_script, -2, "name");
	
	lua_pushboolean(config_script, data.valid);
	lua_setfield(config_script, -2, "valid");
	
	lua_pushstring(config_script, data.argument.c_str());
	
	LUA_ASSERT(lua_pcall(config_script, 2, 2, 0), { });
	
	extension_response response;
	auto page = lua_tostring(config_script, -1);;
	if (!page)
		print_lua_error();
	else
		response.page = page;
	response.response_code = lua_tointeger(config_script, -2);
	if (!response.response_code)
		print_lua_error();
	
	lua_settop(config_script, 0);
	
	return response;
}

bool http_response_is_null(const extension_response& response) { return response.response_code == 0 && response.page.empty(); }
