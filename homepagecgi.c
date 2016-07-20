/** homepage.cgi.c
 * The homepage of cLog 
 * Displays header, sidebar, and latest news stories
 * This gets turned into index.cgi
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/homepagecgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: homepagecgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: homepagecgi.c,v $
 * Revision 1.2  2005/06/22 16:09:13  mxe20
 * Modifications for XHTML/CSS
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.3  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: homepagecgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/homepagecgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "cloghtml.h"
#include "sitevars.h"

int main () {

	cLogInitDB();	

        if(cLogGetUsername(principalName) >= 20) {
		htmlHeader("Error");
                printf("<p>Database name lookup failure");
	}

	if(strlen(principalName) == 0) {
		if(getenv("REMOTE_USER") != NULL) {
			printf("content-type: text/html\r\n\r\n");
			printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">");
			printf("<html><head><title>Logging In</title>");
			printf("<meta http-equiv=\"refresh\" content=\"0;URL=%s\">",var_site_login_url);
			printf("</head><body bgcolor=\"#FFFFFF\">");
			printf("<P>Logging in...<br>\n");
			printf("<P><a href=\"/\">Homepage</a>");
			htmlStaticPrint("footer");
			exit(EXIT_SUCCESS);
		}
	} 

	htmlHeader(var_site_description);

	if(cLogGetGroups(principalName, principalGroups) >= 20) 
		printf("<p>Database group lookup failure");
	
	htmlLeftSide(principalName, principalGroups);

	htmlStaticPrint("contenttablestart");

	shownews(principalName,principalGroups,"0");	
	
	htmlStaticPrint("contenttableend");
	
	htmlStaticPrint("footer");

	return 0;
	
}
	
