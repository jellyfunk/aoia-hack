#include "stdafx.h"
#include "AODatabaseWriter.h"
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;
using namespace boost::assign;

const std::string c_scheme_sql =
    "CREATE TABLE tblAO ("
    "   [aoid] INTEGER NOT NULL PRIMARY KEY UNIQUE,"
    "   [name] TEXT NOT NULL,"
    "   [ql] INTEGER NULL,"
    "   [type] TEXT NOT NULL,"
    "   [description] VARCHAR(256) NULL,"
    "   [flags] INTEGER NULL,"
    "   [properties] INTEGER NULL,"
    "   [icon] INTEGER NULL,"
	"   [islot] INTEGER NULL"
    "   );"

    "CREATE TABLE tblItemReqs ("
    "   [aoid] INTEGER NOT NULL,"
    "   [sequence] INTEGER,"
    "   [type] INTEGER,"
    "   [attribute] INTEGER,"
    "   [value] INTEGER,"
    "   [operator] INTEGER,"
    "   [op_modifier] INTEGER"
    "   );"

    "CREATE TABLE tblItemEffects ("
    "   [aoid] INTEGER NOT NULL,"
    "   [hook] INTEGER,"
    "   [type] INTEGER,"
    "   [hits] INTEGER,"
    "   [delay] INTEGER,"
    "   [target] INTEGER,"
    "   [value1] INTEGER,"
    "   [value2] INTEGER,"
    "   [text] TEXT"
    "   );"

    "CREATE TABLE [tblPocketBoss] ("
    "   [pbid] INTEGER  NOT NULL PRIMARY KEY,"
    "   [ql] INTEGER  NOT NULL,"
    "   [name] TEXT  UNIQUE NOT NULL"
    "   );"

    "CREATE TABLE tblPatterns ( "
    "    [aoid] INTEGER NOT NULL PRIMARY KEY UNIQUE, "
    "    [pattern] TEXT NOT NULL,"
    "    [name] TEXT NOT NULL"
    "    );"

    "CREATE TABLE tblIdentify ( "
    "   [aoid] INTEGER NOT NULL PRIMARY KEY UNIQUE, "
    "   [lowid] INTEGER NOT NULL, "
    "   [highid] INTEGER NOT NULL, "
    "   [purpose] TEXT"
    "   );"

	"CREATE INDEX tblItemEffects_aoid_idx ON tblItemEffects (aoid);"

	"CREATE INDEX tblItemReqs_aoid_idx ON tblItemReqs (aoid);"

	"CREATE INDEX tblAO_aoid_idx ON tblAO (aoid);"

	"CREATE INDEX tblAO_islot_idx ON tblAO (islot);"

	"CREATE INDEX tblAO_type_idx ON tblAO (type);"

	"CREATE INDEX tblAO_name_idx ON tblAO (name);"
    ;


