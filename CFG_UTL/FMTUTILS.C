/* FMTUTILS - utility routines for VME/10 FORMAT */

#include "portab.h"
#include "stdio.h"

/* External functions we need */

EXTERN LONG xread();


/* External variables in FORMAT.C we need */

EXTERN BYTE linebuf[80];
EXTERN LONG ret;
EXTERN BYTE sysarea[0x2000];


/* putword - store a word with byte swapping */

putword(p,value)
BYTE *p;
WORD value;
{
    p[0] = value;		/* low byte */
    p[1] = value >> 8;		/* high byte */
}


/* getword - get a byte-swapped word */

getword(p)
BYTE *p;
{
    return ((p[0] & 0xff) + ((p[1] << 8) & 0xff00));
}


/* sysword - write a word into the system area (unswapped) */

sysword(offset,value)
WORD offset,value;
{
    *(WORD *)(sysarea + offset) = value;
}


/* syslong - write a LONG into the system area (unswapped) */

syslong(offset,value)
WORD offset;
LONG value;
{
    *(LONG *)(sysarea + offset) = value;
}


/* dosword - write a WORD into the system area, swapping the bytes */

dosword(offset,value)
WORD offset,value;
{
    putword(&sysarea[offset],value);
}


/* doslong - write a long word into the system area, swapping the
    words and bytes */

doslong(offset,value)
WORD offset;
LONG value;
{
    dosword(offset,(WORD)value);
    dosword(offset+2,(WORD)(value >> 16));
}


/* error - print an error message, then exit */

error(s)
BYTE *s;
{
    printf(s);
    s_exit(0L);
}


/* getkey - get a single key from the keyboard, convert it to upper case,
	return it */

getkey()
{
    ret = xread(STDIN,linebuf,(LONG)sizeof(linebuf));
    printf("\r\n");
    if (ret <= 0)
	return (0);
    else
	return (toupper(linebuf[0]) & 0xff);
}


/* toupper - convert character to upper case */

toupper(c)
BYTE c;
{
    if (c >= 'a' && c <= 'z')
	return(c - 'a' + 'A');
    else
	return (c);
}


/* chkret - check the return code, and print an error message if nonzero */

chkret(string)
BYTE *string;
{
    if (ret == 0L) return;
    printf("\r\n%s: error %lx\r\n",string,ret);
}


/* movb - copy bytes by count */

movb(source,dest,count)
REG BYTE *source;
REG BYTE *dest;
REG WORD count;
{
    while (count--)
	*dest++ = *source++;
}


/* setb - fill bytes by count */

setb(val,dest,count)
REG BYTE val;
REG BYTE *dest;
REG WORD count;
{
    while (count--)
	*dest++ = val;
}
