/*
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:38:51 $
 * $Header: /home/mxe20/src/clog/RCS/envar.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Id: envar.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: envar.h,v $
 * Revision 1.1  2005/05/11 15:38:51  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:31:04  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: envar.h,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/envar.h,v $
 * $State: Exp $
 */
#ifndef H_ENVAR
#define H_ENVAR

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

typedef struct {
        char *key;
        char *val;
} Envar;

char *copyKey(char *);
char *copyVal(char *);
Envar *parseEnVar(char *env, /*@out@*/int *n);

#endif
