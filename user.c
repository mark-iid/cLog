/*
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:27:02 $
 * $Header: /home/mxe20/src/clog/RCS/user.c,v 1.1 2005/05/11 15:27:02 mxe20 Exp mxe20 $
 * $Id: user.c,v 1.1 2005/05/11 15:27:02 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: user.c,v $
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: user.c,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/user.c,v $
 * $State: Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "clogdb.h"
#include "envar.h"


/** cLogGetUsername
 * checks for a sessionID cookie and looks up the matching 
 * userid in the database. 
 *
 * After a successful return, username must be freed.
 *
 * Successful: returns 0 as error code and puts username in 'username'
 * Unsuccessful error codes: 
 *  10 - No cookie found
 *  15 - No match for session cookie in database
 *  16 - More than one match for session cookie in database
 *  20 - Unable to connect to database
 *  30 - Database quary failed
 *
 * Known Issues:
 *  cookies structure not released - memory leak
 */
int cLogGetUsername(char *username) {
	long nrows = 0; /* number of rows returned from query */
	int num_cookies = 0;
	Envar *cookies;
	char *sessioncookie = NULL;
	int i;

	
	cookies = parseEnVar(getenv("HTTP_COOKIE"), &num_cookies);

	for (i = 0; i < num_cookies; i++) {
		if(strcmp(cookies[i].key,"SessionID") == 0) {
			sessioncookie = strdup(cookies[i].val);
		}
		
	}

	for (i = 0; i < num_cookies; i++) {
		if(cookies[i].key != NULL) free(cookies[i].key);
		if(cookies[i].val != NULL) free(cookies[i].val);
	}
	
	if(sessioncookie == NULL) {
		return 10;
	}

	sprintf(sqlBuffer,"SELECT * FROM users WHERE cookie = \'%s\'",
		sessioncookie);

	if(cLogQueryUserDB() != 0) {
		if(sessioncookie != NULL) free(sessioncookie);
		return 30; /* Database lookup failure */
	}

	if(sessioncookie != NULL) free(sessioncookie);
	nrows = (long) mysql_num_rows(userresult);
	if(nrows == 0)
	{
		return 15;
	} else if(nrows > 1) {
		mysql_free_result(userresult);
		return 16;
	} else {
		MYSQL_ROW row;
		row = mysql_fetch_row(userresult);
		memmove(username,row[1],strlen(row[1])+1);
		mysql_free_result(userresult);
		return 0;
	}
}

int cLogGetGroups(char *principalName, char *principalGroups) {
	MYSQL_ROW row;
	long nrows;
	
	if(strlen(principalName) == 0) return 10;
	
	sprintf(sqlBuffer,"SELECT groups FROM users WHERE username = \'%s\'",
		principalName);
	
	if(cLogQueryUserDB() != 0) return 20;
	
	nrows = (long) mysql_num_rows(userresult);

	if(nrows == 0) {
		return 20;
	} else {
		row = mysql_fetch_row(userresult);
		memmove(principalGroups,row[0],strlen(row[0])+1);
		mysql_free_result(userresult);
	}
	return 0;
}
