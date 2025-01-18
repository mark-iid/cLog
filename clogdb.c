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


/**
 * @brief Initializes the database connection and site variables.
 *
 * This function attempts to connect to the database and initialize
 * global site variables. If the database connection fails, it outputs
 * an HTML error message and terminates the program. Similarly, if the
 * initialization of global site variables fails, it outputs an HTML
 * error message and terminates the program.
 *
 * @note This function will terminate the program if any critical error occurs.
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

/**
 * @brief Connects to the database using MySQL.
 *
 * This function retrieves the database login details, initializes a MySQL connection,
 * and attempts to connect to the specified database.
 *
 * @return int Returns 0 on success, or an error code on failure:
 *             - 1: Unable to get database login details.
 *             - 2: mysql_init() failed.
 *             - 3: mysql_real_connect() failed.
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

/**
 * @brief Executes a query on the user database and stores the result.
 *
 * This function sends a query to the MySQL database using the global 
 * `cLogDB` connection and the `sqlBuffer` query string. It then attempts 
 * to store the result of the query in `userresult`.
 *
 * @return Returns 0 on success, or 1 if an error occurs.
 *
 * If an error occurs during the execution of the query or while storing 
 * the result, an error message is printed to `stderr` and the function 
 * returns 1. The error message includes the MySQL error string.
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


/**
 * @brief Executes a query on the news database and stores the result.
 *
 * This function sends a query to the MySQL database using the global 
 * `cLogDB` connection and the query stored in `sqlBuffer`. It then 
 * attempts to store the result of the query in `newsresult`.
 *
 * @return Returns 0 on success, or 1 if an error occurs.
 *
 * If an error occurs during the execution of the query or while storing 
 * the result, an error message is printed to `stderr` and the function 
 * returns 1. The error message includes the MySQL error code and message.
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

/**
 * @brief Executes a query on the comment database and stores the result.
 *
 * This function sends a query to the MySQL database using the global `cLogDB` connection
 * and the query stored in `sqlBuffer`. It then attempts to store the result of the query
 * in `commentresult`. If an error occurs during the process, it prints an error message
 * to `stderr` and returns 1. If the query is successful, it returns 0.
 *
 * @return int Returns 0 on success, 1 on failure.
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


/**
 * Executes a query on the cLogDB database and stores the result.
 *
 * This function sends a query to the cLogDB database using the SQL statement
 * stored in `sqlBuffer`. It then attempts to store the result of the query.
 * If an error occurs during the query or while storing the result, an error
 * message is printed to `stderr` and the function returns 1. Otherwise, the
 * function returns 0.
 *
 * @return 0 if the query and result storage are successful, 1 if an error occurs.
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

/**
 * cLogGetSiteVars - Retrieves site variables from the database and stores them in global variables.
 *
 * This function executes a SQL query to fetch all records from the 'vars' table in the database.
 * It then iterates through the result set and assigns the values to corresponding global variables
 * based on the variable name.
 *
 * Return:
 *   0 on success, 1 if there is an error executing the SQL query.
 *
 * Global Variables:
 *   var_admin_email - Stores the admin email address.
 *   var_admin_name - Stores the admin name.
 *   var_site_name - Stores the site name.
 *   var_site_root - Stores the site root path.
 *   var_site_templates - Stores the site templates path.
 *   var_site_location - Stores the site location.
 *   var_site_url - Stores the site URL.
 *   var_site_login_url - Stores the site login URL.
 *   var_site_description - Stores the site description.
 *   var_rss_show_description - Stores the RSS show description flag.
 *   var_site_create_user - Stores the site create user flag.
 *
 * Note:
 *   The function uses `strdup` to allocate memory for the global variables. It is the caller's
 *   responsibility to free this memory when it is no longer needed.
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

/**
 * cLogGetNewsTitle - Retrieves the title of a news item from the database.
 * @nid: The news item ID.
 *
 * This function takes a news item ID, constructs a SQL query to fetch the title
 * of the news item from the database, and returns the title as a string. If the
 * query fails or if the news item ID is invalid (i.e., no rows or more than one
 * row is returned), the function prints an error message, displays the footer,
 * and exits the program.
 *
 * Return: A pointer to the title string of the news item.
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

/**
 * @brief Reads database login credentials from a file.
 *
 * This function opens the file "clog.cgi" and reads the database host, name,
 * user, and password from it. Each credential is expected to be on a separate
 * line in the file. The newline characters at the end of each line are removed.
 *
 * @param host A character array to store the database host.
 * @param name A character array to store the database name.
 * @param user A character array to store the database user.
 * @param pwd A character array to store the database password.
 * @return Returns 0 on success, or 1 if an error occurs (e.g., file cannot be opened or read).
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
	
/**
 * Converts an integer to a string.
 *
 * This function takes an integer `n` and converts it to a null-terminated string
 * stored in the character array `s`. The function handles negative numbers by
 * storing a '-' character at the beginning of the string.
 *
 * @param n The integer to be converted.
 * @param s The character array where the resulting string will be stored.
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

/**
 * @brief Reverses a string in place.
 *
 * This function takes a null-terminated string and reverses its characters
 * in place. The reversal is done by swapping characters from the beginning
 * and end of the string moving towards the center.
 *
 * @param s The string to be reversed. It must be a null-terminated character array.
 */
void reverse(char s[]) {
	int c, i, j;
	
	for(i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/**
 * @brief Converts escape characters in a string.
 *
 * This function takes an input string `from` and converts certain characters
 * to their escaped equivalents, storing the result in the output string `to`.
 * Specifically, it converts double quotes (") to \" and single quotes (') to \'.
 *
 * @param from The input string to be converted.
 * @param to The output string where the converted result will be stored.
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


/**
 * Formats a given unformatted date-time string into a more readable format.
 *
 * The input date-time string should be in the format "YYYYMMDDHHMMSS".
 * The output formatted string will be in the format "HH:MM:SS on MM/DD/YYYY".
 *
 * @param unformatted The input date-time string in the format "YYYYMMDDHHMMSS".
 *                    It should be a null-terminated string with a length of 14 characters.
 * @param stimedate   The output buffer where the formatted date-time string will be stored.
 *                    It should be large enough to hold the formatted string, which is 23 characters
 *                    including the null terminator.
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
