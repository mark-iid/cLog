/** rsscgi.c
 * provides an XML/RSS feed
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 16:46:10 $
 * $Header: /home/mxe20/src/clog/RCS/rsscgi.c,v 1.2 2005/05/11 16:46:10 mxe20 Exp mxe20 $
 * $Id: rsscgi.c,v 1.2 2005/05/11 16:46:10 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: rsscgi.c,v $
 * Revision 1.2  2005/05/11 16:46:10  mxe20
 * Added option to supress news item content
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: rsscgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/rsscgi.c,v $
 * $State: Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clogdb.h"
#include "cloghtml.h"
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
	int maxstories = -1;
	long nrows = 0;
	MYSQL_ROW row = NULL;	
	struct field_rec *p;
	char *topicid = NULL;
	char *authorid = NULL;
	
	cLogInitDB();
	
	/* http get */
	urlDecode(getenv("QUERY_STRING"));	
	p=urldec;
	while (p) {
		if(strcmp(p->field_name,"topic") == 0) 
			topicid = strdup(p->field_data);
		if(strcmp(p->field_name,"author") == 0)
			authorid = strdup(p->field_data);
		if(strcmp(p->field_name,"maxentries") == 0)
			maxstories = atoi(p->field_data);
		p=p->next;
	}
	
	/* If no maxstories was specified, default to 10 */
	if(maxstories == -1) maxstories = 10;

	printf("content-type: text/xml; charset=utf-8\r\n\r\n");
	printf("<?xml version=\"1.0\"?>\r\n");
	printf("<rss version=\"2.0\" xmlns:blogChannel=\"http://backend.userland.com/blogChannelModule\">\r\n");
 	printf("<channel>\r\n");
	printf("<title>%s</title>\r\n",var_site_name);
	printf("<language>en-us</language>\r\n");
	printf("<generator>cLog</generator>\r\n");
	printf("<managingEditor>%s</managingEditor>\r\n", var_admin_email);
	printf("<webMaster>%s</webMaster>\r\n", var_admin_email);
	printf("<link>http://%s</link>\r\n",var_site_url);
	printf("<description>%s</description>\r\n",var_site_description);
 
	printf("<image>\r\n");
	printf("<title>%s</title>\r\n",var_site_name);
	printf("<url>http://%s/images/cloglogo.gif</url>\r\n",var_site_url);
	printf("<link>http://%s</link>\r\n",var_site_url);
	printf("<width>144</width>\r\n");
	printf("<height>30</height>\r\n");
	printf("</image>\r\n");
	
	if(maxstories != 0) {
		if(topicid != NULL && authorid == NULL) {
			sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\' AND topic = \'%s\'", topicid);
		} else if(topicid == NULL && authorid != NULL) {
			sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\' AND author = \'%s\'", authorid);
		} else if(topicid != NULL && authorid != NULL) {
			sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\' AND author = \'%s\' AND topic = \'%s\'", authorid, topicid);		
		} else {
			sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\'");
		}
		if(cLogQueryNewsDB() != 0) {
			printf("<title>Critical Error: NumberOfStories lookup</title>\r\n");
			printf("</rdf:RDF>\r\n");
			exit(EXIT_FAILURE);
		}
		nrows = (long) mysql_num_rows(newsresult);
		if(maxstories > nrows) maxstories = nrows;
			
		if(topicid != NULL && authorid == NULL) {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' AND topic = \'%s\' ORDER BY nid DESC LIMIT %i",
				topicid, maxstories);
		} else if(topicid == NULL && authorid != NULL) {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' AND author = \'%s\' ORDER BY nid DESC LIMIT %i",
				authorid, maxstories);
		} else if(topicid != NULL && authorid != NULL) {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' AND author = \'%s\' AND topic = \'%s\' ORDER BY nid DESC LIMIT %i",
				authorid, topicid, maxstories);
		} else {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' ORDER BY nid DESC LIMIT %i",
				maxstories);
		}
	/* If maxstories is 0, show ALL items in a catagory */
	} else {
		if(topicid != NULL && authorid == NULL) {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' AND topic = \'%s\' ORDER BY nid DESC",
				topicid);
		} else if(topicid == NULL && authorid != NULL) {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' AND author = \'%s\' ORDER BY nid DESC",
				authorid);
		} else if(topicid != NULL && authorid != NULL) {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' AND author = \'%s\' AND topic = \'%s\' ORDER BY nid DESC",
				authorid, topicid);
		} else {
			sprintf(sqlBuffer,"SELECT nid,title,intro,author FROM newsitems WHERE status != \'deleted\' ORDER BY nid DESC");
		}
	}
	if(cLogQueryNewsDB() != 0) {
		printf("<title>Critical Error: News item lookup failure</title>\r\n");
		printf("</rdf:RDF>\r\n");
		exit(EXIT_FAILURE);
	}


	while((row = mysql_fetch_row(newsresult))) {
		printf("<item>\r\n");
		printf("<title>");
		printconvrss(row[1]);
		printf("</title>\r\n");
		if(!strcmp(var_rss_show_description, "true")) {
			printf("<description>");
			printconvrss(row[2]);
			printf("</description>\r\n");
		}
		printf("<author>%s</author>\r\n",row[3]);
		printf("<link>http://%s/news.cgi?nid=%s</link>\r\n", var_site_url, row[0]);
		printf("</item>\r\n");
	}
	printf("</channel>\r\n");
	printf("</rss>\r\n");

	return 0;
}
