#include <map>
#include <tchar.h>
#include <shared/UnicodeSupport.h>
#include <Parsers/AOFullCharacterSync.h>
#include <gtest/gtest.h>
#include <iostream>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

struct AOFullCharacterMessageTestFixture : public testing::Test
{
    AOFullCharacterMessageTestFixture() { }

    std::vector<char> LoadBinaryData(const char* filename)
    {
        bfs::path path(filename);
        if (!bfs::exists(path)) {
            throw new std::exception("file not found!");
        }

        std::vector<char> result;

        std::ifstream ifs;
        ifs.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

        if (ifs.is_open())
        {
            int size = (int)ifs.tellg();
            ifs.seekg(0);

            char* buffer = new char[size];
            ifs.read(buffer, size);
            result.insert(result.end(), buffer, buffer + size);
            delete buffer;
        }

        return result;
    }
};

TEST_F(AOFullCharacterMessageTestFixture, XplorerFullyBuffedTest)
{
    std::vector<char> data = LoadBinaryData("fullsync_xplorer_entering_shop_buffed.bin");
    EXPECT_GT(data.size(), (unsigned)0);

    Parsers::AOFullCharacterMessage msg(&*data.begin(), data.size());
    EXPECT_EQ(msg.characterId(), 0x3256A372);
    EXPECT_EQ(msg.entityId(), 0x3256A372);
}

TEST_F(AOFullCharacterMessageTestFixture, XplorerZoningWithPerkLockTimerTest)
{
    std::vector<char> data = LoadBinaryData("fullsync_xplorer_perk_timers.bin");
    EXPECT_GT(data.size(), (unsigned)0);

    Parsers::AOFullCharacterMessage msg(&*data.begin(), data.size());
    EXPECT_EQ(msg.characterId(), 0x3256A372);
    EXPECT_EQ(msg.entityId(), 0x3256A372);
}
