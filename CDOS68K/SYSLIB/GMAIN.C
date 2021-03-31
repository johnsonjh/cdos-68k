#define CTYPE
/*======================================================*
 *  Version 0.100  GMAIN.C                              *
 *                 (for ALPHA CDOS release)             *
 *------------------------------------------------------*
 *	Modules included:				*
 *	  fcreat();	fopen();	fclose();	*
 *	  puts();**	putchar();	fgetc();	*
 *	  fread();	getrc();**	getchar();	*
 *	  pmake();	strlen();	strcmp();	*
 *        fflush();     fputc();        fputs();        *
 *        fwrite();                                     *
 *      ** = non-standard				*
 *                                                      *
 *	General Purpose modules:                        *
 *	  ch_upper();  convert string to upper case     *
 *	  to_bin();    returns address to a printable   *
 * 	  	         16-bit binary string           *
 *	  clear();     initialize a buffer to value     *
 *        mikechar();  part of PRINTF support code	*
 *	  to_prnt();   returns printable characters or .* 
 *        do_err();    makes error codes integer compat.*
 *                                                      *
 *------------------------------------------------------*
 *  VERSION	DATE	  BY	COMMENTS                *
 *------------------------------------------------------*
 *   0.05	08/24/84  cpg   A public service        *
 *                                library.              *
 *   0.051       9/17/84  jmb   Eliminate double opens  *
 *                               of stdout and stdin    *
 *   0.052       9/27/84  jmb   Map PUTS to single call *
 *				of s_write(stdout..);   *
 *                              -->>NON-STANDARD K&R<<--*
 *                              -->>no \n appended  <<--*
 *   0.07	12/04/84  jns   Added ARGC,ARGV.        *
 *   0.08	12/17/84  jns	Added K&R STRCMP(s,t).	*
 *   0.081      12/17/84  cpg   Cleaned up for Alpha    *
 *   0.082      12/28/84  cpg   Added FFLUSH()          *
 *   0.083	01/08/85  jns   Repair ARGC/ARGV        *
 *   0.084	01/10/85  cpg   set flags rel to end    *
 *                               of file on writes and  *
 *                               pointer on reads.      *
 *   0.085	02/06/85  jns	Added FWRITE()          *
 *   0.086      02/07/85  jns   Added DO_ERR()          *
 *   0.087      02/14/85  jns   Cleaned up, integrated  *
 *                              into QALIB.             *
 *   0.088	02/27/85  jns   Fixed FGETC()           *
 *				Added APPEND to FOPEN   *
 *   0.100	05/06/85  cpg	Added char table __atab *
 *======================================================*
 *  INCLUDES:                                           */
#include "portab.h"
#include "ctyp40.h"
#include "system.h"

GLOBAL char	__atab[] = {
	0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,
	0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
	0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
	0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
	0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
	0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
	0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f	};

#define ENVIRON_T 1	/* table from which to get the fnums */
			/* for stdout and stdin.             */

/* defines for ARGC,ARGV processor */

#define CMD_SUCCESS 0L
#define	CMD_TAB_NO  0x44
#define CMDT_BUFSIZ 256L
#define CMD_PID     0L


EXTERN LONG	__OSIF(),s_read(),s_write(),
		s_open(),s_close(),s_create(),s_get();

GLOBAL	int	_argcret;	/* argc to main */
GLOBAL	char	*_argvret[64];	/* argv to main */
GLOBAL  LONG	_getail();

GLOBAL	LONG cnsO; 	/* file number of console output stream  */
GLOBAL	LONG cnsI; 	/* file number of console input stream   */

mainmain()
{
    WORD i;

    LONG ret;
    struct {
	LONG	stdin;
	LONG	stdout;
    } fnum;

    if (ret = s_get(ENVIRON_T,0L,&fnum,(LONG)sizeof(fnum)) < 0L)
		    return(E_NO_FILE);

    cnsO = fnum.stdout;		/* GLOBAL values for 'STDOUT'	     */
    cnsI = fnum.stdin;		/*   and 'STDIN'                     */
	
    if ( (ret = _getail()) < 0L)    /* Call ARGC,ARGV processing routine */
	{
	printf ("\n\rERROR on S_GET of CMDENV, Code = %08LX",ret);
	return (ret);
	}

    main(_argcret,_argvret);	/* Call main and pass ARGC,ARGV      */

    s_close(0,cnsO);
    s_close(0,cnsI);
}
/* ----------------------- END of mainmain --------------------  */


