# Remember some variables.
LET $MYSQLD_DATADIR = `select @@datadir`;
LET $INNODB_PAGE_SIZE = `select @@innodb_page_size`;

--echo #
--echo # Try a series of invalid relative ADD DATAFILE entries.
--echo # General tablespace files can be in the datadir but they cannot be under the datadir.
--echo #
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE './ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE './.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '../spaces/';
--replace_result $MYSQL_TMP_DIR MYSQL_TMP_DIR
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE 'test/bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE './test/bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE './test//bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '../data/test/bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '../data/../data/test/bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '../data/../data/bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '../does_not_exist/bad.ibd';
--replace_result $MYSQL_TMP_DIR MYSQL_TMP_DIR
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE 'test/../bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE './test/../bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE 'test\\..\\bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '..///data//test///..//bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '.\\test\\..\\bad.ibd';
SHOW WARNINGS;
--error ER_WRONG_FILE_NAME
CREATE TABLESPACE bad ADD DATAFILE '..\\\\\\data\\\\test\\\\\\..\\\\bad.ibd';
SHOW WARNINGS;

--echo #
--echo # Try a series of valid relative ADD DATAFILE entries.
--echo # General tablespace files can be in the datadir but they cannot be under the datadir.
--echo #
CREATE TABLESPACE ibport_s01 ADD DATAFILE 'ibport_s01.ibd';
CREATE TABLESPACE ibport_s02 ADD DATAFILE './ibport_s02.ibd';
CREATE TABLESPACE ibport_s03 ADD DATAFILE '../data/ibport_s03.ibd';
CREATE TABLESPACE ibport_s04 ADD DATAFILE '.\\ibport_s04.ibd';
CREATE TABLESPACE ibport_s05 ADD DATAFILE '..\\data\\ibport_s05.ibd';

--echo # Directory listing of MYSQLD_DATADIR/ ibport*
--list_files $MYSQLD_DATADIR/ ibport*

--replace_result  $MYSQLD_DATADIR/ MYSQLD_DATADIR/  $MYSQLD_DATADIR MYSQLD_DATADIR/
SELECT s.space_type 'Type', s.name 'Space Name', d.path 'Path'
	FROM information_schema.innodb_tablespaces s, information_schema.innodb_datafiles d
	WHERE s.name like '%ibport%' AND s.space = d.space ORDER BY s.space;

--replace_result  $MYSQLD_DATADIR/ MYSQLD_DATADIR/  $MYSQLD_DATADIR MYSQLD_DATADIR/
SELECT s.space_type 'Type', s.name 'Space Name', f.file_name 'Path'
	FROM information_schema.innodb_tablespaces s, information_schema.files f
	WHERE s.name like '%ibport%' AND s.space = f.file_id ORDER BY s.space;

DROP TABLESPACE ibport_s01;
DROP TABLESPACE ibport_s02;
DROP TABLESPACE ibport_s03;
DROP TABLESPACE ibport_s04;
DROP TABLESPACE ibport_s05;

--disable_query_log
call mtr.add_suppression("\\[ERROR\\] \\[[^]]*\\] InnoDB: Operating system error number [0-9]* in a file operation");
--enable_query_log
