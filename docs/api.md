## 队列(Queue)模块

本模块为 Lua 提供高效的队列（循环队列）数据结构，支持任意 Lua 对象的入队、出队、索引和转表操作。

### 接口说明

#### 1. `queue.new()`

**功能**：创建一个新的队列对象。

**返回值**：  
- 返回一个队列对象。

**示例**：

```lua
local q = queue.new()
```

---

#### 2. `q:push(value)`

**功能**：将一个值入队（添加到队尾）。

**参数**：  
- `value`：任意 Lua 值。

**返回值**：无。

**示例**：

```lua
q:push(123)
q:push("abc")
```

---

#### 3. `q:pop()`

**功能**：从队头弹出一个值。

**返回值**：  
- 队头的值。如果队列为空，返回 `nil`。

**示例**：

```lua
local v = q:pop()
```

---

#### 4. `q:totable()`

**功能**：将队列内容转为 Lua 数组（table），顺序与队列一致。

**返回值**：  
- 包含所有队列元素的 table。

**示例**：

```lua
local t = q:totable()
for i, v in ipairs(t) do print(i, v) end
```

---

#### 5. `#q`（长度运算符）

**功能**：获取队列当前长度。

**返回值**：  
- 队列中的元素数量。

**示例**：

```lua
print(#q)
```

---

#### 6. `q[i]`（索引访问）

**功能**：按队列顺序访问第 `i` 个元素（1-based）。

**返回值**：  
- 第 `i` 个元素，若越界返回 `nil`。

**示例**：

```lua
print(q[1])
```

---

#### 7. `tostring(q)` 或 `print(q)`

**功能**：获取队列对象的字符串表示。

**返回值**：  
- 形如 `"queue: 0xXXXXXX"` 的字符串。

---

### 注意事项

- 队列可自动扩容，支持高效的循环队列实现。
- 支持存储任意 Lua 对象，包括 table、string、number、boolean、nil 等。
- 队列对象会自动回收内部资源，无需手动释放。
- 队列索引从 1 开始。

---

### 典型用法

```lua
local queue = require "LuaExtendLib.queue"
local q = queue.new()
q:push("a")
q:push("b")
print(q:pop())      -- 输出: a
print(#q)           -- 输出: 1
q:push("c")
local t = q:totable()
for i, v in ipairs(t) do print(i, v) end  -- 输出: 1  b   2  c
```

---

如需扩展功能或自定义行为，请参考 lua_Queue.c 源码实现。
## 随机数模块
本模块为 Lua 提供高性能、可控种子的随机数生成器，支持独立随机状态、整数/浮点随机、区间随机、重设种子等功能。
### 接口说明

#### 1. `rand.new([seed1, seed2])`

**功能**：创建一个新的随机数生成器对象。

**参数**：
- `seed1, seed2`（可选）：两个整数种子。若未提供，则自动使用当前时间和对象地址。

**返回值**：  
- 返回一个随机数生成器对象（以下简称“生成器”）。

**示例**：

```lua
local r = rand.new()
local r2 = rand.new(123, 456)
```

---

#### 2. `r:nextfloat()`  
#### 3. `r:nextfloat(a, b)`

**功能**：生成浮点随机数。

- `r:nextfloat()`：返回 [0,1) 区间的浮点数。
- `r:nextfloat(a, b)`：返回 [a, b) 区间的浮点数。

**示例**：

```lua
print(r:nextfloat())      -- 0 <= x < 1
print(r:nextfloat(10,20)) -- 10 <= x < 20
```

---

#### 4. `r:nextint()`  
#### 5. `r:nextint(a, b)`

**功能**：生成整数随机数。

- `r:nextint()`：返回一个 64 位随机整数。
- `r:nextint(a, b)`：返回 [a, b] 区间的整数。

**示例**：

```lua
print(r:nextint())      -- 任意整数
print(r:nextint(1, 100))-- 1~100 之间的整数
```

---

#### 6. `r:setseed(seed1, seed2)`

**功能**：重设当前生成器的种子。

**参数**：
- `seed1, seed2`：两个整数种子。

**返回值**：无。

**示例**：

```lua
r:setseed(100, 200)
```

---

#### 7. `tostring(r)` 或 `print(r)`

**功能**：获取生成器对象的字符串表示。

**返回值**：  
- 形如 `"rand: 0xXXXXXX"` 的字符串。

---

### 注意事项

- 每个生成器对象互不影响，适合多线程或多实例场景。
- 支持 64 位整数和高精度浮点随机。
- 若需全局随机数，可创建单例生成器并全局复用。

---

### 典型用法

```lua
local rand = require "LuaExtendLib.rand"
local r = rand.new()
print(r:nextfloat())      -- 0~1 之间的浮点数
print(r:nextint(1, 10))   -- 1~10 之间的整数
r:setseed(os.time(), 42)  -- 重设种子
```

---

如需扩展功能或自定义行为，请参考 luaRand.c 源码实现。
## 列表(List)模块
本模块为 Lua 提供高性能的“列表”数据结构，支持高效的插入、删除、查找、排序、拼接等操作，适合频繁变动的顺序数据场景。

