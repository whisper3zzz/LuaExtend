#ifndef _C_REF_H
#define _C_REF_H
#include "lua.h"

int refvalue(lua_State *L, int *maxref, int t); // 获取引用值
void unrefvalue(lua_State *L, int t, int ref);  // 释放引用值
#endif                                          // _C_REF_H