/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/topiccgi.c,v 1.3 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: topiccgi.c,v 1.3 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: topiccgi.c,v $
 * Revision 1.3  2005/06/22 16:09:13  mxe20
 * Modifications for XHTML/CSS
 *
 * Revision 1.2  2005/05/18 21:11:26  mxe20
 * *** empty log message ***
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: topiccgi.c,v $
 * $Revision: 1.3 $
 * $Source: /home/mxe20/src/clog/RCS/topiccgi.c,v $
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

void topiclist();

/**
 * @brief CGI script to display news topics and related news items.
 *
 * This script handles the display of news topics and their associated news items.
 * It retrieves the topic ID from the query string, fetches the topic details from the database,
 * and displays the news items under that topic. It also handles user authentication and 
 * authorization to determine the read level.
 *
 * The script performs the following steps:
 * 1. Initializes the database connection.
 * 2. Outputs the HTML header.
 * 3. Checks user authentication and group membership.
 * 4. Parses the query string to get the topic ID.
 * 5. If the topic ID is "topiclist", it displays the list of topics.
 * 6. Fetches the topic details from the database.
 * 7. Fetches the news items under the topic from the database.
 * 8. Displays the topic details and news items.
 * 9. Fetches and displays the comments count for each news item.
 * 10. Frees the allocated resources and outputs the HTML footer.
 *
 * @return 0 on success, exits with EXIT_FAILURE on critical errors.
 */