```lua
local list = require "LuaExtendLib.list"
```

### 接口说明

#### 1. `list.new([cap])`

**功能**：创建一个新的列表对象。

**参数**：
- `cap`（可选）：初始容量，默认 4。

**返回值**：  
- 返回一个列表对象。

**示例**：

```lua
local ls = list.new()
```

---

#### 2. `list.insert(ls, [pos,] value)`

**功能**：在指定位置插入一个值。

**参数**：
- `ls`：列表对象。
- `pos`（可选）：插入位置（1-based），默认插入到末尾。
- `value`：要插入的值（不能为 nil）。

**返回值**：无。

**示例**：

```lua
list.insert(ls, "a")        -- 末尾插入
list.insert(ls, 1, "b")     -- 插入到第1个位置
```

---

#### 3. `list.remove(ls[, pos]) -> value`

**功能**：移除并返回指定位置的值。

**参数**：
- `ls`：列表对象。
- `pos`（可选）：要移除的位置（1-based），默认移除最后一个。

**返回值**：  
- 被移除的值。

**示例**：

```lua
local v = list.remove(ls, 2)
```

---

#### 4. `list.totable(ls[, i [, j]]) -> table`

**功能**：将列表内容（指定范围）转为 Lua table。

**参数**：
- `ls`：列表对象。
- `i`（可选）：起始索引，默认 1。
- `j`（可选）：结束索引，默认 #ls。

**返回值**：  
- 包含指定范围元素的新 table。

**示例**：

```lua
local t = list.totable(ls, 2, 4)
```

---

#### 5. `list.fromtable(ls, t [, i [, j]])`

**功能**：用 table 的内容（指定范围）重建列表。

**参数**：
- `ls`：列表对象。
- `t`：Lua table。
- `i`（可选）：起始索引，默认 1。
- `j`（可选）：结束索引，默认 #t。

**返回值**：无。

**示例**：

```lua
list.fromtable(ls, {1,2,3,4})
```

---

#### 6. `list.extend(ls, ls2|t)`

**功能**：将另一个列表或 table 的内容追加到当前列表末尾。

**参数**：
- `ls`：列表对象。
- `ls2|t`：另一个列表对象或 Lua table。

**返回值**：无。

**示例**：

```lua
list.extend(ls, {5,6,7})
```

---

#### 7. `list.indexof(ls, value) -> number|nil`

**功能**：查找值在列表中的索引。

**参数**：
- `ls`：列表对象。
- `value`：要查找的值。

**返回值**：  
- 索引（1-based），未找到返回 nil。

**示例**：

```lua
local idx = list.indexof(ls, "a")
```

---

#### 8. `list.concat(ls [, sep [, i [, j]]]) -> string`

**功能**：将列表内容拼接为字符串。

**参数**：
- `ls`：列表对象。
- `sep`（可选）：分隔符，默认空字符串。
- `i`（可选）：起始索引，默认 1。
- `j`（可选）：结束索引，默认 #ls。

**返回值**：  
- 拼接后的字符串。

**示例**：

```lua
local s = list.concat(ls, ",")
```

---

#### 9. `list.clear(ls[, shrink])`

**功能**：清空列表内容。

**参数**：
- `ls`：列表对象。
- `shrink`（可选）：是否收缩内存，默认 false。

**返回值**：无。

**示例**：

```lua
list.clear(ls, true)
```

---

#### 10. `list.exchange(ls, idx1, idx2)`

**功能**：交换列表中两个元素的位置。

**参数**：
- `ls`：列表对象。
- `idx1`、`idx2`：要交换的两个索引（1-based）。

**返回值**：无。

**示例**：

```lua
list.exchange(ls, 1, 3)
```

---

#### 11. `list.sort(ls [, cmp [, key]])`

**功能**：原地排序列表。

**参数**：
- `ls`：列表对象。
- `cmp`（可选）：比较函数 function(a, b) -> boolean，a < b 返回 true。
- `key`（可选）：key 函数 function(v) -> int，返回整型排序 key。优先使用 key。

**返回值**：无。

**示例**：

```lua
list.sort(ls)
list.sort(ls, function(a, b) return a > b end)
list.sort(ls, nil, function(v) return v.score end)
```

---

### 元方法支持

- `ls[i]`：获取第 i 个元素（支持负索引，-1 表示最后一个）。
- `ls[i] = v`：设置第 i 个元素（支持负索引，不能为 nil，`ls[#ls+1]=v` 可追加）。
- `#ls`：获取列表长度。
- `tostring(ls)`：获取列表对象的字符串表示。
- 列表对象自动回收内存，无需手动释放。

---

### 典型用法

```lua
local list = require "LuaExtendLib.list"
local ls = list.new()
list.insert(ls, "a")
list.insert(ls, "b")
print(ls[1])         -- 输出: a
ls[2] = "c"
print(#ls)           -- 输出: 2
list.remove(ls, 1)
list.extend(ls, {1,2,3})
list.sort(ls)
local t = list.totable(ls)
for i, v in ipairs(t) do print(i, v) end
```

---