const std::vector<std::string> c_datatransformation_sql = list_of
    // tblPatterns
    ("INSERT OR REPLACE INTO tblPocketBoss (pbid, ql, name) SELECT aoid, ql, substr(name, 34, length(name)-34) AS name FROM tblAO WHERE name LIKE '%Novictalized Notum Crystal with%' AND LENGTH(name) > 35 ORDER BY aoid")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Aban', ''), 'Abhan', ''), '''', '')) AS name, 'A' AS Pattern FROM tblAO WHERE name LIKE 'ab%an pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Bhotaar', ''), '''', '')) AS name, 'B' AS Pattern FROM tblAO WHERE name LIKE 'b%ar pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Chi', ''), '''', '')) AS name, 'C' AS Pattern FROM tblAO WHERE name LIKE 'chi pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Pattern', ''), 'of', ''), 'Dom', ''), '''', '')) AS name, 'D' AS Pattern FROM tblAO WHERE name LIKE 'dom pattern%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(REPLACE(name, 'Assembly', ''), 'of', ''), 'Aban-Bhotar', ''), '''', '')) AS name, 'AB' AS Pattern FROM tblAO WHERE name LIKE 'a%-b%ar assembly%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(name, 'Aban-Bhotar-Chi Assembly', ''), '''', '')) AS name, 'ABC' AS Pattern FROM tblAO WHERE name LIKE 'a%b%c% assembly%'")
    ("INSERT OR REPLACE INTO tblPatterns (aoid, name, pattern) SELECT aoid, TRIM(REPLACE(REPLACE(REPLACE(name, 'Complete Blueptrint Pattern of', ''), 'Complete Blueprint Pattern of', ''), '''', '')) AS name, 'ABCD' AS Pattern FROM tblAO WHERE name LIKE '%complete%pattern of ''%'")
    // tblIdentify - AI Biomaterial
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247103, 247106, 247107, '(Alien Armor) Low tradeskill requirements')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247105, 247108, 247109, '(Alien Armor) High tradeskill requirements')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247698, 247673, 247674, '(Weapon Upgrade) Brawl, Fast Attack')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247700, 247675, 247676, '(Weapon Upgrade) Brawl, Dimach, Fast Attack')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247702, 247678, 247677, '(Weapon Upgrade) Brawl, Dimach, Fast Attack, Sneak Attack')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247704, 247680, 247679, '(Weapon Upgrade) Dimach, Fast Attack, Parry, Riposte')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247706, 247681, 247682, '(Weapon Upgrade) Dimach, Fast Attack, Sneak Attack, Parry, Riposte')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247708, 247683, 247684, '(Weapon Upgrade) Fling shot')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247710, 247685, 247686, '(Weapon Upgrade) Aimed Shot')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247712, 247687, 247688, '(Weapon Upgrade) Burst')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247714, 247689, 247690, '(Weapon Upgrade) Fling Shot, Burst')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247716, 247691, 247692, '(Weapon Upgrade) Burst, Full Auto')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247718, 247693, 247694, '(Weapon Upgrade) Fling Shot, Aimed Shot')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (247720, 247695, 247696, '(Weapon Upgrade) Burst, Fling Shot, Full Auto')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (254804, 247765, 254805, '(Org City) High QL Buildings')")
    // tblIdentify - Alappa
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269800, 168432, 168432, '(Bracer Gem) HP/ NR/ Nano')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269811, 168473, 168473, '(Bracer Gem) HP/ add Melee Dmg/ %-Reflect/Reflect-Dmg')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269812, 168553, 168553, '(Bracer Gem) HP/ add Fire Dmg/ %-Reflect/Reflect-Dmg')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269813, 168620, 168620, '(Bracer Gem) HP/ add Cold Dmg/ %-Reflect/Reflect-Dmg')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269814, 168717, 168717, '(Bracer Gem) HP/ add Projectile Dmg/ %-Reflect/Reflect-Dmg')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269815, 168843, 168843, '(Bracer Gem) HP/ add Poison Dmg/ %-Reflect/Reflect-Dmg')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269816, 229984, 229984, '')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269817, 230176, 230176, '')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269818, 230216, 230216, '')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (269819, 230256, 230256, '')")
    ("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (270000, 269999, 269999, '(Armor) Agent, Bureaucrat, Nano-Technician, or Meta-Physicist Robe')")

    //("INSERT INTO tblIdentify (aoid, lowid, highid, purpose) VALUES (, , , '')")
    ;


AODatabaseWriter::AODatabaseWriter(std::string const& filename, std::ostream &log)
:   m_log(log)
,   m_db(log)
{
    m_db.Init(from_ascii_copy(filename));
    m_db.Exec("PRAGMA journal_mode=MEMORY");
    m_db.Exec(from_ascii_copy(c_scheme_sql));
    m_db.Exec(STREAM2STR(_T("CREATE VIEW vSchemeVersion AS SELECT '") << CURRENT_AODB_VERSION << _T("' AS Version")));
}


AODatabaseWriter::~AODatabaseWriter()
{
    m_db.Term();
}


void AODatabaseWriter::BeginWrite()
{
    m_db.Begin();
}


