#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void listare(int recursiv, int size_greater, int has_perm, char *path)
{

    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuff;

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("ERROR\nCould not open directory\n");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if (lstat(filePath, &statbuff) == 0)
            {
                if (has_perm == 1)
                {

                    if (statbuff.st_mode & S_IXUSR)
                    {
                        printf("%s\n", filePath);
                    }

                    
                }
                else if((size_greater > 0))
                {

                        if (statbuff.st_size > size_greater)
                    {
                        printf("%s\n", filePath);
                    }
                }
                else
                    {
                        printf("%s\n", filePath);
                    }


                if (S_ISDIR(statbuff.st_mode) && recursiv == 1)
                {
                    listare(recursiv, size_greater, has_perm, filePath);
                }
            }
        }
    }
    closedir(dir);
}

int list_verf(int recursiv, int size_greater, int has_perm, char *path)
{
    DIR *dir = NULL;
    dir = opendir(path);
    if (dir == NULL)
    {
        perror("ERROR\nCould not open directory\n");
        return -1;
    }

    printf("SUCCESS\n");
    listare(recursiv, size_greater, has_perm, path);
    closedir(dir);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {

        int i;

        if (strncmp(argv[1], "variant", 8) == 0)
        {
            printf("%d\n", 59861);
            return 0;
        }

        if (strncmp(argv[1], "list", 5) == 0)
        {
            i = 2;
            int recursive = 0;
            int size_greater_value = 0;
            char *path = NULL;
            int permission = 0;
            while (i < argc)
            {
                if (strncmp(argv[i], "recursive", 10) == 0)
                {
                    recursive = 1;
                }

                if (strncmp(argv[i], "size_greater=", 12) == 0)
                {
                    char *substr = "size_greater=";
                    char *number = argv[i] + strlen(substr);
                    size_greater_value = atoi(number);
                }

                if (strncmp(argv[i], "path=", 5) == 0)
                {
                    char *subsrt = "path=";
                    path = argv[i] + strlen(subsrt);
                }
                if (strncmp(argv[i], "has_perm_execute", 17) == 0)
                {
                    permission = 1;
                }

                i++;
            }

            int lst = list_verf(recursive, size_greater_value, permission, path);
            lst++;
        }
    }
    return 0;
}