#include"buffer.h"
#include"record.h"

using namespace std;

//返回fileInfo
fileInfo *get_file_info(fileInfo *fileList, string fileName,  int fileType)
{
	fileInfo *file_temp = fileList->next;
	while (NULL != file_temp)				//遍历所有的file节点
	{
		if (file_temp->fileName == fileName && file_temp->fileType == fileType)
		{
			return file_temp;
		}
		file_temp = file_temp->next;
	}
	//没有找到这个文件
	cout << "buffer::get_file_info::can't find the file" << endl;
	return NULL;
}

//在文件下查找这个数据块
blockInfo *get_block_in_file(fileInfo *file_temp,   int blockNum)
{
	blockInfo *block_temp = file_temp->firstBlock;
	//遍历该file节点下所有block
	while (NULL != block_temp)
	{
		if (block_temp->blockNum == blockNum)
		{
			return block_temp;
		}
		block_temp = block_temp->next;
	}
	return NULL;
}

//将块从文件节点下移除
blockInfo *remove_block_in_file(fileInfo *file_temp, blockInfo *block)
{
	
	int blockNum = block->blockNum;
	blockInfo *block_last = NULL;
	blockInfo *block_temp = file_temp->firstBlock;
	while (NULL != block_temp)
	{
		if (block_temp->blockNum == blockNum)
		{
			//find the node wanted
			break;
		}
		block_last = block_temp;
		block_temp = block_temp->next;
	}
	if (NULL != block_temp)
	{
		if (NULL == block_last)
		{
			file_temp->firstBlock = NULL;//头节点
		}
		else
		{
			block_last->next = block_temp->next;//中间节点&叶节点
		}
		//返回前一个节点
		return block_last;
	}
	//没找着
	return NULL;
}

//返回可用的块
blockInfo *create_new_Block(fileInfo *fileList)
{
	static int i = 0;//记录现有block数字
					 //搜索空块
	fileInfo *file_temp = fileList->next;
	blockInfo *newblock;
	//首先找有没有free buffer block
	while (NULL != file_temp)
	{
		int freeBlock_num = file_temp->freeNum;
		if (freeBlock_num > 0)
		{
			blockInfo *block_temp = get_block_in_file(file_temp, freeBlock_num);
			if (NULL != block_temp)
			{
				remove_block_in_file(file_temp, block_temp);//从原文件下移除，添加到新文件下
				return block_temp;
			}
			//file记录了freeNUM 但是找不到这个block
			cout << "WANING  buffer::create_new_Block:the freeNum in record may be wrong" << endl;
		}
		file_temp = file_temp->next;
	}
	//没有free buffer block则新建
	if (i < MAX_BLOCK)
	{
		//申请内存
		newblock = new blockInfo;
		i++;
		if (NULL != newblock)
		{
			newblock->cBlock = new char[BLOCK_SIZE];
			if (NULL != newblock->cBlock) return newblock;

		}
	}

	//释放调用次数最少的内存块
	int time= INT_MAX;
	blockInfo *btemp = NULL;
	fileInfo *ftemp = NULL;
	file_temp = fileList->next;

	while (NULL != file_temp)
	{
		
		blockInfo *block_ptr = file_temp->firstBlock;
		while (NULL != block_ptr)
		{
			if (block_ptr->iTime < time)
			{
				time = block_ptr->iTime;
				btemp = block_ptr;
				ftemp = file_temp;
			}
			block_ptr = block_ptr->next;
		}
		file_temp = file_temp->next;
	}
	btemp->iTime = 1;
	remove_block_in_file(ftemp,btemp);
	return btemp;
}

//把block连到file下
void addBlock_toFile(fileInfo *m_fileInfo, blockInfo *m_blockInfo)
{
	//加在最前面
	//m_blockInfo->file = m_fileInfo;
	m_blockInfo->next = m_fileInfo->firstBlock;
	m_fileInfo->firstBlock = m_blockInfo;
}


