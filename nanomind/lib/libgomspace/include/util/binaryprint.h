#ifndef BINARYPRINT_H_
#define BINARYPRINT_H_

#ifndef BYTETOBINARYPATTERN
#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#endif

#ifndef BYTETOBINARY
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)
#endif

#endif /* BINARYPRINT_H_ */
