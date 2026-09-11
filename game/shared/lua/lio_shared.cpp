//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: I/O system for Lua
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "lbaseentity_shared.h"

#include "tier0/memdbgon.h"

// Simple I/O registry: stores connections between entities
static int luasrc_ents_CreateIOOutput(lua_State *L) {
	CBaseEntity *pEnt = luaL_checkentity(L, 1);
	const char *szName = luaL_checkstring(L, 2);
	
	if (!pEnt) {
		lua_pushnil(L);
		return 1;
	}
	
	lua_newtable(L);
	lua_pushstring(L, "entity");
	lua_pushentity(L, pEnt);
	lua_settable(L, -3);
	
	lua_pushstring(L, "name");
	lua_pushstring(L, szName);
	lua_settable(L, -3);
	
	lua_pushstring(L, "type");
	lua_pushstring(L, "output");
	lua_settable(L, -3);
	
	lua_pushstring(L, "connections");
	lua_newtable(L);
	lua_settable(L, -3);
	
	return 1;
}

static int luasrc_ents_CreateIOInput(lua_State *L) {
	CBaseEntity *pEnt = luaL_checkentity(L, 1);
	const char *szName = luaL_checkstring(L, 2);
	
	if (!pEnt) {
		lua_pushnil(L);
		return 1;
	}
	
	lua_newtable(L);
	lua_pushstring(L, "entity");
	lua_pushentity(L, pEnt);
	lua_settable(L, -3);
	
	lua_pushstring(L, "name");
	lua_pushstring(L, szName);
	lua_settable(L, -3);
	
	lua_pushstring(L, "type");
	lua_pushstring(L, "input");
	lua_settable(L, -3);
	
	return 1;
}

static int luasrc_ents_IOConnect(lua_State *L) {
	// input:Connect(output)
	if (!lua_istable(L, 1) || !lua_istable(L, 2)) {
		lua_pushboolean(L, false);
		return 1;
	}
	
	lua_getfield(L, 1, "connections");
	if (lua_istable(L, -1)) {
		int idx = lua_objlen(L, -1) + 1;
		lua_pushvalue(L, 2);
		lua_rawseti(L, -2, idx);
		lua_pop(L, 1);
		lua_pushboolean(L, true);
		return 1;
	}
	
	lua_pushboolean(L, false);
	return 1;
}

static int luasrc_ents_FireIOInput(lua_State *L) {
	// Fire an input with optional value
	if (!lua_istable(L, 1)) {
		return 0;
	}
	
	lua_getfield(L, 1, "entity");
	CBaseEntity *pEnt = lua_toentity(L, -1);
	lua_pop(L, 1);
	
	if (!pEnt) return 0;
	
	lua_getfield(L, 1, "name");
	const char *szInputName = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	// Call hook for input fired
	lua_getglobal(L, "hook");
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "Call");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, "OnIOInputFired");
			lua_pushentity(L, pEnt);
			lua_pushstring(L, szInputName);
			if (lua_gettop(L) > 3 && !lua_isnil(L, 3)) {
				lua_pushvalue(L, 3);
			} else {
				lua_pushnil(L);
			}
			luasrc_pcall(L, 4, 0, 0);
		} else {
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	return 0;
}

static const luaL_Reg io_ents_funcs[] = {
	{"CreateIOOutput", luasrc_ents_CreateIOOutput},
	{"CreateIOInput", luasrc_ents_CreateIOInput},
	{"IOConnect", luasrc_ents_IOConnect},
	{"FireIOInput", luasrc_ents_FireIOInput},
	{NULL, NULL}
};

LUALIB_API int luaopen_io (lua_State *L) {
	luaL_register(L, "io", io_ents_funcs);
	return 1;
}
