/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/

/*===============================================================*
 *   Version 1.0	strcpy.c				 *
 *	This routine copies a null terminated string s to	 *
 *	buffer d, and returns a pointer	to d.			 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *	1.0	01/26/84 KSO -	Created				 *
 *								 *
 *===============================================================*
 *  INCLUDES:                                                    */
#include	"portab.h"

BYTE	*strcpy( d,s )
BYTE	*d;
BYTE	*s;
{
	BYTE	*p;

	for( p = d; *p = *s; p++,s++ );
	return( d );
}
ude	"portab.h"

BYTE	*strcpy( d,s )
BYTE	*d;
BYTE	*