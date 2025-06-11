// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "project3.hpp"
#include <sstream>
using namespace std;

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin >> boolalpha;
    cout << boolalpha;

    Database db;
    db.commandLine(argc, argv);
    db.executeCommand();
    return 0;
}

//commandLine
void Database::commandLine(int argc, char* argv[]) {
    opterr = false;
    int choice;
    int index = 0;

    struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"quiet", no_argument, nullptr, 'q'},
        {nullptr, 0, nullptr, '\0'}
    };

    while ((choice = getopt_long(argc, argv, "hq", long_options, &index)) != -1) {
        switch (choice) {
            case 'h':
                cout << "Usage: ./silly [options] < input.txt>\n"
                          << "Options:\n"
                          << "  -h, --help        Show this help message and exit\n"
                          << "  -q, --quiet       Enable quiet mode (suppress non-summary output)\n";
                exit(0);

            case 'q':
                quietMode = true;
                cout << "quietmode set to true"<<'\n';
                break;

            default:
                cerr << "Error: Invalid command-line argument. Use --help for usage details.\n";
                exit(1);
        }
    }
}

//executeCommand
void Database::executeCommand() {
    //cout << "entered execute command" << "\n";
    string command;
    bool errorFound;

    while (cout << "% " && cin >> command) {
        if (!command.empty() && command[0] == '#') {
            getline(cin, command);
            continue;   // Skip comment lines
        }


        if (command == "CREATE") {
            errorFound = false;
            string tableName;
            int numCols;
            cin >> tableName >> numCols;

            vector<string> columnNames(static_cast<size_t>(numCols));
            vector<ColumnType> columnTypes(static_cast<size_t>(numCols));
            for (size_t i = 0; i < static_cast<size_t>(numCols); ++i) {
                string type;
                cin >> type;
        
                if (type == "string") columnTypes[i] = ColumnType::String;
                else if (type == "double") columnTypes[i] = ColumnType::Double;
                else if (type == "int") columnTypes[i] = ColumnType::Int;
                else {
                    columnTypes[i] = ColumnType::Bool;
                    }
            }
                for (size_t i = 0; i < static_cast<size_t>(numCols); ++i) cin >> columnNames[i];

                if (tables.find(tableName) != tables.end()) {
                    cout << "Error during CREATE: Cannot create already existing table "<< tableName << '\n';
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    errorFound = true;
                    //continue;
                }
                if (!errorFound) {
                    tables[tableName] = new Table(tableName, columnNames, columnTypes);
                    cout << "New table " << tableName << " with column(s) ";
                    for (const auto& col : columnNames){
                        cout << col << " ";
                    }
                        cout << "created\n";
                }
            }


        else if (command == "INSERT") {
            errorFound = false;
            string into, tableName, r;
            int numRows;
            cin >> into >> tableName >> numRows >> r;

            if (tables.find(tableName) == tables.end()) {
                cout << "Error during INSERT: " << tableName << " does not name a table in the database"<<'\n';
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Skip row input
                errorFound = true;
            }

            Table* table = tables[tableName];
            size_t numCols = table->columnTypes.size();
            vector<vector<string>> rows;

            for (int i = 0; i < numRows; i++) {
                vector<string> row;
                string value;

                for (size_t j = 0; j < numCols; j++) {
                    cin>>value;
                    row.push_back(value);
                }
                rows.push_back(row);
            }

            if (tables.find(tableName) == tables.end()) {
                cout<<"Error during INSERT: " <<tableName << " does not name a table in the database" <<'\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
            }

            size_t startN = table->data.size();
            size_t endN = startN + rows.size() - 1;
            for (size_t i = 0; i < rows.size(); ++i) {
                vector<Field> row;
                for (size_t j = 0; j < rows[i].size(); ++j) {
                try {
                    if (table->columnTypes[j] == ColumnType::Int) {
                    row.emplace_back(stoi(rows[i][j]));
                    } else if (table->columnTypes[j] == ColumnType::Double) {
                    row.emplace_back(stod(rows[i][j]));
                    } else if (table->columnTypes[j] == ColumnType::Bool) {
                    if (rows[i][j] != "true" && rows[i][j] != "false") {
                        cerr << "Error: Invalid boolean value '" << rows[i][j] << "'\n";
                        return;
                    }
                    row.emplace_back(rows[i][j] == "true");
                    } else {
                        row.emplace_back(rows[i][j]);
                    }
                } catch (const exception& e) {
                    cerr << "Error: Data conversion failed for value '" << rows[i][j] << "' in column " << j << ": " << e.what() << "\n";
                    return;
                }
            }

            table->insertRow(row);
            }
            if(!errorFound){
                cout << "Added " << rows.size() << " rows to " << tableName 
                << " from position " << startN << " to " << endN << "\n";
            }    
        } 
    
        else if (command == "PRINT") {
            errorFound = false;
            string fromKeyword, tableName;
            int numCols;
            vector<string> columnNames;
            string whereKeyword, conditionCol, op, value;
            bool hasWhereCondition = false;
            bool isAll = false;

            // Read the command parts
            cin >> fromKeyword >> tableName >> numCols;

            // Read the column names
            for (int i = 0; i < numCols; ++i) {
                string colName;
                cin>>colName;
                columnNames.push_back(colName);
            }

            if (cin >> whereKeyword) {
                if (whereKeyword == "WHERE") {
                    hasWhereCondition = true;
                    cin >> conditionCol >> op >> value;
                } else {
                    isAll = true;
                } 
            } else {
                isAll = true;
            }

            Table* table = tables[tableName];

            if (!table) {
                cout << "Error during PRINT: " << tableName << " does not name a table in the database"<< '\n';
                errorFound = true;
            }
            if (!quietMode) {
                for (const string& colName : columnNames) {
                    cout << colName << " ";
                }
                cout << "\n";
            }

            int rowCount = 0;

            for (const auto& row : table->data) {
                bool match = true;
                if (hasWhereCondition) {
                    size_t colIndex = static_cast<size_t>(find(table->columnNames.begin(), table->columnNames.end(), conditionCol) - table->columnNames.begin());
                    if (colIndex == table->columnNames.size()) {
                        cout << "Error during PRINT: " << conditionCol << " does not name a column in " << tableName <<'\n';
                        errorFound = true;
                    }

                    ostringstream oss;
                    oss << row[colIndex];
                    string cellValue = oss.str();
                    if (value == "true" || value == "false") {
                        int intValue = (value == "true") ? 1 : 0;
                        int rowValue = (cellValue == "true" || cellValue == "1") ? 1 : 0;
                        if (op == "=" && rowValue != intValue) match = false;
                        if (op == "<" && rowValue >= intValue) match = false;
                        if (op == ">" && rowValue <= intValue) match = false;
                    }
                    else {
                        if (op == "=" && cellValue != value) match = false;
                        if (op == "<" && cellValue >= value) match = false;
                        if (op == ">" && cellValue <= value) match = false;
                    }
                }

                    if (match || isAll) {
        rowCount++;
        if (!quietMode) {
            for (const string& colName : columnNames) {
                size_t colIndex = static_cast<size_t>(find(table->columnNames.begin(), table->columnNames.end(), colName) - table->columnNames.begin());

                if (colIndex == table->columnNames.size()) {
                    cout << "Error during PRINT: " << colName << " does not name a column in " << tableName << '\n';
                    errorFound = true;
                }
                ostringstream oss;
                oss << row[colIndex];

                if (table->columnTypes[colIndex] == ColumnType::Bool) {
                    cout << (row[colIndex] == 1 ? "true" : "false") << " ";
                } else {
                    cout << oss.str() << " ";
                }
                }
                    cout << "\n";
                }
                 }
                }
    
            if(!errorFound){
                cout << "Printed " << rowCount << " matching rows from " << tableName << "\n";
            }
        }

       else if (command == "DELETE") {
            errorFound = false;
            string fromKeyword, tableName, whereKeyword, columnName, op, value;
    
            cin >> fromKeyword >> tableName >> whereKeyword >> columnName >> op >> value;

            if (tables.find(tableName) == tables.end()) {
                cout << "Error during DELETE: " << tableName << " does not name a table in the database" << '\n';
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Skip row input
                errorFound = true;
            }

            // Use raw pointer (tables now stores Table*).
            Table* table = tables[tableName];
    
            // Find the column index
            auto it = find(table->columnNames.begin(), table->columnNames.end(), columnName);
            if (it == table->columnNames.end()) {
                cout << "Error during DELETE: " << columnName << " does not name a column in " << tableName << "\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                errorFound = true;
            }
            size_t colIndex = static_cast<size_t>(distance(table->columnNames.begin(), it));
            ColumnType type = table->columnTypes[colIndex];
            Field filterValue = (type == ColumnType::Int)    ? Field(stoi(value)) :
                                (type == ColumnType::Double) ? Field(stod(value)) :
                                (type == ColumnType::Bool)   ? Field(value == "true") :
                                                               Field(value);

            size_t before = table->data.size();
            size_t after = before;

                    if (op == "=" && table->indices.count(colIndex) &&
                        table->indices[colIndex].count(filterValue)) {
                        auto& rowIndices = table->indices[colIndex][filterValue];
                        sort(rowIndices.rbegin(), rowIndices.rend()); 
                        for (size_t i : rowIndices) {
                            table->data.erase(table->data.begin() + static_cast<int>(i));
                        }
                table->indices[colIndex].erase(filterValue); // Remove index entry
                after = table->data.size();
            } else {
                // SLOW PATH: Full table scan
                table->data.erase(
                    remove_if(table->data.begin(), table->data.end(),
                        [&](const vector<Field>& row) {
                            const Field& cell = row[colIndex];
                            return (op == "=") ? (cell == filterValue) :
                                   (op == "<") ? (cell < filterValue) :
                                   (op == ">") ? (cell > filterValue) : false;
                        }),
                    table->data.end()
                );
                after = table->data.size();
            }

            size_t deletedRows = before - after;
            if (!errorFound) {
                cout << "Deleted " << deletedRows << " rows from " << tableName << "\n";
            }
            table->indices.clear();
        }

        else if (command == "REMOVE") {
            errorFound = false;
            string tableName;
            cin >> tableName;

            auto it = tables.find(tableName);
            if (it != tables.end()) {
                // Free the allocated memory before removing from the map
                delete it->second;
                tables.erase(it);
                cout << "Table " << tableName << " removed\n";
            } else {
                cout << "Error during REMOVE: " << tableName << " does not name a table in the database" << '\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
            }
        }

        else if (command == "GENERATE") {
            errorFound = false;
            string f, tableName, indexType, index, on, columnName;
            cin >> f >> tableName >> indexType >> index >> on >> columnName;

            if (tables.find(tableName) == tables.end()) {
                cout << "Error during GENERATE: " << tableName << " does not name a table in the database"<<'\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
            }

            Table& table = *tables[tableName];

            auto it = find(table.columnNames.begin(), table.columnNames.end(), columnName);
            if (it == table.columnNames.end()) {
                cout << "Error during GENERATE: " << columnName << " does not name a column in " << tableName <<'\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
            }
    
            size_t colIndex = static_cast<size_t>(distance(table.columnNames.begin(), it));

            // Clear existing index for this column
            table.indices[colIndex].clear();

            // Generate new index
            for (size_t i = 0; i < table.data.size(); ++i) {
                table.indices[colIndex][table.data[i][colIndex]].push_back(i);
            }
            if(!errorFound){
                cout << "Generated " << indexType << " index for table " << table.name 
                << " on column " << columnName << ", with " 
                 << table.indices[colIndex].size() << " distinct keys\n";
            }
        }
        else if (command == "JOIN") {
            bool errorFound = false;
            string table1Name, dummy1, table2Name, where, column1, equalSign, column2, dummy3, printKeyword;
            int numColumns;
            cin >> table1Name >> dummy1 >> table2Name >> where >> column1 >> equalSign >> column2 >> dummy3 >> printKeyword >> numColumns;
            

            if(tables.find(table1Name) == tables.end()){
                cout<<"Error during JOIN: "<<table1Name<<" does not name a table in the database"<<'\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
                // continue;
            }

            if(tables.find(table2Name) == tables.end()){
                cout<<"Error during JOIN: "<<table2Name<<" does not name a table in the database"<<'\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
            }

            Table& t1 = *tables[table1Name];
            Table& t2 = *tables[table2Name];

            // Find join column index in both tables
            auto it1 = find(t1.columnNames.begin(), t1.columnNames.end(), column1);
            auto it2 = find(t2.columnNames.begin(), t2.columnNames.end(), column2);

            if(it1 == t1.columnNames.end()){
                 cout << "Error during JOIN: " << column1 << " does not name a column in " << table1Name <<'\n';
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                 errorFound = true;
                //  continue;
            }

            if(it2 == t2.columnNames.end()){
                cout << "Error during JOIN: " << column2 << " does not name a column in " << table2Name <<'\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                errorFound = true;
            }

            size_t col1 = static_cast<size_t>(distance(t1.columnNames.begin(), it1));
            size_t col2 = static_cast<size_t>(distance(t2.columnNames.begin(), it2));
            // Read the column names and table numbers
            vector<pair<int, string>> columnSelections;  // (tableNumber, columnName)

            // Read <numColumns> number of column names and their table assignments
            for (int i = 0; i < numColumns; ++i) {
                string columnName;
                int tableNumber;
                cin >> columnName >> tableNumber;

                columnSelections.emplace_back(tableNumber, columnName);
            }
            bool error = false;
            for (const auto& selection : columnSelections) {
                int tableNum = selection.first;
                string colName = selection.second;
    
                if (tableNum == 1) {
                    auto it = find(t1.columnNames.begin(), t1.columnNames.end(), colName);
                    if (it == t1.columnNames.end()) {
                        cout << "Error during JOIN: " << colName << " does not name a column in " << table1Name << "\n";
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        errorFound = true;
                        error = true;
                    }
                } else if (tableNum == 2) {
                    auto it = find(t2.columnNames.begin(), t2.columnNames.end(), colName);
                    if (it == t2.columnNames.end()) {
                        cout << "Error during JOIN: " << colName << " does not name a column in " << table2Name << "\n";
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        errorFound = true;
                        error = true;
                    }
                } else {
                    cout << "Error during JOIN: invalid table number (must be 1 or 2)\n";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    errorFound = true;
                }
            }

            if (!error) {
                for (const auto& selection : columnSelections) {
                     cout << selection.second << " ";
                }
                cout << "\n";
            }

            int matchCount = 0;
            bool hasIndex = t2.indices.count(col2);


// Perform the join operation
if (hasIndex) {
    for (size_t i = 0; i < t1.data.size(); ++i) {
        const Field& key = t1.data[i][col1];
        if (t2.indices[col2].count(key)) {
            for (size_t j : t2.indices[col2][key]) {
                matchCount++;
                // Print the matched data for the selected columns if not in quiet mode
                if (!quietMode) {
                    for (const auto& selection : columnSelections) {
                        int tableNum = selection.first;
                        string colName = selection.second;
                        if (tableNum == 1) {
                            size_t colIdx = static_cast<size_t>(distance(
                                t1.columnNames.begin(), 
                                find(t1.columnNames.begin(), t1.columnNames.end(), colName)
                            ));
                            cout << t1.data[i][colIdx] << " ";
                        } else if (tableNum == 2) {
                            size_t colIdx = static_cast<size_t>(distance(
                                t2.columnNames.begin(), 
                                find(t2.columnNames.begin(), t2.columnNames.end(), colName)
                            ));
                            cout << t2.data[j][colIdx] << " ";
                        }
                    }
                    cout << "\n";
                }
            }
        }
    }
} else {
    for (size_t i = 0; i < t1.data.size(); ++i) {
        for (size_t j = 0; j < t2.data.size(); ++j) {
            if (t1.data[i][col1] == t2.data[j][col2]) {
                matchCount++;
                // Print the matched data for the selected columns if not in quiet mode
                if (!quietMode) {
                    for (const auto& selection : columnSelections) {
                        int tableNum = selection.first;
                        string colName = selection.second;
                        if (tableNum == 1) {
                            size_t colIdx = static_cast<size_t>(distance(
                                t1.columnNames.begin(), 
                                find(t1.columnNames.begin(), t1.columnNames.end(), colName)
                            ));
                            cout << t1.data[i][colIdx] << " ";
                        } else if (tableNum == 2) {
                            size_t colIdx = static_cast<size_t>(distance(
                                t2.columnNames.begin(), 
                                find(t2.columnNames.begin(), t2.columnNames.end(), colName)
                            ));
                            cout << t2.data[j][colIdx] << " ";
                        }
                    }
                    cout << "\n";
                }
            }
        }
    }
}

// Always print the summary, even in quiet mode
if (!errorFound) {
    cout << "Printed " << matchCount << " rows from joining " << table1Name << " to " << table2Name << "\n";
}

        }

        else if (command == "QUIT") {
            cout << "Thanks for being silly!\n";
            return;
        }
        else if (command[0] == '#') { 
            string discard;
            getline(cin, discard);  // Skip rest of comment line
        }
        else {
            cerr << "Error: unrecognized command" << '\n';
            string ignore;
            getline(cin, ignore);
        }
    }
}


Field Database::readFieldByType(ColumnType type) {
    if (type == ColumnType::Int) {
        int v;
        cin >> v;
        return Field(v);
    } else if (type == ColumnType::Double) {
        double v;
        cin >> v;
        return Field(v);
    } else if (type == ColumnType::Bool) {
        bool v;
        cin >> boolalpha >> v;
        return Field(v);
    } else {
        string v;
        cin >> v;
        return Field(v);
    }
}



void Table::insertRow(const vector<Field>& row) {
    // if (row.size() != columnNames.size()) {
    //     cerr << "Error: Row size does not match table column count\n";
    //     return;
    // }
    data.push_back(row);
}