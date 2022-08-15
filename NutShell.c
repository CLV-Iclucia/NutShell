#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
char *pwd = NULL;
char **parse(char *cmd, int n, int *argc)
{
    char **str = malloc(sizeof(char*));
    str[0] = malloc(1);
    int m = 0, len = 0, bufsize = 1;
    for(int i = 0; i < n; i++)
    {
        if(cmd[i] == ' ')
        {
            if(len == bufsize)
            {
                str[m] = realloc(str[m], bufsize + 1);
                str[m][len++] = 0;
            }
            else str[m][len++] = 0;
            len = 0;
            bufsize = 1;
            m++;
            str = realloc(str, (m + 1) * sizeof(char*));
            str[m] = malloc(1);
        }
        else
        {
            if(len == bufsize)
            {
                bufsize <<= 1;
                str[m] = realloc(str[m], bufsize);    
            }
            str[m][len++] = cmd[i];
        }
    }
    if(len == bufsize)
    {
        str[m] = realloc(str[m], bufsize + 1);
        str[m][len++] = 0;
    }
    else str[m][len++] = 0;
    m++;
    str = realloc(str, m * sizeof(char*));
    str[m] = NULL;
    *argc = m;
    return str;
}
void process(char* cmd, int n)
{
    if(!n)return ;
    int m;
    char **str = parse(cmd, n, &m);
    if(strcmp(str[0], "quit") == 0)
    {
        for(int i = 0; i < m; i++)
            free(str[i]);
        free(str);
        if(m == 1)
        {
            free(cmd);
            free(pwd);
            exit(0);
        }
        else
        {
            printf("quit: Command not found.\n");
            return ;
        }
    }
    else if(strcmp(str[0], "cd") == 0)
    {
        if(m == 1)
        {
            free(str[0]);
            free(str);
            return ;
        }
        if(m > 2) printf("cd: Too many arguments.\n");
        else chdir(str[1]);
        for(int i = 0; i < m; i++)
            free(str[i]);
        free(str);
        return ;
    }
    else if(strcmp(str[0], "pwd") == 0)
    {
        for(int i = 0; i < m; i++)
            free(str[i]);
        free(str);
        printf("%s\n", pwd);
        return ;
    }
    else
    {
        int pid;
        if((pid = fork()) == 0)
        {
            if(execvp(str[0], str) < 0)
            {
                printf("%s: Command not found.\n", str[0]);
                exit(-1);
            }
        }
        int status, retpid;
        if((retpid = waitpid(-1, &status, 0)) < 0)
        {
            if(retpid == pid)
            {
                for(int i = 0; i < m; i++)
                    free(str[i]);
                free(str);
            }
        }
    }
    
}
int main()
{
    while(1)
    {
        pwd = getcwd(NULL, 0);
        printf("%s>", pwd);
        char* cmd = malloc(sizeof(char));
        int cnt = 0, n = 1;
        while(1)
        {
            char ch = getchar();
            if(ch == '\n') 
            {
                if(cmd[cnt - 1] == '\r')cmd[--cnt] = 0;
                else if(cnt == n)
                {
                    cmd = realloc(cmd, n + 1);
                    cmd[cnt] = 0;
                }
                break;
            }
            else
            {
                if(ch == ' ' && (!cnt || cmd[cnt - 1] == ' ' ))continue;
                if(cnt == n)
                {
                    n <<= 1;
                    cmd = realloc(cmd, n);
                }
                cmd[cnt++] = ch;
            }
        }
        while(cmd[cnt - 1] == ' ')cmd[--cnt] = 0;
        if(cnt) process(cmd, cnt);
        free(cmd);
    }
}