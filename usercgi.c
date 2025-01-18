/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/usercgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: usercgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: usercgi.c,v $
 * Revision 1.2  2005/06/22 16:09:13  mxe20
 * Modifications for XHTML/CSS
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: usercgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/usercgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "user.h"
#include "cloghtml.h"
#include "envar.h"
#include "clogdb.h"

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

void userlist();

/**
 * @brief CGI script to display user information and their news items.
 *
 * This script retrieves and displays user information and their associated news items
 * from a MySQL database. It handles various error conditions and outputs HTML content.
 *
 * @details
 * The script performs the following steps:
 * - Initializes the database connection.
 * - Retrieves the username and groups of the principal user.
 * - Parses the QUERY_STRING environment variable to get the user ID.
 * - If the user ID is "userlist", it calls the userlist() function and exits.
 * - Queries the database for the user information based on the user ID.
 * - Queries the database for news items authored by the user.
 * - For each news item, it queries the database for comments and displays the news item
 *   along with the comment count.
 * - Frees the allocated resources and prints the footer.
 *
 * @return Returns 0 on success, exits with EXIT_FAILURE on critical errors.
 */
int main () {
	int readlevel = 0;
	int maxarchive = 100;
	char *user;
	int numQueryString;
	Envar *query;
	int i = 0;
	MYSQL_ROW row;
	MYSQL_ROW userrow;
	MYSQL_ROW topicrow;
	char stimedate[25];
	long nrows = 0;
	long allcommentcount = 0;
	long l2commentcount = 0;

	cLogInitDB();
	
	htmlHeader("Users");

	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");
	
	if(strstr(principalGroups,"l2read")) readlevel = 2;


	htmlLeftSide(principalName, principalGroups);

	htmlStaticPrint("contenttablestart");

	
	query = parseEnVar(getenv("QUERY_STRING"), &numQueryString);
	if(numQueryString == 0) {
		printf("<P>Critical Error: Invalid User ID");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < numQueryString; i++) {
		if(strcmp(query[i].key,"user") == 0) user = strdup(query[i].val);
	}

	for (i = 0; i < numQueryString; i++) {
		if (query[i].key != NULL) free(query[i].key);
		if (query[i].val != NULL) free(query[i].val);
	}

	
	if(strcmp(user,"userlist") ==0) {
		userlist();
		htmlStaticPrint("contenttableend");
		htmlStaticPrint("footer");
		return 0;
	}

	sprintf(sqlBuffer, "SELECT * FROM users WHERE uid = \"%s\"", user);
	if(cLogQueryUserDB() != 0) {
		printf("<P>Critical Error: User lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	if(mysql_num_rows(userresult) == 0) {
		printf("<P>Critical Error: Invalid User");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	userrow = mysql_fetch_row(userresult);
		
	sprintf(sqlBuffer,"SELECT * FROM newsitems WHERE status != \'deleted\' AND author = \'%s\' ORDER BY nid DESC LIMIT %i",
		userrow[1], maxarchive);
	if(cLogQueryNewsDB() != 0) {
		printf("<P>Critical Error: News item lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
		
	/*printf("<img src=\"%simages/users/%s.gif\" BORDER=\"0\" ALT=\"%s\" TITLE=\"%s\"><br>",
		var_site_templates, userrow[0], userrow[2], userrow[2]);*/
	printf("<h4>%s</h4>", userrow[2]);
	printf("<h3><a href=\"rss.cgi?author=%s\">%s's RSS Feed</a></h3>",
		userrow[1], userrow[2]);
	
	while((row = mysql_fetch_row(newsresult)) != 0) {
		/* timedateformat(row[5], stimedate); */

		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND lid = \'2\'",row[0]);
		if(cLogQueryCommentDB() != 0) {
			printf("<P>Critical Error: comment lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		l2commentcount =  (long) mysql_num_rows(commentresult);
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\'",row[0]);
		if(cLogQueryCommentDB() != 0) {
			printf("<P>Critical Error: comment lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		sprintf(sqlBuffer,"SELECT realname FROM users WHERE username = \'%s\'",row[4]);
		if(cLogQueryUserDB() != 0) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		userrow = mysql_fetch_row(userresult);
		
		allcommentcount =  (long) mysql_num_rows(commentresult);
		if(!strstr(principalGroups,"l2read")) allcommentcount -= l2commentcount;

		printf("<P><font FACE=\"verdana,helvetica,arial\" SIZE=\"3\" COLOR=\"#ffffff\"><a href=\"news.cgi?nid=%s\">%s</a></font>",row[0],row[1]);
		printf("<BR><font FACE=\"verdana,helvetica,arial\" SIZE=\"1\">Posted by %s ",userrow[0]);
		if(allcommentcount == 1) {
			printf("at %s with %li comment</font> &nbsp;&nbsp;",row[4],allcommentcount);
		} else {
			printf("at %s with %li comments</font> &nbsp;&nbsp;",row[4],allcommentcount);
		}
	}

	mysql_free_result(newsresult);
	mysql_free_result(userresult);
	mysql_free_result(commentresult);

	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");
	
	return 0;
}


/**
 * @brief Generates an HTML table listing users who belong to the 'newspost' group.
 *
 * This function queries the database for users who are part of the 'newspost' group,
 * and generates an HTML table displaying these users. Each user is displayed with
 * their associated image (if available) and a link to their user profile.
 *
 * @note The function uses a global buffer `sqlBuffer` for both SQL queries and file paths,
 *       which is not a recommended practice.
 *
 * @details
 * - The function first constructs and executes an SQL query to select users from the 'users' table
 *   where the 'groups' column contains 'newspost'.
 * - If the query fails or returns no rows, an error message is printed and the program exits.
 * - The function then iterates over the result set, generating an HTML table row for each user.
 * - For each user, it checks if an image file exists for the user and includes it in the table if available.
 * - Each user is displayed with a link to their profile and their name.
 *
 * @warning The function exits the program on critical errors, which may not be suitable for all applications.
 */
void userlist() {
	long numusers = 0;
	MYSQL_ROW userrow;
	int i = 0;
	
	sprintf(sqlBuffer, "SELECT * FROM users WHERE groups LIKE \'%%newspost%%\'");
	if(cLogQueryUserDB() != 0) {
		printf("<P>Critical Error: User lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	if(mysql_num_rows(userresult) == 0) {
		printf("<P>Critical Error: Invalid User");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	
	numusers = (long) mysql_num_rows(userresult);
	printf("<table><tr>");
	for(i = 0; i < numusers; i++) {
		userrow = mysql_fetch_row(userresult);
		if (i % 4 == 0) printf("</tr><tr>");
		/* sqlBuffer is being reused as a file path buffer here. This is bad of me but I did not
		   Want to create another variable just for this. So sue me
		*/
		sprintf(sqlBuffer, "%simages/users/%s.gif", var_site_templates, userrow[0]);
		if (!access(sqlBuffer, F_OK))
			printf("<td><a href=\"user.cgi?user=%s\"><img src=\"%simages/users/%s.gif\" BORDER=\"0\" ALT=\"%s\" TITLE=\"%s\"></a></td>",
			userrow[0], var_site_templates, userrow[0], userrow[2], userrow[2]);
		printf("<td><a href=\"user.cgi?user=%s\">%s</a></td>",userrow[0],userrow[2]);
		printf("<td>&nbsp&nbsp&nbsp&nbsp</td>");
	}
	printf("</tr></table>");
}
