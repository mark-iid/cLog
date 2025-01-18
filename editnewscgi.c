/** editnewscgi.c
 * CGI to edit a specific news item 
 * Inputs:
 *	nid - newsID
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/editnewscgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: editnewscgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: editnewscgi.c,v $
 * Revision 1.2  2005/06/22 16:09:13  mxe20
 * *** empty log message ***
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: editnewscgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/editnewscgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include "cloghtml.h"
#include "clogdb.h"
#include "shownews.h"
#include "envar.h"
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
 * @file editnewscgi.c
 * @brief CGI program to display and edit news items.
 *
 * This program is a CGI script that handles the display and editing of news items.
 * It initializes the database connection, parses the query string for the news item ID (nid),
 * and displays the news item along with options to edit or delete it.
 * 
 * The program performs the following steps:
 * 1. Initializes the database connection.
 * 2. Outputs the HTML header for the news item.
 * 3. Retrieves the username and groups of the principal user.
 * 4. Parses the query string to extract the news item ID (nid).
 * 5. Displays the left-side navigation and content table.
 * 6. Displays the news item and options for editing or deleting it.
 * 7. Outputs the HTML footer.
 *
 * @return 0 on successful execution, exits with EXIT_FAILURE on critical error.
 */
int main () {
	int returncode = 0;
	int i;
	char *nid;
	int numQueryString;
	Envar *query;

	cLogInitDB();
	
	htmlHeader("News Item");
	
	returncode = cLogGetUsername(principalName);
	
	if(cLogGetUsername(principalName) >= 20) 
		printf("<p>Database name lookup failure");
	if(cLogGetGroups(principalName, principalGroups) >= 20)
		printf("<p>Database group lookup failure");

	
	query = parseEnVar(getenv("QUERY_STRING"), &numQueryString);
	if(numQueryString == 0) {
		printf("<P>Critical Error: Invalid News Request");
		htmlStaticPrint("footer");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < numQueryString; i++) {
		if(strcmp(query[i].key,"nid") == 0) nid = strdup(query[i].val);
	}

	for (i = 0; i < numQueryString; i++) {
		if(query[i].key != NULL) free(query[i].key);
		if(query[i].val != NULL) free(query[i].val);
	}

	htmlLeftSide(principalName, principalGroups);
	htmlStaticPrint("contenttablestart");
	shownews(principalName, principalGroups, nid);
	
	printf("<p>I'm working on this. Eventually you will be able to edit and delete (if no one has posted a comment) your news submissions -Mark</p>");

	htmlStaticPrint("contenttableend");	
	
	if(nid != NULL) free(nid);
	htmlStaticPrint("footer");
	return 0;
}
