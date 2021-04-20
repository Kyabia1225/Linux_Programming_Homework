# Linux_Programming_Homework

### 概述

#### ls.cpp

ls.cpp模拟Linux中ls命令，完成了'-a', '-l', '-d', '-r', '-R', '-i'六个参数的功能。参数可以随意组合，ls命令后可以跟一个或多个文件名。

#### wc.cpp

wc.cpp模拟Linux中wc命令，完成了不带参数，可以跟一个或多个文件名的计算字数的功能。

### 设计

#### ls.cpp

主要有以下几个变量与函数帮助完成主要功能：

```c++
//global variables: 表示是否含有注释中的参数
bool list_flag = false;       //-l
bool currentDir_flag = false; //-d
bool recursive_flag = false;  //-R
bool reverse_flag = false;    //-r
bool all_flag = false;        //-a
bool inode_flag = false;      //-i
//functions
void do_ls(char *, bool);
bool pstrcmp(char *, char *);      //char*从小到大排序
bool pstrcmp_r(char *, char *);    //char*从大到小排序
void mode_to_letters(int, char *); //表示读写权限
void do_stat(char *);              //利用stat()函数获取文件信息
void show_file_info(char *, struct stat *);
char *uid_to_name(uid_t);          //uid: Type of user identifications.
char *gid_to_name(gid_t);          //gid: Type of group identifications.
```

​	main函数中通过对argc和argv的解析来对全局变量进行修改从而记录参数。在函数do_ls(char* dirname, bool first_recursion)中读取名为dirname的目录名，如果读取不到则报错，否则读取该目录下的所有文件，将这些文件夹或文件保存在`vector<char*>files`中，随后对files进行字典序排序，按照排序完的顺序依次处理。如果含有'-R'参数，需要对子文件全部进行递归，则将当前目录名dirname与文件名通过‘/'组合起来，使用do_ls继续打印，直到读取不到文件，此过程是深度优先遍历。

​	关于do_stat(char* filename)函数，它主要是通过`<stat.h>`中定义的stat函数获取文件详细信息，其中struct stat结构体如下：

```c
struct stat
  {
    __dev_t st_dev;		/* Device.  */
#ifndef __x86_64__
    unsigned short int __pad1;
#endif
#if defined __x86_64__ || !defined __USE_FILE_OFFSET64
    __ino_t st_ino;		/* File serial number.	*/
#else
    __ino_t __st_ino;			/* 32bit file serial number.	*/
#endif
#ifndef __x86_64__
    __mode_t st_mode;			/* File mode.  */
    __nlink_t st_nlink;			/* Link count.  */
#else
    __nlink_t st_nlink;		/* Link count.  */
    __mode_t st_mode;		/* File mode.  */
#endif
    __uid_t st_uid;		/* User ID of the file's owner.	*/
    __gid_t st_gid;		/* Group ID of the file's group.*/
#ifdef __x86_64__
    int __pad0;
#endif
    __dev_t st_rdev;		/* Device number, if device.  */
#ifndef __x86_64__
    unsigned short int __pad2;
#endif
#if defined __x86_64__ || !defined __USE_FILE_OFFSET64
    __off_t st_size;			/* Size of file, in bytes.  */
#else
    __off64_t st_size;			/* Size of file, in bytes.  */
#endif
    __blksize_t st_blksize;	/* Optimal block size for I/O.  */
#if defined __x86_64__  || !defined __USE_FILE_OFFSET64
    __blkcnt_t st_blocks;		/* Number 512-byte blocks allocated. */
#else
    __blkcnt64_t st_blocks;		/* Number 512-byte blocks allocated. */
#endif
#ifdef __USE_XOPEN2K8
    /* Nanosecond resolution timestamps are stored in a format
       equivalent to 'struct timespec'.  This is the type used
       whenever possible but the Unix namespace rules do not allow the
       identifier 'timespec' to appear in the <sys/stat.h> header.
       Therefore we have to handle the use of this header in strictly
       standard-compliant sources special.  */
    struct timespec st_atim;		/* Time of last access.  */
    struct timespec st_mtim;		/* Time of last modification.  */
    struct timespec st_ctim;		/* Time of last status change.  */
# define st_atime st_atim.tv_sec	/* Backward compatibility.  */
# define st_mtime st_mtim.tv_sec
# define st_ctime st_ctim.tv_sec
#else
    __time_t st_atime;			/* Time of last access.  */
    __syscall_ulong_t st_atimensec;	/* Nscecs of last access.  */
    __time_t st_mtime;			/* Time of last modification.  */
    __syscall_ulong_t st_mtimensec;	/* Nsecs of last modification.  */
    __time_t st_ctime;			/* Time of last status change.  */
    __syscall_ulong_t st_ctimensec;	/* Nsecs of last status change.  */
#endif
#ifdef __x86_64__
    __syscall_slong_t __glibc_reserved[3];
#else
# ifndef __USE_FILE_OFFSET64
    unsigned long int __glibc_reserved4;
    unsigned long int __glibc_reserved5;
# else
    __ino64_t st_ino;			/* File serial number.	*/
# endif
#endif
  };
```

​	通过对读取其中fields可以获得所有'-l'需要的值，具体过程参考ls.cpp中show_file_info函数。

### wc.cpp

```c++
int lines, words, bytes, flag;		//处理当前文件或输入统计各种数量
int lines_total = 0, words_total = 0, bytes_total = 0;	//记录总数
//统计文件字符
bool wc_file(char *filename);
//统计键盘输入
void wc();
```

​	如果没有额外参数，则调用wc()统计键盘输入，否则调用wc_file()统计每个参数（即一个或多个文件）的字符，并记录总数量。如果文件数>1，最后输出total统计。wc_file会判断当前文件是不是目录文件，如果不是目录文件或不存在则报错。

​	其中wc与wc_file主要通过process_ch(char*)实现，process_ch函数对单个字符进行处理，如果是'\n'，则行数+1，如果是空白符（'\t', ' ', '\n'）则将flag置为0，flag是0代表当前光标处在上个单词结尾或之后，还未出现新单词。当flag == 0且键入字符不是空白符时，则代表新单词开始，单词数量+1.
