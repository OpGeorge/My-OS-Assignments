#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void parsare(char *path)
{
    int fd = -1;
    fd = open(path, O_RDONLY);

    int version = 0;
    int no_of_sections = 0;

    char sect_name[17];
    int sect_type = 0;
    int sect_offset = 0;
    int sect_size = 0;

    int headerSize = 0;
    char Magic;

    int i = 1;
    if (fd == -1)
    {
        printf("ERROR\n");
        return;
    }

    lseek(fd, -1, SEEK_END);
    read(fd, &Magic, 1);

    lseek(fd, -3, SEEK_END);
    read(fd, &headerSize, 2);

    lseek(fd, -headerSize, SEEK_END);
    read(fd, &version, 2);

    read(fd, &no_of_sections, 1);

    if (Magic != 'b')
    {
        printf("ERROR\n");
        printf("wrong magic\n");
        close(fd);
        return;
    }

    if (version < 17 || version > 71)
    {
        printf("ERROR\n");
        printf("wrong version\n");
        close(fd);
        return;
    }
    if (no_of_sections < 8 || no_of_sections > 11)
    {
        printf("ERROR\n");
        printf("wrong sect_nr");
        close(fd);
        return;
    }
    i = no_of_sections;

    while (i > 0)
    {
        read(fd, sect_name, 17);
        read(fd, &sect_type, 4);

        if (sect_type != 52 && sect_type != 69 && sect_type != 81 && sect_type != 42 && sect_type != 89)
        {
            printf("ERROR\n");
            printf("wrong sect_types\n");
            close(fd);
            return;
        }

        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
        sect_name[0] = '\0';

        i--;
    }
    lseek(fd, -headerSize, SEEK_END);
    lseek(fd, 3, SEEK_CUR);

    i = no_of_sections;
    int j = 1;
    printf("SUCCESS\n");
    printf("version=%d\n", version);
    printf("nr_sections=%d\n", no_of_sections);
    while (i > 0)
    {
        read(fd, sect_name, 17);
        read(fd, &sect_type, 4);
        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
        printf("section%d: %s %d %d\n", j, sect_name, sect_type, sect_size);
        j++;
        i--;
    }
    close(fd);
}

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
                if (S_ISDIR(statbuff.st_mode) && recursiv == 1)
                {
                    listare(recursiv, size_greater, has_perm, filePath);
                    if (size_greater > 0)
                        continue;
                }
                if (has_perm == 1)
                {

                    if (statbuff.st_mode & S_IXUSR)
                    {
                        printf("%s\n", filePath);
                    }
                }
                else if ((size_greater > 0))
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

        if (strncmp(argv[1], "parse", 6) == 0 || strncmp(argv[2], "parse", 6) == 0)
        {
            char *path = NULL;
            if (strncmp(argv[1], "path=", 5) == 0)
            {
                char *substr = "path=";
                path = argv[1] + strlen(substr);
            }
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                char *substr = "path=";
                path = argv[2] + strlen(substr);
            }

            if (path == NULL)
            {
                return -1;
            }
            parsare(path);
        }
    }
    return 0;
}
