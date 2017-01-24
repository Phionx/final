#include "comms.h"
#include <string.h>
char *addHeader(char *to, header head, char *data) {
  int len = strlen(data);
  to[len + 1] = 0;
  for(; len > 0; len--) {
    to[len] = data[len - 1];
  }
  to[0] = head;
  return to;
}

header remHeader(char *data) {
  header killed = data[0];
  strcpy(data, data + 1);
  return killed;
}
