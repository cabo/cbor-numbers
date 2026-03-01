unsigned long timestamp_float32_to_fixed3232(unsigned long b32) {
  unsigned long exp = (b32 >> 23 & 0x1ffUL) - 0x9d;
  if (exp >= 2)
    return 0;
  return ((b32 & 0x7fffffUL) + 0x800000UL) << (exp + 39);
}
