#ifndef C_RAND_H
#define C_RAND_H
#include <stdint.h>  // 提供整数类型定义
#include <float.h>  // 提供浮点数常量如DBL_MANT_DIG
#include <stdint.h> // 提供固定宽度整数类型
#include <sys/stat.h> // 提供文件状态相关功能
#include <time.h>  // 提供时间相关功能

/*
shift64_FIG: 用于去除超出双精度浮点数尾数位数的额外位数。
*当处理64位随机数值时，需要丢弃多余的位来适应浮点数的精度。
*/
#define shift64_FIG (64 - DBL_MANT_DIG)
/* scaleFIG: 用于将随机数值缩放到[0, 1)区间。
 *  通过乘以2^(-DBL_MANT_DIG)将整数随机数转换为适当范围的浮点数。
 */
#define scaleFIG (0.5 / ((uint64_t)1 << (DBL_MANT_DIG - 1)))

/**
 * @brief 随机数生成器状态结构体
 * 存储随机数生成所需的内部状态
 */
typedef struct randstate {
  uint64_t s[4]; // 随机数生成器的内部状态数组
} randstate_t;

/**
 * @brief 使用给定的种子初始化随机数生成器
 * @param state 随机数生成器状态
 * @param seed1 第一个种子值
 * @param seed2 第二个种子值
 */
void randseed(randstate_t *state, uint64_t seed1, uint64_t seed2);

/**
 * @brief 生成下一个64位无符号随机整数
 * @param state 随机数生成器状态
 * @return 64位无符号随机整数
 */
uint64_t randnext(randstate_t *state);

/**
 * @brief 生成一个64位有符号随机整数
 * @param state 随机数生成器状态
 * @return 64位有符号随机整数
 */
static inline int64_t randint(randstate_t *state) {
  return (int64_t)randnext(state);
}

/**
 * @brief 生成一个[0,1)范围内的随机浮点数
 * @param state 随机数生成器状态
 * @return 双精度随机浮点数
 */
static inline double randfloat(randstate_t *state) {
  uint64_t x = randnext(state);
  return (double)(x >> shift64_FIG) * scaleFIG;
}

/**
 * @brief 生成指定范围[a,b)内的随机浮点数
 * @param state 随机数生成器状态
 * @param a 范围下限
 * @param b 范围上限
 * @return 范围内的随机浮点数
 */
static inline double randfltrange(randstate_t *state, double a, double b) {
  double low = a < b ? a : b;
  double up = a < b ? b : a;
  return low + (up - low) * randfloat(state);
}

/**
 * @brief 生成指定范围内的随机整数
 * @param state 随机数生成器状态
 * @param a 范围下限
 * @param b 范围上限
 * @return 范围内的随机整数
 */
int64_t randintrange(randstate_t *state, int64_t a, int64_t b);

#endif // C_RAND_H