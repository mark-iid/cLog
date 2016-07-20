/*
 *
 * $Author: mxe20 $
 * $Date: 2005/06/22 16:09:13 $
 * $Header: /home/mxe20/src/clog/RCS/cloghtml.c,v 1.3 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Id: cloghtml.c,v 1.3 2005/06/22 16:09:13 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: cloghtml.c,v $
 * Revision 1.3  2005/06/22 16:09:13  mxe20
 * Modifications for XHTML/CSS
 *
 * Revision 1.2  2005/05/18 19:21:34  mxe20
 * standard header function now checks authenticated status
 *
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: cloghtml.c,v $
 * $Revision: 1.3 $
 * $Source: /home/mxe20/src/clog/RCS/cloghtml.c,v $
 * $State: Exp $
 */


#include "cloghtml.h"
#include "clogdb.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>

int htmlGetLine(FILE *file, char *buffer);
void htmlMainBox(char *principalName, char *principalGroups);

/** htmlGetLine
 * Reads in one line (until EOL) from open file and 
 *  puts it into the buffer
 * Parameters: 
 *  FILE *file - file descriptor
 *  char *buffer - line buffer (256 character array)
 * Returns:
 *  0 - success
 *  1 - end of file
 */
int htmlGetLine(FILE *file, char *buffer) {
	int i = 0;
	if(file == (FILE *)0) return 10;
	for(i = 0;i < 255;i++) {
		buffer[i] = getc(file);
		if(buffer[i] == '\r' || buffer[i] == '\n') {
			buffer[++i] = '\0';
			return 0;
		}
		if(buffer[i] == EOF) {
			buffer[i] = '\0';
			return 1;
		}
	}
	buffer[++i] = '\0';
	return 2; /* buffer length exceeded */
}

/** htmlHeader
 * Prints the HTML header and title 
 * Parameters:
 *  char *title - Page title
 */
void htmlHeader(char *title) {
	FILE *file = NULL;
	char buffer[256];
	char *templatefile;
	char *htmltemplate = "header.html";
	int x = 0;
	buffer[0] = 0;


	printf("content-type: text/html; charset=utf-8\r\n\r\n");

	templatefile = malloc((strlen(var_site_location) + strlen(var_site_templates)+strlen(htmltemplate) +1));
	sprintf(templatefile,"%s%s%s",var_site_location,var_site_templates,htmltemplate);
	
	
	file = fopen(templatefile, "r");
	free(templatefile);	
	if(file == (FILE *)0) {
		printf("Critical File Open error: header template");
		return;
	}
	do {
		x = htmlGetLine(file, buffer);
		if(x > 1) {
			printf("<P>Error: max line length exceded in header template");
			fclose(file);
			return;
		}
		else if(strstr(buffer,"[% title %]")) printf("%s::%s",var_site_name, title);
		else if(strstr(buffer,"[% css %]")) htmlStaticPrint("ssi/css");
		else printf(buffer);
	} while(x == 0);
	fclose(file);
	htmlStaticPrint("logo");
}

/** htmlHeaderCookie
 * Prints the HTML header and sets a session cookie
 * Parameters:
 *  char *title - Page title
 *  char *cookieID - Name of the cookie
 *  char *cookieContents - value stored in the cookie
 */
void htmlHeaderCookie(char *title, char *cookieID, char *cookieContents) {
	printf("Set-Cookie: %s=%s; domain=%s; path=/; \r\n",cookieID,cookieContents,var_site_url);
	printf("content-type: text/html\r\n\r\n");
	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">");
	printf("<html><head><title>%s</title>",title);
	printf("<meta http-equiv=\"refresh\" content=\"0;URL=http://%s\">",var_site_url);
	printf("</head><body bgcolor=\"#FFFFFF\">");
}

/** htmlReadTemplate
 * Opens a file, then reads it in line by line into a 
 *  linked list
 * Parameters:
 *  char *htmltemplate - temlpate name / file to open
 *  TEMPLATELIST **HtmlTemplate - pointer to linked list
 */
void htmlReadTemplate(char *htmltemplate, TEMPLATELIST **HtmlTemplate) {
	char *first = NULL;
	FILE *file = NULL;
	char *filename;
	char *ext = ".html";
	int x = 0;

	filename = malloc(strlen(var_site_location) + strlen(var_site_templates)+strlen(htmltemplate) +strlen(ext)+1);
	sprintf(filename,"%s%s%s%s",var_site_location,var_site_templates,htmltemplate,ext);
	
	file = fopen(filename, "r");
	free(filename);
	if(file == (FILE *)0) {
		printf("Critical File Open error: %s",filename);
		return;
	}
	(*HtmlTemplate) = (TEMPLATELIST*)malloc(sizeof(TEMPLATELIST));
	first = (char*) (*HtmlTemplate);
	do {
		x = htmlGetLine(file, (*HtmlTemplate)->data);
		if(x > 1) {
			printf("<P>Error: max line length exceded in %s",filename);
			fclose(file);
			return;
		} else if(x == 1) {
			break;
		}
		(*HtmlTemplate)->next = (TEMPLATELIST*)malloc(sizeof(TEMPLATELIST));
		(*HtmlTemplate) = (*HtmlTemplate)->next;
		(*HtmlTemplate)->next = NULL;
	} while(x == 0);
	(*HtmlTemplate) = (TEMPLATELIST*) first;
	fclose(file);
}


