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

/**
 * @brief Reads a line from an HTML file.
 *
 * This function reads characters from the given file stream until it encounters
 * a newline character ('\n'), a carriage return character ('\r'), or the end of
 * the file (EOF). The read characters are stored in the provided buffer.
 *
 * @param file A pointer to the FILE object that identifies the input stream.
 * @param buffer A pointer to a character array where the read line will be stored.
 *               The buffer should be large enough to hold the line, including the
 *               terminating null character.
 * @return An integer indicating the result of the operation:
 *         - 0: Line read successfully.
 *         - 1: End of file reached.
 *         - 2: Buffer length exceeded.
 *         - 10: Invalid file pointer.
 */
int htmlGetLine(FILE *file, char *buffer) {
	int i = 0;
	int c;
	if (file == NULL) return 10;
	/* leave room for terminating NUL in 512-byte buffer */
	while (i < 511) {
		c = getc(file);
		if (c == EOF) {
			if (i == 0) {
				buffer[0] = '\0';
				return 1; /* EOF / no data */
			}
			buffer[i] = '\0';
			return 1; /* EOF after data */
		}
		if (c == '\r' || c == '\n') {
			buffer[i] = '\0';
			return 0; /* line read */
		}
		buffer[i++] = (char)c;
	}
	/* buffer full */
	buffer[511] = '\0';
	return 2; /* buffer length exceeded */
}


/**
 * @brief Generates and prints the HTML header for a web page.
 *
 * This function reads an HTML template file, replaces placeholders with actual values,
 * and prints the resulting HTML header to the standard output. It also prints the
 * content-type header for an HTML response.
 *
 * @param title The title to be inserted into the HTML header.
 *
 * The function performs the following steps:
 * 1. Prints the content-type header for an HTML response.
 * 2. Constructs the full path to the HTML template file.
 * 3. Opens the HTML template file for reading.
 * 4. Reads the template file line by line, replacing placeholders with actual values:
 *    - "[% title %]" is replaced with the site name and the provided title.
 *    - "[% css %]" triggers a call to `htmlStaticPrint` with "ssi/css" as the argument.
 * 5. Prints the processed lines to the standard output.
 * 6. Closes the template file.
 * 7. Calls `htmlStaticPrint` with "logo" as the argument to print the logo.
 *
 * If the template file cannot be opened, an error message is printed and the function returns.
 * If a line in the template file exceeds the maximum length, an error message is printed,
 * the file is closed, and the function returns.
 */
void htmlHeader(char *title) {
	FILE *file = NULL;
	char buffer[512];
	char *templatefile;
	char *htmltemplate = "header.html";
	int x = 0;
	buffer[0] = 0;


	printf("content-type: text/html; charset=utf-8\r\n\r\n");

	templatefile = malloc((strlen(var_site_location) + strlen(var_site_templates)+strlen(htmltemplate) +1));
	sprintf(templatefile,"%s%s%s",var_site_location,var_site_templates,htmltemplate);
	
	file = fopen(templatefile, "r");
	if(templatefile != NULL) free(templatefile);	
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
		else printf("%s",buffer);
	} while(x == 0);
	fclose(file);
	htmlStaticPrint("logo");
}


/**
 * Generates an HTML header with a cookie and a meta refresh tag.
 *
 * @param title The title of the HTML document.
 * @param cookieID The ID of the cookie to be set.
 * @param cookieContents The contents of the cookie to be set.
 *
 * This function prints out the HTTP headers to set a cookie and the content-type
 * as text/html. It also generates a basic HTML structure with a meta refresh tag
 * that redirects to the URL specified by the global variable `var_site_url`.
 */
void htmlHeaderCookie(char *title, char *cookieID, char *cookieContents) {
	printf("Set-Cookie: %s=%s; domain=%s; path=/; \r\n",cookieID,cookieContents,var_site_url);
	printf("content-type: text/html\r\n\r\n");
	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">");
	printf("<html><head><title>%s</title>",title);
	printf("<meta http-equiv=\"refresh\" content=\"0;URL=http://%s\">",var_site_url);
	printf("</head><body bgcolor=\"#FFFFFF\">");
}


/**
 * @brief Reads an HTML template file and stores its contents in a linked list.
 *
 * This function reads the contents of an HTML template file specified by the
 * `htmltemplate` parameter and stores each line in a linked list of `TEMPLATELIST`
 * structures. The linked list is pointed to by the `HtmlTemplate` parameter.
 *
 * @param htmltemplate The name of the HTML template file (without extension) to read.
 * @param HtmlTemplate A pointer to a pointer to a `TEMPLATELIST` structure where the
 *                     contents of the HTML template file will be stored.
 *
 * @note The function constructs the full path to the HTML template file using the
 *       `var_site_location` and `var_site_templates` global variables, and appends
 *       the ".html" extension to the `htmltemplate` parameter.
 *
 * @note The function allocates memory for the filename and the linked list structures.
 *       It is the caller's responsibility to free the allocated memory.
 *
 * @note If the file cannot be opened, the function prints an error message and returns.
 *       If a line in the file exceeds the maximum allowed length, the function prints
 *       an error message, closes the file, and returns.
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
	//if(filename != NULL) free(filename);
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


/**
 * @brief Prints the contents of an HTML template file to the standard output.
 *
 * This function constructs the full path to an HTML template file using the 
 * provided template name and predefined directory paths. It then opens the 
 * file, reads its contents line by line, and prints each line to the standard 
 * output. If the file cannot be opened or a line exceeds the maximum length, 
 * an error message is printed.
 *
 * @param htmltemplate The name of the HTML template file (without extension) to be printed.
 *
 * @note The function assumes that the global variables `var_site_location` and 
 * `var_site_templates` are defined and contain the necessary directory paths.
 *
 * @warning If the file cannot be opened, a critical error message is printed.
 * If a line exceeds the maximum length of 255 characters, an error message is 
 * printed and the function returns immediately.
 */
