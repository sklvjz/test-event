#include <errno.h>
#include <math.h>  
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

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
static void
fifo2_timetout_func(EventSelector *es,
		 int fd,
		 unsigned int flags,
		 void *data)
{
    EventHandler *handler = (EventHandler *)data;
    printf("Fifo2 timeout\n");
    //Event_ChangeTimeout(handler, t);
}


static void
timeout_func(EventSelector *es,
		 int fd,
		 unsigned int flags,
		 void *data)
{
    printf("current time:%d\n",time(NULL));
    EventHandler *handler = (EventHandler *)data;
    struct timeval t;
    t.tv_sec = 2;
    t.tv_usec = 0;
    //Event_ChangeTimeout(handler, t);
}

int main ( int argc, char *argv[] )
{
	struct stat st;
	const char *fifo = "event.fifo";
	const char *fifo2 = "event.fifo2";
	const char *fifo3 = "event.fifo3";
	int socket;
	int socket2;
	int socket3;
 
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
	if (lstat (fifo3, &st) == 0)
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
	unlink (fifo3);
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
	if (mkfifo (fifo3, 0600) == -1)
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
	if (socket2 == -1) 
	{
		perror("open");
		exit (1);
	}
	socket3 = open (fifo3, O_RDWR | O_NONBLOCK, 0);
	if (socket3 == -1) 
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
    //struct timeval t;
    //t.tv_sec = 2;
    //t.tv_usec = 0;
    //EventHandler *event_handle3 = Event_AddTimerHandler(event_selector,
    //                    t,
    //                    timeout_func,
    //                    event_handle3);
    //struct timeval t2;
    //t2.tv_sec = 2;
    //t2.tv_usec = 0;
    //EventHandler * event_handle4 = Event_AddHandlerWithTimeout(event_selector,
    //                    socket3,
    //                    EVENT_FLAG_READABLE,
    //                    t2,
    //                    fifo2_timetout_func,
    //                    NULL);

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

