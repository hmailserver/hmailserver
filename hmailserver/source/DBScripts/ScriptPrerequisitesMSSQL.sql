if exists (select * from sysobjects where id = object_id('hm_drop_column_objects')) drop proc hm_drop_column_objects

CREATE proc hm_drop_column_objects
	@tablename sysname,
	@columnname sysname
as
	set nocount on
	declare @constname sysname,
	@cmd varchar(1024)
	declare curs_constraints cursor for
	-- delete constraints on column level
	SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE
		WHERE TABLE_NAME = @tablename AND COLUMN_NAME = @columnname
	open curs_constraints
	fetch next from curs_constraints into @constname
	while (@@fetch_status = 0)
	begin
		select @cmd = 'ALTER TABLE ' + @tablename + ' DROP CONSTRAINT ' + @constname
		exec(@cmd)
		fetch next from curs_constraints into @constname
	end
	close curs_constraints
	deallocate curs_constraints
	-- delete constraints on table level
	declare curs_constraints cursor for
	select name from sysobjects where id in 
	(select constid from sysconstraints where id = OBJECT_ID(@tablename) and colid in
	(select colid from syscolumns where id = OBJECT_ID(@tablename) and name = @columnname))
	open curs_constraints
	fetch next from curs_constraints into @constname
	while (@@fetch_status = 0)
	begin
		select @cmd = 'ALTER TABLE ' + @tablename + ' DROP CONSTRAINT ' + @constname
		exec(@cmd)
		fetch next from curs_constraints into @constname
	end
	close curs_constraints
	deallocate curs_constraints
	declare curs_constraints cursor for
	-- delete indexes for column
	select name from sysindexes where id = OBJECT_ID(@tablename) and INDEXPROPERTY(id, name, 'IsStatistics') = 0 and
	indid in (select indid from sysindexkeys where id = OBJECT_ID(@tablename)
	and colid in (select colid from syscolumns where id = OBJECT_ID(@tablename) and name = @columnname))
	open curs_constraints
	fetch next from curs_constraints into @constname
	while (@@fetch_status = 0)
	begin
		select @cmd = 'DROP INDEX ' + @tablename + '.' + @constname
		exec(@cmd)
		fetch next from curs_constraints into @constname
	end
	close curs_constraints
	deallocate curs_constraints
return 0

