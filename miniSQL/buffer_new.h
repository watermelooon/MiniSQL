#pragma once

#include <fstream>
#include <iostream>
#include <atlstr.h>
#include <string>

#define BLOCK_SIZE           4096      // the size of one block
#define MAX_BLOCK           40          // the max number of the blocks

using namespace std;



class Block 
{
public:

	bool written;
	string fileName;
	int num;       // the block number of the block,
	char* data;     // the array space for storingthe records in the block in buffer
	bool dirtyBit;     // 0 -> flase�� 1 -> indicate dirty, write back
	int iTime;              // it indicate the times of using the block
	int numofrecords;


public:

	Block* next;
	void init();
	bool writeBlocktoDisk();
	
	bool readBlockfromDisk();
};



class Block_list
{
	Block* block_list;
	int block_count;

public:
	Block_list();
	~Block_list();
	bool clear_block(string fileName, int num);
	Block* getBlock(string fileName, int num);

	bool addBlock(string fileName, int num);

	bool add_record(string fileName, int num, const char *record);

	char* get_record(string fileName, int num, int index);
	//bool deleteBlock(string fileName, int num);

	bool free_fullBlock();

	void closeALL();

};