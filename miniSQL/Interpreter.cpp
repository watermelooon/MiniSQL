#include"interpreter.h"
#include "buffer_new.h"

using namespace std;
/*
create table student(
	sno char(8),
	sname char(10) unique,
	sage int,
	sgender char(1),
	primary key(sno)
);

insert into student values('101','cjt',20,'M')
insert into student values('102','ss',19,'M')
insert into student values('103','hcy',19,'M')

select * from student where sage = 20

*/
int insertTable_api(string database_name, string table_name, string values)
{
	vector<int> attri_bytes;
	vector<string> attributeList;
	vector<vector<string>> opps;
	vector<string> opp;
	vector<string> existed_record;
	string value_record = "";
	string temp;
	string padding;
	string primary_key;
	int pKey_index=0;
	attri_bytes = get_Attribute_bytes(database_name, table_name);
	primary_key = get_primary_key(database_name, table_name);
	attributeList = get_Attribute_list(database_name, table_name);
	for (pKey_index = 0; pKey_index < attributeList.size(); pKey_index++)
	{
		if (attributeList[pKey_index] == primary_key)
			break;
	}
	if (attri_bytes.size() > 0)
	{
		opps.clear();
		opp.clear();
		opp.push_back(primary_key);
		opp.push_back("=");
		for (int i = 0; i < attri_bytes.size(); i++)
		{
			if (values.find(',') == string::npos)
				temp = values;
			else
			{
				temp = values.substr(0, values.find(','));
				values.erase(0, values.find(',') + 1);
			}
			if (temp.find('\'') != string::npos)
			{
				temp.erase(0, 1);
				temp.erase(temp.length() - 1, 1);
			}
			padding.assign(attri_bytes[i] - temp.length(), ' ');
			value_record = value_record + temp + padding;
			if(i==pKey_index)
				opp.push_back(temp);
		}
		opps.push_back(opp);
		existed_record= select_record(database_name, table_name, attributeList, attri_bytes, opps);
		if (existed_record.size() == 0)
			insertTable_value(database_name, table_name, value_record);
		else
			printf("ERROR: primary key value should be unique\n");
	}
	attri_bytes.clear();
	return 0;
}

int selectRecord_api(string database_name, string table_name, string op)
{
	vector<string> attributeList;
	vector<int> attri_bytes;
	vector<vector<string>> opps;
	vector<string> opp;
	string temp;
	attri_bytes = get_Attribute_bytes(database_name, table_name);
	attributeList = get_Attribute_list(database_name, table_name);
	size_t operator_off = 0;
	vector<string> selected_records;
	if (op != "")
	{
		if (op.find("and") == string::npos)
			temp = op;
		else
		{
			temp = op.substr(0, op.find("and"));
			op.erase(0, op.find("and") + 3);
		}
		if ((operator_off = temp.find('>')) != string::npos || (operator_off = temp.find('=')) != string::npos || (operator_off = temp.find('<')) != string::npos)
		{
			opp.push_back(temp.substr(0, operator_off - 1));
			opp.push_back(temp.substr(operator_off, 1));
			temp = temp.substr(operator_off + 2);
			if (temp.find('\'') != string::npos)
			{
				temp.erase(0, 1);
				temp.erase(temp.length() - 1, 1);
			}
			opp.push_back(temp);;
		}
		opps.push_back(opp);
	}
	if (attri_bytes.size() > 0)
	{
		selected_records = select_record(database_name, table_name, attributeList, attri_bytes, opps);
		for (int i = 0; i < selected_records.size(); i++)
		{
			cout << selected_records[i] << endl;
		}
	}
	return 0;
}

