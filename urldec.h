/*
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:38:51 $
 * $Header: /home/mxe20/src/clog/RCS/urldec.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Id: urldec.h,v 1.1 2005/05/11 15:38:51 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: urldec.h,v $
 * Revision 1.1  2005/05/11 15:38:51  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:31:04  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: urldec.h,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/urldec.h,v $
 * $State: Exp $
 */
#ifndef H_URLDEC
#define H_URLDEC

struct  field_rec {
  struct field_rec *next;
  char *field_name;
  char *field_data;
};

extern struct field_rec *urldec;

void urlDecode(char *s);
#endif
