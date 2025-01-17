-- MySQL dump 9.11
--
-- Host: localhost    Database: slashet
-- ------------------------------------------------------
-- Server version	4.0.20-standard

--
-- Table structure for table `comments`
--

CREATE TABLE comments (
  cid mediumint(8) unsigned NOT NULL auto_increment,
  nid mediumint(8) NOT NULL default '0',
  pid mediumint(8) default '0',
  authorid mediumint(8) NOT NULL default '0',
  lid int(1) NOT NULL default '1',
  comment text NOT NULL,
  title varchar(100) NOT NULL default '',
  timestamp timestamp NOT NULL,
  status varchar(100) NOT NULL default 'normal',
  PRIMARY KEY  (cid)
) ENGINE=MyISAM;

--
-- Table structure for table `newsitems`
--

CREATE TABLE newsitems (
  nid mediumint(8) unsigned NOT NULL auto_increment,
  title varchar(100) NOT NULL default '',
  intro text NOT NULL,
  author varchar(10) NOT NULL default '',
  timestamp timestamp NOT NULL,
  status varchar(100) NOT NULL default 'normal',
  topic mediumint(8) unsigned NOT NULL default '1',
  PRIMARY KEY  (nid)
) ENGINE=MyISAM;

--
-- Table structure for table `topics`
--

CREATE TABLE topics (
  tid mediumint(8) unsigned NOT NULL auto_increment,
  topic varchar(100) NOT NULL default '',
  description varchar(100) NOT NULL default '',
  PRIMARY KEY  (tid)
) ENGINE=MyISAM;

--
-- Table structure for table `users`
--

CREATE TABLE users (
  uid int(10) unsigned NOT NULL auto_increment,
  username varchar(10) NOT NULL default '',
  realname varchar(100) default '',
  cookie varchar(32) default '',
  groups varchar(255) default 'registered',
  timestamp timestamp NOT NULL,
  lockout tinyint(1) default '0',
  PRIMARY KEY  (uid)
) ENGINE=MyISAM COMMENT='Registered Users';

--
-- Table structure for table `vars`
--

CREATE TABLE vars (
  var varchar(32) NOT NULL default '0',
  value varchar(255) NOT NULL default '',
  PRIMARY KEY  (var)
) ENGINE=MyISAM;

INSERT INTO slashet_session (var, value) VALUES
('admin_email', 'email@domain.com'),
('admin_name', 'Your name'),
('site_name', 'Your Site Title'),
('site_root', '/'),
('site_templates', '/templates/'),
('site_location', '/var/www/html'),
('site_url', 'your.url.example.com'),
('site_login_url', 'your.url.example.com/login'),
('site_description', 'A pointless weblog'),
('rss_show_description', 'A pointless weblog'),
('site_create_user', 'true');