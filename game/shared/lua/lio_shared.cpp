//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: I/O system for Lua - Input/Output connections for entities
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"

#include "tier0/memdbgon.h"

// I/O table metatable name
#define LUA_IOTABLENAME "IOTable"

static int luasrc_ents_CreateIOOutput(lua_State *L) {
	CBaseEntity *pEnt = luaL_checkentity(L, 1);
	const char *szName = luaL_checkstring(L, 2);
	
	if (!pEnt) {
		lua_pushnil(L);
		return 1;
	}
	
	// Create output table
	lua_newtable(L);
	
	// Push entity reference
	lua_pushstring(L, "entity");
	lua_pushentity(L, pEnt);
	lua_settable(L, -3);
	
	// Push name
	lua_pushstring(L, "name");
	lua_pushstring(L, szName);
	lua_settable(L, -3);
	
	// Push type
	lua_pushstring(L, "type");
	lua_pushstring(L, "output");
	lua_settable(L, -3);
	
	// Create empty connections table
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
	
	// Create input table
	lua_newtable(L);
	
	// Push entity reference
	lua_pushstring(L, "entity");
	lua_pushentity(L, pEnt);
	lua_settable(L, -3);
	
	// Push name
	lua_pushstring(L, "name");
	lua_pushstring(L, szName);
	lua_settable(L, -3);
	
	// Push type
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
	
	// Get input's connections table
	lua_getfield(L, 1, "connections");
	if (!lua_istable(L, -1)) {
		lua_pushboolean(L, false);
		lua_remove(L, -2);  // remove the nil
		return 1;
	}
	
	// Add output reference to connections
	int idx = lua_rawlen(L, -1) + 1;
	lua_pushvalue(L, 2);  // copy output table
	lua_rawseti(L, -2, idx);  // set at index
	lua_pop(L, 1);  // pop connections table
	
	lua_pushboolean(L, true);
	return 1;
}

static int luasrc_ents_FireIOInput(lua_State *L) {
	// Trigger an input on an entity
	if (!lua_istable(L, 1)) {
		return 0;
	}
	
	// Get input's entity
	lua_getfield(L, 1, "entity");
	CBaseEntity *pEnt = lua_toentity(L, -1);
	lua_pop(L, 1);
	
	if (!pEnt) return 0;
	
	// Get input's name
	lua_getfield(L, 1, "name");
	const char *szInputName = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	// Try to call hook for input fired if hook system exists
	lua_getglobal(L, "hook");
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "Call");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, "OnIOInputFired");
			lua_pushentity(L, pEnt);
			lua_pushstring(L, szInputName);
			
			// Push optional value (3rd param if exists)
			if (lua_gettop(L) > 4 && !lua_isnil(L, 3)) {
				lua_pushvalue(L, 3);
			} else {
				lua_pushnil(L);
			}
			
			// Call hook.Call("OnIOInputFired", entity, inputname, value)
			if (lua_pcall(L, 4, 0, 0) != 0) {
				// Ignore errors silently
				lua_pop(L, 1);
			}
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
