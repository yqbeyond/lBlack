#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERSIZE 512 //每条指令的最大长度
#define MAX_CMDS 10 // 一次性最多输入指令的条数
#define MAX_ARGS 10 // 一条指令最多可携带指令的数目

char buffer[BUFFERSIZE + 1]; // 缓冲一条输入指令
char ch; // 记录获取的命令字符
char BUFFER[MAX_CMDS][BUFFERSIZE + 1];
char* INPUT[MAX_CMDS]; // 存储所有一次性输入的指令
int CMDS_LEN[MAX_CMDS]; // 记录每条指令的长度
int num; // 记录输入指令的条数
char* arg[MAX_ARGS]= {NULL}; // 解析后arg[0]为指令，arg[1..]为指令携带的参数
int args; // 当前指令参数个数

char *get_cur_rel_path()
{
    char *path;
    path = get_current_dir_name();
    char *ptr;
    ptr = strrchr(path, '/');
    return ptr+1;
}

char is_fileexist(char* cmd)
{
    char* path, *p;
    int i;
    i = 0;
    path = getenv("PATH");
    p = path;
    while (*p != '\0')
    {
        if (*p != ':')
        {
            buffer[i++] = *p;
        }
        else
        {
            buffer[i++] = '/';
            buffer[i] = '\0';
            strcat(buffer, cmd);
            if (access(buffer, F_OK) == 0)
            {
                return 0;
            }
            else
            {
                i = 0;
            }
        }
        p++;
    }
    return -1;
}

void getcmds()
{
    int count = 0; // the length of current input cmd
    // reset CMDS_LEN
    for (count = 0; count < MAX_CMDS; count++)
    {
        CMDS_LEN[count] = 0;
    }
    count = 0;
    num = 0;
    ch = getchar();

    // get cmds into BUFFER
    while (ch != '\n' && count < BUFFERSIZE * MAX_CMDS)
    {
        if (ch != ';')
        {
            BUFFER[num][CMDS_LEN[num]++] = ch;
        }
        else
        {
            num++;
        }
        ch = getchar();
        count++;
    }
    if(count >= BUFFERSIZE * MAX_CMDS)
    {
        printf("Command too long.\n");
        count = 0;
        return ;
    }
    else
    {

        for (count = 0; count <= num; count++)
        {
            BUFFER[count][CMDS_LEN[count]] = '\0';
        }

    }
    for (count = 0; count <= num; count++)
    {
        INPUT[count] = (char*)malloc(sizeof(char) * (CMDS_LEN[count] + 1));
        strcpy(INPUT[count], BUFFER[count]);
    }

}
/*
void printcmds(int args)
{
    int i = 0;
    for (i = 0; i < args; i++)
    {
        printf("%s ", arg[i]);
    }
    printf("\n");
}
*/

void parse()
{
    int count, i, j, k;

    for (count = 0; count <= num; count++)
    {
        // '<' or '>' or '|'
        args = 0;
        strcpy(buffer, BUFFER[count]);
        for (i = 0, j = 0, k = 0; i <= CMDS_LEN[count]; i++)
        {

            if (INPUT[count][i] == ' ' || INPUT[count][i] == '\t' || INPUT[count][i] == '\0')
            {
                if (j == 0) //
                    continue;
                else
                {
                    buffer[j++] = '\0';
                    arg[k] = (char*) malloc(sizeof(char) * j);
                    args++;
                    strcpy(arg[k], buffer);
                    j = 0;
                    k++;
                }
            }
            else
            {
                buffer[j++] = INPUT[count][i];
            }
        }

        if (args == 0)
        {
            break;
        }
        free(INPUT[count]);

        if (strcmp(arg[0], "bye") == 0 || strcmp(arg[0], "exit") == 0)
        {
            printf("Bye, stupid man.\n");
            exit(0);
        }
        else
        {
            //printcmds(args);
            //is_bj =0;
            arg[k] = (char*) 0;
            if (is_fileexist(arg[0]) == -1)
            {
                printf("command %s not found.\n", arg[0]);
                for (i = 0; i < k; i++)
                {
                    free(arg[i]);
                    continue;
                }
            }
            else // run cmd
            {
                pid_t pid;
                int status;
                if ((pid = fork()) == 0)
                {
                    execv(buffer, arg);
                }
                else
                {
                    if (strcmp(arg[0], "cd") == 0)
                    {
                        chdir(arg[1]);
                    }
                    waitpid(pid, &status, 0);
                }
                for (i = 0; i < k; i++)
                {
                    free(arg[i]);
                }
            }
        }
    }
}


int main(int argc, char**argv)
{
    while(1)
    {
        printf("%s >>> ", get_cur_rel_path());
        getcmds();
        parse();

    }
    return 0;
}