//返回数据
blockInfo *get_file_block(fileInfo *fileList, string fileName,   int fileType,   int blockNum)
{
	blockInfo *block_temp;
	fileInfo *file_temp = get_file_info(fileList, fileName, fileType);

	if (NULL == file_temp)//找不到这个文件
	{
		cout << "buffer::get_file_block::wrong information" << endl;
		return NULL;
	}

	block_temp = get_block_in_file(file_temp, blockNum);
	if (NULL != block_temp)
	{
		if (1 == block_temp->lock)//判断这个块有没有被锁住
		{
			cout << "buffer::the block has been locked" << endl;
			return NULL;
		}

		else if (block_temp->blockNum == blockNum)
		{
			if (0 != block_temp->charNum)	//如果不是空块
			{
				block_temp->iTime++;		//调用次数++
				
			}
			else
			{
				block_temp = readBlock(file_temp, blockNum, block_temp);
				block_temp->iTime = 1;		//调用次数初始化
				
			}
		}
	}
	else
	{
		cout << "buffer::get_file_block::can't find this block >>> begin read" << endl;
		//重新搞一个块过来并添加到文件下
		block_temp = create_new_Block(fileList);
		block_temp->blockNum = blockNum;
		block_temp->dirtyBit = 0;
		block_temp = readBlock(file_temp, blockNum, block_temp);
		addBlock_toFile(file_temp, block_temp);			//将该block增加到头节点下
		block_temp->iTime = 1;					//调用次数初始化
	}
	return block_temp;
}



void closeDatabase(fileInfo *fileList, bool m_flag)
{
	fileInfo *file_temp = fileList->next;
	while (NULL != file_temp)
	{
		//release blocks of current file
		closeFile(file_temp); 
		//release the fileInfo of current file
		fileList->next = file_temp->next;
		delete file_temp;
		file_temp = fileList->next;
	}
}

//关闭文件
void closeFile(fileInfo* file_temp)
{
	blockInfo *block_temp = file_temp->firstBlock;
	blockInfo *block_delete;

	//依次删除该file下的block
	while (NULL != block_temp)
	{
		writeBlock(file_temp, block_temp);
		block_delete = block_temp;
		block_temp = block_temp->next;//指向下一个节点
		delete[] block_delete->cBlock;//释放内存
		delete block_delete;//删除节点
	}

}
//---------------------DB files module-----------------------




//已经判断内存中不存在这一个块、 读取数据
blockInfo* readBlock(fileInfo *file_temp, int m_blockNum, blockInfo *block_temp)
{
	//读取文件
	string fpath;
	if (0 == file_temp->fileType)
	{
		fpath = file_temp->DB_Name + "\\table\\" + file_temp->fileName + ".txt";
	}
	else
	{
		fpath = file_temp->DB_Name + "\\index\\" + file_temp->fileName + ".txt";
	}
	fstream file(fpath, ios::in | ios::out);
	if (!file.is_open())							//检查打开是否成功
	{
		cout << "buffer::readBlock::can't find the file" + fpath << endl;
		return NULL;
	}
	int offset = (m_blockNum - 1) * BLOCK_SIZE;		//计算偏移量
	file.seekp(offset, ios::beg);					//从起始位置开始
	block_temp->cBlock = new char[BLOCK_SIZE];		//申请内存
	file.read(block_temp->cBlock, BLOCK_SIZE);		//读取文件
	file.close();
	return block_temp;
}

//把block指针所指向的块的内容写回磁盘。
void writeBlock(fileInfo *file_temp, blockInfo * block)
{
	if (!block->dirtyBit) return;

	

	string m_fileName = file_temp->fileName;
	int m_blockNum = block->blockNum;
	string fpath;
	//fpath = "C:\\Users\\17205\\Desktop\\miniSQL\\miniSQL\\cjt\\table\\student.txt";
	if (0 == file_temp->fileType)
	{
		fpath = file_temp->DB_Name + "\\table\\" + m_fileName + ".txt";
	}
	else
	{
		fpath = file_temp->DB_Name + "\\index\\" + m_fileName + ".txt";
	}
	
	fstream file(fpath, ios::in | ios::out);
	if (!file.is_open())							//检查打开是否成功
	{
		cout << "buffer::writeBlock::can't find the file" + fpath << endl;
		return;
	}
	int offset = (m_blockNum - 1) * BLOCK_SIZE;
	file.seekp(offset, ios::beg);
	string temp = block->cBlock;
	cout << temp << endl;
	file.write(block->cBlock, temp.length());
	file.close();
}