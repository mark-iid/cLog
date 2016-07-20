/** cpost.cgi.c
 * CGI to post a comment.
 * input is an HTTP put either from commentpost.cgi or this cgi
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/cpostcgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: cpostcgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: cpostcgi.c,v $
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
 * $RCSfile: cpostcgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/cpostcgi.c,v $
 * $State: Exp $
 */
#include <stdlib.h>
#include <string.h>
#include "cloghtml.h"
#include "clogdb.h"
#include "envar.h"
#include "user.h"
#include "urldec.h"

int main () {
	int returncode = 0;
	int i; /* generic counter */
	int err; /* sql error number */
	MYSQL_ROW row;
	MYSQL_ROW userrow;
	int contentlength;	/* length of http post */
	struct field_rec *p;
	char *str = NULL; /* input buffer from http post */
	char *titleStr = NULL;
	char *commentStr = NULL;
	char *titleStrEsc = NULL;
	char *commentStrEsc = NULL;
	char *nid = NULL;	/* newsID */
	char *pid = NULL;	/* parentID */
	char *lid = NULL;	/* security level */
	char *button = NULL;	/* submit or preview button string */
    TEMPLATELIST *HtmlTemplate = NULL;
    char *htmlTemplateStart = NULL;     

	cLogInitDB();
	
	htmlHeader("Comment Post");
	
	contentlength = atoi((getenv("CONTENT_LENGTH")));

	str = malloc(sizeof(char) * contentlength);
	if(str == NULL) {
		printf("<P>Fatal Error, no data posted to this CGI, or out of memory<br>");
		printf("Either way, I don't know how to handle that. Goodbye");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	
	for(i=0;i<contentlength;i++) {
		str[i] = getchar();
	}
	str[++i] = '\0';

	returncode = cLogGetUsername(principalName);

	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");

	if(strstr(principalGroups,"registered") == 0) {
		printf("<p>You are not authorized to post comments");
		htmlStaticPrint("footer");
		free(str);
		exit(EXIT_FAILURE);
	}

	sprintf(sqlBuffer,"SELECT uid FROM users WHERE username = \'%s\'",
		principalName);
	if(cLogQueryUserDB() != 0) {
		printf("<P>Critical Error: comment lookup failure 1");
		htmlStaticPrint("footer");
		free(str);
		exit(EXIT_FAILURE);
	}
	row = mysql_fetch_row(userresult);


	htmlLeftSide(principalName, principalGroups);
	htmlStaticPrint("contenttablestart");
	
	/* decode http post */
	urlDecode(str);	
	p=urldec;
	while (p) {
			if(strcmp(p->field_name,"title") == 0) 
				titleStr = strdup(p->field_data);
			if(strcmp(p->field_name,"comment") == 0)
				commentStr = strdup(p->field_data);
			if(strcmp(p->field_name,"nid") == 0)
				nid = strdup(p->field_data);
			if(strcmp(p->field_name,"pid") == 0)
				pid = strdup(p->field_data);
			if(strcmp(p->field_name,"lid") == 0)
				lid = strdup(p->field_data);
			if(strcmp(p->field_name,"button") == 0)
				button = strdup(p->field_data);
			p=p->next;
	}

	free(str);
	
	/* print comment as it will appear */
    htmlReadTemplate("commenttablemain", &HtmlTemplate);
    htmlTemplateStart = (char*) HtmlTemplate;                                                                     
    while(HtmlTemplate->next != NULL) {
		if(strstr(HtmlTemplate->data, "[% title %]")) {
			printf("%s",titleStr);
		} else if(strstr(HtmlTemplate->data, "[% l2 %]")) {
			if(!strcmp(lid,"2")) printf(" (L2)");
		} else if(strstr(HtmlTemplate->data, "[% author %]")) {
			printf(principalName);
		} else if(strstr(HtmlTemplate->data, "[% timestamp %]")) {
			printf("[TIMESTAMP]");
		/*} else if(strstr(HtmlTemplate->data, "[% permalink %]")) {
			printf(" (<a HREF=\"comments.cgi?cid=%s\">permalink</a>)",row[0],row[0]);
		*/                       
		} else if(strstr(HtmlTemplate->data, "[% comment %]")){
			printconv(commentStr);
		} else if(strstr(HtmlTemplate->data, "[% reply %]")) {
			printf("<!-- reply -->");
		/*} else if(strstr(HtmlTemplate->data, "[% children %]")){*/
			/* this is gonna suck, fix it */
		} else
			printf("%s",HtmlTemplate->data);
		HtmlTemplate = HtmlTemplate->next;
    }                                                                                                     


	if(strstr(button,"Preview Comment") != 0) {
		/* if the preview button was pressed, give user another chance to edit comment */
		printf("<P>Post Comment<HR>");
	
		printf("<form method=post action=\"cpost.cgi\">");
	
		printf("<p>Title: <input name=\"title\" type=text size=50 maxlength=100 value=\"%s\">",titleStr);
		printf("<p>Comment:<p>");
		printf("<textarea wrap=\"virtual\" rows=\"10\" cols=\"50\" name=\"comment\">%s</textarea><br>",commentStr);
		printf("<input type=\"HIDDEN\" name=\"nid\" value=\"%s\">",nid);
		printf("<input type=\"HIDDEN\" name=\"pid\" value=\"%s\">",pid);
		if(strcmp(lid,"2") == 0) {
			printf("<input type=\"HIDDEN\" name=\"lid\" value=\"2\">");
		} else if(strstr(principalGroups,"l2post")) {
			printf("<P><input type=\"radio\" name=\"lid\" value=\"1\" CHECKED>Public Comment<BR>");
			printf("<P><input type=\"radio\" name=\"lid\" value=\"2\">PSU ET Internal Comment<BR></P>");
		} else {
			printf("<input type=\"HIDDEN\" name=\"lid\" value=\"1\">");
		}
		printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Post Comment\">");
		printf("<input type=\"SUBMIT\" name=\"button\" Value=\"Preview Comment\">");
		printf("<INPUT type=\"RESET\" value=\"Reset\"></form>");
		printf("<h5><center>Allowed HTML tags: <b>&lt;b&gt; &lt;/b&gt; &lt;i&gt; &lt;/i&gt; &lt;a href=\"url\"&gt; and &lt;/a&gt<b>. Single Quotes are not allowed.<BR>");
		printf("Also, URLs starting with http:// or https:// will be converted into links</center></h5>");
	} else {
		/* comment was submitted, insert it into the db */
		/* first replace certain characters with escape sequences */		
		titleStrEsc = malloc(strlen(titleStr) * 2);
		commentStrEsc = malloc(strlen(commentStr) * 2);
		escConv(titleStr, titleStrEsc);
		escConv(commentStr, commentStrEsc);
		
		sprintf(sqlBuffer,"INSERT INTO comments (nid, pid, authorid, lid, comment, title) VALUES (\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')",
			nid, pid, row[0], lid, commentStrEsc, titleStrEsc);
	
		err = cLogQueryCommentDB();
		if(err == 0) {
			printf("<p>Comment posted - <a href=\"/\">Return Home</a>");		
		}
		if (err != 0) htmlDBError(principalName, err);
		if(titleStrEsc != NULL) free(titleStrEsc);
		if(titleStrEsc != NULL) free(commentStrEsc);
	}		
		
	
	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");
	
	if(titleStr != NULL) free(titleStr);
	if(commentStr != NULL) free(commentStr);
	if(nid != NULL) free(nid);
	if(pid != NULL) free(pid);
	if(lid != NULL) free(lid);
	
	return 0;
}
