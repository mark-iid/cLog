/** commentpost.cgi.c
 * CGI for posting comments. 
 *  Vars Passed into:
 *		nid = NewsID, the news item the comment belongs to
 *		pid = ParentID, the parent comment of this new comment
 *			(0 if the news item itself is the parent
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/commentpostcgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: commentpostcgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: commentpostcgi.c,v $
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
 * $RCSfile: commentpostcgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/commentpostcgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cloghtml.h"
#include "clogdb.h"
#include "user.h"
#include "urldec.h"
#include "shownews.h"
#include "showcomments.h"

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
struct field_rec *urldec;

/**
 * @file commentpostcgi.c
 * @brief CGI script for posting comments on news items.
 *
 * This script handles the posting of comments on news items. It performs
 * various checks to ensure the user is authorized to post comments and
 * generates the HTML form for comment submission.
 *
 * @details
 * The script performs the following steps:
 * 1. Initializes the database connection.
 * 2. Retrieves the username and groups of the principal user.
 * 3. Checks if the user is authorized to post comments.
 * 4. Decodes the HTTP GET parameters to retrieve the news ID (nid) and parent ID (pid).
 * 5. Displays the parent news item if the parent ID is 0.
 * 6. Generates the HTML form for comment submission.
 * 7. If the comment is a response to another comment, it retrieves the parent comment details.
 * 8. Sets the appropriate visibility (public or internal) for the comment based on the parent comment and user authorization.
 * 9. Displays the allowed HTML tags for comments.
 * 10. Frees allocated memory and exits.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
int main () {
	int returncode = 0;
	MYSQL_ROW commentrow;
	struct field_rec *p;
	char *nid = NULL;	/* newsID */
	char *pid = NULL;	/* ParentID */

	cLogInitDB();
	
	htmlHeader("Comment Post");
	
	returncode = cLogGetUsername(principalName);

	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");

	/* Must be in the group 'registered' to post comments */
	if(!strstr(principalGroups,"registered")) {
		printf("<p>You are not authorized to post comments");
		htmlStaticPrint("footer");
		return 1;
	}

	/* http get */
	urlDecode(getenv("QUERY_STRING"));
	p=urldec;
	while (p) {
		if(strcmp(p->field_name,"nid") == 0) 
			nid = strdup(p->field_data);
		if(strcmp(p->field_name,"pid") == 0)
			pid = strdup(p->field_data);
		p=p->next;
	}


	htmlLeftSide(principalName, principalGroups);
	htmlStaticPrint("contenttablestart");
	
	/* Show parent news item */
	if(atoi(pid) == 0) shownews(principalName, principalGroups, nid);
	printf("<P>Post Comment<HR>");
	
	/* comment submission form */
	printf("<form method=post action=\"cpost.cgi\">");
	if(atoi(pid) == 0) printf("<p>Title: <input name=\"title\" type=text size=50 maxlength=100>");
	/* create comment title from parent title, appending "re:" if necessary */
	else {
		char re[3];
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE cid = \'%s\'",pid);
        if(cLogQueryCommentDB() != 0) {
			printf("<P>Critical Error: comment lookup failure 2");
            htmlStaticPrint("footer");
            exit(EXIT_FAILURE);
       	}
        commentrow = mysql_fetch_row(commentresult);
        if(commentrow[4] == "2" && (!strstr(principalGroups,"l2read") || !strstr(principalGroups,"l2post"))) {
        	printf("<P>You do not have authorization to respond to this comment"); 
            htmlStaticPrint("footer");
            exit(EXIT_FAILURE);
        }
		strncpy(re,commentrow[6],3);
		re[2] = 0;
		if(strcmp(re,"re:") == 0 || strcmp(re,"Re:") == 0 || strcmp(re,"RE:") == 0) {
			printf("<p>Title: <input name=\"title\" type=text size=50 maxlength=100 value=\"%s\">",commentrow[6]);
		} else {
			printf("<p>Title: <input name=\"title\" type=text size=50 maxlength=100 value=\"%s%s\">","Re: ",commentrow[6]);
		}
	}
	printf("<p>Comment:<p>");
	printf("<textarea wrap=\"virtual\" rows=\"10\" cols=\"50\" name=\"comment\"></textarea><br>");
	printf("<input type=\"HIDDEN\" name=\"nid\" value=\"%s\">",nid); /* newsID */
	printf("<input type=\"HIDDEN\" name=\"pid\" value=\"%s\">",pid); /* parentID */

	/* if the parent is an internal comment, make new comment also internal.
	 * Otherwise present user with the option (if they are authorized to post internal)
	 */
	
	if(atoi(pid) != 0) {
	 	if (strcmp(commentrow[4],"2") == 0) {
			printf("<input type=\"HIDDEN\" name=\"lid\" value=\"2\">");
		} else {
			printf("<P><input type=\"radio\" name=\"lid\" value=\"1\" CHECKED>Public Comment<BR>");
			printf("<P><input type=\"radio\" name=\"lid\" value=\"2\">Internal Comment<BR></P>");
		}
	} else if(strstr(principalGroups,"l2post")) {
		printf("<P><input type=\"radio\" name=\"lid\" value=\"1\" CHECKED>Public Comment<BR>");
		printf("<P><input type=\"radio\" name=\"lid\" value=\"2\">Internal Comment<BR></P>");
	} else {
		printf("<input type=\"HIDDEN\" name=\"lid\" value=\"1\">");
	}
	
	printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Post Comment\">");
	printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Preview Comment\">");
	printf("<INPUT type=\"RESET\" value=\"Reset\"></form>");
	printf("<h5><center>Allowed HTML tags: <b>&lt;b&gt; &lt;/b&gt; &lt;i&gt; &lt;/i&gt; &lt;a href=\"url\"&gt; and &lt;/a&gt<b>. Single Quotes are not allowed.<BR>");
	printf("Also, URLs starting with http:// or https:// will be converted into links</center></h5>");
	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");

	if(nid != NULL) free(nid);
	if(pid != NULL) free(pid);
	return 0;
}