/*=========================================================*
 *  _GETAIL  --  Implement the ARGC function for 'C' from  *
 *               the CMDENV table.                         *
 *             The PID given is 0 to return the values for *
 *             the calling process.                        *
 *---------------------------------------------------------*
 *	VERSION	DATE		AUTHOR	COMMENTS           *
 *---------------------------------------------------------*
 *	1.0	11/30/84	jns     Integration        *
 *      1.1     12/27/84        cpg     Alpha definitions  *
 *	1.2	01/08/85	jns	Error handling     *
 *=========================================================*/

LONG _getail()
{
static	struct	cmdenv_t
	    {
		BYTE		cmd_file[128];	/*  the command file  */
		BYTE		cmd_tail[128];	/*  the command tail  */
	    }  cmd_ptr;

	LONG	sg_ret;
	int	x;

	/*  Get the CMDENV table for the current process in the       */
	/*    structure CMDENV_T                                      */

	if( (sg_ret = s_get(CMD_TAB_NO,CMD_PID,&cmd_ptr,CMDT_BUFSIZ)) < 0L )
	   {
		return(sg_ret);
	   }

	/*  ARGC,ARGV Processor  */

	_argcret = 1;
	_argvret[0] = &cmd_ptr.cmd_file[0];	/* argv[0]= command    */

	for (x=0; cmd_ptr.cmd_tail[x]; x++)	/* step thru the tail  */
						/*   die on first null */
	{
	  if (x == 0)				/* set up the 1st arg  */
	    {
	      _argvret[_argcret] = &cmd_ptr.cmd_tail[x];
	      ++_argcret;
	    }

	  if (cmd_ptr.cmd_tail[x] == 32)	/* Space? set to null   */
						/* set pointer to next  */
	    {
	      cmd_ptr.cmd_tail[x] = '\0';

	      if (cmd_ptr.cmd_tail[x+1] != 32)
		{
		  _argvret[_argcret] = &cmd_ptr.cmd_tail[x + 1];
		  ++_argcret;
		}
	    }
	}
	return(CMD_SUCCESS);
}

/*=========================================================*
 *            GENERAL PURPOSE UTILITIES                    *
 *=========================================================*/

/*	TO_PRNT() - return only printable characters.		*/
/*		    return periods for all non-printables.	*/

char to_prnt(i_byte)
	char	i_byte;
{
	if((i_byte < 0x20) || (i_byte > 0x7f))
	  {
	    return(0x2e);
	  }
	return(i_byte);
}

/*	TO_BIN() - convert to binary	    */
/*		returns address to a string */

LONG	to_bin(x)		/* This will convert an INTEGER */
	int x;			/* value to a string of binary  */
{				/* characters. to be printed as */
	int	knt = 0;	/* '%s' -- a string.            */
	int	b_msk = 0x8000;
static	char	z[17];

	while( knt < 16 )
	  {
	    if( x & b_msk )
	      {
		z[knt] = 0x31;
	      }
	      else
		z[knt] = 0x30;
	    x <<= 1;
	    knt++;
	  }
	z[knt++] = 0x0;
	return(z);
}

/*	CLEAR() - clear an array (set to specified byte).	*/

int	clear(c_area,c_len,c_byte)
	char	*c_area;
	char	c_byte;
	int	c_len;
{
	int	knt1;

	for (knt1 = 0; knt1 < c_len; knt1++)
	  {
	    c_area[knt1] = c_byte;
	  }

	return(knt1);
}

/*	CH_UPPER() - convert to upper case	*/

int	ch_upper(a_str)
	char	*a_str;
{
	int	len_str,knt2;
	char	*p;

	p = a_str;
	while (p++ != 0) len_str++;

	for (knt2 = 0; knt2 < p; knt2++)
	  {
	    a_str[knt2] = toupper(a_str[knt2]);
	  }

	return(knt2);
}

