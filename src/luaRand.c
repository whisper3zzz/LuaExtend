#include "C_Rand.h"
#include "conf.h"
#include "lauxlib.h"
#include "lua.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RAND_UD "CO_RAND"
#define torandstate(L) ((randstate_t *)luaL_checkudata((L), 1, RAND_UD))

static int rand_new(lua_State *L) {
  randstate_t *state = (randstate_t *)_newuserdata(L, sizeof(randstate_t));
  if (lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
    lua_Integer seed1 = lua_tointeger(L, 1);
    lua_Integer seed2 = lua_tointeger(L, 2);
    randseed(state, (uint64_t)seed1, (uint64_t)seed2);

  } else {
    randseed(state, (uint64_t)time(NULL), (uint64_t)(size_t)state);
  }
  luaL_setmetatable(L, RAND_UD);
  return 1;
}
static int rand_tostring(lua_State *L) {
  randstate_t *state = torandstate(L);
  lua_pushfstring(L, "rand:%p", state);
  return 1;
}
static int rand_nextfloat(lua_State *L) {
  randstate_t *state = torandstate(L);
  double r;
  switch (lua_gettop(L)) {
  case 1:
    r = randfloat(state);
    lua_pushnumber(L, (lua_Number)r);
    return 1;
  case 3:
    r = randfltrange(state, (double)luaL_checknumber(L, 2),
                     (double)luaL_checknumber(L, 3));
    lua_pushnumber(L, (lua_Number)r);
    return 1;
  default:
    return luaL_error(L, "Wrong number of arguments");
  }
}
static int rand_nextint(lua_State *L) {
  randstate_t *state = torandstate(L);
  int64_t r;
  switch (lua_gettop(L)) {
  case 1:
    r = randint(state);
    lua_pushinteger(L, (int64_t)r);
    return 1;
  case 3:
    r = randintrange(state, (int64_t)luaL_checkinteger(L, 2),
                     (int64_t)luaL_checkinteger(L, 3));
    lua_pushinteger(L, (int64_t)r);
    return 1;
  default:
    return luaL_error(L, "Wrong number of arguments");
  }
}
static int rand_setseed(lua_State *L) {
	randstate_t *state = torandstate(L);
	lua_Integer seed1 = luaL_checkinteger(L, 2);
	lua_Integer seed2 = luaL_checkinteger(L, 3);
	randseed(state, (uint64_t)seed1, (uint64_t)seed2);
	return 0;
}
static const luaL_Reg lib[] = {
	{"new", rand_new}, 
	{NULL, NULL}
};

static const luaL_Reg rand_mt[] = {
	{"__index", NULL},
	{"__tostring", rand_tostring},
	{"nextfloat", rand_nextfloat},
	{"nextint", rand_nextint},
	{"setseed", rand_setseed},
	{NULL, NULL}
};
LUAOPEN int luaopen_LuaExtendLib_rand(lua_State *L) {
	luaL_checkversion(L);

	// create metatable of oset
	luaL_newmetatable(L, RAND_UD);		// S: mt
	luaL_setfuncs(L, rand_mt, 0);		// S: mt
	lua_pushvalue(L, -1);				// S: mt mt
	lua_setfield(L, -2, "__index");		// S: mt

	luaL_newlib(L, lib);				// S: mt lib
	return 1;
}