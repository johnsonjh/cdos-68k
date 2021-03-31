/*	@(#)vmqueue.c	1.3		*/
/*
 *	General Queue Routines for use with Concurrent DOS-68K
 *	drivers on the VME/10.
 *
 *	Any critical region protection required when manipulating
 *	a queue is driver dependent and is assumed to be handled
 *	by the caller.
 *
 *	Additionally, the user of these routines are expected to
 *	check qfull() and qempty() before calling enqueue() and
 *	dequeue(), respectively.  We don't return errors and silently
 *	ignore the requests.
 */

#include "portab.h"
#include "io.h"
#include "vmqueue.h"

/*
 *	Allocate and initialize a queue, return a pointer
 *	to the QUEUE structure.
 */

QUEUE	*getqueue()
{
	REG QUEUE	*q;

	q = (QUEUE *)salloc((LONG)sizeof(QUEUE));

	q->q_head = 0;
	q->q_tail = 0;
	q->q_count = 0;
	q->q_underflow = 0;
	q->q_overflow = 0;

	return q;
}

/*
 *	Put a character in the queue.
 */

VOID enqueue(q,c)
REG QUEUE	*q;
UBYTE	c;
{
	if( qfull(q) )
	{
		q->q_overflow++;
		return;
	}
	
	q->q_buffer[q->q_head++] = c;
	q->q_count++;

	if( q->q_head >= QBUFSIZ )	/* wrap queue around */
		q->q_head = 0;
}

/*
 *	Pull a character from the queue.
 */

UBYTE dequeue(q)
REG QUEUE	*q;
{
	UBYTE	c;

	if( qempty(q) )
	{
		q->q_underflow++;
		return '\0';		/* as good as anything to return */
	}

	c = q->q_buffer[q->q_tail++];
	q->q_count--;

	if( q->q_tail >= QBUFSIZ )
		q->q_tail = 0;
	
	return c;
}

*/
	}

	c = q->q_buffer[q->q_tail++];
	q->q_count--;

	if( q->q_tail >= QBUFSIZ )
		q->q_tail =

	
	return c;
}

*/
	}

	c = q->q_buffer[q->q_tail++];
	q->q_count--;

	if( q->q_tail >= QBUFSIZ )
		q->q_tail =