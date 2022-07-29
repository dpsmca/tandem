#ifndef BASE64_H
#define BASE64_H

#ifndef _STRING_H_
  #include <cstring>
#endif

int b64_decode_mio (char *dest, char *src, size_t size);
#endif /* BASE64_H */
