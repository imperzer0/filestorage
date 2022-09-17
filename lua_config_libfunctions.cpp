// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include "lua_config_libfunctions.h"
#include "qrcodegen.cpp"
#include "server.h"
#include <map>


#ifndef EXTERNAL_TEST

#include "Gen_QR.h"
#include "perceptron.hpp"


#endif


#undef LUA_ASSERT
#define LUA_ASSERT(L, expr, ret_expr) if (Assert(L, expr)) return ret_expr

static void dumpstack(lua_State* L)
{
	int top = lua_gettop(L);
	for (int i = 1; i <= top; i++)
	{
		printf("%d\t%s\t", i, luaL_typename(L, i));
		switch (lua_type(L, i))
		{
			case LUA_TNUMBER:
				printf("%g\n", lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("%s\n", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
				break;
			case LUA_TNIL:
				printf("%s\n", "nil");
				break;
			default:
				printf("%p\n", lua_topointer(L, i));
				break;
		}
	}
	printf("\n");
}


namespace ac
{
	enum : LUA_INTEGER
	{
		sigm = 0,
		sigm_scaled,
		tanh,
		swish,
		size
	};
}

inline static void lua_pushacfn(lua_State* L, LUA_INTEGER ac, const char* name)
{
	lua_pushinteger(L, ac);
	lua_setfield(L, -2, name);
}

inline static neural::neural_function get_acfn(LUA_INTEGER ac)
{
	switch (ac)
	{
		case ac::sigm:
			return neural::sigm_activation;
		case ac::sigm_scaled:
			return neural::sigm_scaled_activation;
		case ac::tanh:
			return neural::tanh_activation;
		case ac::swish:
			return neural::swish_activation;
		default:
			return nullptr;
	}
}

inline static neural::neural_function get_deacfn(LUA_INTEGER ac)
{
	switch (ac)
	{
		case ac::sigm:
			return neural::sigm_deactivation;
		case ac::sigm_scaled:
			return neural::sigm_scaled_deactivation;
		case ac::tanh:
			return neural::tanh_deactivation;
		case ac::swish:
			return neural::swish_deactivation;
		default:
			return nullptr;
	}
}


int GenQR_SVG(lua_State* L);


int Perceptron_new(lua_State* L);

int Perceptron_use(lua_State* L);

int Perceptron_train(lua_State* L);

int Perceptron_destroy(lua_State* L);


void register_lib_functions(lua_State* L)
{
	lua_pushcfunction(L, GenQR_SVG);
	lua_setglobal(L, "GenQR_SVG");
	
	lua_createtable(L, 0, 1);
	
	lua_pushcfunction(L, Perceptron_new);
	lua_setfield(L, -2, "new");
	
	lua_createtable(L, 0, ac::size);
	
	lua_pushacfn(L, ac::sigm, "sigm");
	lua_pushacfn(L, ac::sigm_scaled, "sigm_scaled");
	lua_pushacfn(L, ac::tanh, "tanh");
	lua_pushacfn(L, ac::swish, "swish");
	
	lua_setfield(L, -2, "acf");
	
	lua_setglobal(L, "Perceptron");
}


int GenQR_SVG(lua_State* L)
{
	if (log_level[0] == '4') dumpstack(L);
	
	auto text = luaL_checkstring(L, -4);
	auto border = luaL_checkinteger(L, -3);
	auto bg = luaL_checkstring(L, -2);
	auto fg = luaL_checkstring(L, -1);
	lua_pushstring(L, To_svg_string(generate_qr_code(text), border, bg, fg).c_str());
	return 1;
}


size_t last_address = 0;
std::map<size_t, neural::perceptron> perc_heap;

LUA_INTEGER create_instance(const std::vector<size_t>& sizes, neural::neuron_t min, neural::neuron_t max, LUA_INTEGER ac)
{
	perc_heap.insert(
			decltype(perc_heap)::value_type{
					++last_address,
					{ sizes, min, max, get_acfn(ac), get_deacfn(ac) }
			}
	);
	return (LUA_INTEGER)last_address;
}

neural::perceptron* access_instance(LUA_INTEGER address)
{
	try { return &perc_heap.at(address); }
	catch (std::out_of_range& e) { return nullptr; }
}

void destroy_instance(LUA_INTEGER address)
{
	auto found = perc_heap.find(address);
	if (found != perc_heap.end())
		perc_heap.erase(found);
}

void create_lua_object(lua_State* L, LUA_INTEGER address)
{
	lua_createtable(L, 0, 4);
	
	lua_pushinteger(L, address);
	lua_setfield(L, -2, "ref");
	
	lua_pushcfunction(L, Perceptron_use);
	lua_setfield(L, -2, "use");
	
	lua_pushcfunction(L, Perceptron_train);
	lua_setfield(L, -2, "train");
	
	lua_pushcfunction(L, Perceptron_destroy);
	lua_setfield(L, -2, "destroy");
}

int Perceptron_new(lua_State* L)
{
	if (log_level[0] == '4') dumpstack(L);
	
	auto min = luaL_checknumber(L, -3);
	auto max = luaL_checknumber(L, -2);
	auto ac = luaL_checkinteger(L, -1);
	
	auto len = lua_rawlen(L, -4);
	
	std::vector<size_t> sizes;
	for (LUA_INTEGER i = 1; i <= len; ++i)
	{
		lua_rawgeti(L, -4, i);
		sizes.emplace_back(luaL_checkinteger(L, -1));
		lua_pop(L, 1);
	}
	
	create_lua_object(L, create_instance(sizes, min, max, ac));
	
	return 1;
}

int Perceptron_use(lua_State* L)
{
	if (log_level[0] == '4') dumpstack(L);
	
	lua_getfield(L, -2, "ref");
	
	auto address = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	
	auto len = lua_rawlen(L, -1);
	
	std::vector<neural::neuron_t> inputs;
	for (auto i = 1; i <= len; ++i)
	{
		lua_rawgeti(L, -1, i);
		inputs.emplace_back(luaL_checknumber(L, -1));
		lua_pop(L, 1);
	}
	
	auto results = access_instance(address)->use(std::move(inputs));
	
	lua_createtable(L, static_cast<int>(results.size()), 0);
	
	for (int i = 0; i < results.size(); ++i)
	{
		lua_pushnumber(L, static_cast<LUA_NUMBER>(results[i]));
		lua_rawseti(L, -2, i + 1);
	}
	
	return 1;
}

int Perceptron_train(lua_State* L)
{
	if (log_level[0] == '4') dumpstack(L);
	
	lua_getfield(L, -3, "ref");
	
	auto address = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	
	auto lr = luaL_checknumber(L, -1);
	
	auto len = lua_rawlen(L, -2);
	
	std::vector<neural::neuron_t> samples;
	for (auto i = 1; i <= len; ++i)
	{
		lua_rawgeti(L, -2, i);
		samples.emplace_back(luaL_checknumber(L, -1));
		lua_pop(L, 1);
	}
	
	access_instance(address)->teach(samples, lr);
	
	create_lua_object(L, address);
	
	return 1;
}

int Perceptron_destroy(lua_State* L)
{
	if (log_level[0] == '4') dumpstack(L);
	
	lua_getfield(L, -1, "ref");
	
	LUA_INTEGER address = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	
	destroy_instance(address);
	
	return 0;
}