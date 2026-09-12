//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
// Purpose: I/O system for Lua
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "tier0/memdbgon.h"

static int lua_io_create_output(lua_State *L) {
	lua_newtable(L);
	lua_pushstring(L, "type");
	lua_pushstring(L, "output");
	lua_settable(L, -3);
	lua_pushstring(L, "name");
	lua_pushstring(L, luaL_checkstring(L, 2));
	lua_settable(L, -3);
	lua_pushstring(L, "entity");
	lua_pushvalue(L, 1);
	lua_settable(L, -3);
	return 1;
}

static int lua_io_create_input(lua_State *L) {
	lua_newtable(L);
	lua_pushstring(L, "type");
	lua_pushstring(L, "input");
	lua_settable(L, -3);
	lua_pushstring(L, "name");
	lua_pushstring(L, luaL_checkstring(L, 2));
	lua_settable(L, -3);
	lua_pushstring(L, "entity");
	lua_pushvalue(L, 1);
	lua_settable(L, -3);
	return 1;
}

static int lua_io_connect(lua_State *L) {
	if (!lua_istable(L, 1)) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_getfield(L, 1, "connections");
	if (!lua_istable(L, -1)) {
		lua_newtable(L);
		lua_setfield(L, 1, "connections");
		lua_getfield(L, 1, "connections");
	}
	lua_pushvalue(L, 2);
	lua_rawseti(L, -2, luaL_len(L, -2) + 1);
	lua_pushboolean(L, 1);
	return 1;
}

static int lua_io_fire(lua_State *L) {
	lua_getglobal(L, "hook");
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "Call");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, "OnIOFired");
			lua_pushvalue(L, 1);
			lua_pcall(L, 2, 0, 0);
		} else {
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	return 0;
}

static const luaL_Reg io_lib[] = {
	{"CreateOutput", lua_io_create_output},
	{"CreateInput", lua_io_create_input},
	{"Connect", lua_io_connect},
	{"Fire", lua_io_fire},
	{NULL, NULL}
};

LUALIB_API int luaopen_io(lua_State *L) {
	luaL_register(L, "io", io_lib);
	return 1;
}
