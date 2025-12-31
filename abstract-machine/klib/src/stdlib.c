#include <am.h>
#include <klib-macros.h>
#include <klib.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) { next = seed; }

int abs(int x) { return (x < 0 ? -x : x); }

int atoi(const char *nptr) {
  int x = 0;
  while (*nptr == ' ') {
    nptr++;
  }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  // 维护一个静态变量记录当前堆的位置
  static uintptr_t hbrk = 0;

  // 第一次调用时，初始化 hbrk 为 heap.start（8字节对齐）
  if (hbrk == 0) {
    hbrk = (uintptr_t)ROUNDUP((uintptr_t)heap.start, 8);
  }

  // 将 size 向上对齐到 8 字节边界
  size = (size_t)ROUNDUP(size, 8);

  // 保存当前位置作为返回值
  void *ptr = (void *)hbrk;

  // 更新 hbrk 指向下一个可用位置
  hbrk += size;

  // 检查是否超出堆区范围
  assert((uintptr_t)hbrk <= (uintptr_t)heap.end);

  return ptr;
#endif
  return NULL;
}

void free(void *ptr) {
  // 简单实现：不做任何操作，只分配不释放
}

#endif
