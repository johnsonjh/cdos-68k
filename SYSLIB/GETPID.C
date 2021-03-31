/* GPD.C -- 01/02/85 -- ES = 7.00 */
/*=========================================================*
 *     GETPID() -- Get the calling PID from the ENVIRON    *
 *               table. PID is returned as a LONG.         *
 *		 K&R standard.                             *
 *       Usage:                                            *
 *                 LONG	pid;                               *
 *                                                         *
 *                 pid = getpid();                         *
 *---------------------------------------------------------*
 *	VERSION	DATE		AUTHOR	COMMENTS           *
 *---------------------------------------------------------*
 *	0.01	09/19/84	cpg     First Try.         *
 *      0.20    01/02/85        cpg     Modified for QALIB.*
 *=========================================================*
 *  INCLUDES:                                              */
#include "portab.h"

#define E_SUCCESS 0L
#define	E_TAB_NO  01
#define ET_BUFSIZ 32

EXTERN  LONG	s_get();

struct	environ
    {
	LONG	std_out;	/* fnum for stdout         */
	LONG	std_in;		/*   "   "  stdin          */
	LONG	std_err;	/*   "   "  stderr         */
	LONG	ovr_ly;		/*   "   "  loading prog.  */
	BYTE	user,group;
	WORD	f_id;		/* process family ID       */
	LONG	pid;		/* process ID (current)    */
	WORD	rn_id;		/* requestor node ID       */
	WORD	rf_id;		/* requestor family ID     */
	LONG	r_pid;		/* requestor process ID    */
    }  en_ptr;

LONG	getpid()
{
	LONG	sg_ret;

	if( ( sg_ret = s_get(E_TAB_NO,0L,&en_ptr,ET_BUFSIZ)) < E_SUCCESS )
	   {
		return( sg_ret );
	   } else return( en_ptr.pid );

}
