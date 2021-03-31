/******************************************************************************
* FLAGS.H - SVC Parameter Block Flag and Mode Definitions 	Version 1.2.01
*******************************************************************************
* Copyright (c) 1985, Digital Research, Inc. All Rights Reserved. The Software
* Code contained in this listing is proprietary to Digital Research, Inc.,
* Monterey, California and is covered by U.S. and other copyright protection.
* Unauthorized copying, adaptation, distribution, use or display is prohibited
* and may be subject to civil and criminal penalties.  Disclosure to others is
* prohibited.  For the terms and conditions of software code use refer to the
* appropriate Digital Research License Agreement.
*******************************************************************************
* Version   Date   Who  Description
* ======= ======== ===  =======================================================
* v1.2.01 04/28/86 FRH	Add COMMAND and FE Mode Bits
* v1.2.00 04/16/86 FRH	Add this header
******************************************************************************/

	/* MODE Field */

#define M_ASYNC		0x01	/* Asynchronous function		     */
#define	M_UADDR		0x02	/* Addresses are in user space		     */
#define M_COMMAND	0x04	/* Originated by COMMAND RM		     */
#define M_FE		0x08	/* Originated by Front End		     */

	/* OPTION Field */

#define	O_FILE		0x00	/* CREATE - Disk or Message Pipe	     */
#define O_DIR		0x01	/* CREATE - Directory			     */
#define O_VCON		0x02	/* CREATE - Virtual Console		     */

	/* FLAGS Field (attributes) */

#define A_OPENMSK A_SET+A_EXEC+A_WRITE+A_READ+A_SHARE+A_SHRO+A_SHFP+A_REDUCE

		/* bit 0 */
#define A_CHARPL	0x0001	/* Character Plane - ALTER,COPY		     */
#define A_PCLOSE	0x0001	/* Partial Close - CLOSE		     */
#define A_BSCREEN	0x0001	/* Bit Map Screen - VCCREATE		     */
#define A_SYSTEM	0x0001	/* Set System Level (0=Process) - DEFINE     */
#define A_MCTRL		0x0001	/* Mouse Control (0=KB control) - KCTRL      */
#define A_FLUSH		0x0001	/* Flush before or after I/O - READ,WRITE    */
#define A_CLIP		0x0001	/* Clip to Current Window - RWAIT	     */
#define A_ABSOLUTE	0x0001	/* Absolute (0=Relative) - TIMER	     */
#define A_REPLACE	0x0001	/* Replace (0=Add) - XLAT		     */
#define A_SET		0x0001	/* Set - CREATE,INSTALL,OPEN		     */
#define A_HIDDEN	0x0001	/* Lookup Hidden Files			     */
				/* LOCK - See A_LCKMSK			     */

		/* bit 1 */
#define A_ATTRPL	0x0002	/* Attribute Plane - ALTER,COPY		     */
#define A_EXEC		0x0002	/* Execute Privs - OPEN, CREATE		     */
#define A_BBORDER	0x0002	/* Bit map Borders - VCCREATE		     */
#define A_RETURN	0x0002	/* Return (0=Set) - DEFINE		     */
#define A_DELIM		0x0002	/* Read until Delimiter - READ		     */
#define A_TRUNCATE	0x0002	/* Truncate File - WRITE		     */
#define A_EXIT		0x0002	/* Wait for exit (0=entry) - RWAIT	     */
#define A_SYSFILE	0x0002	/* Lookup System Files			     */
				/* LOCK - See A_LCKMSK			     */

		/* bit 0 and 1 */

#define A_LCKMSK	0x0003	/* Lock Mask - LOCK			     */
#define	A_UNLOCK	0x0000	/* Unlock				     */
#define A_EXLOCK	0x0001	/* Exlusive Lock			     */
#define	A_EWLOCK	0x0002	/* Exclusive Write Lock			     */
#define	A_SWLOCK	0x0003	/* Shared Write Lock			     */

		/* bit 2 */
