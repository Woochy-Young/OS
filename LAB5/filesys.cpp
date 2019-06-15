#include <stdio.h>
#include <string.h>
#include <windows.h>

#define FILEBLOCK 3
#define DATABLOCK 4
#define FILEDESSIZE 100
#define L 1129
#define B 32
#define KEEPSIZE  105

typedef struct{
	char name[10];
	short int length;
	short int allo[10];
} filedes;

typedef struct{
	char Buffer[B];
	char *p;
	int id;
	int index;
	filedes dsc;
} OpenFileTable;

bool read_block(int i, char *p);
bool write_block(int i, char *p);
void intiFS();
bool create(char *filename);
bool destroy(char *filename);
bool open(char *filename);
bool close(char *index);
bool read(char *index, char *mem_area, int count);
bool write(char *index, char *mem_area, int count);
bool directory();
int getFreeDataBlock();
int getFileHeadBlock();
void setFileHeadBlock();
bool addFile(int fileIndex);
void save_date();
void load_data();

char ldisk[L][B];
char map[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
char temp_block[B];
int no_block;
OpenFileTable openFileTable;


bool read_block(int i, char *p){
	memcpy(p, ldisk[i], B);
	return true;
}

bool write_block(int i, char *p){
	memcpy(ldisk[i], p, B);
	return true;
}

void intiFS(){
	char filename[] = "catalog";
	char *p = filename;
	int length = 0;
	int index = getFileHeadBlock();
	filedes fd;
	memset(ldisk, 0, L*B);
	memcpy(&fd.name, filename, length + 1);
	fd.length = -1;
	for (int i = 0; i < 10; i++){
		fd.allo[i] = -1;
	}
	write_block(FILEBLOCK + DATABLOCK + index, (char*)&fd);
	no_block = 0;
	setFileHeadBlock();
	return;
}

bool create(char *filename){
	char *p = filename;
	int length = 0;
	if ((length = strlen(filename))>10){
		printf("文件名太长！\n");
		return false;
	}
	int index = getFileHeadBlock();
	setFileHeadBlock();
	p = filename;
	filedes fd;
	memcpy(&fd.name, filename, length + 1);
	fd.length = 0;
	for (int i = 0; i < 10; i++){
		fd.allo[i] = -1;
	}
	fd.allo[0] = getFreeDataBlock();
	setFileHeadBlock();
	memset(temp_block, -1, B);
	write_block(fd.allo[0], temp_block);
	write_block(FILEBLOCK + DATABLOCK + index, (char*)&fd);
	addFile(index);
	return true;
}

bool destroy(char *filename){
	char temp_block[B];
	filedes fd;
	read_block(DATABLOCK + FILEBLOCK, (char*)&fd);
	if (fd.allo[0] == -1){
		printf("文件不存在!\n");
		return false;
	}
	for (int i = 0; i < 10; i++){
		if (fd.allo[i] == -1)continue;
		read_block(fd.allo[i], temp_block);
		for (int j = 0; j < B; j++){
			if (temp_block[j] != -1){
				filedes tfd;
				read_block(DATABLOCK + FILEBLOCK + temp_block[j], (char*)&tfd);
				if (memcmp(filename, tfd.name, strlen(filename)) == 0){
					int fileIndex = temp_block[j];
					temp_block[j] = -1;
					write_block(fd.allo[i], temp_block);
					read_block(0, temp_block);
					int a = fileIndex / 8;
					int b = fileIndex % 8;
					temp_block[a] = temp_block[a] & (~map[b]);
					write_block(0, temp_block);
				}
			}
		}
	}
	return true;
}

bool open(char *filename){
	filedes fd;
	read_block(DATABLOCK + FILEBLOCK, (char*)&fd);
	for (int i = 0; i < 10; i++){
		if (fd.allo[i] == -1){
			printf("文件不存在!\n");
			return false;
		}
		read_block(fd.allo[i], temp_block);
		for (int j = 0; j < B; j++){
			if (temp_block[j] != -1){
				filedes temp_fd;
				read_block(DATABLOCK + FILEBLOCK + temp_block[j], (char*)&temp_fd);
				if (memcmp(filename, temp_fd.name, strlen(filename)) == 0){
					openFileTable.id = temp_block[j];
					openFileTable.index = 0;
					if (temp_fd.allo[0] == -1){
						openFileTable.p == NULL;
					}
					read_block(temp_fd.allo[0], openFileTable.Buffer);
					openFileTable.p = openFileTable.Buffer;
					openFileTable.dsc = temp_fd;
					return true;
				}
			}
		}
	}
	return false;
}

bool close(char *index){
	if (openFileTable.id == -1){
		printf("没有打开的文件\n");
		return false;
	}
	openFileTable.id = -1;
	return true;
}

bool read(char *index, char *mem_area, int count){
	int left = B - (openFileTable.p - openFileTable.Buffer);
	while (count){
		if (count - left > 0){ //当前缓冲区不能满足请求，再取
			count -= left;
			if (openFileTable.p == NULL){
				printf("越界访问\n");
				return false;
			}
			memcpy(mem_area, openFileTable.p, left);
			openFileTable.index++;
			if (openFileTable.dsc.allo[openFileTable.index] == -1){
				openFileTable.p = NULL;
			}
			read_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.p);
			left = B;
		}
		else{
			if (openFileTable.p == NULL){
				printf("越界访问\n");
				return false;
			}
			memcpy(mem_area, openFileTable.p, count);
			write_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.Buffer);
			openFileTable.p += left;
			count = 0;
			openFileTable.p += count;
		}
	}
	return true;
}


