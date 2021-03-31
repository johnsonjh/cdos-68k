/*---------------------------------------------------------*
 *     TOOLS.C -- GP work routines                         *
 *---------------------------------------------------------*
 * Changed getnum variable tix to ULONG   - 12/11/84 - jns *
 * Looks OK for alpha - removed pr*() functions            *
 *     ****ROUTINES NOT TESTED ****       _ 12/28/84 - cpg *
 * Changed getstr to handle <CR> only     - 01/11/85 - jns *
 *    trash <LF> - CR LF required         - 01/11/85 - jns *
 * Changed getnum to handle '-' signs     - 02/27/85 - jns *
 * Fixed getnam to return  number of char - 02/27/85 - jns *
 * Fixed getnum to use the STOI function. - 05/06/85 - cpg *
 * removed newfil() and remfil()          - 05/14/85 - cpg *
 * Fixed to ignore LF in getstr()         - 05/14/85 - cpg *
 *---------------------------------------------------------*/
#include <portab.h>
#include <ctyp40.h>

EXTERN LONG s_get();
EXTERN LONG s_lookup();
EXTERN LONG s_read();
EXTERN LONG s_write();
EXTERN LONG s_create();
EXTERN LONG s_delete();
EXTERN LONG s_open();
EXTERN LONG s_close();
EXTERN BYTE getchar();
EXTERN LONG stoi();

#define T_PIPE	16
#define T_FILE	32

#define NO_XLAT 0x0000
#define	C_SHARE	0x0400
#define	O_SHARE	0x0010

#define PTAB struct ptabstruct
PTAB
{
	LONG pt_key;
	BYTE pt_name[8];
	WORD pt_size;
	WORD pt_rsize;
};

#define DFTAB struct dftabstruct
DFTAB
{
	LONG df_key;
	BYTE df_name[12];
	WORD df_recs;
	BYTE df_user;
	BYTE df_group;
	WORD df_protect;
	WORD df_time;
	WORD df_date;
	WORD df_attr;
	LONG df_size;
	WORD df_mody;
	BYTE df_modm;
	BYTE df_modd;
	LONG df_modt;
};

#define FTAB struct ftabstruct
FTAB
{
	LONG ft_fn;
	BYTE ft_name[32];
	WORD ft_size;
	WORD ft_rsize;
};

GLOBAL FTAB ftable[20];

#define ltabsiz 1048
#define	mbufsiz	1048

GLOBAL BYTE ltab[ltabsiz];

BYTE mbuf[1048];
BYTE tbuf[128];

/*---------------------------------------------------------*
 *      GETNUM() -- uses STOI() function to return an      *
 *                  integer value.                         *
 *---------------------------------------------------------*
 *          USAGE:                                         *
 *		val = getnum( "prompt string");            *
 *                                                         *
 *              where --  LONG	val;                       *
 *                                                         *
 *		Negatives must have a leading '-'.         *
 *		Input strings preceeded with '0x' are      *
 *		treated as hex numbers and strings         *
 *		preceeded with only '0' are octal.         *
 *		Any other string defaults to decimal.      *
 *---------------------------------------------------------*
 *  1.10  05/06/85    cpg                                  *
 *---------------------------------------------------------*/

LONG getnum( str1 )
    char *str1;
{
    LONG tix;
    BYTE  c[16];
    WORD  i;
    char *psstr;

    printf( str1 );
    c[0] = 14;
    getstr( c );
    printf( "\n\r" );

    psstr = &c[2];
    tix = stoi( psstr );

    return( tix );

}
/*---------------------------------------------------------*/

getnam(h,s)
BYTE *h,*s;
{
    int i;
    char c[82];

	c[0] = 80;
	printf(h);
	getstr(c);
	printf("\r\n");
	for (i=0; i<c[1]; i++) *s++ = c[2+i];
	*s = 0 ;
	return(i);	
}

/*---------------------------------------------------------*/

err(errno)
    LONG errno;
{
	printf("\r\n Error:  %ld \r\n",errno);
}

/*---------------------------------------------------------*/

getstr(s)
    BYTE *s;
{
    BYTE c;
    int  i;

/*---------------------------------------------------------*
 *	   s[0] = length of buffer                         *
 *	   s[1] = number of chars actually read            *
 *	   s[2] = first char in buffer                     *
 *---------------------------------------------------------*/

	i = 0;
	while ( i < s[0])
	{
	    c = getchar();		/* Returns a byte            */

	    switch(c)
	    {
		case 24:	i = 0;		  /* CTRL-X          */
				break;

		case 127:			  /* DELETE key      */
		case 8:		if (i) i--;	  /* Backspace       */
				break;

		/* this assumes ALL input terminated by a CR only    */
		/* ---  modified 05/14/85      cpg                   */
		case 13:	s[1] = (BYTE) i;  /* Carriage return */
				i = s[0];
				break;

		/* In this instance -- forget the LF                 */
		case 10:	c = getchar();	  /* ignore the LF   */
				break;

		default :	if (c >= 32)	  /* Otherwise print  */
				{                 /* as a character   */ 
				    i++;
				    s[i+1] = c;
				}
	    }
	}
}
/*---------------------------------------------------------*
 *                 END of QATOOLS.C                        *
 *---------------------------------------------------------*/
