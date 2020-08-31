#pragma once
#include <fstream>
#include <iostream>
#include <atlstr.h>
#include <string>

#define BLOCK_SIZE           4096      // the size of one block
#define MAX_BLOCK           40          // the max number of the blocks

using namespace std;



struct blockInfo {
	int blockNum;       // the block number of the block,
	bool dirtyBit;     // 0 -> flase£¬ 1 -> indicate dirty, write back
	blockInfo*next;         //the pointer point to next block
							//fileInfo *file;     // the pointer point to the file, which the block belongs to
	int charNum;     // the number of chars in theblock
	char* cBlock;     // the array space for storingthe records in the block in buffer
	int iTime;              // it indicate the times of using the block
	int lock;                // prevent the blockfrom replacing
};



struct fileInfo {
	string DB_Name;
	int fileType;                       //0-> data file£¬ 1 -> index file
	string fileName;        // the name of the file
	int recordAmount;              // the number of record in the file
	int freeNum;                // the free block number which could be used for thefile
	int recordLength;	        // the length of the record in the file
	fileInfo *next;                     // the pointer points to the next file
	blockInfo *firstBlock;   // point to the first block within the file
};





void writeBlock(fileInfo *file_temp, blockInfo * block);
blockInfo* readBlock(fileInfo *file_temp, int m_blockNum, blockInfo *block_temp);
void closeFile(fileInfo* file_temp);
void closeDatabase(fileInfo *fileList, bool m_flag);
blockInfo *get_file_block(fileInfo *fileList, string fileName, int fileType, int blockNum);
void addBlock_toFile(fileInfo *m_fileInfo, blockInfo *m_blockInfo);
blockInfo *create_new_Block(fileInfo *fileList);
blockInfo *remove_block_in_file(fileInfo *file_temp, blockInfo *block);
blockInfo *get_block_in_file(fileInfo *file_temp, int blockNum);
fileInfo *get_file_info(fileInfo *fileList, string fileName, int fileType);