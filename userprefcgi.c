/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/userprefcgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: userprefcgi.c,v 1.2 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: userprefcgi.c,v $
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
 * $RCSfile: userprefcgi.c,v $
 * $Revision: 1.2 $
 * $Source: /home/mxe20/src/clog/RCS/userprefcgi.c,v $
 * $State: Exp $
 */
#include <stdio.h>
#include "clogdb.h"
#include "cloghtml.h"
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
 * @brief Main function to display user preferences.
 *
 * This function initializes the database connection, retrieves the username and groups,
 * and generates an HTML page displaying the user preferences.
 *
 * @return int Returns 0 on success, 1 on failure.
 */
int main() {
	int returncode = 0;
	int i = 0;
	MYSQL_ROW row;
	MYSQL_FIELD *field;

	// Initialize database connection
	cLogInitDB();

	// Generate HTML header
	htmlHeader("User Preferences");

	// Retrieve username and groups
	if (cLogGetUsername(principalName) >= 20)
		printf("<p>Database name lookup failure");
	if (cLogGetGroups(principalName, principalGroups) >= 20)
		printf("<p>Database group lookup failure");

	// Generate left side of the HTML page
	htmlLeftSide(principalName, principalGroups);
	htmlStaticPrint("contenttablestart");

	// Display user preferences
	printf("<P>User Preferences Page");
	printf("<BR>Currently there is really nothing you can do here %s<P>", principalName);

	// Query user information from the database
	sprintf(sqlBuffer, "SELECT uid,username,realname,groups,lockout FROM users WHERE username = '%s'", principalName);

	if (cLogQueryUserDB() != 0) {
		return 1;
	}

	// Generate HTML table with user information
	printf("<table border=2>\n<tr>");

	row = mysql_fetch_row(userresult);

	while ((field = mysql_fetch_field(userresult))) {
		printf("<td>%s</td>", field->name);
	}

	if (cLogQueryUserDB() != 0) {
		return 1;
	}

	printf("<tr>");

	row = mysql_fetch_row(userresult);

	while ((field = mysql_fetch_field(userresult))) {
		printf("<td>%s</td>", row[i++]);
	}
	printf("</table>");

	// Generate HTML footer
	htmlStaticPrint("contenttableend");
	htmlStaticPrint("footer");

	return 0;
}
