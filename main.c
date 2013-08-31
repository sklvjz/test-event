#include <errno.h>
#include <math.h>  
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "event.h"

/* Event Selector */
EventSelector *event_selector;

static void
fifo_read(EventSelector *es,
		 int fd,
		 unsigned int flags,
		 void *data)
{
	char buf[255];
	int len;

    memset(buf,0,sizeof(buf));
	len = read(fd, buf, sizeof(buf) - 1);
	if (len == -1) 
    {
		perror("read");
		return;
	} 
    else if (len == 0) 
    {
		fprintf(stderr, "Connection closed\n");
		return;
	}

	buf[len] = '\0';
    printf("func:%s,line:%d, %s\n",__func__,__LINE__, buf);
}
static void
fifo2_read(EventSelector *es,
		 int fd,
		 unsigned int flags,
		 void *data)
{
	char buf[255];
	int len;

    memset(buf,0,sizeof(buf));
	len = read(fd, buf, sizeof(buf) - 1);
	if (len == -1) 
    {
		perror("read");
		return;
	} 
    else if (len == 0) 
    {
		fprintf(stderr, "Connection closed\n");
		return;
	}

	buf[len] = '\0';
    printf("func:%s,line:%d, %s\n",__func__,__LINE__, buf);
}



int main ( int argc, char *argv[] )
{
	struct stat st;
	const char *fifo = "event.fifo";
	const char *fifo2 = "event.fifo2";
	int socket;
	int socket2;
 
	if (lstat (fifo, &st) == 0)
	{
		if ((st.st_mode & S_IFMT) == S_IFREG)
        {
			errno = EEXIST;
			perror("lstat");
			exit (1);
		}
	}
	if (lstat (fifo2, &st) == 0)
	{
		if ((st.st_mode & S_IFMT) == S_IFREG)
        {
			errno = EEXIST;
			perror("lstat");
			exit (1);
		}
	}

	unlink (fifo);
	unlink (fifo2);
	if (mkfifo (fifo, 0600) == -1)
	{
		perror("mkfifo");
		exit (1);
	}
	if (mkfifo (fifo2, 0600) == -1)
	{
		perror("mkfifo");
		exit (1);
	}

	socket = open (fifo, O_RDWR | O_NONBLOCK, 0);
	if (socket == -1) 
	{
		perror("open");
		exit (1);
	}
	socket2 = open (fifo2, O_RDWR | O_NONBLOCK, 0);
	if (socket == -1) 
	{
		perror("open");
		exit (1);
	}

    event_selector = Event_CreateSelector();
    if (!event_selector) 
    {
        printf("Could not create EventSelector -- probably out of memory\n");
    }

	EventHandler *event_handle = Event_AddHandler(event_selector,
					    socket,
					    EVENT_FLAG_READABLE,
					    fifo_read,
					    &socket);
	EventHandler *event_handle2 = Event_AddHandler(event_selector,
					    socket2,
					    EVENT_FLAG_READABLE,
					    fifo2_read,
					    &socket2);

    for(;;)
    {
        if( Event_HandleEvent(event_selector) < 0 )
        {
            printf("Event_HandleEvent");
        }
    }

    Event_DestroySelector(event_selector);
	return EXIT_SUCCESS;
}

