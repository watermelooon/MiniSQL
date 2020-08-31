#pragma once

#include"record.h"
#include <string>
#include"buffer.h"
#include "buffer_new.h"
#include <cstring>
#include <vector>
#include <map>
using namespace std;

int createTable_record(string database_name, string table_name);

int insertTable_value(string database_name, string table_name, string orig_values);

void exit_record();

void dropTable_record(string database_name, string table_name);

vector<string> select_record(string database_name, string table_name, vector<string>& attributeList, vector<int>& attri_bytes, vector<vector<string>>& op);

bool delete_record(string database_name, string table_name, vector<string>& attributeList, vector<int>& attri_bytes, vector<vector<string>>& op);