/*	DO_ERR() - convert error codes to integer compatible format	*
 *		   (must be cast as type WORD)				*/

LONG	do_err(x)

	LONG	x;
{
	x &= 0x0FFF;	/* get rid of everything above 3rd place */
	x = -x;
	return(x);
}

/*---------------------------------------------------------*
 *	MIKECHAR() -- an interesting abberation.           *
 *                    Support Code for the PRINTF function *
 *---------------------------------------------------------*/

int	mikechar(c,mode)
	BYTE	c;
	int	mode;	/* 0=normal write, 1=init, -1=dump */
{
	static	BYTE	buff[128];
	static	BYTE	*ptr = &buff[0];

	switch(mode)
	{
		case 0:
			*ptr++ = c;
			*ptr = '\0';
			break;		
		case 1:
			puts(&buff[0]);
			buff[0]='\0';
			ptr = &buff[0];
	}
	return(0);
}


/*=========================================================*
 *           Pseudo runtime library functions              *
 *=========================================================*/


/*---------------------------------------------------------*
 *	FCREAT() -- Create & open a file                   *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    Date      Name      Comments                         *
 *---------------------------------------------------------*
 *    09/25/84  cpg       First time thru                  *
 *=========================================================*/

LONG	fcreat(c_name,c_mode)
	BYTE	*c_name;
	int	c_mode;			/* Create flags	*/
{
	LONG	c_ret;
	BYTE	option;

	option = 0;			/* Disk or Pipe	*/

	c_ret = s_create(option,c_mode,c_name,0,0,0L);

	return(c_ret);

}

/*---------------------------------------------------------*
 *      Pipe create -- non standard call                   *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    Date      Name      Comments                         *
 *---------------------------------------------------------*
 *    09/25/84  cpg       First time thru                  *
 *=========================================================*/

LONG	pmake(p_name,p_mode,p_siz)
	BYTE	*p_name;
	int	p_mode;			/* Create flags	*/
	LONG	p_siz;
{
	LONG	p_ret;
	BYTE	option;

	option = 0;			/* Disk or Pipe create	*/

	p_ret = s_create(option,p_mode,p_name,0,0,p_siz);

	return(p_ret);

}

/*---------------------------------------------------------*
 *	FOPEN() -- a file open method                      *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    Date      Name      Comments                         *
 *---------------------------------------------------------*
 *    09/25/84  cpg       First time thru                  *
 *    02/27/84  jns       Change o_mode to char ptr        *
 *			  Check for valid open mode        *
 *			  Added APPEND functionality       *
 *			  Added CREATE on file not found   *
 *=========================================================*/
 
LONG	fopen(p_name,o_mode)	/* K&R standard open function */
	BYTE	*p_name, *o_mode;
{
	LONG	o_fn;
	int	mode_b;
	int	append;

	append = 0;	/* zero out the append flag */

	if (*o_mode != 'r' && *o_mode != 'w' && *o_mode != 'a')
		{
		printf("\r\nIllegal opening mode.");
		return(-1);
		}

	   if (*o_mode == 'r')
		mode_b = 0x2018;	/* Shared read/only access */

	   else if (*o_mode == 'w')
		mode_b = 0x201C;	/* Shared Read/Write */

	   	else if (*o_mode == 'a')
		    {
		    mode_b = 0x201C;	/* Read and write access   */
		    append = 1;		/* Set the append mode flag */
		    }

	o_fn = s_open(mode_b,p_name);

	if (o_fn == 0x80204010L)	/* file not found error */
		o_fn = fcreat(p_name,mode_b);

	if (append && (o_fn >= 0L))
		s_seek(0x0100,o_fn,0L);	/* Offset 0 relative to EOF */

	return(o_fn);
}

/*---------------------------------------------------------*
 *	FCLOSE() -- a file close method                    *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    Date      Name      Comments                         *
 *---------------------------------------------------------*
 *    09/25/84  cpg       First time thru                  *
 *    12/28/84  cpg       Added Flag for absolute close    *
 *                                                         *
 *=========================================================*/

