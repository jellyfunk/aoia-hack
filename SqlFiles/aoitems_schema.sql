CREATE TABLE tblAO (
	aoid INTEGER NOT NULL PRIMARY KEY UNIQUE,
	ql NUMERIC, 
	type TEXT, 
	fromaoid NUMERIC, 
	name TEXT
) ;


CREATE TABLE tPatterns (
	aoid INTEGER NOT NULL PRIMARY KEY UNIQUE,
	pbid INTEGER NOT NULL,
	pattern CHAR NOT NULL
) ;

CREATE VIEW vAPatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "ab%an pattern%" ;

CREATE VIEW vBPatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "b%ar pattern%" ;

CREATE VIEW vCPatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "chi pattern%" ;

CREATE VIEW vDPatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "dom pattern%" ;

CREATE VIEW vABPatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "a%-b%ar assembly%" ;

CREATE VIEW vABCPatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "a%b%c% assembly%" ;

CREATE VIEW vCompletePatterns AS 
	SELECT aoid,ql,name FROM tblAO WHERE name LIKE "%complete%pattern of '%" ;

CREATE VIEW vPBNames AS 
	SELECT aoid, ql, substr(name, 33, length(name)-33) AS name FROM tblAO WHERE name LIKE "%Novictalized Notum Crystal with%" ;