bool write(char *index, char *mem_area, int count){
	int left = B - (openFileTable.p - openFileTable.Buffer);
	while (count){
		if (count - left > 0){ //当前缓冲区不能满足请求，再取
			count -= left;
			if (openFileTable.p == NULL){
				printf("访问超过范围\n");
				return false;
			}
			memcpy(openFileTable.Buffer, mem_area, left);
			write_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.Buffer);
			openFileTable.index++;
			if (openFileTable.dsc.allo[openFileTable.index] == -1){
				openFileTable.p = NULL;
			}
			openFileTable.p = openFileTable.Buffer;
			left = B;
		}
		else{
			if (openFileTable.p == NULL){
				printf("访问超过范围\n");
				return false;
			}
			memcpy(openFileTable.p, mem_area, count);
			write_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.Buffer);
			openFileTable.p += count;
			openFileTable.dsc.length += count;
			write_block(DATABLOCK + FILEBLOCK + openFileTable.id, (char*)&openFileTable.dsc);
			return true;
		}
	}
	return false;
}

bool directory(){
	filedes fd;
	read_block(FILEBLOCK + DATABLOCK, (char*)&fd);
	printf("文件名称   文件大小\n");
	for (int i = 0; i < 10; i++){
		if (fd.allo[i] != -1){
			read_block(fd.allo[i], temp_block);
			for (int j = 0; j < B; j++){
				if (temp_block[j] != -1){
					filedes temp[B];
					read_block(FILEBLOCK + DATABLOCK + temp_block[j], (char*)&temp);
					printf("%s\t\t", temp->name);
					printf("%d\n", temp->length);
				}
			}
		}
	}
	return true;
}

int getFreeDataBlock(){
	int bIndex = FILEBLOCK;
	while (1){
		read_block(bIndex++, temp_block);
		for (int i = 0; i < B; i++){
			if (temp_block[i] != 0xff){
				for (int k = 0; k < 8; k++){
					if (!(map[k] & temp_block[i])){
						temp_block[i] = map[k] | temp_block[i];
						no_block = bIndex - 1;
						return KEEPSIZE + (bIndex - 2) * B * 8 + i * 8 + k;
					}
				}
			}
		}
	}
	return 0;
}

