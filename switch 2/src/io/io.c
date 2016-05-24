#include "../basic/basic.h"

ssize_t writen(int fd, const void *buf, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	ptr = buf;
	nleft = n;
	while (nleft > 0)
	{
		if ((nwritten = write(fd, ptr, nleft)) <= 0) 
		{
			if ((nwritten < 0) && (errno == EINTR)) 
				nwritten = 0; 	    
       			else 
				return(-1);	    /* errore */
		}
		nleft -= nwritten;
		ptr += nwritten;  
	}
	return(n-nleft);	/* restituisce >= 0 */
}



ssize_t readn(int fd, void *buf, size_t n)
{
	size_t  nleft;
	ssize_t nread;
	char *ptr;
	ptr = buf;
	nleft = n;
	while (nleft > 0) 
	{
		if ((nread = read(fd, ptr, nleft)) < 0) 
		{
			if (errno == EINTR)
				nread = 0;
			else
				return(-1);
		} 
		else if (nread == 0)
		break;	/* EOF */
		nleft -= nread;
		ptr += nread;
	}
	return(n-nleft);	/* restituisce >= 0 */
}



int myflock(int fd,int cmd)
{
	struct flock fl;
	fl.l_whence=SEEK_SET;
	fl.l_start=0;
	fl.l_len=0;
	if (cmd==LOCK_EX)
		fl.l_type=F_WRLCK;
	else
		fl.l_type=F_UNLCK;
	return fcntl(fd,F_SETLKW,&fl);
}



