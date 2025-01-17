/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/postcgi.c,v 1.4 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: postcgi.c,v 1.4 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: postcgi.c,v $
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
 * $RCSfile: postcgi.c,v $
 * $Revision: 1.4 $
 * $Source: /home/mxe20/src/clog/RCS/postcgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cloghtml.h"
#include "clogdb.h"
#include "envar.h"
#include "user.h"
#include "urldec.h"
#include "sitevars.h"

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

int main () {
	int returncode = 0;
	long numtopics = 0;
	MYSQL_ROW topicrow;
	MYSQL_ROW row;
	MYSQL_ROW userrow;
	int i = 0;
	int err;
	int nid;
	int contentlength;
	struct field_rec *p;
	char str[10000];
	char *titleStr = NULL;
	char *introStr = NULL;
	char *bodyStr = NULL;
	char *topicStr = NULL;
	char *titleStrEsc = NULL;
	char *introStrEsc = NULL;
	char *bodyStrEsc = NULL;
	char *button = NULL;
	char *selected = NULL;
	TEMPLATELIST *HtmlTemplate = NULL;
	char *htmlTemplateStart = NULL;
	FILE *fp;

	cLogInitDB();
	
	fp = freopen("/tmp/post.log", "a+", stderr); 
	
	contentlength = atoi((getenv("CONTENT_LENGTH")));

	for(i=0;i<contentlength;i++) {
		str[i] = getchar();
	}
	str[++i] = '\0';
	
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

	htmlLeftSide(principalName, principalGroups);
	htmlStaticPrint("contenttablestart");

	
	urlDecode(str);	
	p=urldec;
	while (p) {
			p->field_name,p->field_data;
			if(!strcmp(p->field_name,"title")) 
				titleStr = strdup(p->field_data);
			if(!strcmp(p->field_name,"intro"))
				introStr = strdup(p->field_data);
			/*
			if(!strcmp(p->field_name,"body"))
				bodyStr = strdup(p->field_data);
			*/
			if(!strcmp(p->field_name,"topic"))
				topicStr = strdup(p->field_data);
			if(!strcmp(p->field_name,"button"))
				button = strdup(p->field_data);
			p=p->next;
	}
	
	/* lookup user name of news item author */
	sprintf(sqlBuffer,"SELECT realname FROM users WHERE username = \'%s\'",
	principalName);
	if(cLogQueryUserDB() != 0) {
		printf("<P>Critical Error: user lookup failure");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	userrow = mysql_fetch_row(userresult);
	
	/* lookup topic info */
	sprintf(sqlBuffer, "SELECT * FROM topics WHERE tid = \"%s\"", topicStr);
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
	topicrow = mysql_fetch_row(topicresult);
	fprintf(stderr, "cLog: post.cgi - read in template, preparing to loop\n"); 

	/* read in the newstablemain template and point to the first line */
	htmlReadTemplate("newstablemain", &HtmlTemplate);
	htmlTemplateStart = (char*) HtmlTemplate;

	/* look for special tags in template */
	while(HtmlTemplate != NULL) {
		if(strstr(HtmlTemplate->data, "[% title %]")) {
			printconv(titleStr);
		} else if(strstr(HtmlTemplate->data, "[% author %]")) {
			printf("%s",userrow[0]);
		} else if(strstr(HtmlTemplate->data, "[% timedate %]")) {
			printf("[TIMESTAMP]");
		} else if(strstr(HtmlTemplate->data, "[% intro %]")) {
			printconv(introStr);
		} else if(strstr(HtmlTemplate->data, "[% topic %]")) {
			printf("<a href=\"topic.cgi?topic=%s\"><img src=\"%simages/topics/%s.gif\" BORDER=\"0\" ALT=\"%s\" TITLE=\"%s\"></a>",
				topicStr, var_site_templates, topicStr, topicrow[1], topicrow[1]);
		} else if(strstr(HtmlTemplate->data, "[% edit %]")) {
			/* Do nothing */
			printf("<!-- edit -->");
		} else if(strstr(HtmlTemplate->data, "[% body %]")) {
			printf("<!-- body -->");
		} else if(strstr(HtmlTemplate->data, "[% permalink %]")) {
			/* Do nothing */
			printf("<!-- permalink -->");
		} else if(strstr(HtmlTemplate->data, "[% reply %]")) {
			/* Do nothing */
			printf("<!-- reply -->");
		} else /* no special tags, just print it */
			printf("%s",HtmlTemplate->data);
			
		HtmlTemplate = HtmlTemplate->next;
	}
	
	fprintf(stderr, "cLog: post.cgi - after loop\n"); 


	if(!strcmp(button,"Preview Story")) {
		
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
		
		printf("<P>Submit Story<HR>");
		printf("<form method=post action=\"post.cgi\">");
		printf("<p>Topic: <select name=\"topic\">\n");
		for(i = 0; i < numtopics; i++) {
			selected = NULL;
			topicrow = mysql_fetch_row(topicresult);
			if(!strcmp(topicrow[0], topicStr)) selected = strdup("selected");
			printf("<option value=\"%s\"%s>%s\n", topicrow[0], selected, topicrow[1]);
			if(selected != NULL) free(selected);
		}
		printf("</select>\n");
		printf("<p>Title: <input name=\"title\" type=text size=\"50\" value=\"%s\">",titleStr);
		printf("<p>News Item:<p>");
		printf("<textarea wrap=\"virtual\" rows=\"20\" cols=\"75\" name=\"intro\">%s</textarea>",introStr);
		/*
		printf("<p>Story Body:<p>");
		printf("<textarea wrap=\"virtual\" rows=\"10\" cols=\"50\" name=\"body\">%s</textarea><BR>",bodyStr);
		*/
		printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Submit Story\">");
		printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Preview Story\">");
		printf("<INPUT type=\"RESET\" value=\"Reset\"></form>");
		printf("<h5><center>Allowed HTML tags: <b>&lt;b&gt; &lt;/b&gt; &lt;i&gt; &lt;/i&gt; &lt;a href=\"url\"&gt; and &lt;/a&gt<b>. Single Quotes are not allowed.<BR>");
		printf("Also, URLs starting with http:// or https:// will be converted into links</center></h5>");
	} else {
	
		titleStrEsc = malloc(strlen(titleStr) * 2);
		introStrEsc = malloc(strlen(introStr) * 2);
		/*
		if(strlen(bodyStr) != 0)
			bodyStrEsc = malloc(strlen(bodyStr) * 2);
		*/
		escConv(titleStr, titleStrEsc);
		escConv(introStr, introStrEsc);
		/*
		if(strlen(bodyStr) != 0)
			escConv(bodyStr, bodyStrEsc);
		*/
		sprintf(sqlBuffer,"INSERT INTO newsitems (title, intro, timestamp, author, topic) VALUES (\'%s\',\'%s\',sysdate(),\'%s\',\'%s\')",
			titleStrEsc, introStrEsc, principalName, topicStr);

		err = cLogQueryNewsDB();
		
		if(err == 0)  {
			printf("<p>Story posted - <a href=\"/\">Return Home</a>");		
		} else { 
			printf("<p>Story post failed - <a href=\"/\">Return Home</a>");
			printf("<p>SQL Error: %s", mysql_error(&cLogDB));
			//htmlDBError(principalName, err);
		}
		/* free(bodyStrEsc); */		
	}	
	
	
	free(titleStr);
	free(introStr);
	/* if(bodyStr != NULL) free(bodyStr); */

	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");
	return 0;
}
