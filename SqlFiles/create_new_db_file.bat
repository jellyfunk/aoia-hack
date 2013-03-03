@echo off
echo Creating aoitems.db tables and views.
..\Externals\SQLite\sqlite3.exe -init aoitems_schema.sql aoitems.db .quit

echo Importing item list.
..\Externals\SQLite\sqlite3.exe -init aoitems_import.sql aoitems.db .quit

echo Building pockectboss patterns. This will take a while!
..\Externals\SQLite\sqlite3.exe -init build_tPatterns.sql aoitems.db .quit