int deleteRecord_api(string database_name, string table_name, string op)
{
	vector<string> attributeList;
	vector<int> attri_bytes;
	vector<vector<string>> opps;
	vector<string> opp;
	string temp;
	attri_bytes = get_Attribute_bytes(database_name, table_name);
	attributeList = get_Attribute_list(database_name, table_name);
	size_t operator_off = 0;
	if (op != "")
	{
		if (op.find("and") == string::npos)
			temp = op;
		else
		{
			temp = op.substr(0, op.find("and"));
			op.erase(0, op.find("and") + 3);
		}
		if ((operator_off = temp.find('>')) != string::npos || (operator_off = temp.find('=')) != string::npos || (operator_off = temp.find('<')) != string::npos)
		{
			opp.push_back(temp.substr(0, operator_off - 1));
			opp.push_back(temp.substr(operator_off, 1));
			temp = temp.substr(operator_off + 2);
			if (temp.find('\'') != string::npos)
			{
				temp.erase(0, 1);
				temp.erase(temp.length() - 1, 1);
			}
			opp.push_back(temp);;
		}
		opps.push_back(opp);
	}
	if (attri_bytes.size() > 0)
		delete_record(database_name, table_name, attributeList, attri_bytes, opps);
	return 0;
}


int createTable_api(string database_name, string *command, int lines)
{
	string *attri_list, *type_list;
	int attribute_num = lines - 3;
	attri_list = new string[attribute_num];
	type_list = new string[attribute_num];
	string table_name = command[0].substr(13, command[0].length() - 14);
	string pKey = "";
	for (int i = 1; i < lines - 1; i++)
	{
		if (command[i].substr(0, 11) == "primary key")//最后一行主键
		{
			pKey = command[i].substr(12);
			pKey = pKey.erase(pKey.length() - 1, 1);
			break;
		}
		int index = 0;
		while (command[i].find('\t') == 0 || command[i].find(' ') == 0)//delete tap and blankspace at the beginning
			command[i].erase(0, 1);
		if (i<lines - 2)//detele ',' in the end
			command[i].erase(command[i].length() - 1, 1);
		attri_list[i - 1] = command[i].substr(0, command[i].find(' '));
		type_list[i - 1] = command[i].substr(command[i].find(' ') + 1);
		cout << attri_list[i - 1] + " " << type_list[i - 1] << endl;
	}
	if( 0 == createTable_catalog(database_name, table_name, pKey, attribute_num, attri_list, type_list))//if file does not exist, initiate the .txt file
		createTable_record(database_name, table_name);
	return 0;
}

/*
int main()
{
	string command[10];
	string database_name = "cjt";
	int lines;
	while (1)
	{
		for (int i = 1; i < 10; i++)
			command[i] = "";
		printf("-->");
		lines = 0;
		getline(cin, command[0]);
		lines++;
		if (command[0].compare(0, 12, "create table") == 0)
		{
			for (int i = 1; i < 10; i++)
			{
				getline(cin, command[i]);
				lines++;
				if (command[i] == ");")
					break;
			}
			createTable_api(database_name, command, lines);
		}
		else if (command[0].compare(0, 10, "drop table") == 0)
		{
			string table_name = command[0].substr(11, command[0].length() - 11);
			if (0 == dropTable_catalog(database_name, table_name))
				dropTable_record(database_name, table_name);
		}
		else if (command[0].compare(0, 11, "insert into") == 0)
		{
			string table_name = command[0].substr(12, command[0].find("values")-13);
			
			//提取values()括号内的字符串
			string values = command[0].substr(command[0].find("values")+7);
			values.erase(values.length() - 1, 1);

			insertTable_api(database_name, table_name, values);
		}
		else if (command[0].compare(0, 6, "select") == 0)
		{
			string table_name;
			string op="";
			if (command[0].find("where") != string::npos)
			{
				table_name = command[0].substr(14, command[0].find("where") - 15);
				op = command[0].substr(command[0].find("where") + 6);
			}
			else
				table_name = command[0].substr(14);
			selectRecord_api(database_name, table_name, op);
		}
		else if (command[0].compare(0, 6, "delete") == 0)
		{
			string table_name;
			string op = "";
			if (command[0].find("where") != string::npos)//delete from student where 
			{
				table_name = command[0].substr(12, command[0].find("where") - 13);
				op = command[0].substr(command[0].find("where") + 6);
			}
			else
				table_name = command[0].substr(12);
			deleteRecord_api(database_name, table_name, op);
		}

		else if (command[0].compare(0, 4, "exit") == 0)
		{
			exit_record();
			printf("MiniSQL exited successfully\n");
			//break;
		}
		printf("\n");
	}
	system("pause");
	return 0;
}
*/
