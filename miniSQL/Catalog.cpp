#include"catalog.h"
using namespace std;

class XML
{
private:
	TiXmlDocument doc;

public:
	TiXmlElement* load_root(string database_name)
	{
		string database_path = database_name + ".xml";
		if (!doc.LoadFile(database_path.c_str()))
		{
			cerr << doc.ErrorDesc() << endl;
			return NULL;
		}
		TiXmlElement* root = doc.FirstChildElement();
		if (root == NULL)
		{
			cerr << "ERROR:Fail to load file: No root element." << endl;
			return NULL;
		}
		return root;
	}
	void save(string path)
	{
		path = path + ".xml";
		doc.SaveFile(path.c_str());
	}
	void close()
	{
		doc.Clear();
	}
};

vector<int> get_Attribute_bytes(string database_name, string table_name)
{
	vector<int> Attri_bytes;
	TiXmlElement* root;
	XML xml_file;
	root = xml_file.load_root(database_name);
	bool found = false;
	TiXmlElement* table = root->FirstChildElement();
	while (table != NULL)
	{
		string s = table->Value();
		//cout << "#" << s << "," << table_name.c_str() << "#" << endl;
		if (s == table_name.c_str())
		{
			found = true;
			//int num = atoi(table->Attribute("attributeNum"));
			for (TiXmlElement* attribute = table->FirstChildElement(); attribute != NULL; attribute = attribute->NextSiblingElement())
			{
				string text = attribute->GetText();
				if (text.compare(0, 4, "char") == 0)
					Attri_bytes.push_back(atoi(text.substr(5, text.find(')') - 5).c_str()));
				else if (text.compare(0, 3, "int") == 0)
					Attri_bytes.push_back(10);
				else if (text.compare(0, 5, "float") == 0)
					Attri_bytes.push_back(10);
			}
			break;
		}
		table = table->NextSiblingElement();
	}
	xml_file.close();

	if (found == false)
	{
		printf("ERROR: No such table\n");
	}
	return Attri_bytes;
}

vector<string> get_Attribute_list(string database_name, string table_name)
{
	vector<string> Attribute_list;
	TiXmlElement* root;
	XML xml_file;
	root = xml_file.load_root(database_name);
	bool found = false;
	TiXmlElement* table = root->FirstChildElement();
	while (table != NULL)
	{
		string s = table->Value();
		if (s == table_name.c_str())
		{
			found = true;
			//int num = atoi(table->Attribute("attributeNum"));
			for (TiXmlElement* attribute = table->FirstChildElement(); attribute != NULL; attribute = attribute->NextSiblingElement())
			{
				string attributeName = attribute->Value();
				Attribute_list.push_back(attributeName);
			}
			break;
		}
		table = table->NextSiblingElement();
	}
	xml_file.close();
	if (found == false)
	{
		printf("ERROR: No such table\n");
	}
	return Attribute_list;
}

string get_primary_key(string database_name, string table_name)
{
	TiXmlElement* root;
	XML xml_file;
	string primary_key;
	root = xml_file.load_root(database_name);
	bool found = false;
	TiXmlElement* table = root->FirstChildElement();
	while (table != NULL)
	{
		string s = table->Value();
		if (s == table_name.c_str())
		{
			primary_key = table->Attribute("primary");
			found = true;
			break;
		}
		table = table->NextSiblingElement();
	}
	xml_file.close();
	if (found == false)
	{
		printf("ERROR: Primary key not found\n");
	}
	return primary_key;
}



bool createTable_catalog(string database_name, string table_name, string pKey, int num, string* attributeList, string *typeList)
{
	
	TiXmlElement* root;
	XML xml_file;
	root = xml_file.load_root(database_name);
	TiXmlElement* table = root->FirstChildElement();
	while (table != NULL)
	{
		string name = table->Value();
		if (name == table_name.c_str())
		{
			printf("ERROR:Table name already exists!\n");
			return 1;
		}
		table = table->NextSiblingElement();
	}

	table = new TiXmlElement(table_name.c_str());
	root->LinkEndChild(table);
	table->SetAttribute("attributeNum", num);
	table->SetAttribute("primary", pKey.c_str());
	TiXmlElement* table_attribute;
	TiXmlText *attribute_type;
	for (int i=0; i<num;i++)
	{
		table_attribute = new TiXmlElement((attributeList[i].c_str()));
		table->LinkEndChild(table_attribute);

		attribute_type = new TiXmlText(typeList[i].c_str());
		table_attribute->LinkEndChild(attribute_type);
	}
	xml_file.save(database_name);
	xml_file.close();
	printf("Create table successfully\n");
	return 0;
}

bool dropTable_catalog(string database_name, string table_name)
{
	int flag=0;

	TiXmlElement* root;
	XML xml_file;
	root = xml_file.load_root(database_name);
	TiXmlElement* table = root->FirstChildElement();
	while (table != NULL)
	{
		string s = table->Value();
		if (s == table_name.c_str())
		{
			root->RemoveChild(table);
			flag = 1;
			break;
		}
		table = table->NextSiblingElement();
	}
	//doc.SaveFile("cjt.xml");
	//doc.Clear();
	xml_file.save(database_name);
	xml_file.close();
	if (flag == 1)
	{
		printf("Drop table successfully\n");
		return 0;
	}
	else
	{
		printf("ERROR:Fail to drop table: no such table\n");
		return 1;
	}
}