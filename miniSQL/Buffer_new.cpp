#include "buffer_new.h"

void Block::init()
{
	this->data = new char[BLOCK_SIZE];
	memset(this->data, 0, BLOCK_SIZE);
	strcpy_s(this->data, 5, "0   ");
	this->dirtyBit = 0;
	this->fileName = "";
	this->iTime = 0;
	this->next = NULL;
	this->num = 0;
	this->written = 0;
	this->numofrecords = 0;
}


bool Block::writeBlocktoDisk()
{
	if (!this->dirtyBit) return 1;
	string fpath;
	fpath = fileName + ".txt";
	//fstream file(fpath, ios::in | ios::out);
	ofstream file(fpath, ios::out);
	if (!file)							
	{
		cout << "buffer::writeBlock::can't fi nd the file:" + fpath << endl;
		return 1;
	}
	int offset = (num - 1) * BLOCK_SIZE;
	file.seekp(offset, ios::beg);
	string temp = this->data;
	cout << "Write to file " << fileName << ": " << temp << endl;
	file.write(this->data, temp.length());
	file.close();

	this->dirtyBit = 0;
	return 0;
}

bool Block::readBlockfromDisk()
{
	string fpath;
	fpath = fileName + ".txt";
	//fstream file(fpath, ios::in | ios::out);
	ifstream file(fpath, ios::in);
	if (!file)						
	{
		cout << "buffer::readBlock::can't find the file:" + fpath << endl;
		return 1;
	}
	int offset = (num - 1) * BLOCK_SIZE;
	file.seekg(offset, ios::beg);
	file.read(this->data, BLOCK_SIZE);
	file.close();
	return 0;
}



Block_list::Block_list()
{
	block_count=0;
	block_list=new Block;
	block_list->next=NULL;
}

Block_list::~Block_list()
{
	this->closeALL();
}

bool Block_list::addBlock(string fileName, int num)
{
	if(this->block_count >= MAX_BLOCK)
		this->free_fullBlock();
	
	Block* temp = new Block;
	Block* temp_ptr;

	temp->init();
	temp->fileName=fileName;
	temp->num=num;
	temp_ptr=this->block_list->next;
	this->block_list->next=temp;
	temp->next=temp_ptr;
	this->block_count++;
	return 0;
}
/*
bool Block_list::add_record(string fileName, int num, const char *record)
{
	int count;
	int i = 0;
	Block* temp= this->getBlock(fileName,num);

	string blockdata= temp->data;
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
	dataNew = blockdata + record;
	strcpy_s(temp->data, dataNew.size()+1, dataNew.c_str());
	temp->dirtyBit = 1;
	temp->numofrecords++;
	cout << "#" << temp->data << "#" << endl;
	return 0;
}
*/


bool Block_list::clear_block(string fileName, int num)
{
	Block* temp = this->getBlock(fileName, num);
	memset(temp->data, 0, BLOCK_SIZE);
	strcpy_s(temp->data, 5, "0   ");
	temp->dirtyBit = 1;
	temp->writeBlocktoDisk();
	temp->fileName = "";
	temp->iTime = 0;
	temp->num = 0;
	temp->written = 0;
	temp->numofrecords = 0;

	return 0;
}



bool Block_list::free_fullBlock()
{
	Block* block_ptr, *ptr_last;
	Block *LRU, *LRU_last;
	block_ptr=this->block_list->next;
	ptr_last = LRU = LRU_last = block_ptr;

	while(block_ptr != NULL)
	{
		if(block_ptr->iTime > LRU->iTime)
		{
			LRU_last = ptr_last;
			LRU = block_ptr;
		}
		ptr_last = block_ptr;
		block_ptr=block_ptr->next;
	}

	LRU->writeBlocktoDisk();
	
	LRU_last->next=LRU->next;
	delete LRU->data;
	delete LRU;
	this->block_count--;
	return 0;
}

Block* Block_list::getBlock(string fileName, int num)
{
	Block* temp;
	temp=this->block_list->next;
	while(temp != NULL)
	{
		if(temp->fileName==fileName && temp->num==num)
		{
			temp->iTime--;
			return temp;
		}
			
		temp=temp->next;
	}

	printf("Blockdata is not found in the buffer, read from disk instead\n");
	addBlock(fileName, num); //add to the head of the block list
	this->block_list->next->readBlockfromDisk();
	this->block_list->next->written = 1;
	
	string blockdata = this->block_list->next->data;
	this->block_list->next->numofrecords = atoi(blockdata.substr(0, 4).c_str());
	return this->block_list->next;
}


void Block_list::closeALL()
{
	Block* close;
	Block* close_temp;
	close=this->block_list->next;
	while(close != NULL)
	{
		cout << close->dirtyBit << endl;
		close->writeBlocktoDisk();
		close_temp=close;
		close=close->next;
		delete[] close_temp->data;
		delete close_temp;
	}
	
}


