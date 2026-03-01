unsigned long timestamp_float64_to_fixed3232(unsigned long b64) {
  unsigned long exp = (b64 >> 52 & 0xfffUL) - 0x41d;
  if (exp >= 2)
    return 0;
  return ((b64 & 0xfffffffffffffUL)
          + 0x10000000000000UL) << (exp + 10);
}
