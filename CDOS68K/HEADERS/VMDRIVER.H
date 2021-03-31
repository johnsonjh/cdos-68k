/*	@(#)vmdriver.h	1.15		*/
/*
 *	Concurrent 4.0 driver header file
 *
 *	This file complies with External Specification
 *	Revision 7.00  dated Dec 7, 1984
 */

/*	driver header table */
typedef struct
{
	WORD	drvrresvd;	/* reserved				*/
	BYTE	maxunits;	/* max number of units driver supports	*/
	BYTE	drvrflgs;	/* driver level interface information	*/
	LONG	(* init)();	/* init entry point			*/
	LONG	(* subdrive)();	/* subdrive entry point			*/
	LONG	(* uninit)();	/* uninit entry point			*/
	LONG	(* select)();	/* select entry point			*/
	LONG	(* flush)();	/* flush entry point			*/
	LONG	(* read)();	/* read entry point			*/
	LONG	(* write)();	/* write entry point			*/
	LONG	(* get)();	/* get entry point			*/
	LONG	(* set)();	/* set entry point			*/
	LONG	(* special)();	/* special entry point			*/
	LONG	dreserved[3];	/* reserved				*/
	BYTE	**drvrrlrp;	/* addr of ready list root in OS data	*/
	LONG	(** services)();/* addr of driver services table	*/
} DRVRHDR;

/*	driver flags */
#define	DFSYNCD	1	/* synchronize at driver level, otherwise re-entrant */
#define	DFSYNCU	2	/* synchronize at unit level, otherwise re-entrant */
#define DFSYNCR 4	/* synchronize at resource manager, otherwise re-ent */
#define DFWORDD 8	/* word oriented device, otherwise byte device	*/
#define	USRADDR	0x8000	/* address is user space address		*/
#define	SYSADDR	0	/* address is system space address		*/

/*	driver entry routine parameter blocks */

/*	subdrive parameter block */
typedef struct
{
	BYTE	subunit;	/* driver unit that subdriver is for	*/
	BYTE	subdunit;	/* subdriver unit number		*/
	WORD	subaccess;	/* INSTALL access flags of subdriver	*/
	DRVRHDR	*subheader;	/* address of subdriver's driver header	*/
} SUBDRVPB;


/*	driver services */

/*	flag system */
LONG	flagclr();	/* clear a flag				*/
LONG	flagevent();	/* get an event mask for later waiting	*/
LONG	flagget();	/* allocate a flag			*/
LONG	flagrel();	/* deallocate a flag			*/
LONG	flagset();	/* set a flag				*/

/*	asynchronous service routines */
VOID	doasr();	/* schedule asr for execution		*/
VOID	dsptch();	/* force a dispatch			*/
VOID	nextasr();	/* schedule asr for execution on event	*/

/*	device polling */
LONG	pollevent();	/* establish poll routine		*/

/*	memory management */
BYTE	*mapphys();	/* get specified physical memory into system memory */
VOID	mapu();		/* replace current user's memory with that specified */
VOID	mlock();	/* lock current user memory into physical memory */
LONG	mrange();	/* check that memory range is within user memory */
VOID	munlock();	/* unlock current user memory from physical memory */
BYTE	*paddr();	/* convert system address to physical address */
BYTE	*saddr();	/* convert user address to system address */
BYTE	*salloc();	/* allocate system memory from tpa	*/
LONG	sfree();	/* free memory allocated by salloc	*/
BYTE	*uaddr();	/* convert system address to user address */
VOID	unmapu();	/* restore calling process's user space	*/

/*	critical regions */
LONG	mxevent();	/* try to obtain access to a mutual exclusion region */
LONG	mxinit();	/* obtain a mutual exclusion parameter block */
LONG	mxrel();	/* release a mutual exclusion region	*/
LONG	mxuninit();	/* release a mutual exclusion parameter block */
VOID	noabort();	/* begin a no abort region		*/
VOID	nodisp();	/* begin a no dispatch region		*/
VOID	okabort();	/* end a no abort region		*/
VOID	okdisp();	/* end a no dispatch region		*/

/*	system process creation */
LONG	pcreate();	/* create a system process		*/

/*	interrupt service routines */
VOID	setvec();	/* attach an isr to an interrupt vector	*/

/*	operating system functions */
LONG	supif();	/* call an operating system function	*/


/*	driver services parameter blocks */

/*	mapphys parameter block */
typedef struct
{
	LONG	mpzero;
	BYTE	*pbegaddr;
	LONG	plength;
} MAPPB;
/*	mapphys memory type */
#define MAPCODE	0	/* physical memory contains code	*/
#define MAPDATA	1	/* physical memory contains data	*/

/*	ISR's can cause immediate dispatch (return 1), or not (return 0) */
#define NO_DISPATCH	0	/* do not dispatch on ISR return	*/
#define DISPATCH	1	/* dispatch on ISR return		*/

/*
 *	Parameter block used to call the supif supervisor
 *	timer function.  Used in the keyboard driver.
 */

struct timeblk
{
	BYTE	t_async;
	BYTE	t_zero;
	WORD	t_flags;
	LONG	t_swi;
	LONG	t_time;
};

#define F_TIMER	24	/* supif function number for TIMER function */


/*	error return definitions */
#define E_SUCCESS	0L	/* no error				*/
#define SUCCESS		0L	/* another way to say no error		*/
#define E_IMPLEMENT	0x4009L	/* this function not implemented	*/
#define DEVERR(DEV, ERR) ((LONG)(0x80000000 | (DEV << 16) | ERR))

 E_IMPLEMENT	0x4009L	/* this function not implemented	*/
#de