#include <gtest/gtest.h>
#include <mockitopp/mockitopp.hpp>
#include <csvexport/CSVDataModel.h>

using namespace aoia;
using namespace sqlite;
using namespace mockitopp::matcher;

namespace
{
    struct null_deleter
    {
        void operator()(void const*) const
        {
        }
    };
}


class CSVDataModelTests
    : public testing::Test
{
protected:
    virtual void SetUp()
    {
        db.reset(&mockDB.getInstance(), null_deleter());
        containermanager.reset(&mockContainerManager.getInstance(), null_deleter());
        table.reset(&mockTable.getInstance(), null_deleter());
    }

    mockitopp::mock_object<IDB> mockDB;
    mockitopp::mock_object<IContainerManager> mockContainerManager;
    mockitopp::mock_object<ITable> mockTable;

    IDBPtr db;
    IContainerManagerPtr containermanager;
    ITablePtr table;
};


TEST_F(CSVDataModelTests, Construction_Always_RunQueryOnce)
{
    // Arrange
    mockDB(static_cast<ITablePtr (IDB::*)(std::tstring const&) const>(&IDB::ExecTable))
        .when(any<std::tstring const&>())
        .thenReturn(table);

    // Act
    CSVDataModel subject(db, containermanager, _T(""), _T(""));

    // Assert
    EXPECT_TRUE(mockDB(static_cast<ITablePtr (IDB::*)(std::tstring const&) const>(&IDB::ExecTable)).exactly(1));
}


TEST_F(CSVDataModelTests, GetColumnCount_Always_ReturnExpectedValue)
{
    // Arrange
    mockDB(static_cast<ITablePtr (IDB::*)(std::tstring const&) const>(&IDB::ExecTable))
        .when(any<std::tstring const&>())
        .thenReturn(table);

    CSVDataModel subject(db, containermanager, _T(""), _T(""));

    // Act
    unsigned int actual = subject.getColumnCount();

    // Assert
    EXPECT_EQ(9, actual);
}


TEST_F(CSVDataModelTests, GetColumnName_Always_ReturnExpectedValue)
{
    // Arrange
    mockDB(static_cast<ITablePtr (IDB::*)(std::tstring const&) const>(&IDB::ExecTable))
        .when(any<std::tstring const&>())
        .thenReturn(table);

    // Act
    CSVDataModel subject(db, containermanager, _T(""), _T(""));

    // Assert
    EXPECT_EQ(_T("Item Name"), subject.getColumnName(0));
    EXPECT_EQ(_T("QL"), subject.getColumnName(1));
    EXPECT_EQ(_T("Character"), subject.getColumnName(2));
    EXPECT_EQ(_T("Backpack"), subject.getColumnName(3));
    EXPECT_EQ(_T("Location"), subject.getColumnName(4));
    EXPECT_EQ(_T("LowID"), subject.getColumnName(5));
    EXPECT_EQ(_T("HighID"), subject.getColumnName(6));
    EXPECT_EQ(_T("ContainerID"), subject.getColumnName(7));
    EXPECT_EQ(_T("Link"), subject.getColumnName(8));
    EXPECT_EQ(_T(""), subject.getColumnName(9));
}


TEST_F(CSVDataModelTests, GetItemCount_EmptyQueryResult_ReturnZeroRows)
{
    // Arrange
    mockDB(static_cast<ITablePtr (IDB::*)(std::tstring const&) const>(&IDB::ExecTable))
        .when(any<std::tstring const&>())
        .thenReturn(table);
    mockTable(&ITable::Rows)
        .when()
        .thenReturn(0);

    CSVDataModel subject(db, containermanager, _T(""), _T(""));

    // Act
    unsigned int actual = subject.getItemCount();

    // Assert
    EXPECT_EQ(0, actual);
}


TEST_F(CSVDataModelTests, GetItemProperty_InvalidRow_ReturnEmptyString)
{
    // Arrange
    mockDB(static_cast<ITablePtr (IDB::*)(std::tstring const&) const>(&IDB::ExecTable))
        .when(any<std::tstring const&>())
        .thenReturn(table);
    mockTable(&ITable::Data)
        .when(any<unsigned int>(), any<unsigned int>())
        .thenReturn("");

    CSVDataModel subject(db, containermanager, _T(""), _T(""));

    // Act
    std::tstring actual = subject.getItemProperty(1, 0);

    // Assert
    EXPECT_TRUE(actual.empty());
}