如需扩展功能或自定义行为，请参考 luaList.c 源码实现。
## 序列化模块 

本模块提供高效的 Lua 序列化与反序列化能力，支持 `nil`、`boolean`、`number`、`string`、`table`、`lightuserdata` 类型。序列化格式紧凑，适合网络传输或本地存储。

### 接口说明

#### 1. `seri.pack(...)`

**功能**：将任意数量的 Lua 对象序列化为二进制字符串。

**参数**：  
- 支持任意数量和类型（nil, boolean, number, string, table, lightuserdata）的 Lua 值。

**返回值**：  
- 返回一个二进制字符串，表示所有参数的序列化结果。

**示例**：

```lua
local s = seri.pack(123, "abc", {x=1, y=2}, true, nil)
-- s 为序列化后的二进制字符串
```

#### 2. `seri.unpack(str)`

**功能**：将由 `pack` 生成的二进制字符串反序列化为 Lua 对象。

**参数**：  
- `str`：由 `pack` 生成的二进制字符串。

**返回值**：  
- 返回解包得到的所有 Lua 对象，顺序与 pack 时一致。

**示例**：

```lua
local a, b, c, d, e = seri.unpack(s)
-- a == 123, b == "abc", c == {x=1, y=2}, d == true, e == nil
```

### 注意事项

- 支持 table 的嵌套和数组/哈希混合结构，最大递归深度为 32 层。
- lightuserdata 仅适用于同一进程内传递，不能跨进程/跨平台。
- 不支持 function、userdata（非 light）、thread 等类型。
- pack/unpack 支持多个对象的序列化与反序列化。

### 典型用法

```lua
local seri = require "LuaExtendLib.seri"

local t = {1, 2, a=10, b=20}
local bin = seri.pack(t, 42, "hello")
local t2, n, s = seri.unpack(bin)
print(t2.a, n, s)  -- 输出: 10  42  hello
```

---

如需扩展类型支持或自定义序列化规则，请参考 luaSeri.c 源码实现。
## JSON 模块

本模块为 Lua 提供高效的 JSON 编解码能力，支持注释、空表处理、数字 key 转字符串、最大嵌套层数等特性。

### 接口说明

#### 1. `json.load(str [, maxdepth [, allowcomment]])`

**功能**：将 JSON 字符串解析为 Lua 对象。

**参数**：
- `str`：待解析的 JSON 字符串（必须是 UTF-8 编码）。
- `maxdepth`（可选）：最大嵌套层数，默认 128。
- `allowcomment`（可选）：是否允许 C 风格注释，默认 false。

**返回值**：
- 返回解析得到的 Lua 对象。JSON 的 `null` 会被解析为 `json.null`（即 lightuserdata(NULL)）。

**示例**：

```lua
local obj = json.load('{"a":1,"b":[2,3],"c":null}')
print(obj.a, obj.b[1], obj.c == json.null)  -- 输出: 1  2  true
```

#### 2. `json.dump(obj [, format [, empty_as_array [, num_as_str [, maxdepth]]]])`

**功能**：将 Lua 对象序列化为 JSON 字符串。

**参数**：
- `obj`：要序列化的 Lua 对象（table、string、number、boolean、nil、json.null）。
- `format`（可选）：是否格式化输出（带缩进），默认 false。
- `empty_as_array`（可选）：空表是否序列化为 `[]`，默认 false（即 `{}`）。
- `num_as_str`（可选）：数字 key 是否转为字符串 key，默认 false。
- `maxdepth`（可选）：最大递归层数，默认 128。

**返回值**：
- 返回 JSON 字符串。

**示例**：

```lua
local t = {a=1, b={2,3}, c=json.null}
local s = json.dump(t)
print(s)  -- 输出: {"a":1,"b":[2,3],"c":null}
```

#### 3. `json.null`

**说明**：  
- 代表 JSON 的 `null` 值。解析时 JSON 的 `null` 会变为 `json.null`，序列化时 `json.null` 会输出为 `null`。
- 其本质是 `lightuserdata(NULL)`，可用于区分 Lua 的 `nil`。

**示例**：

```lua
local t = {a = json.null}
local s = json.dump(t)
print(s)  -- 输出: {"a":null}
```

### 注意事项

- 仅支持 table、string、number、boolean、nil、json.null 类型，其他类型会报错。
- table 的 key 必须为字符串，或设置 `num_as_str` 为 true 时允许数字 key。
- 空表默认序列化为 `{}`，可通过 `empty_as_array` 控制为 `[]`。
- 支持最大嵌套层数限制，防止恶意或错误数据导致栈溢出。
- 支持 Unicode 字符和转义序列。
- 解析时可选支持 C 风格注释（、`/* ... */`）。

### 典型用法

```lua
local json = require "LuaExtendLib.json"

-- 解析
local obj = json.load('{"x":123,"y":[1,2,3],"z":null}')
print(obj.x, obj.y[2], obj.z == json.null)

-- 序列化
local s = json.dump({a=1, b={2,3}, c=json.null}, true)
print(s)
```

---

如需自定义行为或扩展类型支持，请参考 luaJson.c 源码实现。