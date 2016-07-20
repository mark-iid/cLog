/*
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:38:51 $
 * $Header: /home/mxe20/src/clog/RCS/cloghtml.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Id: cloghtml.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: cloghtml.h,v $
 * Revision 1.1  2005/05/11 15:38:51  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:31:04  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: cloghtml.h,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/cloghtml.h,v $
 * $State: Exp $
 */

#ifndef H_CLOGHTML
#define H_CLOGHTML
#include <stdio.h>

typedef struct templatelist {
	char data[256];
	struct templatelist *next;
} TEMPLATELIST;

void htmlHeader(char *title);
void htmlHeaderCookie(char *title, char *cookieID, char *cookieContents);
void htmlLeftSide(char *principalName, char *principalGroups);
void htmlMainTableStart();
void htmlStaticPrint(char *htmltemplate);
void printconv(char *s);
void printconvrss(char *s);
void htmlReadTemplate(char *htmltemplate, TEMPLATELIST **HtmlTemplate);
void htmlFreeTemplate(TEMPLATELIST *HtmlTemplate);
void htmlDBError(char *principalname, int err);

#endif
