// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include "config_script.h"
#include "constants.hpp"


#ifndef EXTERNAL_TEST

#include "resources.hpp"
#include "mongoose.h"


#endif


#include <cstdio>
#include <sys/stat.h>
#include <string>

#include <lua.hpp>


#define LUA_ASSERT(expr, ret_expr) if (assert(expr)) return ret_expr


static lua_State* config_script = luaL_newstate();

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


void overwrite_config()
{
	#ifndef EXTERNAL_TEST
	FILE* fp = fopen(CONFIG_SCRIPT_FILE, "wb");
	fwrite(filestorageconf_lua, sizeof filestorageconf_lua[0], LEN(filestorageconf_lua), fp);
	fclose(fp);
	#endif
}

void init_config_script()
{
	// init lua state
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
	
	lua_newtable(config_script);
	lua_setglobal(config_script, "Extensions");
	
	luaL_dostring(config_script, R"===(
table.tostring = function(table)
    local next = next
    if next(table) == nil then
       return "{ }"
    end
    result = "{ "
    for k, v in pairs(table) do
        result = result .. "'" .. tostring(k) .. "': '" .. tostring(v) .. "', "
    end
    result = string.sub(result, 1, string.len(result) - 2)
    return result .. " }"
end
)===");
	
	LUA_ASSERT(luaL_dofile(config_script, CONFIG_SCRIPT_FILE),);
}

void close_config_script() { lua_close(config_script); }

extension_response call_lua_extension(const extension_data& data)
{
	lua_getglobal(config_script, "Extensions");
	
	lua_getfield(config_script, -1, data.name.c_str());
	
	lua_createtable(config_script, 0, 3);
	
	lua_pushstring(config_script, data.login.c_str());
	lua_setfield(config_script, -2, "login");
	
	lua_pushstring(config_script, data.password.c_str());
	lua_setfield(config_script, -2, "password");
	
	lua_pushstring(config_script, data.name.c_str());
	lua_setfield(config_script, -2, "name");
	
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
