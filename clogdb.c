 /** cLogdb.c
 * MySQL API functions and associated utility functions.
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 16:46:10 $
 * $Header: /home/mxe20/src/clog/RCS/clogdb.c,v 1.2 2005/05/11 16:46:10 mxe20 Exp mxe20 $
 * $Id: clogdb.c,v 1.2 2005/05/11 16:46:10 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: clogdb.c,v $
 * Revision 1.2  2005/05/11 16:46:10  mxe20
 * Added global variables
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: clogdb.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/clogdb.c,v $
 * $State: Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "clogdb.h"
#include "cloghtml.h"
#include "sitevars.h"

MYSQL cLogDB;
char sqlBuffer[65000]; 
MYSQL_RES *newsresult = NULL;
MYSQL_RES *userresult = NULL;
MYSQL_RES *commentresult = NULL;
MYSQL_RES *topicresult = NULL;

void reverse(char s[]);
int getDBLogin(char host[], char name[], char user[], char pwd[]);
int cLogGetSiteVars();


/** cLogInitDB
 * calls cLogConnectDB and cLogGetSiteVars
 * Terminates with an error message if unsucessful
 */
 void cLogInitDB() {
	 int err = 0;
	 err = cLogConnectDB();
	 if(err != 0) {
		printf("content-type: text/html; charset=utf-8\r\n\r\n");
		printf("<html><head><title>Critical Error</title><head>");
		printf("<body><p>Critical error: database connect failed");
		printf("<p>Error #%i</body></html>",err);
		exit(EXIT_FAILURE);
	 }
	 if(cLogGetSiteVars() != 0) {
 		printf("content-type: text/html; charset=utf-8\r\n\r\n");
		printf("<html><head><title>Critical Error</title><head>");
		printf("<body><p>Critical Error: Global Variable Init Failure");
		printf("</body></html>");
		exit(EXIT_FAILURE);
	 }
 }

/** cLogConnectDB
 * Connects to the database
 * returns 0 if successful, 1 if connect failed
 */
