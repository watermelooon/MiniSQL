#include "record.h"


Block_list Block_buff;

int createTable_record(string database_name, string table_name)
{
	Block_buff.addBlock(table_name, 1);
	return 0;
}

int insertTable_value(string database_name, string table_name, string orig_values)
{
	int count;
	int i = 0;
	Block* temp = Block_buff.getBlock(table_name, 1);

	string blockdata = temp->data;
	string dataNew;
	char c[5];
	string head_cout;
	count = atoi(blockdata.substr(0, 4).c_str());
	count++;
	_itoa_s(count, c, 5, 10);
	for (i = 0; i < 4; i++)
	{
		if (c[i] >= 48 && c[i] <= 57)
		{
			continue;
		}
		else
			c[i] = ' ';
	}
	c[i] = '\0';
	blockdata.replace(0, 4, c);
	dataNew = blockdata + orig_values;
	strcpy_s(temp->data, dataNew.size() + 1, dataNew.c_str());
	temp->dirtyBit = 1;
	temp->numofrecords++;
	cout << "#" << temp->data << "#" << endl;
	return 0;
}

vector<string> select_record(string database_name, string table_name, vector<string>& attributeList, vector<int>& attri_bytes, vector<vector<string>>& op)
{
	Block* temp = Block_buff.getBlock(table_name, 1);
	string records = temp->data;
	string current_record;
	int record_length=0;
	int offset = 0;
	int row = op.size();
	int lastAttribute_off=0;
	map<string, int> attri_num;
	map<int, int> attri_off;
	vector<string> selected_records;

	for (int i = 0; i < attri_bytes.size(); i++)
	{
		record_length = record_length + attri_bytes[i];
		attri_off[i] = lastAttribute_off;			//存放偏移量的map
		lastAttribute_off = lastAttribute_off + attri_bytes[i];
		attri_num[attributeList[i]] = i;			//存放下标的map
	}
	

	for (int i =0; i< temp->numofrecords; i++)
	{
		int j = 0;
		offset = 4 + i * record_length;
		current_record = records.substr(offset, record_length);
		for (j = 0; j < row; j++)
		{
			
			//valid = 0; 不满足条件就退出循环，不打印当前数据
			if (op[j][1] == "=")//数字和字符串都有可能，因此直接填充' '到等长进行字符串比较;
			{
				int t = attri_num[string(op[j][0])];
				string value_temp;
				string padding;
				padding.assign(attri_bytes[t] - op[j][2].length(), ' ');
				value_temp = op[j][2] + padding;
				if (current_record.substr(attri_off[t], attri_bytes[t]) != value_temp)
					break;
			}
			else if (op[j][1] == ">")//atoi()转换成整形时可以自动去除末尾的空格
			{
				int t = attri_num[string(op[j][0])];
				if (atoi(current_record.substr(attri_off[t], attri_bytes[t]).c_str()) <= atoi(op[j][2].c_str()))
					break;
			}
			else if (op[j][1] == "<")
			{
				int t = attri_num[string(op[j][0])];
				if (atoi(current_record.substr(attri_off[t], attri_bytes[t]).c_str()) >= atoi(op[j][2].c_str()))
					break;
			}
		}
		if ((row == 0) || (j == row and row > 0))//valid =1; 打印数据
			selected_records.push_back(records.substr(offset, record_length));
	}
	return selected_records;
}

bool delete_record(string database_name, string table_name, vector<string>& attributeList, vector<int>& attri_bytes, vector<vector<string>>& op)
{
	Block* temp = Block_buff.getBlock(table_name, 1);
	string records = temp->data;
	string current_record;
	int record_length = 0;
	int offset = 0;
	int row = op.size();
	int lastAttribute_off = 0;
	int deleted_number = 0;
	map<string, int> attri_num;
	map<int, int> attri_off;
	vector<string> selected_records;
	int numofrecords_copy = temp->numofrecords;
	for (int i = 0; i < attri_bytes.size(); i++)
	{
		record_length = record_length + attri_bytes[i];
		attri_off[i] = lastAttribute_off;			//存放偏移量的map
		lastAttribute_off = lastAttribute_off + attri_bytes[i];
		attri_num[attributeList[i]] = i;			//存放下标的map
	}
	for (int i = 0; i < numofrecords_copy; i++)
	{
		int j = 0;
		offset = 4 + i * record_length - deleted_number * record_length;
		current_record = records.substr(offset, record_length);
		for (j = 0; j < row; j++)
		{
			//valid = 0; 不满足条件就退出循环，不删除当前数据
			if (op[j][1] == "=")//数字和字符串都有可能，因此直接填充' '到等长进行字符串比较;
			{
				int t = attri_num[string(op[j][0])];
				string value_temp;
				string padding;
				padding.assign(attri_bytes[t] - op[j][2].length(), ' ');
				value_temp = op[j][2] + padding;
				if (current_record.substr(attri_off[t], attri_bytes[t]) != value_temp)
					break;
			}
			else if (op[j][1] == ">")//atoi()转换成整形时可以自动去除末尾的空格
			{
				int t = attri_num[string(op[j][0])];
				if (atoi(current_record.substr(attri_off[t], attri_bytes[t]).c_str()) <= atoi(op[j][2].c_str()))
					break;
			}
			else if (op[j][1] == "<")
			{
				int t = attri_num[string(op[j][0])];
				if (atoi(current_record.substr(attri_off[t], attri_bytes[t]).c_str()) >= atoi(op[j][2].c_str()))
					break;
			}
		}
		if ((row == 0) || (j == row and row > 0))//valid =1; 删除数据
		{
			char c[5];
			int k = 0;
			cout << "delete record: " << records.substr(offset, record_length) << endl;
			records.erase(offset, record_length);
			deleted_number++;
			temp->numofrecords--;
			temp->dirtyBit = 1;
			_itoa_s(temp->numofrecords, c, 5, 10);
			for (k = 0; k < 4; k++)
			{
				if (c[k] >= 48 && c[k] <= 57)
				{
					continue;
				}
				else
					c[k] = ' ';
			}
			c[k] = '\0';
			records.replace(0, 4, c);
		}
	}
	if (deleted_number > 0)
		strcpy_s(temp->data, records.size() + 1, records.c_str());
	else
		printf("no record is deleted\n");
	return 0;
}

void exit_record()
{
	Block_buff.closeALL();
}

void dropTable_record(string database_name, string table_name)
{
	Block_buff.clear_block(table_name, 1);
}

