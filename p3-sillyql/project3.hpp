
// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <getopt.h>
#include "Field.h"
#include <algorithm>  
#include <limits>

using namespace std;

class Table {
public:
    string name;
    vector<string> columnNames;
    vector<ColumnType> columnTypes;
    vector<vector<Field>> data;  

    unordered_map<size_t, unordered_map<Field, vector<size_t>>> indices;


    Table(const string& tableName, const vector<string>& colNames, const vector<ColumnType>& colTypes)
        : name(tableName), columnNames(colNames), columnTypes(colTypes) {}

    void insertRow(const vector<Field>& row);
};



class Database {
public:
    // unordered_map<string, unique_ptr<Table>> tables;
    unordered_map<string, Table*> tables;

    bool quietMode = false;

    Database() = default;

    ~Database() {
    for (auto& pair : tables) {
        delete pair.second;  // Free the allocated Table objects
    }
    tables.clear();  // Ensure the map is empty after cleanup
    }   
    
    void executeCommand();
    void commandLine(int argc, char* argv[]);
    // void createTable();
    // void insertIntoTable(const string& tableName, const vector<vector<string>>& rows);
    // void deleteFromTable(const string& tableName, const string& columnName, const string& op, const string& value);
    // void removeTable();
    // void printTable(const string& tableName, const vector<string>& columnNames, bool hasWhereCondition, bool isAll, const string& conditionCol, const string& op, const string& value);
    // void generateIndex(Table& table, const string& indexType, const string& columnName);
    //void joinTables();
    Field readFieldByType(ColumnType type);
    //bool errorFound;
};

