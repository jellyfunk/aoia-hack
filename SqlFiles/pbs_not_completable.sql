
SELECT *
FROM tPatterns
WHERE pattern LIKE "ABCD" AND pbid NOT IN (
SELECT pbid FROM tPatterns WHERE pattern LIKE "A"
)

