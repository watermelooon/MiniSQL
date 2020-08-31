#include"buffer.h"
#include"record.h"

using namespace std;

//����fileInfo
fileInfo *get_file_info(fileInfo *fileList, string fileName,  int fileType)
{
	fileInfo *file_temp = fileList->next;
	while (NULL != file_temp)				//�������е�file�ڵ�
	{
		if (file_temp->fileName == fileName && file_temp->fileType == fileType)
		{
			return file_temp;
		}
		file_temp = file_temp->next;
	}
	//û���ҵ�����ļ�
	cout << "buffer::get_file_info::can't find the file" << endl;
	return NULL;
}

//���ļ��²���������ݿ�
blockInfo *get_block_in_file(fileInfo *file_temp,   int blockNum)
{
	blockInfo *block_temp = file_temp->firstBlock;
	//������file�ڵ�������block
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

//������ļ��ڵ����Ƴ�
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
			file_temp->firstBlock = NULL;//ͷ�ڵ�
		}
		else
		{
			block_last->next = block_temp->next;//�м�ڵ�&Ҷ�ڵ�
		}
		//����ǰһ���ڵ�
		return block_last;
	}
	//û����
	return NULL;
}

//���ؿ��õĿ�
blockInfo *create_new_Block(fileInfo *fileList)
{
	static int i = 0;//��¼����block����
					 //�����տ�
	fileInfo *file_temp = fileList->next;
	blockInfo *newblock;
	//��������û��free buffer block
	while (NULL != file_temp)
	{
		int freeBlock_num = file_temp->freeNum;
		if (freeBlock_num > 0)
		{
			blockInfo *block_temp = get_block_in_file(file_temp, freeBlock_num);
			if (NULL != block_temp)
			{
				remove_block_in_file(file_temp, block_temp);//��ԭ�ļ����Ƴ�����ӵ����ļ���
				return block_temp;
			}
			//file��¼��freeNUM �����Ҳ������block
			cout << "WANING  buffer::create_new_Block:the freeNum in record may be wrong" << endl;
		}
		file_temp = file_temp->next;
	}
	//û��free buffer block���½�
	if (i < MAX_BLOCK)
	{
		//�����ڴ�
		newblock = new blockInfo;
		i++;
		if (NULL != newblock)
		{
			newblock->cBlock = new char[BLOCK_SIZE];
			if (NULL != newblock->cBlock) return newblock;

		}
	}

	//�ͷŵ��ô������ٵ��ڴ��
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

//��block����file��
void addBlock_toFile(fileInfo *m_fileInfo, blockInfo *m_blockInfo)
{
	//������ǰ��
	//m_blockInfo->file = m_fileInfo;
	m_blockInfo->next = m_fileInfo->firstBlock;
	m_fileInfo->firstBlock = m_blockInfo;
}


//��������
blockInfo *get_file_block(fileInfo *fileList, string fileName,   int fileType,   int blockNum)
{
	blockInfo *block_temp;
	fileInfo *file_temp = get_file_info(fileList, fileName, fileType);

	if (NULL == file_temp)//�Ҳ�������ļ�
	{
		cout << "buffer::get_file_block::wrong information" << endl;
		return NULL;
	}

	block_temp = get_block_in_file(file_temp, blockNum);
	if (NULL != block_temp)
	{
		if (1 == block_temp->lock)//�ж��������û�б���ס
		{
			cout << "buffer::the block has been locked" << endl;
			return NULL;
		}

		else if (block_temp->blockNum == blockNum)
		{
			if (0 != block_temp->charNum)	//������ǿտ�
			{
				block_temp->iTime++;		//���ô���++
				
			}
			else
			{
				block_temp = readBlock(file_temp, blockNum, block_temp);
				block_temp->iTime = 1;		//���ô�����ʼ��
				
			}
		}
	}
	else
	{
		cout << "buffer::get_file_block::can't find this block >>> begin read" << endl;
		//���¸�һ�����������ӵ��ļ���
		block_temp = create_new_Block(fileList);
		block_temp->blockNum = blockNum;
		block_temp->dirtyBit = 0;
		block_temp = readBlock(file_temp, blockNum, block_temp);
		addBlock_toFile(file_temp, block_temp);			//����block���ӵ�ͷ�ڵ���
		block_temp->iTime = 1;					//���ô�����ʼ��
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

//�ر��ļ�
void closeFile(fileInfo* file_temp)
{
	blockInfo *block_temp = file_temp->firstBlock;
	blockInfo *block_delete;

	//����ɾ����file�µ�block
	while (NULL != block_temp)
	{
		writeBlock(file_temp, block_temp);
		block_delete = block_temp;
		block_temp = block_temp->next;//ָ����һ���ڵ�
		delete[] block_delete->cBlock;//�ͷ��ڴ�
		delete block_delete;//ɾ���ڵ�
	}

}
//---------------------DB files module-----------------------




//�Ѿ��ж��ڴ��в�������һ���顢 ��ȡ����
blockInfo* readBlock(fileInfo *file_temp, int m_blockNum, blockInfo *block_temp)
{
	//��ȡ�ļ�
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
	if (!file.is_open())							//�����Ƿ�ɹ�
	{
		cout << "buffer::readBlock::can't find the file" + fpath << endl;
		return NULL;
	}
	int offset = (m_blockNum - 1) * BLOCK_SIZE;		//����ƫ����
	file.seekp(offset, ios::beg);					//����ʼλ�ÿ�ʼ
	block_temp->cBlock = new char[BLOCK_SIZE];		//�����ڴ�
	file.read(block_temp->cBlock, BLOCK_SIZE);		//��ȡ�ļ�
	file.close();
	return block_temp;
}

//��blockָ����ָ��Ŀ������д�ش��̡�
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
	if (!file.is_open())							//�����Ƿ�ɹ�
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