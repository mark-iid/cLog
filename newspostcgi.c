/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/newspostcgi.c,v 1.4 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: newspostcgi.c,v 1.4 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: newspostcgi.c,v $
 * Revision 1.4  2005/06/22 16:09:13  mxe20
 * Modifications for XHTML/CSS
 *
 * Revision 1.3  2005/05/18 21:13:22  mxe20
 * removed topic suggestion link
 *
 * Revision 1.2  2005/05/11 16:46:10  mxe20
 * Expanded text input field size
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: newspostcgi.c,v $
 * $Revision: 1.4 $
 * $Source: /home/mxe20/src/clog/RCS/newspostcgi.c,v $
 * $State: Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cloghtml.h"
#include "clogdb.h"
#include "user.h"


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
 * @file newspostcgi.c
 * @brief CGI script for submitting news posts.
 *
 * This script handles the submission of news posts by authorized users.
 * It connects to the database, retrieves available topics, and displays
 * a form for submitting a news story.
 *
 * The script performs the following steps:
 * 1. Initializes the database connection.
 * 2. Outputs the HTML header.
 * 3. Retrieves the username and groups of the current user.
 * 4. Checks if the user is authorized to post stories.
 * 5. Retrieves the list of available topics from the database.
 * 6. Displays the form for submitting a news story.
 * 7. Outputs the HTML footer.
 *
 * The form includes fields for selecting a topic, entering a title, and
 * writing the news item. It also provides options to submit or preview
 * the story, and to reset the form.
 *
 * @return Returns 0 on success, or 1 on authorization failure.
 */
int main () {
	int returncode = 0;
	int i = 0;
	long numtopics = 0;
	MYSQL_ROW topicrow;
	
	cLogInitDB();
	
	htmlHeader("News Post");
	
	returncode = cLogGetUsername(principalName);

	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");

	if(!strstr(principalGroups,"newspost")) {
		printf("<p>You are not authorized to post stories");
		htmlStaticPrint("footer");
		return 1;
	}
	
	sprintf(sqlBuffer, "SELECT * FROM topics");
	if(cLogQueryTopicDB() != 0) {
		printf("<P>Critical Error: Topic lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	numtopics = (long) mysql_num_rows(topicresult);
	if(numtopics == 0) {
		printf("<P>Critical Error: Invalid Topic");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}


	htmlLeftSide(principalName, principalGroups);
	htmlStaticPrint("contenttablestart");

	
	printf("<h3>Submit Story</h3><hr />");
	
	printf("<form method=post action=\"post.cgi\">");

	printf("<p>Topic: <select name=\"topic\">\n");
	for(i = 0; i < numtopics; i++) {
		topicrow = mysql_fetch_row(topicresult);
		printf("<option value=\"%s\">%s\n", topicrow[0], topicrow[1]);
	}
	printf("</select>\n");
		
	printf("<p>Title: <input name=\"title\" type=text size=\"50\" />");
	printf("<p>News Item:<p>");
	printf("<textarea wrap=\"virtual\" rows=\"20\" cols=\"75\" name=\"intro\"></textarea><br />");
	/*
	printf("<p>Story Body:<p>");
	printf("<textarea wrap=\"virtual\" rows=\"10\" cols=\"50\" name=\"body\"></textarea><BR>");
	*/
	printf("<br />");
	printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Submit Story\" />");
	printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Preview Story\" />");
	printf("<INPUT type=\"RESET\" value=\"Reset\" /></form>");
	printf("<h5><center>Allowed HTML tags: <b>&lt;b&gt; &lt;/b&gt; &lt;i&gt; &lt;/i&gt; &lt;a href=\"url\"&gt; and &lt;/a&gt;<b>. Single Quotes are not allowed.<br />");
	printf("Also, URLs starting with http:// or https:// will be converted into links</center></h5>");
	
	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");
	
}
