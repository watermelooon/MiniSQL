#pragma once
#include <iostream>
#include "tinyxml.h"
#include <cstdarg>
#include <string>
#include <vector>
#include "buffer_new.h"
using namespace std;

bool createTable_catalog(string database_name, string table_name, string pKey, int num, string* attributeList, string *typeList);

vector<int> get_Attribute_bytes(string database_name, string table_name);

bool dropTable_catalog(string database_name, string table_name);

vector<string> get_Attribute_list(string database_name, string table_name);

string get_primary_key(string database_name, string table_name);