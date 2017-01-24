#include "comms.h"
#include <string.h>
#include <fcntl.h>


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

setBlocking(int fd, int blocking) {
  int flags = fcntl(fd, F_GETFL, 0);
  if(blocking)
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);  // make reading from sd blocking
  else
    fcntl(fd, F_SETFL, flags |  O_NONBLOCK);
}