LONG	fclose(c_strm)
	LONG	c_strm;
{
	LONG	cl_ret;
	WORD	flags;

	flags = 0x0000;			/* Full close		*/
	cl_ret = s_close(flags,c_strm);

	return(cl_ret);

}	

/*---------------------------------------------------------*
 *	FFLUSH() -- a partial close to flush buffers       *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    Date      Name      Comments                         *
 *---------------------------------------------------------*
 *    12/28/84  cpg       First time (copy of fclose)      *
 *=========================================================*/

LONG	fflush(c_strm)
	LONG	c_strm;
{
	LONG	cl_ret;
	WORD	flags;

	flags = 0x0001;			/* Partial close */
	cl_ret = s_close(flags,c_strm);

	return(cl_ret);

}	

/*=========================================================*
 *       FREAD()        DRC Language guide.  p. 3-25       *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     Added variables flags and of_set   *
 *                      Reads are now relative to current  *
 *                      file pointer.                      *
 *    02/07/85  jns     Added error return code section    *
 *=========================================================*/

int	fread(i_buf,i_size,i_nums,i_file)
	char	*i_buf;
	int	i_size,i_nums;
	LONG	i_file;
{
	int	flags,i_knt,i_read;
	LONG	of_set,retc;

	flags = 0x0100;		/* offset relative to file pointer */
	i_knt = i_nums;
	i_read = 0;
	of_set = 0L;

	while ( i_knt-- )
	{
	    if( (retc = s_read(flags,i_file,i_buf,(LONG)i_size,of_set)) < 0L)
		{
		retc = do_err(retc);	/* make it a word type error code */
		if ( (WORD)retc == -3 )	/* if EOF */
			return(i_read);	/* then return how far we got */
		return((WORD)retc);	/* else return the code as integer */
		}
	    else 
		{
		i_read++;
		i_buf += i_size;
		}
	}
	return(i_read);
}		

/*=========================================================*
 *    FWRITE()          DRC Language guide.  p. 3-25       *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    02/06/85  jns     Development.                       *
 *    02/06/85  jns     Added error return codes section   *
 *=========================================================*/

int	fwrite(i_buf,i_size,i_nums,i_file)
	char	*i_buf;
	int	i_size,i_nums;
	LONG	i_file;
{
	int	flags,i_knt,i_write;
	LONG	of_set,retc;

	flags = 0x0100;		/* offset relative to file pointer */
	i_knt = i_nums;
	i_write = 0;
	of_set = 0L;

	while ( i_knt-- )
	{
	    if( (retc = s_write(flags,i_file,i_buf,(LONG)i_size,of_set)) < 0L)
		{
		retc = do_err(retc);	/* make it a word type error code */
		if ( (WORD)retc == -3 )	/* if EOF */
			return(i_write);/* then return how far we got */
		return((WORD)retc);	/* else return the code as integer */
		}
	    else 
		{
		i_write++;
		i_buf += i_size;
		}
	}
	return(i_write);
}		

/*---------------------------------------------------------*
 *	FGETC() -- read a char from a file                 *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    12/28/84  cpg     More efficiency, added flags       *
 *    02/27/85  jns     Added variable nr_char to fix bug  *
 *=========================================================*/

BYTE	fgetc(in_strm)
	LONG	*in_strm;
{
	LONG nr_char;
	BYTE r_char;
	WORD flags;

	flags = 0x0100;		/* rel to file pointer	   */

	if( (nr_char = s_read(flags,in_strm,&r_char,1L,0L)) < 0L)
	    return(-127);

	return(r_char);

}

/*---------------------------------------------------------*
 *	PUTS() -- almost a K&R standard                    *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    10/10/84  jmb     More efficiency                    *
 *    01/10/85  cpg     Added FLAGS relative to end file   *
 *=========================================================*/
    
int	puts(s)			/* places string into output  	*/ 
	BYTE	*s;		/* stream a byte at a time and  */
{				/* appends a newline to end of  */
	LONG	ret;		/* of the string.		*/
	WORD	flags;

	flags = 0x0200;		/* relative to end of file */

	if (ret=s_write(flags,cnsO,s,(LONG)strlen(s),0L) < 0L)
		return(-1);
	return(0);
}

