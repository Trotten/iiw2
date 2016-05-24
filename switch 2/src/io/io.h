#ifndef IO_H
#define IO_H


ssize_t writen(int fd, const void *buf, size_t n);
ssize_t readn(int fd, void *buf, size_t n);
int myflock(int fd,int cmd);


#endif
