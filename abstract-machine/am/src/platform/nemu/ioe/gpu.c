#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // 初始化时可以做一些测试（可选）
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  // 从 VGACTL_ADDR 读取屏幕大小寄存器
  // 格式：高16位是width，低16位是height
  uint32_t screen_info = inl(VGACTL_ADDR);
  uint32_t width = screen_info >> 16;     // 获取高16位
  uint32_t height = screen_info & 0xFFFF; // 获取低16位

  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = width,
                           .height = height,
                           .vmemsz = width * height * sizeof(uint32_t)};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // 获取屏幕宽度（用于计算帧缓冲地址）
  uint32_t screen_info = inl(VGACTL_ADDR);
  uint32_t screen_width = screen_info >> 16;

  // 计算帧缓冲的起始地址和像素数据的指针
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pixels = (uint32_t *)ctl->pixels;

  // 逐行绘制矩形图像
  // 从 (x, y) 开始，绘制 w*h 的矩形
  for (int j = 0; j < ctl->h; j++) {
    for (int i = 0; i < ctl->w; i++) {
      // 计算屏幕上的位置：(x+i, y+j)
      int screen_offset = (ctl->y + j) * screen_width + (ctl->x + i);
      // 计算像素数据的位置（按行优先）
      int pixel_offset = j * ctl->w + i;
      // 写入像素
      fb[screen_offset] = pixels[pixel_offset];
    }
  }

  // 如果需要同步，写入同步寄存器
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
