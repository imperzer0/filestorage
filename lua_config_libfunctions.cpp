// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include "lua_config_libfunctions.h"
#include "qrcodegen.cpp"
#include <string>
#include <sstream>


#ifndef EXTERNAL_TEST

#include "mongoose.h"
#include "Gen_QR.h"


#endif


#undef LUA_ASSERT
#define LUA_ASSERT(L, expr, ret_expr) if (Assert(L, expr)) return ret_expr


#ifndef EXTERNAL_TEST

inline static void print_lua_error(lua_State* L) { MG_ERROR(("[Lua] reported an error: %s", lua_tostring(L, -1))); }

#else

inline static void print_lua_error(lua_State* L) { fprintf(stderr, "[Lua] reported an error: %s", lua_tostring(L, -1)); }

#endif

inline static bool Assert(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		print_lua_error(L);
		return true;
	}
	return false;
}



int GenQR_SVG(lua_State* L);


void register_lib_functions(lua_State* L)
{
	lua_pushcfunction(L, GenQR_SVG);
	lua_setglobal(L, "GenQR_SVG");
}


int GenQR_SVG(lua_State* L)
{
	auto text = luaL_checkstring(L, -4);
	auto border = luaL_checkinteger(L, -3);
	auto bg = luaL_checkstring(L, -2);
	auto fg = luaL_checkstring(L, -1);
	lua_pushstring(L, To_svg_string(generate_qr_code(text), border, bg, fg).c_str());
	return 1;
}