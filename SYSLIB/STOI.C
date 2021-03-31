/*---------------------------------------------------------*
 *      STOI.C -- a string to hex conversion for inclusion *
 *                into QALIB.L86.                          *
 *---------------------------------------------------------*
 *  1.0  05/06/85  cpg   first try.                        *
 *---------------------------------------------------------*/
#include "portab.h"
#include "ctyp40.h"

LONG stoi( str )
	char	*str;
{
	LONG	lval = 0;
	char	*istr;
	int	sign = 1;

	istr = str;

	while( *istr == ' ' || *istr == '\t' || *istr == '\n' )
		istr++;

	if( *istr == '-' )
	{
		sign = -1;
		istr++;
	}
	if( *istr == '0' )
	{
		istr++;
		if( *istr == 'x' || *istr == 'X' )
		{
		    istr++;
		    while( ('0' <= *istr && *istr <= '9') ||
			   ('a' <= *istr && *istr <= 'f') ||
			   ('A' <= *istr && *istr <= 'F')  )
		    {
			lval *= 16;
			lval += ('0' <= *istr && *istr <= '9') ?
				*istr - '0' : (*istr & 0x47) - 'A' + 10;
			istr++;
		    }
		}
		else
		{
		    while( '0' <= *istr && *istr <= '7' )
		    {
			lval *= 8;
			lval += *istr++ - '0';
		    }
		}
	}
	else
	{
		while( '0' <= *istr && *istr <= '9' )
		{
		    lval *= 10;
		    lval += *istr++ - '0';
		}
	}

	*str = istr;
	return( lval * sign );
}
/*---------------------------------------------------------*
 *                 End of STOI.C routine                   *
 *---------------------------------------------------------*/