void htmlStaticPrint(char *htmltemplate) {
	FILE *file = NULL;
	char buffer[512];
	char *filename;
	char *ext = ".html";

	int x = 0;
	
	filename = malloc((strlen(var_site_location) + strlen(var_site_templates)+strlen(htmltemplate)+strlen(ext) +1));
	sprintf(filename,"%s%s%s%s",var_site_location,var_site_templates,htmltemplate,ext);
	
	file = fopen(filename, "r");
	if(file == (FILE *)0) {
		printf("Critical File Open error: %s",filename);
		if(filename != NULL) free(filename);  
		return;
	}
	//if(filename != NULL) free(filename);  
	do {
		x = htmlGetLine(file, buffer);
		if(x > 1) { 
			printf("<P>Error: max line length exceded in %s",filename);
			fclose(file);
			return;
		}
		printf("%s",buffer);
	} while(x == 0);
	fclose(file);
}


/**
 * @brief Generates the left side of the HTML page.
 *
 * This function creates the left side of the HTML page by calling the 
 * htmlMainBox function with the principal's groups and then prints a 
 * static "usefulbox".
 *
 * @param principalName The name of the principal (not used in this function).
 * @param principalGroups The groups associated with the principal.
 */
void htmlLeftSide(char *principalName, char *principalGroups) {
	htmlMainBox(principalGroups, principalGroups);
	htmlStaticPrint("usefulbox");
}


/**
 * @brief Generates the main HTML box content based on user roles and templates.
 *
 * This function reads an HTML template file and dynamically generates HTML content
 * based on the provided principal name and groups. It replaces specific placeholders
 * in the template with corresponding HTML elements.
 *
 * @param principalName The name of the principal (user) to be used in the HTML content.
 * @param principalGroups The groups to which the principal (user) belongs.
 *
 * The function performs the following steps:
 * 1. Constructs the full path to the HTML template file.
 * 2. Opens the HTML template file for reading.
 * 3. Reads the template file line by line.
 * 4. Replaces specific placeholders in the template with corresponding HTML elements
 *    based on the principal's name and groups.
 * 5. Outputs the generated HTML content.
 * 6. Closes the template file.
 *
 * Placeholders in the template:
 * - [% login %]: Replaced with a login link if the principal name is empty.
 * - [% preferences %]: Replaced with a preferences link if the principal name is not empty.
 * - [% site admin %]: Replaced with a site admin link if the principal belongs to the "admin" group.
 * - [% newspost %]: Replaced with a post news link if the principal belongs to the "newspost" group.
 *
 * @note The function prints error messages to the standard output if it encounters
 *       issues such as file open errors or line length exceeding the buffer size.
 */
void htmlMainBox(char *principalName, char *principalGroups) {
	FILE *file = NULL;
	char buffer[512];
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
			printf("%s",buffer);
	} while(x == 0);
	fclose(file);
}


/**
 * @brief Handles database errors by printing an error message and exiting the program.
 *
 * This function prints an HTML-formatted error message indicating that a database error
 * has occurred. It includes a mailto link to contact the administrator with the error code.
 * After printing the message, it frees the memory allocated for the principal name, prints
 * a static footer, and exits the program with a failure status.
 *
 * @param principalName A pointer to a string containing the principal name. This memory
 *                      will be freed by the function.
 * @param err An integer representing the error code to be displayed in the error message.
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

/**
 * @brief Converts and prints a given string with HTML-safe characters and tags.
 *
 * This function processes the input string `s` and converts certain characters and sequences 
 * into their HTML-safe equivalents. It handles HTML tags such as <b>, <i>, <a>, and <br>, 
 * and converts special characters like double quotes, single quotes, and ampersands into 
 * their corresponding HTML entities.
 *
 * @param s The input string to be converted and printed.
 *
 * The function performs the following conversions:
 * - Converts <b>, <i>, <a>, and <br> tags to their HTML-safe equivalents.
 * - Converts URLs starting with "http" or "https" into HTML anchor tags.
 * - Converts double quotes (") to &#34;.
 * - Converts single quotes (') to &#39;.
 * - Converts ampersands (&) to &amp;.
 * - Converts carriage return and line feed (CRLF) sequences to <BR />.
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

/**
 * @brief Converts and prints a given string with special handling for HTML tags and URLs.
 *
 * This function processes the input string `s` and prints it with special handling for:
 * - HTML tags such as `<a href>`, `<br>`, and `</a>`.
 * - URLs starting with "http" or "https".
 * - Special characters like double quotes (`"`), single quotes (`'`), and ampersands (`&`).
 * - Newline characters (`\r\n`).
 *
 * The function ensures that:
 * - `<a href>` tags are wrapped with `<![CDATA[ ... ]]>`.
 * - `<br>` tags are converted to `<br />`.
 * - URLs are converted to clickable links.
 * - Special characters are converted to their corresponding HTML entities.
 * - Newline characters are converted to `<BR />`.
 *
 * @param s The input string to be processed and printed.
 */
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
