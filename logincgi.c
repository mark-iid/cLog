/** login.cgi.c
 * After logging in through the http auth that protects this file,
 *  this program checks to see if the user is in the database.
 * If so, it generates a new session key and sets it as a cookie for the user.
 * If not, it creates a new user in the database and sets the session key.
 * Inputs:
 *	The environment variable "REMOTE_USER" set by the http auth program
 *
 * $Author: mxe20 $
 * $Date: 2005/05/18 19:21:34 $
 * $Header: /home/mxe20/src/clog/RCS/logincgi.c,v 1.3 2005/05/18 19:21:34 mxe20 Exp mxe20 $
 * $Id: logincgi.c,v 1.3 2005/05/18 19:21:34 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: logincgi.c,v $
 * Revision 1.3  2005/05/18 19:21:34  mxe20
 * Tweaked login process (account creation)
 *
 * Revision 1.2  2005/05/11 16:46:10  mxe20
 * Added option to not create new users
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: logincgi.c,v $
 * $Revision: 1.3 $
 * $Source: /home/mxe20/src/clog/RCS/logincgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include <uuid/uuid.h>
#include "clogdb.h"
#include "cloghtml.h"

char *var_admin_email;
char *var_admin_name;
char *var_site_name;
char *var_site_root;
char *var_site_templates;
char *var_site_location;
char *var_site_url;
char *var_site_login_url;
char *var_site_description;
char *var_rss_show_description;
char *var_site_create_user;

void sessionIdGen(char *sessionID);

/**
 * @file logincgi.c
 * @brief CGI script for handling user login and session management.
 *
 * This script handles user login by verifying the REMOTE_USER environment variable,
 * generating a session cookie, and interacting with a MySQL database to manage user
 * sessions and credentials.
 *
 * The script performs the following steps:
 * 1. Initializes the database connection.
 * 2. Checks if the REMOTE_USER environment variable is set.
 * 3. Generates a session cookie.
 * 4. Queries the database for the user associated with the REMOTE_USER.
 * 5. If the user does not exist and user creation is allowed, creates a new user.
 * 6. If the user does not exist and user creation is not allowed, denies access.
 * 7. If the user exists, logs the user in and sets the session cookie in the database.
 * 8. Handles any database errors that occur during the process.
 * 9. Frees allocated memory and prints the footer.
 *
 * @return 0 on success, 1 on error.
 */
int main() {
	char *principalName; /* REMOTE_USER */
	char cookie[32]; /* Session Cookie - DCE UUID */
	long nrows = 0;
	int err = 0;
	MYSQL_ROW row;

	cLogInitDB();
	
	if(getenv("REMOTE_USER") == NULL) {
		htmlHeader("Error");
		printf("<P>Critical Error: UserID not found");
		printf("<p>If you are accessing this from a Shibboleth Origin, ");
		printf("you must have your attribute release policy set to release "); 
		printf("eduPersonScopedAffiliation and eduPersonPrincipalname to this target");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	
	principalName = strdup(getenv("REMOTE_USER"));

	sessionIdGen(cookie);
	
	sprintf(sqlBuffer,"SELECT * FROM users WHERE username = \'%s\'",
			principalName);
	
	
	if(cLogQueryUserDB() != 0) {
		htmlHeader("Error");
		printf("Database Error");
		htmlStaticPrint("footer");
		if(principalName != NULL) (principalName);
		return 1;
	} 
	
	
	nrows = (long) mysql_num_rows(userresult);
	if(nrows == 0 && !strcmp(var_site_create_user,"true")) {
		/* if there is no user with this principal name, create one */
		sprintf(sqlBuffer,"INSERT INTO users (username, realname, groups) VALUES (\'%s\',\"%s\",\'registered\')",
			principalName, principalName);

	        htmlHeaderCookie("Logging In","SessionID",cookie);
        	printf("<P>Logging in...<br>\n");
       		printf("<P><a href=\"/\">Homepage</a>");                            
		printf("<P>Adding user to database");
		err = cLogQueryUserDB();
		if(err == 0) {
			sprintf(sqlBuffer,"SELECT * FROM users WHERE username = \'%s\'",
					principalName);
			err = cLogQueryUserDB();
			if(err == 0) {
				nrows = (long) mysql_num_rows(userresult);
			}
		}
	} else if(nrows == 0 && strcmp(var_site_create_user,"true")) {
		/* there is no user with this principal name, and we are not creating it */
		htmlHeader("Access Denied");
		printf("<h3>Access Denied</h3><p>You do not have the proper credentials to access this resourse.</p>");	
		htmlStaticPrint("footer");
		if(principalName != NULL) free(principalName);
		return 1;
	} else { 
		htmlHeaderCookie("Logging In","SessionID",cookie);
        	printf("<P>Logging in...<br>\n");
        	printf("<P><a href=\"/\">Homepage</a>");                                                                      }

	if(err !=0 || nrows != 1) htmlDBError(principalName, err);

	/* set the cookie in the db */
	sprintf(sqlBuffer,"UPDATE users SET users.cookie=\'%s\' WHERE users.username=\'%s\'",
		cookie, principalName);
	err = cLogQueryUserDB();
	if(err != 0) htmlDBError(principalName, err);
		
	sprintf(sqlBuffer,"SELECT * FROM users WHERE username = \'%s\'",
		principalName);
	err = cLogQueryUserDB();
	if(err != 0) htmlDBError(principalName, err);
				
	if(principalName != NULL) free(principalName);
	htmlStaticPrint("footer");
	return 0;
}

/**
 * @brief Generates a session ID string from a UUID.
 *
 * This function generates a UUID and converts it into a string representation
 * to be used as a session ID. The generated session ID is stored in the 
 * provided character array.
 *
 * @param sessionID A character array where the generated session ID will be stored.
 *                  The array should be large enough to hold the resulting string.
 */
void sessionIdGen(char *sessionID) {
	uuid_t sessionUuid;
	int i;
	int singleDigits = 0;
	char *sessionPtr;
	
	uuid_generate(sessionUuid);
	sessionPtr = &sessionID[0];
	/* this is a uuid_t, I want a string */
	for(i=1;i<17;i++) {
		sprintf(sessionPtr,"%x",(unsigned int)sessionUuid[i-1]);
		/* keep track of single digit values so we know how far to advance in the array */
		singleDigits += 2-(strlen(sessionPtr)); 
		sessionPtr = &sessionID[i+i-singleDigits];
	}
}

