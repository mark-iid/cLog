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

/**
 * cLogGetUsername - Retrieves the username associated with the session cookie.
 * @username: A pointer to a buffer where the username will be stored.
 *
 * This function retrieves the username associated with the session cookie
 * from the HTTP_COOKIE environment variable. It parses the cookies, extracts
 * the session cookie, and queries the database to find the corresponding
 * username. The username is then copied to the provided buffer.
 *
 * Return: 
 *  0  - Success, username retrieved and stored in the provided buffer.
 * 10 - Session cookie not found.
 * 15 - No matching user found in the database.
 * 16 - Multiple matching users found in the database.
 * 30 - Database lookup failure.
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

/**
 * @brief Retrieves the groups associated with a given principal (user).
 *
 * This function queries the database to get the groups for the specified principal (user).
 * If the principal name is empty, it returns an error code 10. If the query fails or no
 * groups are found for the principal, it returns an error code 20. Otherwise, it copies
 * the groups to the provided buffer and returns 0.
 *
 * @param principalName The name of the principal (user) whose groups are to be retrieved.
 * @param principalGroups A buffer to store the retrieved groups.
 * @return int 0 on success, 10 if the principal name is empty, 20 if the query fails or no groups are found.
 */
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
