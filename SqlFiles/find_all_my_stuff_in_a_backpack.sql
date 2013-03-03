ATTACH DATABASE "aoitems.db" AS ao;
SELECT substr(name,0,length(name)-1) AS Name, tItems.ql AS QL, stack AS Stack
FROM tItems JOIN tblAO ON keylow = aoid
WHERE parent = 1