/** htmlStaticPrint
 * Prints out the passed templatelist
 * Parameters: 
 *  char *htmltemplate - the html template to print
 */
void htmlStaticPrint(char *htmltemplate) {
	FILE *file = NULL;
	char buffer[256];
	char *filename;
	char *ext = ".html";

	int x = 0;
	
	filename = malloc((strlen(var_site_location) + strlen(var_site_templates)+strlen(htmltemplate)+strlen(ext) +1));
	sprintf(filename,"%s%s%s%s",var_site_location,var_site_templates,htmltemplate,ext);
	
	file = fopen(filename, "r");
	if(file == (FILE *)0) {
		printf("Critical File Open error: %s",filename);
		free(filename);  
		return;
	}
	free(filename);  
	do {
		x = htmlGetLine(file, buffer);
		if(x > 1) { 
			printf("<P>Error: max line length exceded in %s",filename);
			fclose(file);
			return;
		}
		printf(buffer);
	} while(x == 0);
	fclose(file);
}

/** htmlLeftSide
 * Draws the left side (whatever boxes should be there
 * This exists so that someday it can customized by the user
 */
void htmlLeftSide(char *principalName, char *principalGroups) {
	htmlMainBox(principalGroups, principalGroups);
	htmlStaticPrint("usefulbox");
}

/** htmlMainBox
 * Draws the box with all the site links
 * Parameters: 
 *  char *principalName - user name
 *  char *principalGroups - group list
 */
void htmlMainBox(char *principalName, char *principalGroups) {
	FILE *file = NULL;
	char buffer[256];
	char *filename;
	char *htmltemplate = "mainbox.html";
	int x = 0;
	buffer[0] = 0;

	filename = malloc((strlen(var_site_location) + strlen(var_site_templates)+strlen(htmltemplate) +1));
	sprintf(filename,"%s%s%s",var_site_location,var_site_templates,htmltemplate);
	
	file = fopen(filename, "r");

	if(file == (FILE *)0) {
		printf("Critical File Open error: mainbox template");
		return;
	}
	do {
		x = htmlGetLine(file, buffer);
		if(x > 1) {
			printf("<P>Error: max line length exceded in mainbox template");
			fclose(file);
			return;
		}
		if(strstr(buffer,"[% login %]")) {
			if(strlen(principalName) == 0)
			printf("<li><a href=\"%s\">Login</a></li>",var_site_login_url);
		} else if(strstr(buffer,"[% preferences %]")) {
			if(strlen(principalName) != 0) 
				printf("<li><a href=\"userpref.cgi\">Preferences</a></li>");
		} else if(strstr(buffer,"[% site admin %]")) {
			if(strstr(principalGroups,"admin")) 
				printf("<li><a href=\"admin.cgi\">Site Admin</a></li>");
		} else if(strstr(buffer, "[% newspost %]")) {
			if(strstr(principalGroups,"newspost"))
				printf("<li><a href=\"newspost.cgi\">Post News</a></li>");
		} else 
			printf(buffer);
	} while(x == 0);
	fclose(file);
}

/** htmlDBError
 * Prints out the error message for a DB error
 * Parameters:
 *  int err - Error code
 */
void htmlDBError(char *principalName, int err)
{
		printf("<p>Something horrible happened in the database!<BR>");
		printf("Contact <a href=\"mailto:%s\"%s</a>",var_admin_email,var_admin_name);
		printf("and say that you saw error code = %i and what you were doing",err);
		free(principalName);
		htmlStaticPrint("footer");	
		exit(EXIT_FAILURE);
}	


/** printconv
 * Converts string to be printed into "HTML safe" string.
 * It does this by stripping out all html except for <b>, 
 *  </b>, <i>, </i>, <br>, <a>, and </a> tags. Also converts
 *  all strings beginning with "http://" or "https://"
 *  into links.
 * Parameters:
 *  char *s - string to convert and print
 */
