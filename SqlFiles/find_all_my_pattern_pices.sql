ATTACH DATABASE "aoitems.db" AS aodb;
SELECT itemidx, pbid, pattern, ql, parent, slot, owner
FROM tPatterns JOIN tItems ON keylow = aoid
ORDER BY itemidx

