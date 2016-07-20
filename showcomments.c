/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/showcomments.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: showcomments.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: showcomments.c,v $
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
 * $RCSfile: showcomments.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/showcomments.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "clogdb.h"
#include "showcomments.h"
#include "cloghtml.h"

typedef struct {
	char *cid;
	char *nid;
	char *pid;
	char *lid;
	char *author;
	char *title;
	char *timestamp;
} Comment_struct;


void showchildcomments(char *pid, Comment_struct *comments,int commentcount);

/* Show all comments belonging to news item nnid with parent comment id npid) */
void showcomments(char *principalName, char *principalGroups, int nnid, int npid) {
	long commentcount;
	MYSQL_ROW row;
	MYSQL_ROW userrow;
	int i;
	char nid[10]; 
	char pid[10];
	char stimedate[25];
	char *author;
    TEMPLATELIST *HtmlTemplate = NULL;
    char *htmlTemplateStart = NULL;     
	
	Comment_struct *comments = NULL;
	
	itoa(nnid,nid);
	itoa(npid,pid);
	
	
	if(!strstr(principalGroups,"l2read")) {
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND lid < \'2\' AND pid != \'%s\'",nid,pid);
		if(cLogQueryCommentDB()) {
			printf("<P>Critical Error: comment lookup failure 1");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		commentcount =  mysql_num_rows(commentresult);
	} else { 	
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND pid != \'%s\'",nid,pid);
		if(cLogQueryCommentDB()) {
			printf("<P>Critical Error: comment lookup failure 2");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		commentcount =  mysql_num_rows(commentresult);
	}
	comments = realloc(comments, sizeof(Comment_struct) * commentcount);
	for(i = 0;i < commentcount;i++) {
		char *author;
		row = mysql_fetch_row(commentresult);
		comments[i].cid = strdup(row[0]);
		comments[i].nid = strdup(row[1]);
		comments[i].pid = strdup(row[2]);
		author = strdup(row[3]);
		sprintf(sqlBuffer,"SELECT realname FROM users WHERE uid = \'%s\'",author);
		if(cLogQueryUserDB()) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		if(!(userrow = mysql_fetch_row(userresult))) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		comments[i].author = strdup(userrow[0]);
		comments[i].lid = strdup(row[4]);
		comments[i].title = strdup(row[6]);
		comments[i].timestamp = strdup(row[7]);
	}
	



	if(!strstr(principalGroups,"l2read")) {
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND lid < \'2\' AND pid = \'%s\'",nid,pid);
		if(cLogQueryCommentDB()) {
			printf("<P>Critical Error: comment lookup failure 3");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
	} else {
		sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND pid = \'%s\'",nid,pid);
		if(cLogQueryCommentDB()) {
			printf("<P>Critical Error: comment lookup failure 4");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
	}
	
        htmlReadTemplate("commenttablemain", &HtmlTemplate);
        htmlTemplateStart = (char*) HtmlTemplate;                                                                     

	while((row = mysql_fetch_row(commentresult))) {
		/* timedateformat(row[7], stimedate); */
	
		sprintf(sqlBuffer,"SELECT username,realname FROM users WHERE uid = \'%s\'",row[3]);
		if(cLogQueryUserDB()) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		if(!(userrow = mysql_fetch_row(userresult))) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
	
        HtmlTemplate = (TEMPLATELIST*) htmlTemplateStart;

                while(HtmlTemplate->next != NULL) {
                        if(strstr(HtmlTemplate->data, "[% title %]")) {
                                printf("%s",row[6]);
                        } else if(strstr(HtmlTemplate->data, "[% l2 %]")) {
                                if(!strcmp(row[4],"2")) printf(" (L2)");
                        } else if(strstr(HtmlTemplate->data, "[% author %]")) {
                                printf("%s",userrow[1]);
                        } else if(strstr(HtmlTemplate->data, "[% timestamp %]")) {
                                printf("%s",row[7]);
                        /*} else if(strstr(HtmlTemplate->data, "[% permalink %]")) {
                                printf(" (<a HREF=\"comments.cgi?cid=%s\">permalink</a>)",row[0],row[0]);
						*/                       
						} else if(strstr(HtmlTemplate->data, "[% comment %]")){
                                printconv(row[5]);
                        } else if(strstr(HtmlTemplate->data, "[% reply %]")) {
                                if (strstr(principalGroups,"registered"))
                                        printf("[ <a HREF=\"commentpost.cgi?nid=%s&amp;pid=%s\">Reply to This</a>]",row[1],row[0]);
                        /*} else if(strstr(HtmlTemplate->data, "[% children %]")){*/
                                /* this is gonna suck, fix it */
                        } else
                                printf("%s",HtmlTemplate->data);

                        HtmlTemplate = HtmlTemplate->next;
                }                                                                                                     
	
		showchildcomments(row[0], comments,commentcount);
		
	}	
	return;
}

void showchildcomments(char *pid, Comment_struct *comments,int commentcount) {
	int i;
	char timestamp[15];
	for(i = 0; i < commentcount; i++) {
		if(!strcmp(comments[i].pid,pid)) {
			timedateformat(comments[i].timestamp,timestamp);
			printf("<ul><li><a HREF=\"comments.cgi?nid=%s&amp;pid=%s\">%s</a>",comments[i].nid,comments[i].pid,comments[i].title);
			if(!strcmp(comments[i].lid,"2")) printf("(Internal)");		

			printf(" by %s posted on %s",comments[i].author,comments[i].timestamp);
			showchildcomments(comments[i].cid, comments,commentcount);
			printf("</li></ul>");
		}
	}
}
