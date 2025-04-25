//提供统一的流接口，用于各种数据源的读取操作

#ifndef _LUASTREAM_H
#define _LUASTREAM_H
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "conf.h"

//-----------------------------------------------------------------------------
// istream - 输入流实现

// 流读取函数类型定义 - 接收用户数据和大小指针，返回读取的数据块
typedef const char* (*stream_reader_f)(void *ud, size_t *sz);

// 输入流结构体 - 提供统一的数据读取接口
typedef struct istream {
	const char *p;		// 当前buff块的读取位置指针
	const char *b;		// 当前buff块的起始位置
	size_t n;			// 当前buff块还剩余的字节数
	size_t bn;			// 已经获取的总buff块大小
	stream_reader_f reader;  // 读取器函数指针
	void *ud;           // 用户数据指针，传递给reader
} istream_t;

// 初始化输入流对象
static inline void istream_init(istream_t *stm, stream_reader_f reader,  void *ud) {
	assert(reader != NULL);
	stm->reader = reader;
	stm->ud = ud;
	stm->p = NULL;
	stm->b = NULL;
	stm->n = 0;
	stm->bn = 0;
}

// 填充流缓冲区的内部函数
int _istream_fill(istream_t *stm);

// 从流中读取一个字符
// 返回读取的字符，如果到达流末尾则返回EOF
static inline int istream_getc(istream_t *stm) {
	if (stm->n-- > 0) {
		return (unsigned char)*stm->p++;
	} else if (_istream_fill(stm)) {
		stm->n--;
		return *stm->p++;
	} else
		return EOF;
}

// 获取流的当前读取位置（已读取的总字节数）
static inline size_t istream_pos(istream_t *stm) {
	return stm->bn - stm->n;
}

// 从流中读取n个字节到缓冲区b
// 返回实际读取的字节数
size_t istream_getb(istream_t *stm, void *b, size_t n);

//---------------------------------
// istrstream - 字符串输入流实现

// 字符串输入流的数据结构
typedef struct strreader {
	const char *str;    // 源字符串
	size_t sz;          // 字符串长度
} strreader_t;

// 字符串输入流的读取函数实现
const char* strreader_read(void *ud, size_t *sz);

// 初始化字符串输入流
static inline void istrstream_init(istream_t *stm, strreader_t *sreader, const char *str, size_t sz) {
	sreader->str = str;
	sreader->sz = sz;
	istream_init(stm, strreader_read, sreader);
}

//---------------------------------
// ifilestream - 文件输入流实现

#define FR_MIN_BUFFSZ 64  // 文件读取的最小缓冲区大小

// 文件输入流的数据结构
typedef struct filereader {
	FILE *f;		// 文件句柄
	char *b;		// 缓存区
	size_t bsz;		// 缓存区的总大小
	size_t n;		// 缓存区中的有效数据大小
} filereader_t;

// 文件输入流的读取函数实现
const char* filereader_read(void *ud, size_t *sz);

// 初始化文件输入流（使用已打开的文件）
static inline void ifilestream_init(istream_t *stm, filereader_t *freader, FILE* f, size_t bsz) {
	freader->f = f;
	freader->n = 0;
	freader->bsz = bsz < FR_MIN_BUFFSZ ? FR_MIN_BUFFSZ : bsz;
	freader->b = (char*)_malloc(bsz);
	istream_init(stm, filereader_read, freader);
}

// 初始化文件输入流（通过文件名打开）
// bsz指定缓冲区大小，如果为0表示尝试预加载整个文件内容(最多2G)
// 成功返回1，失败返回0
static inline int ifilestream_initf(istream_t *stm, filereader_t *freader, const char *fname, int bsz) {
	freader->f = NULL;
	freader->b = NULL;
	FILE *f = fopen(fname, "rb");
	if (f != NULL) {
		ifilestream_init(stm, freader, f, bsz);
		return 1;
	}
	return 0;
}

