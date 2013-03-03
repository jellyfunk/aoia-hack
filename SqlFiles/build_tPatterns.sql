INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "A" AS pattern
FROM vPBNames pb JOIN vAPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));


INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "B" AS pattern
FROM vPBNames pb JOIN vBPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));


INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "C" AS pattern
FROM vPBNames pb JOIN vCPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));


INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "D" AS pattern
FROM vPBNames pb JOIN vDPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));


INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "AB" AS pattern
FROM vPBNames pb JOIN vABPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));


INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "ABC" AS pattern
FROM vPBNames pb JOIN vABCPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));


INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "ABCD" AS pattern
FROM vPBNames pb JOIN vCompletePatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name));

