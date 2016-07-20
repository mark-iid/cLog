#
# cLog makefile
#  Mark Earnest
#
# NOTE: This is REALLY basic. You are responsible for creating 
#  the relevent directories and setting the variables here.
#
# $Author: mxe20 $
# $Date: 2005/01/10 18:32:51 $
# $Header: /home/mxe20/src/clog/RCS/Makefile,v 1.4 2005/01/10 18:32:51 mxe20 Exp $
# $Id: Makefile,v 1.4 2005/01/10 18:32:51 mxe20 Exp $
# $Locker:  $
# $Log: Makefile,v $
# Revision 1.4  2005/01/10 18:32:51  mxe20
# Added co and ci targets
#
# Revision 1.3  2005/01/10 16:46:09  mxe20
# Added RCS macros
#
# $Name:  $
# $RCSfile: Makefile,v $
# $Revision: 1.4 $
# $Source: /home/mxe20/src/clog/RCS/Makefile,v $
# $State: Exp $

TEMPLATESRCDIR = templates/
INSTALLROOTDIR = /var/www/html/
MYSQLLINK = -L/usr/lib/mysql -lmysqlclient -lz 
MYSQLINC = -I/usr/include/mysql 

PROGS = \
	homepage.cgi \
	login.cgi \
	news.cgi \
	userpref.cgi \
	newspost.cgi \
	post.cgi \
	comments.cgi \
	commentpost.cgi \
	cpost.cgi \
	archive.cgi \
	editnews.cgi \
	topic.cgi \
	rss.cgi \
	user.cgi

CFILES = \
	archivecgi.c \
	commentpostcgi.c \
	editnewscgi.c \
	homepagecgi.c \
	newspostcgi.c \
	showcomments.c \
	urldec.c \
	clogdb.c \
	commentscgi.c \
	logincgi.c \
	postcgi.c \
	shownews.c \
	user.c \
	cloghtml.c \
	cpostcgi.c \
	envar.c \
	newscgi.c \
	rsscgi.c \
	topiccgi.c \
	userprefcgi.c \
	usercgi.c 

HFILES = \
	clogdb.h \
	cloghtml.h \
	envar.h \
	showcomments.h \
	shownews.h \
	sitevars.h \
	urldec.h \
	user.h	
	
TEMPLATES = \
	$(TEMPLATESRCDIR)/css/clog.css \
 	$(TEMPLATESRCDIR)/ssi/css.html \
	$(TEMPLATESRCDIR)header.html \
	$(TEMPLATESRCDIR)logo.html \
	$(TEMPLATESRCDIR)mainbox.html \
	$(TEMPLATESRCDIR)usefulbox.html \
	$(TEMPLATESRCDIR)contenttablestart.html \
	$(TEMPLATESRCDIR)contenttableend.html \
	$(TEMPLATESRCDIR)commenttablemain.html \
	$(TEMPLATESRCDIR)newstablemain.html \
	$(TEMPLATESRCDIR)footer.html
	
all: $(PROGS)

homepage.cgi: homepagecgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o
	$(CC) -o homepage.cgi homepagecgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o $(MYSQLLINK)

archive.cgi: archivecgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o
	$(CC) -o archive.cgi archivecgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o $(MYSQLLINK)

topic.cgi: topiccgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o
	$(CC) -o topic.cgi topiccgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o $(MYSQLLINK)

user.cgi: usercgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o
	$(CC) -o user.cgi usercgi.o user.o shownews.o showcomments.o clogdb.o cloghtml.o envar.o $(MYSQLLINK)

login.cgi: logincgi.o clogdb.o cloghtml.o envar.o
	$(CC) -o login.cgi logincgi.o clogdb.o cloghtml.o envar.o $(MYSQLLINK) -luuid

news.cgi: newscgi.o user.o clogdb.o cloghtml.o envar.o shownews.o showcomments.o
	$(CC) -o news.cgi newscgi.o user.o clogdb.o cloghtml.o shownews.o showcomments.o envar.o $(MYSQLLINK)
	
editnews.cgi: editnewscgi.o user.o clogdb.o cloghtml.o envar.o shownews.o 
	$(CC) -o editnews.cgi editnewscgi.o user.o clogdb.o cloghtml.o envar.o shownews.o $(MYSQLLINK)

comments.cgi: commentscgi.o user.o clogdb.o cloghtml.o envar.o showcomments.o urldec.o
	$(CC) -o comments.cgi commentscgi.o user.o clogdb.o cloghtml.o showcomments.o envar.o urldec.o $(MYSQLLINK)

userpref.cgi: userprefcgi.o user.o clogdb.o cloghtml.o envar.o
	$(CC) -o userpref.cgi userprefcgi.o user.o clogdb.o cloghtml.o envar.o $(MYSQLLINK)

newspost.cgi: newspostcgi.o user.o clogdb.o cloghtml.o envar.o
	$(CC) -o newspost.cgi newspostcgi.o user.o clogdb.o cloghtml.o envar.o $(MYSQLLINK)

commentpost.cgi: commentpostcgi.o user.o clogdb.o cloghtml.o envar.o urldec.o shownews.o showcomments.o
	$(CC) -o commentpost.cgi commentpostcgi.o user.o clogdb.o cloghtml.o envar.o urldec.o shownews.o showcomments.o $(MYSQLLINK)

post.cgi: postcgi.o user.o clogdb.o cloghtml.o envar.o urldec.o
	$(CC) -o post.cgi postcgi.o user.o clogdb.o cloghtml.o envar.o urldec.o $(MYSQLLINK)

