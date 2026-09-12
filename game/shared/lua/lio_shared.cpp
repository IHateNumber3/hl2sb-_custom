#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "tier0/memdbgon.h"

static int luasrc_IO_CreateOutput(lua_State *L) {
	lua_newtable(L);
	lua_pushstring(L, "type");
	lua_pushstring(L, "output");
	lua_settable(L, -3);
	return 1;
}

static int luasrc_IO_CreateInput(lua_State *L) {
	lua_newtable(L);
	lua_pushstring(L, "type");
	lua_pushstring(L, "input");
	lua_settable(L, -3);
	return 1;
}

static const luaL_Reg io_funcs[] = {
	{"CreateOutput", luasrc_IO_CreateOutput},
	{"CreateInput", luasrc_IO_CreateInput},
	{NULL, NULL}
};

LUALIB_API int luaopen_inout (lua_State *L) {
	luaL_register(L, LUA_INOUTLIBNAME, io_funcs);
	return 1;
}
