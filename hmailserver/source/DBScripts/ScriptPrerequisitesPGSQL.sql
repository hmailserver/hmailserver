CREATE or REPLACE function hm_drop_column_objects (table_name varchar, col_name varchar) 
returns void as $$
DECLARE
	ind record;
BEGIN
   FOR ind IN SELECT * from pg_indexes WHERE tablename = table_name 
	and (indexdef like '%(' || col_name || '%' or
	     indexdef like '%' ||  col_name || ',%' or
	     indexdef like '%' ||  col_name || ')')
	LOOP
	
	EXECUTE 'DROP INDEX ' || ind.indexname;
   END LOOP;
   
END;
$$ language plpgsql ;