cpost.cgi: cpostcgi.o user.o clogdb.o cloghtml.o envar.o urldec.o
	$(CC) -o cpost.cgi cpostcgi.o user.o clogdb.o cloghtml.o envar.o urldec.o $(MYSQLLINK)

rss.cgi: rsscgi.o clogdb.o cloghtml.o urldec.o
	$(CC) -o rss.cgi rsscgi.o clogdb.o urldec.o cloghtml.o $(MYSQLLINK)

postcgi.o: postcgi.c clogdb.h cloghtml.h envar.h urldec.h
	$(CC) -c postcgi.c $(MYSQLINC)

cpostcgi.o: cpostcgi.c clogdb.h cloghtml.h envar.h urldec.h
	$(CC) -c cpostcgi.c $(MYSQLINC)

newspostcgi.o: newspostcgi.c clogdb.h cloghtml.h envar.h
	$(CC) -c newspostcgi.c $(MYSQLINC)

commentpostcgi.o: commentpostcgi.c clogdb.h cloghtml.h urldec.h user.h shownews.h showcomments.h
	$(CC) -c commentpostcgi.c $(MYSQLINC)

homepagecgi.o: homepagecgi.c clogdb.c clogdb.h
	$(CC) -c homepagecgi.c $(MYSQLINC)

archivecgi.o: archivecgi.c clogdb.h cloghtml.h user.h
	$(CC) -c archivecgi.c $(MYSQLINC)

topiccgi.o: topiccgi.c clogdb.h cloghtml.h user.h
	$(CC) -c topiccgi.c $(MYSQLINC)

usercgi.o: usercgi.c clogdb.h cloghtml.h user.h
	$(CC) -c usercgi.c $(MYSQLINC)

userprefcgi.o: userprefcgi.c user.h clogdb.h cloghtml.h envar.h
	$(CC) -c userprefcgi.c $(MYSQLINC)

user.o: user.c clogdb.h
	$(CC) -c user.c $(MYSQLINC)

shownews.o: shownews.c cloghtml.h clogdb.h shownews.h
	$(CC) -c shownews.c $(MYSQLINC)

showcomments.o: showcomments.c clogdb.h showcomments.h cloghtml.h
	$(CC) -c showcomments.c $(MYSQLINC)

newscgi.o: newscgi.c cloghtml.h envar.h user.h
	$(CC) -c newscgi.c $(MYSQLINC)

commentscgi.o: commentscgi.c cloghtml.h clogdb.h user.h urldec.h
	$(CC) -c commentscgi.c $(MYSQLINC)

logincgi.o: logincgi.c
	$(CC) -c logincgi.c $(MYSQLINC)

clogdb.o: clogdb.c clogdb.h
	$(CC) -c clogdb.c $(MYSQLINC)

cloghtml.o: cloghtml.c cloghtml.h clogdb.h
	$(CC) -c cloghtml.c $(MYSQLINC)

urldec.o: urldec.c urldec.h
	$(CC) -c urldec.c

envar.o: envar.c
	$(CC) -c envar.c

editnewscgi.o: editnewscgi.c cloghtml.h envar.h user.h
	$(CC) -c editnewscgi.c $(MYSQLINC)

rsscgi.o: rsscgi.c clogdb.h
	$(CC) -c rsscgi.c $(MYSQLINC)

install: $(PROGS)
	cp login.cgi $(INSTALLROOTDIR)login/login.cgi
	cp homepage.cgi $(INSTALLROOTDIR)index.cgi
	cp news.cgi $(INSTALLROOTDIR)news.cgi
	cp userpref.cgi $(INSTALLROOTDIR)userpref.cgi
	cp newspost.cgi $(INSTALLROOTDIR)newspost.cgi
	cp post.cgi $(INSTALLROOTDIR)post.cgi
	cp comments.cgi $(INSTALLROOTDIR)comments.cgi
	cp commentpost.cgi $(INSTALLROOTDIR)commentpost.cgi
	cp cpost.cgi $(INSTALLROOTDIR)cpost.cgi
	cp archive.cgi $(INSTALLROOTDIR)archive.cgi
	cp rss.cgi $(INSTALLROOTDIR)rss.cgi
	cp topic.cgi $(INSTALLROOTDIR)topic.cgi
	cp editnews.cgi $(INSTALLROOTDIR)editnews.cgi
	cp user.cgi $(INSTALLROOTDIR)user.cgi

install-templates: $(TEMPLATES)
	mkdir -p $(TEMPLATESRCDIR)
	mkdir -p $(TEMPLATESRCDIR)images/
	mkdir -p $(TEMPLATESRCDIR)images/topics/
	cp $(TEMPLATESRCDIR)*.html $(INSTALLROOTDIR)templates
	cp $(TEMPLATESRCDIR)css/clog.css $(INSTALLROOTDIR)templates/css/
	cp $(TEMPLATESRCDIR)ssi/css.html $(INSTALLROOTDIR)templates/ssi/
	cp $(TEMPLATESRCDIR)images/*.gif $(INSTALLROOTDIR)templates/images/
	cp $(TEMPLATESRCDIR)images/topics/*.gif $(INSTALLROOTDIR)templates/images/topics

clean:
	$(RM) $(PROGS) *.o *~ *.out

co:
	co -l $(CFILES)
	co -l $(HFILES)

ci:	$(CFILES) $(HFILES)
	ci $(CFILES)
	ci $(HFILES)

tarball: $(CFILES) $(HFILES)
	make clean
	cd ..
	tar -czvf clog.tar.gz --exclude=RCS clog
