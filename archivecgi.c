/** archive.cgi.c
 * displays older stories with an option to search 
 *
 * TODO: Remove HTML from code, use template instead
 * TODO: Create search function
 * 
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/archivecgi.c,v 1.3 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: archivecgi.c,v 1.3 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: archivecgi.c,v $
 * Revision 1.3  2005/06/22 16:09:13  mxe20
 * Modifications for XHTML/CSS
 *
 * Revision 1.2  2005/05/18 21:11:26  mxe20
 * *** empty log message ***
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 16:41:04  mxe20
 * Added RCS Macros
 *
 * $Name:  $
 * $RCSfile: archivecgi.c,v $
 * $Revision: 1.3 $
 * $Source: /home/mxe20/src/clog/RCS/archivecgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"
#include "cloghtml.h"
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

/**
 * @file archivecgi.c
 * @brief This file contains the main function for generating the news archive page.
 *
 * The main function connects to the database, retrieves news items and their associated comments,
 * and generates an HTML page displaying the news archive.
 *
 * @details
 * The function performs the following steps:
 * 1. Initializes the database connection.
 * 2. Generates the HTML header for the news archive page.
 * 3. Retrieves the username and group information for the current user.
 * 4. Retrieves the total number of news stories from the database.
 * 5. Retrieves the latest news stories up to a specified limit.
 * 6. For each news story, retrieves the associated comments and user information.
 * 7. Generates the HTML content for each news story, including the number of comments.
 * 8. Frees the database result sets and generates the HTML footer.
 *
 * @note The maximum number of news stories (`maxstories`) and the maximum number of archived stories (`maxarchive`)
 *       are currently hardcoded but should be made configurable in the future.
 *
 * @todo
 * - Drop the `maxarchive` number when the search function is created.
 * - Make `maxstories` a user-configurable option.
 *
 * @return Returns 0 on successful execution, exits with `EXIT_FAILURE` on critical errors.
 */
int main () {
	int maxarchive = 1000; /* TODO: drop this number when search function is created */
	int maxstories = 10;  /* TODO: make this a user configurable option */
	int i = 0;
	MYSQL_ROW newsrow;
	MYSQL_ROW userrow;
	char stimedate[25];
	long numrows = 0;
	long allcommentcount = 0;
	long l2commentcount = 0;

	cLogInitDB();
	
	htmlHeader("News Archive");

	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");
	

	htmlLeftSide(principalName, principalGroups);

	sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\'");
	if(cLogQueryNewsDB() != 0) {
		printf("<P>Critical Error: NumberOfStories lookup");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	numrows = (long) mysql_num_rows(newsresult);
	if(maxstories > numrows) maxstories = numrows;

	
	sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\' ORDER BY nid DESC LIMIT %i",
		maxstories);
	if(cLogQueryNewsDB() != 0) {
		printf("<P>Critical Error: News item lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
		
	for(i = 0; i< maxstories; i++) newsrow = mysql_fetch_row(newsresult);
		
	sprintf(sqlBuffer,"SELECT * FROM newsitems WHERE nid < \'%s\' AND status != \'deleted\' ORDER BY nid DESC LIMIT %i",
		newsrow[0], maxarchive);

	if(cLogQueryNewsDB() != 0) {
		printf("<P>Critical Error: News item lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}

	htmlStaticPrint("contenttablestart");
	printf("<P>There are %li stories in the database\n\r",numrows);
	while((newsrow = mysql_fetch_row(newsresult)) != 0) {
		// timedateformat(newsrow[5], stimedate); 


		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND lid = \'2\'",newsrow[0]);
		if(cLogQueryCommentDB() != 0) {
			printf("<P>Critical Error: comment lookup failure");
			htmlStaticPrint("contenttableend");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		l2commentcount =  (long) mysql_num_rows(commentresult);
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\'",newsrow[0]);
		if(cLogQueryCommentDB() != 0) {
			printf("<P>Critical Error: comment lookup failure");
			htmlStaticPrint("contenttableend");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		sprintf(sqlBuffer,"SELECT realname FROM users WHERE username = \'%s\'",newsrow[3]);
		if(cLogQueryUserDB() != 0) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("contenttableend");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		userrow = mysql_fetch_row(userresult);
		
		allcommentcount =  (long) mysql_num_rows(commentresult);
		if(!strstr(principalGroups,"l2read")) allcommentcount -= l2commentcount;

		printf("<br /><h3><a href=\"news.cgi?nid=%s\">%s</a></h3>\n\r",newsrow[0],newsrow[1]);
		printf("<h2>Posted by %s ",userrow[0]);
		if(allcommentcount == 1) {
			printf("at %s with %li comment</h2>\n\r",newsrow[4],allcommentcount);
		} else {
			printf("at %s with %li comments</h2>\n\r",newsrow[4],allcommentcount);
		}		
	}
	

	if(newsresult != NULL) mysql_free_result(newsresult);
	//if(userresult != NULL) mysql_free_result(userresult);
	if(commentresult != NULL) mysql_free_result(commentresult);

	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");
	return 0;
}