int main () {
	int readlevel = 0;
	int maxarchive = 100;
	char *topic;
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
	
	htmlHeader("News Topics");

	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");
	
	if(strstr(principalGroups,"l2read")) readlevel = 2;


	htmlLeftSide(principalName, principalGroups);

	htmlStaticPrint("contenttablestart");

	
	query = parseEnVar(getenv("QUERY_STRING"), &numQueryString);
	if(numQueryString == 0) {
		printf("<p>Critical Error: Invalid Topic ID");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < numQueryString; i++) {
		if(strcmp(query[i].key,"topic") == 0) topic = strdup(query[i].val);
	}

	for (i = 0; i < numQueryString; i++) {
		if(query[i].key != NULL) free(query[i].key);
		if(query[i].val != NULL) free(query[i].val);
	}
	
	if(strcmp(topic,"topiclist") ==0) {
		topiclist();
		htmlStaticPrint("contenttableend");
		htmlStaticPrint("footer");
		return 0;
	}

	sprintf(sqlBuffer, "SELECT * FROM topics WHERE tid = \"%s\"", topic);
	if(cLogQueryTopicDB() != 0) {
		printf("<p>Critical Error: Topic lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	if(mysql_num_rows(topicresult) == 0) {
		printf("<p>Critical Error: Invalid Topic");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	topicrow = mysql_fetch_row(topicresult);
		
	sprintf(sqlBuffer,"SELECT * FROM newsitems WHERE status != \'deleted\' AND topic = \'%s\' ORDER BY nid DESC LIMIT %i",
		topic, maxarchive);
	if(cLogQueryNewsDB() != 0) {
		printf("<p>Critical Error: News item lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
		
	printf("<p><img src=\"%simages/topics/%s.gif\" border=\"0\" alt=\"%s\" title=\"%s\">",
		var_site_templates, topicrow[0], topicrow[1], topicrow[1]);
	printf("<h4>%s</h4>", topicrow[1]);
	printf("<h3><a href=\"rss.cgi?topic=%s\">%s RSS Feed</a></h3>",
		topicrow[0], topicrow[1]);

	if(mysql_num_rows(newsresult) == 0) {
		printf("<p>There are no news items posted under this topic.</p>");
	} else {	
		while((row = mysql_fetch_row(newsresult)) != 0) {
			timedateformat(row[4], stimedate);

			sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND lid = \'2\'",row[0]);
			if(cLogQueryCommentDB() != 0) {
				printf("<p>Critical Error: comment lookup failure");
				htmlStaticPrint("footer");
				exit(EXIT_FAILURE);
			}
			l2commentcount =  (long) mysql_num_rows(commentresult);
			sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\'",row[0]);
			if(cLogQueryCommentDB() != 0) {
				printf("<p>Critical Error: comment lookup failure");
				htmlStaticPrint("footer");
				exit(EXIT_FAILURE);
			}
			sprintf(sqlBuffer,"SELECT realname FROM users WHERE username = \'%s\'",row[3]);
			if(cLogQueryUserDB() != 0) {
				printf("<p>Critical Error: user lookup failure");
				htmlStaticPrint("footer");
				exit(EXIT_FAILURE);
			}
			userrow = mysql_fetch_row(userresult);
			
			allcommentcount =  (long) mysql_num_rows(commentresult);
			if(!strstr(principalGroups,"l2read")) allcommentcount -= l2commentcount;

		 printf("<br /><h3><a href=\"news.cgi?nid=%s\">%s</a></h3>\n\r",row[0],row[1]);
			printf("<h2>Posted by %s ",userrow[0]);
			if(allcommentcount == 1) {
				printf("at %s with %li comment</h2></p>\n\r",row[4],allcommentcount);
			} else {
				printf("at %s with %li comments</h2></p>\n\r",row[4],allcommentcount);
			}
		}
	}

	mysql_free_result(newsresult);
	mysql_free_result(userresult);
	mysql_free_result(commentresult);
	mysql_free_result(topicresult);

	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");
	
	return 0;
}


/**
 * @brief Generates an HTML table listing topics from the database.
 *
 * This function queries the database for all topics and generates an HTML table
 * displaying the topics. Each topic is displayed with an image and a link to the
 * topic's page. The images are located in the "images/topics/" directory.
 *
 * The function performs the following steps:
 * 1. Executes a SQL query to select all topics from the database.
 * 2. Checks if the query was successful. If not, prints an error message and exits.
 * 3. Checks if any topics were returned by the query. If not, prints an error message and exits.
 * 4. Iterates over the topics and generates an HTML table row for each topic.
 * 5. Each row contains an image and a link to the topic's page.
 *
 * @note The function uses a global buffer `sqlBuffer` for constructing file paths.
 *       This is not ideal and should be refactored to use a separate buffer.
 *
 * @warning The function exits the program on critical errors, which may not be
 *          suitable for all applications.
 */
void topiclist() {
	long numtopics = 0;
	MYSQL_ROW topicrow;
	int i = 0;
	
	sprintf(sqlBuffer, "SELECT * FROM topics");
	if(cLogQueryTopicDB() != 0) {
		printf("<P>Critical Error: Topic lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	if(mysql_num_rows(topicresult) == 0) {
		printf("<P>Critical Error: Invalid Topic");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	
	numtopics = (long) mysql_num_rows(topicresult);
	printf("<table><tr>");
	for(i = 0; i < numtopics; i++) {
		topicrow = mysql_fetch_row(topicresult);
		if (i % 4 == 0) printf("</tr><tr><td><br><br></td></tr><tr>");
		/* sqlBuffer is being reused as a file path buffer here. This is bad of me but I did not
		   Want to create another variable just for this. So sue me
		*/
		sprintf(sqlBuffer, "%simages/topics/%s.gif", var_site_templates, topicrow[0]);
		printf("<td><a href=\"topic.cgi?topic=%s\"><img src=\"%simages/topics/%s.gif\" BORDER=\"0\" ALT=\"%s\" TITLE=\"%s\"></a>",
		topicrow[0], var_site_templates, topicrow[0], topicrow[2], topicrow[1]);
		printf("<br><a href=\"topic.cgi?topic=%s\">%s</a></td>",topicrow[0],topicrow[1]);
		printf("<td>&nbsp&nbsp&nbsp&nbsp</td>");
	}
	printf("</tr></table>");
}
