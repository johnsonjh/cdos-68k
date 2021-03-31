/*********************************************************************
*    STRNCMP  -  compares strings up to n chars
*
*	WORD strncmp(s1,s2,n)
*	BYTE *s1, *s2;
*	UWORD n;
*
*	'strncmp' compares null terminated strings s1 and s2, and examines
*	  at most n chars.
*	Always compares at least 1 char.
*	n < 0 compares many, many characters.
*	Returns:
*		strncmp < 0  if  s1<s2  (within n chars)
*		strncmp = 0  if  s1=s2	   "    "   "
*		strncmp > 0  if  s1>s2     "    "   "
*********************************************************************/

#include <portab.h>

WORD	strncmp(s1,s2,num)			/* CLEAR FUNCTION ***********/
REG	BYTE *s1, *s2;
REG	WORD num;
{	
	for( ; --num > 0  &&  (*s1 == *s2); s1++, s2++ )
		if( *s1 == NULL )
			return(0);
	return(*s1 - *s2);
}
	WORD num;
{	
	for( ; --num > 0  &&  (*s1 == *s2); s1++, s2++ )
		if( *s1 == NULL )
			return(0