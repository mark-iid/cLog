/*(
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:38:51 $
 * $Header: /home/mxe20/src/clog/RCS/clogdb.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Id: clogdb.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: clogdb.h,v $
 * Revision 1.1  2005/05/11 15:38:51  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:31:04  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: clogdb.h,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/clogdb.h,v $
 * $State: Exp $
 */

#ifndef H_CLOGDB
#define H_CLOGDB

#include <mysql/mysql.h>
#include "sitevars.h"
extern MYSQL cLogDB;

/** is this as large as it needs to be? Or too large? 
  * note: this is not an overflow risk
  */
extern char sqlBuffer[65000]; 

extern MYSQL_RES *newsresult;
extern MYSQL_RES *userresult;
extern MYSQL_RES *commentresult;
extern MYSQL_RES *topicresult;

void cLogInitDB();
int cLogConnectDB();
int cLogQueryUserDB();
int cLogQueryNewsDB();
int cLogQueryCommentDB();
int cLogQueryTopicDB();
void escConv(char *from, char *to);
void timedateformat(char unformatted[], char stimedate[]);
char * cLogGetNewsTitle(int nid);

void itoa(int n, char s[]);
#endif
