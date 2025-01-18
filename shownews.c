/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/shownews.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: shownews.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: shownews.c,v $
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
 * $RCSfile: shownews.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/shownews.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clogdb.h"
#include "cloghtml.h"
#include "shownews.h"

void timedateformat(char unformatted[], char stimedate[]);


/**
 * @brief Displays news items based on the provided parameters.
 *
 * This function retrieves and displays news items from a database. It can either
 * display the latest news items or a specific news item based on the provided
 * news ID (nid). The function also formats and displays various details about
 * each news item, such as the title, author, timestamp, introduction, topic, 
 * and comments.
 *
 * @param principalName The name of the principal user.
 * @param principalGroups The groups the principal user belongs to.
 * @param nid The news ID to display. If 0, the latest news items are displayed.
 *
 * The function performs the following steps:
 * 1. Determines the read level based on the principal's groups.
 * 2. If nid is 0, retrieves the latest news items up to a maximum of 10.
 * 3. If nid is not 0, retrieves the specific news item with the given nid.
 * 4. Reads the HTML template for displaying news items.
 * 5. For each news item retrieved:
 *    - Formats the timestamp.
 *    - Looks up the author of the news item.
 *    - Looks up the topic information.
 *    - Resets the template linked list pointer.
 *    - Processes and replaces special tags in the template with actual data.
 *    - Displays the formatted news item.
 *
 * The function handles various critical errors by printing an error message
 * and exiting the program.
 */
void shownews(char *principalName, char *principalGroups, char *nid) {
	MYSQL_ROW row = NULL;	
	MYSQL_ROW userrow = NULL;
	MYSQL_ROW topicrow = NULL;
	int readlevel = 0;
	int maxstories = 10;
	int nnid = atoi(nid);
	char stimedate[25];
	long nrows = 0;
	long allcommentcount = 0;
	long l2commentcount = 0;
	TEMPLATELIST *HtmlTemplate = NULL;
	char *htmlTemplateStart = NULL;

	if(strstr(principalGroups,"l2read")) readlevel = 2;

	/* nnid == 0 - grab the last 10 news items
	 * otherwise, grab the news item with the nid == nnid
	 */
	if(nnid == 0) {
		sprintf(sqlBuffer,"SELECT nid FROM newsitems WHERE status != \'deleted\'");
		if(cLogQueryNewsDB() != 0) {
			printf("<P>Critical Error: NumberOfStories lookup");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		nrows = (long) mysql_num_rows(newsresult);
		if(maxstories > nrows) maxstories = nrows;
	
		sprintf(sqlBuffer,"SELECT * FROM newsitems WHERE status != \'deleted\' ORDER BY nid DESC LIMIT %i",
			maxstories);
		if(cLogQueryNewsDB() != 0) {
			printf("<P>Critical Error: News item lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
			}
	} else {
		sprintf(sqlBuffer,"SELECT * FROM newsitems WHERE nid = \'%s\' AND status != \'deleted\'",nid);
		int result = -1;
		result = cLogQueryNewsDB();
		if(result != 0) {
			printf("<P>Critical Error: News item lookup failure");
			htmlStaticPrint("footer");
			exit(0);
		}
		if(mysql_num_rows(newsresult) == 0) {
			printf("<p>Invalid news ID");
			htmlStaticPrint("footer");
			exit(0);
		}
	}
	
	/* read in the newstablemain template and point to the first line */
	htmlReadTemplate("newstablemain", &HtmlTemplate);
	htmlTemplateStart = (char*) HtmlTemplate;
	
	/* repeat for each row returned from the news lookup */
	while((row = mysql_fetch_row(newsresult))) {
		timedateformat(row[4], stimedate);

		/* lookup user name of news item author */
		sprintf(sqlBuffer,"SELECT * FROM users WHERE username = \'%s\'",
		row[3]);
		if(cLogQueryUserDB() != 0) {
			printf("<P>Critical Error: user lookup failure");
			htmlStaticPrint("footer");
			exit(EXIT_FAILURE);
		}
		userrow = mysql_fetch_row(userresult);

		
		/* lookup topic info */
		sprintf(sqlBuffer, "SELECT * FROM topics WHERE tid = \"%s\"", row[6]);
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

		/* reset template linked list pointer */
		HtmlTemplate = (TEMPLATELIST*) htmlTemplateStart;

		/* look for special tags in template */
		while(HtmlTemplate != NULL) {
			if(strstr(HtmlTemplate->data, "[% title %]")) {
				printf("%s",row[1]);
			} else if(strstr(HtmlTemplate->data, "[% author %]")) {
				printf("<a href=\"user.cgi?user=%s\">%s</a>",userrow[0],userrow[2]);
			} else if(strstr(HtmlTemplate->data, "[% timedate %]")) {
				/* printf("%s",stimedate); */
				printf("%s",row[4]);
			} else if(strstr(HtmlTemplate->data, "[% intro %]")) {
				printconv(row[2]);
			} else if(strstr(HtmlTemplate->data, "[% topic %]")) {
				printf("<a href=\"topic.cgi?topic=%s\"><img src=\"%simages/topics/%s.gif\" border=\"0\" alt=\"%s\" title=\"%s\" /></a>",
					row[6], var_site_templates, row[6], topicrow[1], topicrow[1]);
			} else if(strstr(HtmlTemplate->data, "[% edit %]")) {
				if((strcmp(principalName, row[3]) == 0) || strstr(principalGroups,"admin") != 0)
					printf(" <a href=\"editnews.cgi?nid=%i\">[EDIT]</a>",atoi(row[0]));
			} else if(strstr(HtmlTemplate->data, "[% permalink %]")) {
				if(nnid == 0) {
					printf("(<a HREF=\"news.cgi?nid=%i\">PermaLink</a>)",atoi(row[0]));
					sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\' AND lid = \'2\'",row[0]);
					if(cLogQueryCommentDB() != 0) {
						printf("<P>Critical Error: comment lookup failure");
						htmlStaticPrint("footer");
						exit(EXIT_FAILURE);
					}
					l2commentcount = (long) mysql_num_rows(commentresult);
					sprintf(sqlBuffer,"SELECT * FROM comments WHERE nid = \'%s\'",row[0]);
					if(cLogQueryCommentDB() != 0) {
						printf("<P>Critical Error: comment lookup failure");
						htmlStaticPrint("footer");
						exit(EXIT_FAILURE);
					}
					allcommentcount = (long) mysql_num_rows(commentresult);
					if(!strstr(principalGroups,"l2read")) allcommentcount -= l2commentcount;
					if(allcommentcount == 1) printf(" <a HREF=\"news.cgi?nid=%i\">%li comment</a>",atoi(row[0]),allcommentcount);
					else if(allcommentcount > 1) printf(" <a HREF=\"news.cgi?nid=%i\">%li comments</a>",atoi(row[0]),allcommentcount);
				}
			} else if(strstr(HtmlTemplate->data, "[% reply %]")) {
				if(/* nnid != 0 && */ strstr(principalGroups,"registered") !=0) {
					/* printf("<font SIZE=\"2\">"); */
					printf("(<a HREF=\"commentpost.cgi?nid=%s&amp;pid=0\">Reply to This</a>)",row[0]);
				}
			} else /* no special tags, just print it */
				printf("%s",HtmlTemplate->data);
				
			HtmlTemplate = HtmlTemplate->next;
		}
	}
}
