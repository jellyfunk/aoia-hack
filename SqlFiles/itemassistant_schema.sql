CREATE TABLE tItems (itemidx INTEGER NOT NULL PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT UNIQUE DEFAULT '1', keylow INTEGER, keyhigh INTEGER, ql INTEGER, stack INTEGER DEFAULT '1', parent INTEGER NOT NULL DEFAULT '2', slot INTEGER, children INTEGER, owner INTEGER NOT NULL);
CREATE VIEW vBankItems AS SELECT * FROM tItems WHERE parent=1;
CREATE VIEW vContainers AS SELECT * FROM tItems WHERE children > 0;
CREATE VIEW vInvItems AS SELECT * FROM tItems WHERE parent=2;
CREATE INDEX iOwner ON tItems (owner);
CREATE INDEX iParent ON tItems (parent);
CREATE TABLE tToons (charid, charname);
CREATE UNIQUE INDEX iCharId ON tToons (charid);
CREATE VIEW vSchemeVersion AS SELECT '1' AS Version;
