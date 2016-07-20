/*
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:27:02 $
 * $Header: /home/mxe20/src/clog/RCS/urldec.c,v 1.1 2005/05/11 15:27:02 mxe20 Exp mxe20 $
 * $Id: urldec.c,v 1.1 2005/05/11 15:27:02 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: urldec.c,v $
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: urldec.c,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/urldec.c,v $
 * $State: Exp $
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "urldec.h"

void urlDecode(char *s) {
	char end_str[]="";
	char *d,*i;
	int x,y;
	struct field_rec *p;
	d=s;
	while (*d) {
		s=strchr(d,'&');
		if (s) *s++='\0'; else s=end_str;
		i=strchr(d,'=');
		if(i) {
			*i++='\0';
			p=(struct field_rec *)malloc(sizeof(struct field_rec));
			if(p) {
				p->next=urldec;
				urldec=p;
				p->field_name=(char *)malloc(strlen(d)+1);
				if (p->field_name) {
					strcpy(p->field_name,d);
					p->field_data=(char *)malloc(strlen(i)+1);
					if(p->field_data) {
						d=p->field_data;
						while (*i) {
							switch (*i) {
								case '+':
									i++;
									*d++=' ';
									break;
								case '%':
									i++;
									x=((int)*i++)-48;
									if(x>9) x-=7;
									x<<=4;
									y=((int)*i++)-48;
									if(y>9)y-=7;
									*d++=(char)(x|y);
									break;
								default:
									*d++=*i++;
									break;
							}
						}
						*d='\0';
					} else s=end_str;
				} else s=end_str;
			} else s=end_str;
		}
		d=s;
	}
}	