void printconv(char *s) {
	int len = strlen(s);
	int i, j, begin, end, spaces = 0;
	for(i = 0;i < len;) {
		if(s[i] == '<') {
			if(tolower(s[i+1]) == 'b' && s[i+2] == '>') { /* <b> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
			} else if(tolower(s[i+1]) == 'b' && tolower(s[i+2]) == 'r' && s[i+3] == '>') { /* <br> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(' ');
				putchar('/');
				putchar(s[i++]);
			} else if(tolower(s[i+1]) == 'i' && s[i+2] == '>') { /* <i> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
			} else if(tolower(s[i+1]) == 'a' && s[i+2] == ' ' && tolower(s[i+3]) == 'h' && tolower(s[i+4]) == 'r' && tolower(s[i+5]) == 'e' && tolower(s[i+6]) == 'f') {
				while(s[i] != '>') putchar(s[i++]);
			} else if(s[i+1] == '/' && tolower(s[i+2]) == 'b' && s[i+3] == '>') { /* </b> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
			} else if(s[i+1] == '/' && tolower(s[i+2]) == 'i' && s[i+3] == '>') { /* </i> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
			} else if(s[i+1] == '/' && tolower(s[i+2]) == 'a' && s[i+3] == '>') { /* </a> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
			} else {
				i++;
				while(s[i-1] != '>') {
					i++;
				}
			}
		} else if(s[i] == 13 && s[i+1] == 10) {
			printf("<BR />");
			i++;
		} else if(tolower(s[i]) == 'h' && tolower(s[i+1]) == 't' && tolower(s[i+2]) == 't' && tolower(s[i+3]) == 'p' && (s[i+4] == ':' || (tolower(s[i+4]) == 's' && s[i+5] == ':'))) {
			printf("<a href=\"");
			begin = i;
			while(s[i] != ':') putchar(s[i++]);
			putchar(s[i++]);
			putchar(s[i++]);
			putchar(s[i++]);
			while(isgraph(s[i])) putchar(s[i++]);
			end = i-1;
			printf("\">");
			for(j = begin, spaces = 0; j <= end; j++) {
				putchar(s[j]);
				if(++spaces == 60) {
					putchar(' ');
					spaces = 0;
				}
			}
			printf("</a>");			
		} else if(s[i] == '\"' || (s[i] == 20 && (s[i+1] == 18 || s[i+1] == 19))) {
			printf("&#34;"); /* " character */
			i++;
		} else if(s[i] == '\'') { 
			printf("&#39;"); /* ' character */
			i++;
		} else if(s[i] == '&') {
			printf("&amp;"); /* & character */
			i++;
		} else {
			putchar(s[i++]);
		}	
	}
}

void printconvrss(char *s) {
	int len = strlen(s);
	int i, j, begin, end, spaces = 0;
	/* if(len > 200) len = 200; */
	for(i = 0;i < len;) {
		if(s[i] == '<') {
			if(tolower(s[i+1]) == 'a' && s[i+2] == ' ' && tolower(s[i+3]) == 'h' && tolower(s[i+4]) == 'r' && tolower(s[i+5]) == 'e' && tolower(s[i+6]) == 'f') {
				printf("<![CDATA[");
				while(s[i] != '>') {
					putchar(s[i++]);
				}
			} else if(tolower(s[i+1]) == 'b' && tolower(s[i+2]) == 'r' && s[i+3] == '>') { /* <br> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(' ');
				putchar('/');
				putchar(s[i++]);
			} else if(s[i+1] == '/' && tolower(s[i+2]) == 'a' && s[i+3] == '>') { /* </a> */
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				putchar(s[i++]);
				printf("]]>");
			} else {
				i++;
				while(s[i-1] != '>') {
					i++;
				}
			}
		} else if(tolower(s[i]) == 'h' && tolower(s[i+1]) == 't' && tolower(s[i+2]) == 't' && tolower(s[i+3]) == 'p' && (s[i+4] == ':' || (tolower(s[i+4]) == 's' && s[i+5] == ':'))) {
			printf("<![CDATA[<a href=\"");
			begin = i;
			while(s[i] != ':') putchar(s[i++]);
			putchar(s[i++]);
			putchar(s[i++]);
			putchar(s[i++]);
			while(isgraph(s[i])) putchar(s[i++]);
			end = i-1;
			printf("\">");
			for(j = begin, spaces = 0; j <= end; j++) {
				putchar(s[j]);
				if(++spaces == 60) {
					putchar(' ');
					spaces = 0;
				}
			}
			printf("</a>]]>");	
		} else if(s[i] == '\"' || (s[i] == 20 && (s[i+1] == 18 || s[i+1] == 19))) {
			printf("&#34;");
			i++;
		} else if(s[i] == '\'') {
			printf("&#39;");
			i++;
		} else if(s[i] == '&') {
			printf("&#38;");
			i++;
		} else if(s[i] == 13 && s[i+1] == 10) {
			printf("<![CDATA[<BR />]]>");
			i++;
		} else {
			if(spaces == 60) {putchar(' ');spaces = 0;}
			putchar(s[i++]);
			if(s[i] != ' ') spaces++; else spaces = 0;
		}	
	}
}
