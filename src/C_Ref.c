#include "C_Ref.h"
#include "lauxlib.h"
#include "lua.h"
#define freelist 0 // 空闲列表头索引
int refvalue(lua_State *L, int *maxref, int t) {
  int ref;
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1); // 弹出nil
    return LUA_REFNIL;
  }
  t = lua_absindex(L, t);          // 获取绝对索引
  lua_rawgeti(L, t, freelist);     // 获取第一个空闲元素
  ref = (int)lua_tointeger(L, -1); // 获取引用值
  lua_pop(L, 1);                   // 弹出引用值
  if (ref != 0) {
    lua_rawgeti(L, t, ref); // 从空闲列表中删除引用值
    lua_rawseti(L, t, freelist);
  } else {
    ref = ++(*maxref); // 获取新的引用值
  }
  lua_rawseti(L, t, ref); // 设置引用值
  return ref;
}
void unrefvalue(lua_State *L, int t, int ref) {
  if (ref >= 0) {
    t = lua_absindex(L, t);      // 获取绝对索引
    lua_rawgeti(L, t, freelist); // 获取空闲列表头
    lua_rawseti(L, t, ref);      // 设置引用值为头
    lua_pushinteger(L, ref);
    lua_rawseti(L, t, freelist); // 设置空闲列表头
  }
}