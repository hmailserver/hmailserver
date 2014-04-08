alter table mysql.db change Db Db char(64) binary DEFAULT '' NOT NULL;

alter table mysql.host change Db Db char(64) binary DEFAULT '' NOT NULL;

alter table mysql.user change max_questions max_questions int(11) unsigned DEFAULT 0  NOT NULL;

alter table mysql.tables_priv change Db Db char(64) binary DEFAULT '' NOT NULL, change Host Host char(60) binary DEFAULT '' NOT NULL, change User User char(16) binary DEFAULT '' NOT NULL, change Table_name Table_name char(64) binary DEFAULT '' NOT NULL;

alter table mysql.columns_priv change Db Db char(64) binary DEFAULT '' NOT NULL, change Host Host char(60) binary DEFAULT '' NOT NULL, change User User char(16) binary DEFAULT '' NOT NULL, change Table_name Table_name char(64) binary DEFAULT '' NOT NULL, change Column_name Column_name char(64) binary DEFAULT '' NOT NULL;

ALTER TABLE mysql.user
  MODIFY Host char(60) NOT NULL default '',
  MODIFY User char(16) NOT NULL default '',
  MODIFY Password char(41) NOT NULL default '',
  ENGINE=MyISAM, CONVERT TO CHARACTER SET utf8 COLLATE utf8_bin;
  
ALTER TABLE mysql.user
  MODIFY Select_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Insert_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Update_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Delete_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Create_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Drop_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Reload_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Shutdown_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Process_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY File_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Grant_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY References_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Index_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Alter_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Show_db_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Super_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Create_tmp_table_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Lock_tables_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Execute_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Repl_slave_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Repl_client_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY ssl_type enum('','ANY','X509', 'SPECIFIED') COLLATE utf8_general_ci DEFAULT '' NOT NULL;

ALTER TABLE mysql.db
  MODIFY Host char(60) NOT NULL default '',
  MODIFY Db char(64) NOT NULL default '',
  MODIFY User char(16) NOT NULL default '',
  ENGINE=MyISAM, CONVERT TO CHARACTER SET utf8 COLLATE utf8_bin;
  
ALTER TABLE mysql.db
  MODIFY  Select_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Insert_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Update_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Delete_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Create_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Drop_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Grant_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  References_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Index_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Alter_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Create_tmp_table_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY  Lock_tables_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL;
  
ALTER TABLE mysql.host
  MODIFY Host char(60) NOT NULL default '',
  MODIFY Db char(64) NOT NULL default '',
  ENGINE=MyISAM, CONVERT TO CHARACTER SET utf8 COLLATE utf8_bin;
  
ALTER TABLE mysql.host
  MODIFY Select_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Insert_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Update_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Delete_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Create_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Drop_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Grant_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY References_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Index_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Alter_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Create_tmp_table_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
  MODIFY Lock_tables_priv enum('N','Y') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL;
  
ALTER TABLE mysql.func
  ENGINE=MyISAM, CONVERT TO CHARACTER SET utf8 COLLATE utf8_bin;
  
ALTER TABLE mysql.func
  MODIFY type enum ('function','aggregate') COLLATE utf8_general_ci NOT NULL;
  
ALTER TABLE mysql.columns_priv
  MODIFY Host char(60) NOT NULL default '',
  MODIFY Db char(64) NOT NULL default '',
  MODIFY User char(16) NOT NULL default '',
  MODIFY Table_name char(64) NOT NULL default '',
  MODIFY Column_name char(64) NOT NULL default '',
  ENGINE=MyISAM, CONVERT TO CHARACTER SET utf8 COLLATE utf8_bin;
  
ALTER TABLE mysql.columns_priv
  MODIFY Column_priv set('Select','Insert','Update','References') COLLATE utf8_general_ci DEFAULT '' NOT NULL;
  
