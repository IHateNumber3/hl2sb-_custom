#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "tier0/memdbgon.h"

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
	
	lua_pushstring(L, "connections");
	lua_newtable(L);
	lua_settable(L, -3);
	
	return 1;
}

static int luasrc_ents_IOConnect(lua_State *L) {
	if (!lua_istable(L, 1) || !lua_istable(L, 2)) {
		lua_pushboolean(L, false);
		return 1;
	}
	
	// Add output to input's connections
	lua_getfield(L, 1, "connections");
	if (!lua_istable(L, -1)) {
		lua_newtable(L);
		lua_setfield(L, 1, "connections");
		lua_getfield(L, 1, "connections");
	}
	
	int idx = lua_objlen(L, -1);
	lua_pushvalue(L, 2);
	lua_rawseti(L, -2, idx + 1);
	lua_pop(L, 1);
	
	lua_pushboolean(L, true);
	return 1;
}

static int luasrc_ents_FireIOInput(lua_State *L);

static int luasrc_ents_FireIOOutput(lua_State *L) {
	if (!lua_istable(L, 1)) {
		return 0;
	}
	
	int paramCount = lua_gettop(L);
	
	lua_getfield(L, 1, "entity");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	CBaseEntity *pEnt = lua_toentity(L, -1);
	lua_pop(L, 1);
	
	if (!pEnt) return 0;
	
	lua_getfield(L, 1, "name");
	const char *szOutputName = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	lua_getglobal(L, "hook");
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "Call");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, "OnIOOutputFired");
			lua_pushnil(L);
			lua_pushentity(L, pEnt);
			lua_pushstring(L, szOutputName ? szOutputName : "");
			
			if (paramCount > 1) {
				lua_pushvalue(L, 2);
			} else {
				lua_pushnil(L);
			}
			
			lua_pcall(L, 5, 0, 0);
		} else {
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	// Fire connected inputs
	lua_getfield(L, 1, "connections");
	if (lua_istable(L, -1)) {
		int len = lua_objlen(L, -1);
		for (int i = 1; i <= len; i++) {
			lua_rawgeti(L, -1, i);
			if (lua_istable(L, -1)) {
				lua_pushcfunction(L, luasrc_ents_FireIOInput);
				lua_pushvalue(L, -2);
				
				if (paramCount > 1) {
					lua_pushvalue(L, 2);
				}
				
				lua_pcall(L, paramCount > 1 ? 2 : 1, 0, 0);
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	return 0;
}

static int luasrc_ents_FireIOInput(lua_State *L) {
	if (!lua_istable(L, 1)) {
		return 0;
	}
	
	int paramCount = lua_gettop(L);
	
	lua_getfield(L, 1, "entity");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	CBaseEntity *pEnt = lua_toentity(L, -1);
	lua_pop(L, 1);
	
	if (!pEnt) return 0;
	
	lua_getfield(L, 1, "name");
	const char *szInputName = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	lua_getglobal(L, "hook");
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "Call");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, "OnIOInputFired");
			lua_pushnil(L);
			lua_pushentity(L, pEnt);
			lua_pushstring(L, szInputName ? szInputName : "");
			
			if (paramCount > 1) {
				lua_pushvalue(L, 2);
			} else {
				lua_pushnil(L);
			}
			
			lua_pcall(L, 5, 0, 0);
		} else {
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	// Fire connected outputs
	lua_getfield(L, 1, "connections");
	if (lua_istable(L, -1)) {
		int len = lua_objlen(L, -1);
		for (int i = 1; i <= len; i++) {
			lua_rawgeti(L, -1, i);
			if (lua_istable(L, -1)) {
				lua_pushcfunction(L, luasrc_ents_FireIOOutput);
				lua_pushvalue(L, -2);
				
				if (paramCount > 1) {
					lua_pushvalue(L, 2);
				}
				
				lua_pcall(L, paramCount > 1 ? 2 : 1, 0, 0);
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	return 0;
}

static const luaL_Reg inout_funcs[] = {
	{"CreateIOOutput", luasrc_ents_CreateIOOutput},
	{"CreateIOInput", luasrc_ents_CreateIOInput},
	{"IOConnect", luasrc_ents_IOConnect},
	{"FireIOInput", luasrc_ents_FireIOInput},
	{"FireIOOutput", luasrc_ents_FireIOOutput},
	{NULL, NULL}
};

LUALIB_API int luaopen_inout (lua_State *L) {
	luaL_register(L, "inout", inout_funcs);
	return 1;
}
