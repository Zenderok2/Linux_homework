#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <signal.h> 
#include <unistd.h> 
 
void handle_sighup(int sig)
{ 
    printf("Configuration reloaded\n"); 
}

void dump_memory(int pid)
{ 
    char path[64]; 
    snprintf(path, sizeof(path), "/proc/%d/mem", pid); 
    char command[128]; 
    snprintf(command, sizeof(command), "cat %s > /tmp/memory_dump_%d.txt", path, pid); 
    system(command); 
} 
 
int main()
{ 
    char input[1024]; 
    FILE *history = fopen("history.txt", "a"); 
 
    signal(SIGHUP, handle_sighup); 
 
    while (1)
    { 
        printf("> "); 
        if (fgets(input, sizeof(input), stdin) == NULL)
        { 
            printf("\nЗавершение программы.\n"); 
            break; 
        } 
 
        input[strcspn(input, "\n")] = '\0'; 
  
        fprintf(history, "%s\n", input); 
        fflush(history); 
  
        if (strcmp(input, "exit") == 0 || strcmp(input, "\\q") == 0)
        { 
            printf("Выход\n"); 
            break; 
        } 
 
        else if (strncmp(input, "echo ", 5) == 0)
        { 
            printf("%s\n", input + 5); 
        } 
 
        else if (strncmp(input, "\\e $", 4) == 0)
        { 
            char *var = getenv(input + 4); 
            if (var)
            { 
                printf("%s\n", var); 
            }
            else
            { 
                printf("Переменная не найдена\n"); 
            } 
        } 
 
        else if (strncmp(input, "./", 2) == 0)
        { 
            system(input); 
        } 
 
        else if (strcmp(input, "\\l /dev/sda") == 0)
        { 
            system("lsblk /dev/sda"); 
        } 
 
        else if (strcmp(input, "\\cron") == 0)
        { 
            system("mount --bind /etc/cron.d /tmp/vfs"); 
            system("crontab -l > /tmp/vfs/crontab.txt"); 
            printf("VFS подключен в /tmp/vfs и список задач сохранен.\n"); 
        }

        else if (strncmp(input, "\\mem ", 5) == 0)
        { 
            int pid = atoi(input + 5);
            if (pid > 0)
            { 
                dump_memory(pid); 
                printf("Дамп памяти процесса %d сохранен в /tmp.\n", pid); 
            }
            else
            { 
                printf("Некорректный PID.\n"); 
            } 
        } 
 
        else
        { 
            printf("Неизвестная команда: %s\n", input); 
        } 
    } 
 
    fclose(history); 
    return 0; 
}
