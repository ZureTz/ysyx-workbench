#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  // 从键盘数据寄存器读取按键码
  uint32_t key_data = inl(KBD_ADDR);

  // 提取按键状态：最高位表示按下(1)或释放(0)
  kbd->keydown = (key_data & KEYDOWN_MASK) != 0;

  // 提取按键码：低15位为键盘码
  kbd->keycode = key_data & ~KEYDOWN_MASK;
}
