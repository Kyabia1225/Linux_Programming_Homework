#include <iostream>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

bool list_flag = false;       //-l
bool currentDir_flag = false; //-d
bool recursive_flag = false;  //-R
bool reverse_flag = false;    //-r
bool all_flag = false;        //-a
bool inode_flag = false;      //-i

bool isSingleFile = true; //本条ls命令中只有一个文件的情况

void do_ls(char *, bool);
bool pstrcmp(char *, char *);      //char*从小到大排序
bool pstrcmp_r(char *, char *);    //char*从大到小排序
void mode_to_letters(int, char *); //表示读写权限
void do_stat(char *);              //利用stat()函数获取文件信息
void show_file_info(char *, struct stat *);
char *uid_to_name(uid_t); //Type of user identifications.
char *gid_to_name(gid_t); //Type of group identifications.

int main(int argc, char **argv)
{
    if (argc == 1)
    { //不带参数
        do_ls((char *)".", true);
    }
    else
    {
        vector<char *> files;
        while (--argc)
        {
            ++argv;
            if (*argv[0] == '-')
            {
                if ((*argv)[1] == 'a')
                {
                    all_flag = true;
                }
                else if ((*argv)[1] == 'l')
                {
                    list_flag = true;
                }
                else if ((*argv)[1] == 'R')
                {
                    recursive_flag = true;
                }
                else if ((*argv)[1] == 'd')
                {
                    currentDir_flag = true;
                }
                else if ((*argv)[1] == 'i')
                {
                    inode_flag = true;
                }
                else if ((*argv)[1] == 'r')
                {
                    reverse_flag = true;
                }
                else
                {
                    cerr << "ls: invalid option -- '" << (*argv)[1] << "'\n";
                    exit(-1); //异常指令，返回-1退出
                }
            }
            else
            {
                files.push_back(*argv);
            }
        }
        if (files.empty())
        { //ls -abcdefg
            do_ls((char *)".", true);
            return 0;
        }
        else if (files.size() > 1)
        {
            isSingleFile = false; //多个文件 需要输出文件名
        }
        if (reverse_flag)
        {
            sort(files.begin(), files.end(), pstrcmp_r);
        }
        else
        {
            sort(files.begin(), files.end(), pstrcmp);
        }
        for (auto i = files.begin(); i < files.end(); ++i)
        {
            do_ls(*i, true);
        }
    }
    return 0;
}

bool pstrcmp(char *a, char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] < b[i])
        {
            return true;
        }
        else if (a[i] > b[i])
        {
            return false;
        }
        else
        {
            ++i;
        }
    }
    if (a[i] == '\0')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool pstrcmp_r(char *a, char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] < b[i])
        {
            return false;
        }
        else if (a[i] > b[i])
        {
            return true;
        }
        else
        {
            ++i;
        }
    }
    if (a[i] == '\0')
    {
        return false;
    }
    else
    {
        return true;
    }
}

void do_ls(char *dirname, bool first_recursion)
{
    DIR *dir;
    dirent *direntp;
    if ((dir = opendir(dirname)) == NULL)
    {
        if (first_recursion)
        {
            cerr << "ls: cannot access '" << dirname << "': No such file or directory\n";
            return;
        }
        else
        {
            return; //递归过程中不会找不到文件
        }
    }
    else
    {
        vector<char *> files;
        if (currentDir_flag)
        { //含有-d, 跳过对-a -r -R的处理
            files.push_back(dirname);
            goto FILES_ADDRESS;
        }
        while ((direntp = readdir(dir)) != NULL)
        {
            if (all_flag || direntp->d_name[0] != '.')
            {
                files.push_back(direntp->d_name);
            }
        }
        if (reverse_flag)
        {
            sort(files.begin(), files.end(), pstrcmp_r);
        }
        else
        {
            sort(files.begin(), files.end(), pstrcmp);
        }
    FILES_ADDRESS:
        if ((recursive_flag & !currentDir_flag) || !isSingleFile)
        {
            cout << dirname << ":\n";
            if (files.empty())
            {
                cout << endl;
            }
        }
        if (files.empty())
        {
            return;
        }
        if (list_flag)
        {
            for (auto i = files.begin(); i < files.end(); ++i)
            {
                char *prefix = (char *)malloc(strlen(dirname) + 2);
                stpcpy(prefix, dirname);
                strcat(prefix, "/");
                do_stat(strcat(prefix, *i));
                printf("%s\n", *i);
                delete (prefix);
            }
        }
        else
        {
            for (auto i = files.begin(); i < files.end(); ++i)
            {
                struct stat info;
                stat(*i, &info); //获取文件信息，不会找不到
                if (inode_flag)
                {
                    cout << info.st_ino << " ";
                }
                cout << *i << "  ";
            }
            if ((recursive_flag & !currentDir_flag) || !isSingleFile)
            {
                cout << endl;
            }
            if (!files.empty())
            {
                cout << endl;
            }
        }
        //处理递归操作
        if (recursive_flag && !currentDir_flag)
        {
            if (files.empty())
            {
                return;
            }
            char *prefix = new char[strlen(dirname) + 2];
            strcpy(prefix, dirname);
            strcat(prefix, "/");
            for (auto i = files.begin(); i < files.end(); ++i)
            {
                char *prefix_tmp = new char[strlen(prefix) + 1];
                strcpy(prefix_tmp, prefix);
                strcat(prefix_tmp, *i);
                do_ls(prefix_tmp, false);
                delete (prefix_tmp);
            }
            delete (prefix);
        }
    }
    closedir(dir);
}

void do_stat(char *filename)
{
    struct stat info;
    stat(filename, &info);
    show_file_info(filename, &info);
}

void show_file_info(char *filename, struct stat *info_p)
{ //ls -l 输出信息
    char modestr[11];
    mode_to_letters(info_p->st_mode, modestr);
    if (inode_flag)
    {
        printf("%ld ", info_p->st_ino);
    }
    printf("%s ", modestr);
    printf("%ld ", info_p->st_nlink);
    printf("%s ", uid_to_name(info_p->st_uid));
    printf("%s", gid_to_name(info_p->st_gid));
    printf("%8ld ", info_p->st_size);
    printf("%.12s ", 4 + ctime(&info_p->st_ctime));
}

void mode_to_letters(int mode, char *str)
{
    strcpy(str, "----------");

    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    if (S_ISFIFO(mode))
        str[0] = 'f';
    if (S_ISLNK(mode))
        str[0] = 'l';
    if (S_ISSOCK(mode))
        str[0] = 's';

    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';

    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';

    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
}

char *uid_to_name(uid_t uid)
{
    struct passwd *pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(uid)) == NULL)
    {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
    {
        return pw_ptr->pw_name;
    }
}

char *gid_to_name(gid_t gid)
{
    struct group *grp_ptr;
    static char numstr[10];

    if ((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
    {
        return grp_ptr->gr_name;
    }
}