#define A_EXTPL		0x0004	/* Extension Plane - ALTER,COPY		     */
#define A_WRITE		0x0004	/* Write - CREATE,INSTALL,OPEN		     */
#define A_SIZE		0x0004  /* Size Spec (0=same as parent) - VCCREATE   */
#define A_NODESCT	0x0004	/* Non-Destructive - READ		     */
#define A_VOLUME	0x0004	/* Lookup Volume Label			     */

		/* bit 3 */
#define A_READ		0x0008	/* Read - CREATE,INSTALL,OPEN		     */
#define A_DELSC		0x0008	/* Remove Parent Screen - VCCREATE	     */
#define A_PREINIT	0x0008	/* Preinitialized - READ		     */
#define A_INCLDIR	0x0008	/* Lookup DIR files			     */

		/* bit 4 */
#define A_NOPROC	0x0010	/* No process - COMMAND			     */
#define A_SHARE		0x0010	/* Shared - CREATE,INSTALL,OPEN		     */
#define A_DELINCL	0x0010	/* Include Delimiter - READ		     */
#define A_EXCLNORM	0x0010	/* Don't Lookup Normal files		     */

		/* bit 5 */
#define A_CHAIN		0x0020	/* Chain (0=procedure) - COMMAND	     */
#define A_SHRO		0x0020	/* Allow R/O shared (0=Allow R/W) - CREATE,  */
				/*	OPEN				     */
#define A_REMOVE	0x0020	/* Removeable Driver - INSTALL		     */
#define A_EDIT		0x0020	/* Edited - READ			     */

		/* bit 6 */
#define A_MEMKEEP	0x0040	/* No not Release Memory - COMMAND	     */
#define A_SHFP		0x0040	/* Share FP with Family (0=unique) - OPEN,   */
				/*	CREATE				     */
#define A_DEVLOCK	0x0040	/* Device Locks Allowed - INSTALL	     */

		/* bit 7 */
#define A_NOEXCL	0x0080	/* No Exclusive Access - INSTALL	     */
#define A_ZFILL		0x0080	/* Zero Fill Space - CREATE, WRITE	     */
#define A_REDUCE	0x0080	/* Allow Reduced Access - OPEN		     */
#define	A_NEWFMLY	0x0080	/* Create a new family - COMMAND	     */

		/* bit 8 */
#define A_PART		0x0100	/* Partitions enabled - INSTALL		     */
#define A_TEMP		0x0100	/* Delete on Last Close - CREATE, VCCREATE   */
				/* READ, WRITE, SEEK, LOCK - see A_OFFMSK    */

		/* bit 9 */
#define A_VERIFY	0x0200	/* Verify Writes on this media - INSTALL     */
#define	A_CONTIG	0x0200	/* Contiguous File - CREATE		     */
				/* READ, WRITE, SEEK, LOCK - see A_OFFMSK    */

		/* bit 8 and 9 */
#define A_OFFMSK	0x0300	/* Offset Mask - READ, WRITE, SEEK, LOCK     */
#define A_BOFOFF	0x0000	/* Relative to Beginning of File	     */
#define A_FPOFF		0x0100	/* Relative to File Pointer		     */
#define A_EOFOFF	0x0200	/* Relative to End of File		     */

		/* bit 10 */
#define	A_DELETE	0x0400	/* Delete File if exists - CREATE	     */

		/* bit 11 */
#define A_OEM		0x0800	/* OEM specific bit everywhere - for internal*/
				/*	use only, must be 0 from user entry  */

		/* bit 12 */
#define A_SECURITY	0x1000	/* Use specified Security Word - CREATE	     */

		/* bit 13 */
#define A_FORCE		0x2000	/* Force Case - COMMAND, CREATE, DELETE	     */
				/*	INSTALL, LOOKUP, OPEN, RENAME	     */

		/* bit 14 */
#define A_LIT		0x4000	/* Literal Name - COMMAND,CREATE,DELETE,     */
				/*	INSTALL,LOOKUP,OPEN,RENAME,	     */
				/*	Reserved for DEFINE		     */

		/* bit 15 */
#define A_UADDR		0x8000	/* Use to tell drivers that buffer is a	     */
				/*	user address			     */

/

		/* bit 15 */
#define A_UADDR
x8000	/* Use to tell drivers that buffer is a	     */
				/*	user address			     */

/

		/* bit 15 */
#define A_UADDR