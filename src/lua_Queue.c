#include "C_Ref.h"
#include "conf.h"
#include "lauxlib.h"
#include "lua.h"
#include "stdlib.h"
#include <string.h>
#include <sys/stat.h>

typedef struct queue_t {
  int *refs;
  int maxref;
  int size;
  int head;
  int tail;
} queue_t;
const char *const QUEUE_KEY = "_QUEUE";
#define check_queue(L) (queue_t *)luaL_checkudata(L, 1, QUEUE_KEY)
#define INIT_SIZE 8
//--------------------------------------------------------------------------------------------------
static int queue_new(lua_State *L) {
  queue_t *q = (queue_t *)lua_newuserdata(L, sizeof(queue_t));
  memset(q, 0, sizeof(queue_t));
  q->size = INIT_SIZE;
  q->refs = (int *)_malloc(q->size * sizeof(int));
  luaL_getmetatable(L, QUEUE_KEY); // <ud|mt>
  lua_setmetatable(L, -2);         // <ud>
  lua_newtable(L);                 // <ud|tab>
  lua_setuservalue(L, -2);         // <ud>
  return 1;
}
inline static int get_len(queue_t *q) {
  if (q->head <= q->tail) {
    return q->tail - q->head;

  } else {
    return q->size - (q->head - q->tail);
  }
}
static int queue_len(lua_State *L) {
  queue_t *q = check_queue(L);
  lua_pushinteger(L, get_len(q));
  return 1;
}
static int queue_gc(lua_State *L) {
  queue_t *q = check_queue(L);
  _free(q->refs);
  return 0;
}
static int queue_tostring(lua_State *L) {
  queue_t *q = check_queue(L);
  lua_pushfstring(L, "queue: %p", q);
  return 1;
}
static int queue_index(lua_State *L) {
  queue_t *q = check_queue(L);
  int i = (int)luaL_checkinteger(L, 2) - 1;
  luaL_argcheck(L, i >= 0, 2, "index out of range");
  if (i >= q->size) {
    lua_pushnil(L);
    return 1;
  } else {
    i = (q->head + i) % q->size;
    int ref = q->refs[i];
    if (ref == LUA_REFNIL) {
      lua_pushnil(L);
      return 1;
    } else {
      lua_getuservalue(L, 1);
      lua_rawgeti(L, -1, ref);
      return 1;
    }
  }
}
static void trygrow(queue_t *q) {
  int qsz = get_len(q);
  if (qsz + 1 >= q->size) {
    int newsz = q->size * 2;
    q->refs = _realloc(q->refs, newsz * sizeof(int));
    if (q->tail < q->head) {
      int count = q->size - q->head;
      int newhead = newsz - count;
      memmove(q->refs + newhead, q->refs + q->head, count * sizeof(int));
      q->head = newhead;
    }
    q->size = newsz;
  }
}

static int queue_push(lua_State *L) {
  queue_t *q = check_queue(L);
  luaL_checkany(L, 2);
  trygrow(q);
  lua_getuservalue(L, 1);                         // q|v|t
  lua_pushvalue(L, 2);                            // q|v|t|v
  q->refs[q->tail] = refvalue(L, &q->maxref, -2); // q|v|t
  q->tail = (q->tail + 1) % q->size;
  return 0;
}

static int queue_pop(lua_State *L) {
  queue_t *q = check_queue(L);
  if (!get_len(q)) {
    lua_pushnil(L);
    return 1;
  }
  int ref = q->refs[q->head];
  q->head = (q->head + 1) % q->size;
  if (ref == LUA_REFNIL) {
    lua_pushnil(L);
    return 1;
  } else {
    lua_getuservalue(L, 1); // q|t
    unrefvalue(L, -1, ref); // q|t|v
    return 1;
  }
}

static int queue_totable(lua_State *L) {
  queue_t *q = check_queue(L);
  int n = get_len(q);
  lua_getuservalue(L, 1);   // q|rt
  lua_createtable(L, n, 0); // q|rt|t
  int ref;
  int i;
  for (i = 0; i < n; ++i) {
    ref = q->refs[(q->head + i) % q->size];
    if (ref == LUA_REFNIL) {
      lua_pushnil(L);
    } else {
      lua_rawgeti(L, -2, ref);
    }
    lua_rawseti(L, -2, i + 1);
  }
  return 1;
}
//---------------------------------------------------------------------------------
static luaL_Reg f[] = {
    {"new", queue_new},         {"push", queue_push}, {"pop", queue_pop},
    {"totable", queue_totable}, {NULL, NULL},
};

static luaL_Reg m[] = {
    {"__len", queue_len},
    {"__gc", queue_gc},
    {"__tostring", queue_tostring},
    {"__index", queue_index},
    {NULL, NULL},
};

LUAOPEN int luaopen_LuaExtendLib_queue(lua_State *L) {
  luaL_checkversion(L);
  luaL_newmetatable(L, QUEUE_KEY);
  luaL_setfuncs(L, m, 0);
  luaL_newlib(L, f);
  return 1;
}