int getFileHeadBlock(){
	read_block(0, temp_block);
	char *p = temp_block;
	for (int i = 0; i < B * FILEBLOCK; i++){
		for (int k = 0; k < 8; k++){
			int tt = *p&map[k];
			if (!tt){
				*p = *p^map[k];
				no_block = 0;
				return i * 8 + k;
			}
		}
		p++;
	}
	return -1;
}
void setFileHeadBlock(){
	write_block(no_block, temp_block);
}

bool addFile(int fileIndex)
{
	filedes fp;
	read_block(DATABLOCK + FILEBLOCK, (char *)&fp);
	int dataIndex = 0;

	while (dataIndex < 10){
		if (fp.allo[dataIndex] == -1){
			fp.allo[0] = getFreeDataBlock();
			setFileHeadBlock();
			temp_block[0] = fileIndex;
			for (int i = 1; i < B; i++){
				temp_block[i] = -1;
			}
			write_block(fp.allo[dataIndex], temp_block);
			write_block(DATABLOCK + FILEBLOCK, (char *)&fp);
			return true;
		}
		read_block(fp.allo[dataIndex++], temp_block);
		for (int i = 0; i < B; i++){
			if (temp_block[i] == -1){
				temp_block[i] = fileIndex;
				write_block(fp.allo[dataIndex - 1], temp_block);
				return true;
			}
		}
	}
	return false;
}

void save_date(){
	FILE *fp = fopen("data.txt", "w");
	if (fp == NULL){
		printf("无法打开存储文件\n");
		return;
	}
	fwrite(&ldisk[0][0], 1, L*B, fp);
	fclose(fp);
}

void load_data(){
	FILE *fp = fopen("data.txt", "r");
	if (fp == NULL){
		printf("无法打开存储文件\n");
		return;
	}
	fread(&ldisk[0][0], 1, L*B, fp);
	fclose(fp);
}

int main(){
	intiFS();
	load_data();
	while (1){
		printf("|******************************************************|\n");
		printf("|*1 创建文件                                          *|\n");
		printf("|*2 删除文件                                          *|\n");
		printf("|*3 打开文件                                          *|\n");
		printf("|*4 查看目录                                          *|\n");
		printf("|*5 格式化                                            *|\n");
		printf("|*0 退出系统                                          *|\n");
		printf("|******************************************************|\n");
		printf("请选择操作：");
		char ch;
		ch = getchar();
		getchar();
		switch (ch){
		case '0':
			save_date();
			return 0;
		case '1':
			printf("请输入文件名:\n");
			char filename1[10];
			scanf("%s", filename1);
			if (create(filename1)){
				printf("创建成功!\n");
			}
			else
				printf("创建失败!\n");
			getchar();
			break;
		case '2':
			printf("请输入要删除的文件名:");
			char filename[10];
			scanf("%s", filename);
			getchar();
			printf("成功删除!\n");
			destroy(filename);
			break;
		case '3':
			printf("请输入文件名: ");
			scanf("%s", filename);
			getchar();
			if (!open(filename)){
				printf("打开文件失败\n");
				break;
			}
			printf("1 读取文件\n");
			printf("2 写入文件\n");
			printf("请选择：");
			ch = getchar();
			switch (ch){
			case '1':
				getchar();
				char temp[B];
				read(NULL, temp, B);
				printf("%s\n", temp);
				close(filename);
				break;
			case '2':
				getchar();
				char str[100];
				printf("请输入写入内容：\n");
				scanf("%s", str);
				write(NULL, str, strlen(str) + 1);
				close(filename);
				getchar();
				break;
			default:
				break;
			}
		case '4':
			directory();
			printf("\n");
			break;
		case '5':
			intiFS();
			printf("格式化成功！\n");
			break;
		}
	}
	return 0;
}