ALTER TABLE mysql.tables_priv
  MODIFY Host char(60) NOT NULL default '',
  MODIFY Db char(64) NOT NULL default '',
  MODIFY User char(16) NOT NULL default '',
  MODIFY Table_name char(64) NOT NULL default '',
  MODIFY Grantor char(77) NOT NULL default '',
  ENGINE=MyISAM, CONVERT TO CHARACTER SET utf8 COLLATE utf8_bin;
  
ALTER TABLE mysql.tables_priv
  MODIFY Table_priv set('Select','Insert','Update','Delete','Create','Drop','Grant','References','Index','Alter') COLLATE utf8_general_ci DEFAULT '' NOT NULL,
  MODIFY Column_priv set('Select','Insert','Update','References') COLLATE utf8_general_ci DEFAULT '' NOT NULL;

CREATE TABLE IF NOT EXISTS mysql.help_topic (
   help_topic_id int unsigned not null,
   name varchar(64) not null,
   help_category_id smallint unsigned not null,
   description text not null,
   example text not null,
   url varchar(128) not null,
   primary key (help_topic_id), unique index (name)
   ) CHARACTER SET utf8 comment='help topics';

CREATE TABLE IF NOT EXISTS mysql.help_category (
   help_category_id smallint unsigned not null,
   name varchar(64) not null,
   parent_category_id smallint unsigned null,
   url varchar(128) not null,
   primary key (help_category_id),
   unique index (name)
   ) CHARACTER SET utf8 comment='help categories';

CREATE TABLE IF NOT EXISTS mysql.help_relation (
   help_topic_id int unsigned not null references help_topic,
   help_keyword_id  int unsigned not null references help_keyword,
   primary key (help_keyword_id, help_topic_id)
   ) CHARACTER SET utf8 comment='keyword-topic relation';

CREATE TABLE IF NOT EXISTS mysql.help_keyword (
   help_keyword_id int unsigned not null,
   name varchar(64) not null,
   primary key (help_keyword_id),
   unique index (name)
   ) CHARACTER SET utf8 comment='help keywords';

CREATE TABLE IF NOT EXISTS mysql.time_zone_name (
   Name char(64) NOT NULL,   
   Time_zone_id int  unsigned NOT NULL,
   PRIMARY KEY Name (Name) 
   ) CHARACTER SET utf8 comment='Time zone names';

CREATE TABLE IF NOT EXISTS mysql.time_zone (
   Time_zone_id int unsigned NOT NULL auto_increment,
   Use_leap_seconds  enum('Y','N') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL,
   PRIMARY KEY TzId (Time_zone_id) 
   ) CHARACTER SET utf8 comment='Time zones';

ALTER TABLE mysql.time_zone
  MODIFY Use_leap_seconds enum('Y','N') COLLATE utf8_general_ci DEFAULT 'N' NOT NULL;

CREATE TABLE IF NOT EXISTS mysql.time_zone_transition (
   Time_zone_id int unsigned NOT NULL,
   Transition_time bigint signed NOT NULL,   
   Transition_type_id int unsigned NOT NULL,
   PRIMARY KEY TzIdTranTime (Time_zone_id, Transition_time) 
   ) CHARACTER SET utf8 comment='Time zone transitions';

CREATE TABLE IF NOT EXISTS mysql.time_zone_transition_type (
   Time_zone_id int unsigned NOT NULL,
   Transition_type_id int unsigned NOT NULL,
   Offset int signed DEFAULT 0 NOT NULL,
   Is_DST tinyint unsigned DEFAULT 0 NOT NULL,
   Abbreviation char(8) DEFAULT '' NOT NULL,
   PRIMARY KEY TzIdTrTId (Time_zone_id, Transition_type_id) 
   ) CHARACTER SET utf8 comment='Time zone transition types';

CREATE TABLE IF NOT EXISTS mysql.time_zone_leap_second (
   Transition_time bigint signed NOT NULL,
   Correction int signed NOT NULL,   
   PRIMARY KEY TranTime (Transition_time) 
   ) CHARACTER SET utf8 comment='Leap seconds information for time zones';
   


