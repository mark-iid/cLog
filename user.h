/*
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:38:51 $
 * $Header: /home/mxe20/src/clog/RCS/user.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Id: user.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: user.h,v $
 * Revision 1.1  2005/05/11 15:38:51  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:31:04  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: user.h,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/user.h,v $
 * $State: Exp $
 */
#ifndef H_USER
#define H_USER
int cLogGetUsername(char *username);
int cLogGetGroups(char *principalName, char *principalGroups);

char principalName[20] = "\0";
char principalGroups[255] = "\0";

#endif
