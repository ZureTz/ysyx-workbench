#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// Helper function to convert integer to string
static int int_to_str(char *buf, int bufsize, long long num, int base,
                      int is_signed, int uppercase) {
  char tmp[64];
  int i = 0;
  int is_negative = 0;
  unsigned long long unum;

  if (is_signed && num < 0) {
    is_negative = 1;
    unum = -num;
  } else {
    unum = (unsigned long long)num;
  }

  if (unum == 0) {
    tmp[i++] = '0';
  } else {
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    while (unum > 0) {
      tmp[i++] = digits[unum % base];
      unum /= base;
    }
  }

  int len = i + (is_negative ? 1 : 0);
  if (len >= bufsize) {
    return len; // Would overflow
  }

  int pos = 0;
  if (is_negative) {
    buf[pos++] = '-';
  }

  while (i > 0) {
    buf[pos++] = tmp[--i];
  }
  buf[pos] = '\0';

  return pos;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  size_t pos = 0;
  const char *p = fmt;

  if (n == 0) {
    return 0;
  }

  while (*p && pos < n - 1) {
    if (*p != '%') {
      out[pos++] = *p++;
      continue;
    }

    p++; // Skip '%'

    if (*p == '%') {
      out[pos++] = '%';
      p++;
      continue;
    }

    // Parse format specifier
    int width = 0;
    int precision = -1;
    int is_long = 0;
    int is_longlong = 0;
    int zero_pad = 0;

    // Check for flags
    if (*p == '0') {
      zero_pad = 1;
      p++;
    }

    // Parse width
    while (*p >= '0' && *p <= '9') {
      width = width * 10 + (*p - '0');
      p++;
    }

    // Parse precision
    if (*p == '.') {
      p++;
      precision = 0;
      while (*p >= '0' && *p <= '9') {
        precision = precision * 10 + (*p - '0');
        p++;
      }
    }

    // Parse length modifier
    if (*p == 'l') {
      is_long = 1;
      p++;
      if (*p == 'l') {
        is_longlong = 1;
        p++;
      }
    }

    // Process conversion specifier
    char temp[128];
    int len = 0;

    switch (*p) {
    case 'd':
    case 'i': {
      long long val;
      if (is_longlong) {
        val = va_arg(ap, long long);
      } else if (is_long) {
        val = va_arg(ap, long);
      } else {
        val = va_arg(ap, int);
      }
      len = int_to_str(temp, sizeof(temp), val, 10, 1, 0);
      break;
    }

    case 'u': {
      unsigned long long val;
      if (is_longlong) {
        val = va_arg(ap, unsigned long long);
      } else if (is_long) {
        val = va_arg(ap, unsigned long);
      } else {
        val = va_arg(ap, unsigned int);
      }
      len = int_to_str(temp, sizeof(temp), val, 10, 0, 0);
      break;
    }

    case 'x':
    case 'X': {
      unsigned long long val;
      if (is_longlong) {
        val = va_arg(ap, unsigned long long);
      } else if (is_long) {
        val = va_arg(ap, unsigned long);
      } else {
        val = va_arg(ap, unsigned int);
      }
      len = int_to_str(temp, sizeof(temp), val, 16, 0, (*p == 'X'));
      break;
    }

    case 'o': {
      unsigned long long val;
      if (is_longlong) {
        val = va_arg(ap, unsigned long long);
      } else if (is_long) {
        val = va_arg(ap, unsigned long);
      } else {
        val = va_arg(ap, unsigned int);
      }
      len = int_to_str(temp, sizeof(temp), val, 8, 0, 0);
      break;
    }

    case 'p': {
      void *ptr = va_arg(ap, void *);
      out[pos++] = '0';
      if (pos >= n - 1)
        break;
      out[pos++] = 'x';
      if (pos >= n - 1)
        break;
      len = int_to_str(temp, sizeof(temp), (unsigned long)ptr, 16, 0, 0);
      break;
    }

    case 's': {
      const char *s = va_arg(ap, const char *);
      if (s == NULL) {
        s = "(null)";
      }
      len = strlen(s);
      if (precision >= 0 && len > precision) {
        len = precision;
      }
      for (int i = 0; i < len && pos < n - 1; i++) {
        out[pos++] = s[i];
      }
      len = 0; // Already copied
      break;
    }

    case 'c': {
      char c = (char)va_arg(ap, int);
      out[pos++] = c;
      len = 0;
      break;
    }

    default:
      out[pos++] = *p;
      len = 0;
      break;
    }

    // Copy the formatted number with padding
    if (len > 0) {
      // Apply width padding for numbers
      int pad_len = width > len ? width - len : 0;
      char pad_char = zero_pad ? '0' : ' ';

      // Handle negative sign for zero padding
      if (zero_pad && temp[0] == '-') {
        if (pos < n - 1) {
          out[pos++] = '-';
        }
        // Add padding zeros
        for (int i = 0; i < pad_len && pos < n - 1; i++) {
          out[pos++] = '0';
        }
        // Copy the rest (skip the minus sign)
        for (int i = 1; i < len && pos < n - 1; i++) {
          out[pos++] = temp[i];
        }
      } else {
        // Normal padding
        for (int i = 0; i < pad_len && pos < n - 1; i++) {
          out[pos++] = pad_char;
        }
        for (int i = 0; i < len && pos < n - 1; i++) {
          out[pos++] = temp[i];
        }
      }
    }

    if (*p)
      p++;
  }

  out[pos] = '\0';
  return pos;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int printf(const char *fmt, ...) {
  char buf[4096]; // Temporary buffer for formatted output
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  // Output only what was written to buffer
  int output_len = ret < (int)sizeof(buf) ? ret : (int)sizeof(buf) - 1;
  for (int i = 0; i < output_len; i++) {
    putch(buf[i]);
  }

  return ret;
}

#endif