void AODatabaseWriter::WriteItem(boost::shared_ptr<ao_item> item)
{
    static std::map<unsigned int, std::string> s_ItemTypeMap = boost::assign::map_list_of
        (AODB_ITEM_MISC,    "Misc")
        (AODB_ITEM_WEAPON,  "Weapon")
        (AODB_ITEM_ARMOR,   "Armor")
        (AODB_ITEM_IMPLANT, "Implant")
        (AODB_ITEM_TEMPLATE,"Template")
        (AODB_ITEM_SPIRIT,  "Spirit");

    if (item->name.empty()) {
        return; // Strange item
    }

    if (s_ItemTypeMap.find(item->type) == s_ItemTypeMap.end()) {
        assert(false);  // Found unknown object type!
        return;
    }

    // Special case for getting item 212334 into the DB. Looks fubar'ed by funcom in 17.1 patch.
    if (item->aoid != 212334 && item->ql == 0) {
        m_log << "Skipped item with QL zero. AOID: " << item->aoid << std::endl;
        return;
    }

    // We need to escape the ' symbol inside the strings to have the SQL be valid.
    std::string name = boost::algorithm::replace_all_copy(item->name, "'", "''");
    std::string desc = boost::algorithm::replace_all_copy(item->description, "'", "''");

    std::ostringstream sql;
    sql << "INSERT INTO tblAO (aoid, name, ql, type, islot, description, flags, properties, icon) VALUES ("
        << item->aoid << ", "
        << "'" << name << "', "
        << item->ql << ", "
        << "'" << s_ItemTypeMap[item->type] << "', "
		<< item->slot << ", "
        << "'" << desc << "', "
        << item->flags << ", "
        << item->props << ", "
        << item->icon << ")";

    m_db.Exec(from_ascii_copy(sql.str()));

    // Loop through all the requirements for an item/nano and write them to the DB.
    for (std::list<ao_item_req>::iterator it = item->reqs.begin(); it != item->reqs.end(); ++it)
    {
        writeRequirement(item->aoid, *it);
    }

    // Loop through all the effects for an item/nano and write them to the DB.
    for (std::list<ao_item_effect>::iterator it = item->effs.begin(); it != item->effs.end(); ++it)
    {
        writeEffect(item->aoid, *it);
    }
}


void AODatabaseWriter::writeRequirement(unsigned int aoid, ao_item_req const& req)
{
    std::ostringstream sql;
    sql << "INSERT INTO tblItemReqs (aoid, sequence, type, attribute, value, operator, op_modifier) VALUES ("
        << aoid << ", "
        << req.id << ", "
        << req.type << ", "
        << req.attribute << ", "
        << req.count << ", "
        << req.op << ", "
        << req.opmod << ")";
    m_db.Exec(from_ascii_copy(sql.str()));
}


void AODatabaseWriter::writeEffect(unsigned int aoid, ao_item_effect const& eff)
{
    // HACK 1 [SUPERSEDED]: Only dump the first value from the vector for now.
	// HACK 1A: Dump first or first two values from the vector
    unsigned int value1 = 0;
    unsigned int value2 = 0;
    if (!eff.values.empty())
    {
		if (eff.values.size() == 1) {
            value1 = eff.values.at(0);
	    } else {
            value1 = eff.values.at(0);
            value2 = eff.values.at(1);
	    }
    }

    // Escape ' symbols in the text.
    std::string text = eff.text;
    boost::algorithm::replace_all(text, "'", "''");

    // HACK 2: We skip altogether the requirements associated with an effect for now.
    std::ostringstream sql;
    sql << "INSERT INTO tblItemEffects (aoid, hook, type, hits, delay, target, value1, value2, text) VALUES ("
        << aoid << ", "
        << eff.hook << ", "
        << eff.type << ", "
        << eff.hits << ", "
        << eff.delay << ", "
        << eff.target << ", "
        << value1 << ", "
        << value2 << ", "
        << "'" << text << "')";
    m_db.Exec(from_ascii_copy(sql.str()));
}


void AODatabaseWriter::CommitItems()
{
    m_db.Commit();
}


void AODatabaseWriter::AbortWrite()
{
    m_db.Rollback();
}


void AODatabaseWriter::PostProcessData()
{
    for (std::vector<std::string>::const_iterator it = c_datatransformation_sql.begin(); it != c_datatransformation_sql.end(); ++it) {
        if (!m_db.Exec(*it)) {
            assert(false);
            m_log << "Error while postprocessing data." << std::endl;
        }
    }
}
