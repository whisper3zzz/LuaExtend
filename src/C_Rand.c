#include "C_Rand.h"
#include <stdint.h>

// _rotleft: 对64位无符号整数x进行循环左移n位。
// 参数：
//   x - 需要循环左移的64位无符号整数
//   n - 左移的位数
// 返回值：
//   循环左移后的结果

static inline uint64_t _rotleft(uint64_t x, int n) {
  return (x << n) | (x >> (64 - n));
}
// randseed: 初始化随机数生成器的状态。
// 参数：
//   state - 指向随机数状态结构体的指针
//   seed1 - 第一个种子值
//   seed2 - 第二个种子值

void randseed(randstate_t *state, uint64_t seed1, uint64_t seed2) {
  uint64_t *s = state->s;
  s[0] = seed1;
  s[1] = 0xff;
  s[2] = seed2;
  s[3] = 0;
}
// randnext: 生成下一个随机数，并更新状态。
// 参数：
//   state - 指向随机数状态结构体的指针
// 返回值：
//   生成的64位无符号随机数

uint64_t randnext(randstate_t *state) {
  uint64_t *s = state->s;
  const uint64_t result = _rotleft(s[1] * 5, 7) * 9;
  const uint64_t t = s[1] << 17;
  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];
  s[2] ^= t;
  s[3] = _rotleft(s[3], 45);
  return result;
}
// project: 将随机数ran投影到[0, n]区间内，保证均匀分布。
// 参数：
//   ran   - 原始随机数
//   n     - 区间上界
//   state - 指向随机数状态结构体的指针
// 返回值：
//   投影后的随机数，范围为[0, n]

static uint64_t project(uint64_t ran, uint64_t n, randstate_t *state) {
  if ((n & (n + 1)) == 0) {
    return ran & n;
  } else {
    uint64_t lim = n;
    lim |= (lim >> 1);
    lim |= (lim >> 2);
    lim |= (lim >> 4);
    lim |= (lim >> 8);
    lim |= (lim >> 16);
    lim |= (lim >> 32);
    while ((ran &= lim) > n) {
      ran = randnext(state);
    }
    return ran;
  }
}
// randintrange: 生成[a, b]区间内的随机整数（包含a和b）。
// 参数：
//   state - 指向随机数状态结构体的指针
//   a     - 区间下界
//   b     - 区间上界
// 返回值：
//   区间[a, b]内的随机整数

int64_t randintrange(randstate_t *state, int64_t a, int64_t b) {
  if (a == b)
    return a;
  uint64_t low = a < b ? a : b;
  uint64_t up = a < b ? b : a;
  uint64_t rv = randnext(state);
  uint64_t p = project(rv, up - low, state);
  return (int64_t)low + p;
}