/* ZEROFIL.C */
#include "portab.h"

zerofil(p,s)
    BYTE *p;
    WORD s;
{
	while (s--) *p++ = 0;
}
