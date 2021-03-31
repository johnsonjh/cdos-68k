/*---------------------------------------------------------*
 *	CVT.C -- A program to test the STOI function.      *
 *---------------------------------------------------------*
 *  0.1    05/03/85  cpg   First cut                       *
 *  0.2    05/10/85  cpg   modified for CDOS 286           *
 *                         fixed to_lbin()                 *
 *---------------------------------------------------------*
 *   INCLUDES:                                             */
#include "portab.h"
#include "std40.h"
#include "ctyp40.h"


LONG	getnum();
LONG	to_lbin();
WORD	EXIT = 1;

main()
{
  LONG num;

  while( EXIT )
  {
    num = getnum( "\nEnter a number ( ^C to exit) >> " );
    if( EXIT )
    printf("\t%08lx hex; %08ld dec; %08lo oct;\n\r",
			num,num,num);
    printf("\t'%s'b \n\r",to_lbin( num ) );
    }
    printf("\n------------------- Done -------------------\n");
}

/*---------------------------------------------------------*
 *                  END OF CVT.C main()                    *
 *---------------------------------------------------------*
 *                START OF UTILITY ROUTINES                *
 *---------------------------------------------------------*/

LONG	to_lbin( x )		/* This will convert an INTEGER */
	LONG x;			/* value to a string of binary  */
{				/* characters. to be printed as */
	int	knt = 0;	/* '%s' -- a string.            */
	LONG	b_msk = 0x80000000;
static	char	z[39];

	while( knt < 35 )
	  {
	    if( x & b_msk )
	      {
		z[knt] = 0x31;
	      }
	      else
		z[knt] = 0x30;
	    x <<= 1;
	    knt++;
	    if( knt == 8 || knt == 17 || knt == 26 )
		z[knt++] = 0x20;
	  }
	z[knt++] = 0x0;

	return(z);

}

/*---------------------------------------------------------*
 *	END of CODE                                        *
 *---------------------------------------------------------*/