int cLogConnectDB() {
    char dbhost[256];
    char dbname[256];
    char dbpwd[256]; 
    char dbuser[256];

    // Get database login details
    if (getDBLogin(dbhost, dbname, dbuser, dbpwd) != 0) {
        fprintf(stderr, "Error: Unable to get database login details.\n");
        return 1;
    }

    // Initialize MySQL connection
    if (mysql_init(&cLogDB) == NULL) {
        fprintf(stderr, "Error: mysql_init() failed.\n");
        return 2;
    }

    // Connect to the database
    if (mysql_real_connect(&cLogDB, dbhost, dbuser, dbpwd, dbname, 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error: mysql_real_connect() failed.\n");
        fprintf(stderr, "MySQL error: %s\n", mysql_error(&cLogDB));
        mysql_close(&cLogDB);
        return 3;
    }

    return 0;
}

void cLogCloseDB() {
    mysql_close(&cLogDB);
}

/** cLogQueryUserDB()
 * Querys the user table in the database based on what is in sqlBuffer
 * returns 0 if successful, 1 if query failed
 */
int cLogQueryUserDB() {
	mysql_real_query(&cLogDB,sqlBuffer,(unsigned)strlen(sqlBuffer));
	userresult = mysql_store_result(&cLogDB);
	if (mysql_errno(&cLogDB) != 0) {
		fprintf(stderr, "Error: cLogQueryUserDB.mysql_store_result() failed.\n");
		fprintf(stderr, "MySQL error: %s\n", mysql_error(&cLogDB));
		return 1;
	}
	return 0;
}

/** cLogQueryNewsDB()
 * Querys the news table in the database based on what is in sqlBuffer
 * returns 0 if successful, 1 if query failed
 */
int cLogQueryNewsDB() {
	mysql_real_query(&cLogDB,sqlBuffer,(unsigned)strlen(sqlBuffer));
	newsresult = mysql_store_result(&cLogDB);
	if (mysql_errno(&cLogDB) != 0) {
		fprintf(stderr, "Error: cLogQueryNewsDB.mysql_store_result() failed.\n");
		fprintf(stderr, "MySQL error: %s\n", mysql_error(&cLogDB));
		return 1;
	}
	return 0;
}

/** cLogQueryCommentDB()
 * Querys the comment table in the database based on what is in sqlBuffer
 * returns 0 if successful, 1 if query failed
 */
int cLogQueryCommentDB() {
	mysql_real_query(&cLogDB,sqlBuffer,(unsigned)strlen(sqlBuffer));
	commentresult = mysql_store_result(&cLogDB);
	if (mysql_errno(&cLogDB) != 0) {
		fprintf(stderr, "Error: cLogQueryCommentDB.mysql_store_result() failed.\n");
		fprintf(stderr, "MySQL error: %s\n", mysql_error(&cLogDB));
		return 1;
	}
	return 0;
}

/** cLogQueryTopicDB()
 * Querys the topic table in the database based on what is in sqlBuffer
 * returns 0 if successful, 1 if query failed
 */
int cLogQueryTopicDB() {
	mysql_real_query(&cLogDB,sqlBuffer,(unsigned)strlen(sqlBuffer));
	topicresult = mysql_store_result(&cLogDB);
	if (mysql_errno(&cLogDB) != 0) {
		fprintf(stderr, "Error: cLogQueryTopicDB.mysql_store_result() failed.\n");
		fprintf(stderr, "MySQL error: %s\n", mysql_error(&cLogDB));
		return 1;
	}
	return 0;
}

/** cLogQueryVarsDB()
 * Querys the variables table in the database based on what is in sqlBuffer
 * returns 0 if successful, 1 if query failed
 */
int cLogGetSiteVars() {
	MYSQL_RES *varsresult;
	MYSQL_ROW row;
	int err;
	sprintf(sqlBuffer,"SELECT * FROM vars");
	err = mysql_real_query(&cLogDB,sqlBuffer,(unsigned)(int)strlen(sqlBuffer));
	if(err != 0) {
		return 1;
	}
	varsresult = mysql_store_result(&cLogDB);
	while((row = mysql_fetch_row(varsresult))){
		if(strstr(row[0], "admin_email")) {
			var_admin_email = strdup(row[1]);
		} else if(strstr(row[0], "admin_name")) {
			var_admin_name = strdup(row[1]);
		} else if(strstr(row[0], "site_name")) {
			var_site_name = strdup(row[1]);
		} else if(strstr(row[0], "site_root")) {
			var_site_root = strdup(row[1]);
		} else if(strstr(row[0], "site_templates")) {
			var_site_templates = strdup(row[1]);
		} else if(strstr(row[0], "site_location")) {
			var_site_location = strdup(row[1]);
		} else if(strstr(row[0], "site_url")) {
			var_site_url = strdup(row[1]);
		} else if(strstr(row[0], "site_login_url")) {
			var_site_login_url = strdup(row[1]);
		} else if(strstr(row[0], "site_description")) {
			var_site_description = strdup(row[1]);
		} else if(strstr(row[0], "rss_show_description")) {
			var_rss_show_description = strdup(row[1]);
		} else if(strstr(row[0], "site_create_user")) {
			var_site_create_user = strdup(row[1]);
		}
		
	}
	return 0;
}

/** cLogGetNewsTitle(int nid)
 * Returns the title field of a news item entry in the DB
 * int nid  = News DB ID
 */
char *cLogGetNewsTitle(int nid) {
	MYSQL_ROW row = NULL;
	char anid[10];
	long nrows = 0;
	itoa(nid,anid);
	sprintf(sqlBuffer,"SELECT title FROM newsitems WHERE nid = \'%s\' AND status != \'deleted\'",anid);
                if(cLogQueryNewsDB() != 0) {
                        printf("<P>Critical Error: News item lookup failure");
                        htmlStaticPrint("footer");
                        exit(EXIT_FAILURE);
                }
                if((mysql_num_rows(newsresult) == 0) || (mysql_num_rows(newsresult) > 1)) {
                        printf("<p>Invalid news ID");
                        htmlStaticPrint("footer");
                        exit(EXIT_FAILURE);
        }
	row = mysql_fetch_row(newsresult);
	return row[0];
}

/** getDBLogin(char name[], char pwd[])
 * Reads the first line in the file specified in the environment 
 * variable CLOGDB and returns this value (the db password)
 */
int getDBLogin(char host[], char name[], char user[], char pwd[]) {
    FILE *file = fopen("clog.cgi", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open clog.cgi for reading.\n");
        return 1;
    }

    // Read dbhost
    if (fgets(host, 256, file) == NULL) {
        fclose(file);
        return 1;
    }
    host[strcspn(host, "\r\n")] = '\0'; // Remove newline character

    // Read dbname
    if (fgets(name, 256, file) == NULL) {
        fclose(file);
        return 1;
    }
    name[strcspn(name, "\r\n")] = '\0'; // Remove newline character

    // Read dbuser
    if (fgets(user, 256, file) == NULL) {
        fclose(file);
        return 1;
    }
    user[strcspn(user, "\r\n")] = '\0'; // Remove newline character

    // Read dbpwd
    if (fgets(pwd, 256, file) == NULL) {
        fclose(file);
        return 1;
    }
    pwd[strcspn(pwd, "\r\n")] = '\0'; // Remove newline character

    fclose(file);
    return 0;
}
	
/** itoa
 * Converts integers to character strings
 * This is straight out of a K&R example
*/
void itoa(int n, char s[]) {
	int i, sign;
	unsigned int num;

	if((sign = n) < 0)
		num = -n;
	else
		num = n;
	i = 0;
	do {
		s[i++] = num % 10 + '0';
	} while ((num /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

/** reverse
 * reverses a character string_array
 * This is straight out of a K&R example
 */
void reverse(char s[]) {
	int c, i, j;
	
	for(i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
/** escConv
 * converts a string with single and double quotes into a
 *	string with escape characters instead
 */
void escConv(char *from, char *to) {
	while (*from) {
		if (*from == '\"') { /* " */
			*to++ = '\\';
			*to++ = '\"';
		} else if (*from == '\'') { /* ' */
			*to++ = '\\';
			*to++ = '\'';
		} else {
			*to++ = *from;
		}
		from++;
	}
	*to = '\0';
}

/** timedateformat
 * Takes a TIMESTAMP MySQL field and converts it to
 * 	a human readable time and date string
 */
void timedateformat(char unformatted[], char stimedate[]) {
	if(strlen(unformatted) > 14) return;
	stimedate[0] = unformatted[8]; /* hour */
	stimedate[1] = unformatted[9];
	stimedate[2] = ':';
	stimedate[3] = unformatted[10]; /* minute */
	stimedate[4] = unformatted[11]; 
	stimedate[5] = ':';
	stimedate[6] = unformatted[12]; /* second */
	stimedate[7] = unformatted[13];
	stimedate[8] = ' ';
	stimedate[9] = 'o';
	stimedate[10] = 'n';
	stimedate[11] = ' ';
	stimedate[12] = unformatted[4]; /* month */
	stimedate[13] = unformatted[5]; 
	stimedate[14] = '/';
	stimedate[15] = unformatted[6]; /* day */
	stimedate[16] = unformatted[7];
	stimedate[17] = '/';
	stimedate[18] = unformatted[0]; /* year */
	stimedate[19] = unformatted[1];
	stimedate[20] = unformatted[2];
	stimedate[21] = unformatted[3];
	stimedate[22] = 0;
	return;
}