/*---------------------------------------------------------*
 *	PUTCHAR() -- almost a K&R standard                 *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    01/10/85  cpg	Added FLAGS relative to end of     *
 *                       file.                             *
 *                                                         *
 *=========================================================*/

int	putchar(c)
	BYTE	c;
{
	LONG	ret;
	WORD	flags;

	flags = 0x0200;

	if( (ret = s_write(flags,cnsO,&c,1L,0L)) < 0 )
		return( -1);
	return(0);

}

/*---------------------------------------------------------*
 *	FPUTC() -- almost a K&R standard                   *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    12/28/84  cpg     More efficiency, added flags       *
 *    01/10/85  cpg     changed FLAGS to 0x200             *
 *=========================================================*/

LONG	fputc(i_char,i_strm)
	BYTE	i_char;
	LONG	i_strm;
{
	LONG	f_pret;
	WORD	flags;

	flags = 0x0200;		/* relative to end file */

	if( (f_pret = s_write(flags,i_strm,&i_char,1L,0L)) < 0)
		return(f_pret);

	return(0L);

}

/*---------------------------------------------------------*
 *	FPUTS() -- almost a K&R standard                   *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    12/28/84  cpg     More efficiency, added flags       *
 *    01/10/85  cpg     changed FLAGS to 0x200             *
 *=========================================================*/

LONG	fputs(i_strng,i_strm)
	BYTE	*i_strng;
	LONG	i_strm;
{
	LONG	f_psret;
	LONG	r_pos;
	WORD	flags;

	flags = 0x0200;		/* relative to end of file */

	r_pos = 0L;
	while(*i_strng)
	{
	  if( (f_psret =  s_write(flags,i_strm,i_strng++,1L,r_pos++)) < 0)
		return(f_psret);
	}

	return(0L);

}

/*---------------------------------------------------------*
 *	GETRC() -- Non-standard, gets a raw character      *
 *---------------------------------------------------------*	
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    01/10/85  cpg     Added FLAGS -- rel to file ptr     *
 *=========================================================*/

LONG getrc(p)			/* puts char in byte 'p' and    */
	BYTE *p;		/* returns 0 for success and    */
{				/* -127 for a s_read() error    */
	LONG	retc;
	WORD	flags;

	flags = 0x0100;		/* relative to file ptr    */

	if ( (retc = s_read(flags,cnsI,p,1L,0L)) < 0L )
		return(-127L);

	return(0L);
}

/*---------------------------------------------------------*
 *	GETCHAR() -- almost a K&R standard                 *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    09/25/84  cpg     A first cut                        *
 *    01/10/85  cpg	Added FLAGS -- rel to file ptr     *
 *=========================================================*/

BYTE getchar()			/* returns the raw char or a    */
{				/* -127 on an s_read error      */
	BYTE	in_chr;
	WORD	flags;
	LONG	n_chr;

	flags = 0x0100;		/* relative to file ptr    */

	if ( (n_chr = s_read(flags,cnsI,&in_chr,1L,0L)) < 0 )
		return(-127);
	
	return(in_chr);

}

/*---------------------------------------------------------*
 *	STRLEN() -- K&R standard                           *
 *---------------------------------------------------------*
 *                   UPDATE LOG                            *
 *---------------------------------------------------------*
 *    10/10/84  jmb     First time                         *
 *                                                         *
 *=========================================================*/

strlen(s)	/*return length of a string */
char *s;
{
	char *p = s;

	while (*p != '\0')
		p++;
	return(p-s);
}

/*---------------------------------------------------------*
 *	STRCMP() -- K&R standard                           *
 *---------------------------------------------------------*
 *	VERSION	DATE		AUTHOR	COMMENTS           *
 *---------------------------------------------------------*
 *      1.0     12/17/84        jns     Integration        *
 *=========================================================*/

strcmp(s,t)	/* return < 0 if s < t, 0 if s == t, > 0 if s > t */
char	s[], t[];
{
	int	i;

	i = 0;
	while (s[i] == t[i])
		if (s[i++] == '\0')
			return(0);
	return(s[i] - t[i]);
}


/*========================================================*
 *		END OF GMAIN.C                            *
 *========================================================*/
