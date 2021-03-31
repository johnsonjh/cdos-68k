/*	@(#)vmqueue.h	1.3		*/
/****************************************************************************
 *
 *	These are the variables that control the ring buffer for
 *	port driver types.
 *
 *****************************************************************************/

#define BIGQUEUE 0

#if BIGQUEUE

#define	QBUFSIZ	512			/* Size of input and output buffers */

#define QHIWATER	512-50	/* High water mark - we're getting full boys */
#define QLOWATER	50	/* Yes, you can start filling us up again */

#else

#define	QBUFSIZ	80			/* Size of input and output buffers */

#define QHIWATER	60	/* High water mark - we're getting full boys */
#define QLOWATER	20	/* Yes, you can start filling us up again */

#endif

#define	QUEUE	struct _QueUeStrUct

QUEUE
{
	WORD	q_head;
	WORD	q_tail;
	UWORD	q_count;
	LONG	q_overflow;
	LONG	q_underflow;
	UBYTE	q_buffer[QBUFSIZ];
};

#define qempty(q)	((q)->q_count == 0)
#define qfull(q)	((q)->q_count >= QBUFSIZ)
#define qcnt(q)		((q)->q_count)
#define qhiwater(q)	((q)->q_count >= QHIWATER)
#define qlowater(q)	((q)->q_count <= QLOWATER)

UBYTE	dequeue();
VOID	enqueue();
QUEUE	*getqueue();
q_count >= QHIWATER)
#define qlowater(q)	((q)->q_count <= QLOWATER)

UBYTE	dequeue();
VOID	enqueue