// 关闭文件输入流并释放资源
// 成功返回非0值，失败返回0
static inline int ifilestream_close(filereader_t *freader) {
	// printf("ifilestream_close, %p, %p\n", freader->b, freader->f);
	if (freader->b) {
		_free(freader->b);
		freader->b = NULL;
	}
	if (freader->f) {
		int ret = fclose(freader->f);
		freader->f = NULL;
		return ret;
	}
	return 1;
}

// 跳过文件开头的UTF8 BOM标记
void ifilestream_skipbom(filereader_t *freader);

//-----------------------------------------------------------------------------
// membuffer - 可增长的内存缓冲区实现

#define STACK_BUFF_SIZE 512  // 初始栈缓冲区大小

// 内存缓冲区结构体
typedef struct membuffer {
	char *b;		// 内存buffer指针
	size_t sz;		// buffer已用字节数
	size_t cap;		// buffer总容量
	char s[STACK_BUFF_SIZE];  // 初始栈空间，避免小缓冲区的堆分配
} membuffer_t;

// 初始化内存缓冲区
static inline void membuffer_init(membuffer_t *buff) {
	buff->b = buff->s;  // 初始使用栈空间
	buff->cap = STACK_BUFF_SIZE;
	buff->sz = 0;
}

// 减少缓冲区使用大小
static inline void membuffer_remove_size(membuffer_t *buff, size_t sz) {
	buff->sz -= sz;
}

// 增加缓冲区使用大小（不检查容量）
static inline void membuffer_add_size(membuffer_t *buff, size_t sz) {
	buff->sz += sz;
}

// 重置缓冲区（清空内容但保留容量）
static inline void membuffer_reset(membuffer_t *buff) {
	buff->sz = 0;
}

// 释放缓冲区分配的堆内存
static inline void membuffer_free(membuffer_t *buff) {
	if (buff->b && buff->b != buff->s) {
		_free(buff->b);
		buff->b = NULL;
	}
}

// 内部函数：增长缓冲区容量到指定大小
static inline void _membuffer_grow(membuffer_t *buff, size_t needsz) {
	if (buff->cap < needsz) {
		size_t newcap = buff->cap * 2;
		if (newcap < needsz)
			newcap = needsz;
		if (buff->b == buff->s) {
			buff->b = (char*)_malloc(newcap);
			memcpy(buff->b, buff->s, buff->sz);
		} else {
			buff->b = (char*)_realloc(buff->b, newcap);
		}
		buff->cap = newcap;
	}
}

// 确保缓冲区有足够空间容纳额外的sz字节
static inline void membuffer_ensure_space(membuffer_t *buff, size_t sz) {
	if (buff->sz + sz > buff->cap) {
		_membuffer_grow(buff, buff->sz+sz);
	}
}

// 向缓冲区添加一个字符（自动扩容）
static inline void membuffer_putc(membuffer_t *buff, char c) {
	membuffer_ensure_space(buff, 1);
	buff->b[buff->sz++] = c;
}

// 向缓冲区写入一段内存数据（自动扩容）
static inline void membuffer_putb(membuffer_t *buff, const void *b, size_t sz) {
	membuffer_ensure_space(buff, sz);
	memcpy(buff->b + buff->sz, b, sz);
	buff->sz += sz;
}

// 向缓冲区添加一个字符（不检查容量，调用前必须确保有足够空间）
static inline void membuffer_putc_unsafe(membuffer_t *buff, char c) {
	buff->b[buff->sz++] = c;
}

// 向缓冲区写入一段内存数据（不检查容量，调用前必须确保有足够空间）
static inline void membuffer_putb_unsafe(membuffer_t *buff, const void *b, size_t sz) {
	memcpy(buff->b + buff->sz, b, sz);
	buff->sz += sz;
}

// 获取缓冲区当前写入位置的指针
static inline char* membuffer_getp(membuffer_t *buff) {
	return buff->b + buff->sz;
}


#endif