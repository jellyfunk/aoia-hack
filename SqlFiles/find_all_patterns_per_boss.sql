INSERT OR REPLACE INTO tPatterns (aoid, pbid, pattern)
SELECT ao.aoid AS aoid, pb.aoid AS pbid, "B" AS pattern
FROM vPBNames pb JOIN vBPatterns ao
ON pb.name = substr(ao.name, -length(pb.name)-1, length(pb.name))


