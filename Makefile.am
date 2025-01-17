bin_PROGRAMS = homepage.cgi archive.cgi topic.cgi user.cgi login.cgi news.cgi \
                editnews.cgi comments.cgi userpref.cgi newspost.cgi commentpost.cgi \
                post.cgi cpost.cgi rss.cgi

homepage_cgi_SOURCES = homepagecgi.c user.c shownews.c showcomments.c clogdb.c cloghtml.c envar.c
archive_cgi_SOURCES = archivecgi.c user.c shownews.c showcomments.c clogdb.c cloghtml.c envar.c
topic_cgi_SOURCES = topiccgi.c user.c shownews.c showcomments.c clogdb.c cloghtml.c envar.c
user_cgi_SOURCES = usercgi.c user.c shownews.c showcomments.c clogdb.c cloghtml.c envar.c
login_cgi_SOURCES = logincgi.c clogdb.c cloghtml.c envar.c
news_cgi_SOURCES = newscgi.c user.c clogdb.c cloghtml.c shownews.c showcomments.c envar.c
editnews_cgi_SOURCES = editnewscgi.c user.c clogdb.c cloghtml.c envar.c shownews.c
comments_cgi_SOURCES = commentscgi.c user.c clogdb.c cloghtml.c envar.c showcomments.c urldec.c
userpref_cgi_SOURCES = userprefcgi.c user.c clogdb.c cloghtml.c envar.c
newspost_cgi_SOURCES = newspostcgi.c user.c clogdb.c cloghtml.c envar.c
commentpost_cgi_SOURCES = commentpostcgi.c user.c clogdb.c cloghtml.c envar.c urldec.c shownews.c showcomments.c
post_cgi_SOURCES = postcgi.c user.c clogdb.c cloghtml.c envar.c urldec.c
cpost_cgi_SOURCES = cpostcgi.c user.c clogdb.c cloghtml.c envar.c urldec.c
rss_cgi_SOURCES = rsscgi.c clogdb.c cloghtml.c urldec.c

AM_CPPFLAGS = -I/usr/include/mysql
AM_LDFLAGS = -L/usr/lib/mysql
LDADD = -lmysqlclient -lz -luuid

bindir = /var/www/html

install-exec-hook:
	cp $(bin_PROGRAMS) $(DESTDIR)$(bindir)

install-data-hook:
	mkdir -p $(DESTDIR)$(datadir)/templates/css
	mkdir -p $(DESTDIR)$(datadir)/templates/ssi
	mkdir -p $(DESTDIR)$(datadir)/templates/images
	mkdir -p $(DESTDIR)$(datadir)/templates/images/topics
	cp templates/*.html $(DESTDIR)$(datadir)/templates
	cp templates/css/clog.css $(DESTDIR)$(datadir)/templates/css/
	cp templates/ssi/css.html $(DESTDIR)$(datadir)/templates/ssi/
	cp templates/images/*.gif $(DESTDIR)$(datadir)/templates/images/
	cp templates/images/topics/*.gif $(DESTDIR)$(datadir)/templates/images/topics

clean-local:
	rm -rf build *.o *~ *.out
