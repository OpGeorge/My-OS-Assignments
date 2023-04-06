#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int findSFverf(char *path)
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

        return 1;
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

        close(fd);
        return 1;
    }

    if (version < 17 || version > 71)
    {

        close(fd);
        return 1;
    }
    if (no_of_sections < 8 || no_of_sections > 11)
    {

        close(fd);
        return 1;
    }
    i = no_of_sections;

    while (i > 0)
    {
        read(fd, sect_name, 17);
        read(fd, &sect_type, 4);

        if (sect_type != 52 && sect_type != 69 && sect_type != 81 && sect_type != 42 && sect_type != 89)
        {

            close(fd);
            return 1;
        }

        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
        sect_name[0] = '\0';

        i--;
    }

    int offsetArray[no_of_sections];
    int sizeArray[no_of_sections];

    lseek(fd, 3, SEEK_SET);
    for (int i = 0; i < no_of_sections; i++)
    {

        lseek(fd, 21, SEEK_CUR);
        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
        offsetArray[i] = sect_offset;
        sizeArray[i] = sect_size;
    }
    int nrlines;
    for (int i = 0; i < no_of_sections; i++)
    {
        lseek(fd, offsetArray[i], SEEK_SET);
        char *buffer = (char *)calloc(sizeArray[i], sizeof(char));
        nrlines = 0;
        read(fd, buffer, sizeArray[i]);
        for (int j = 0; j < sizeArray[i] - 1; j++)
        {
            if (buffer[j] == 0x0D && buffer[j + 1] == 0x0A)
            {
                nrlines++;
            }
        }
        if (nrlines < 14)
        {

            free(buffer);
            close(fd);
            return 1;
        }
        free(buffer);
    }
    close(fd);
    return 0;
}

void findall(char *path, int firstItr)
{

    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuff;
    int success;
    dir = opendir(path);
    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        closedir(dir);
        return;
    }
    if (firstItr == 0)
    {
        printf("SUCCESS\n");
        success = 1;
    }

    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if (lstat(filePath, &statbuff) == 0)
            {

                if (S_ISREG(statbuff.st_mode))
                {
                    
                    printf("%s\n", filePath);
                }

                if (S_ISDIR(statbuff.st_mode))
                {
                    findall(filePath, success);
                }
            }
        }
    }
    closedir(dir);
}

void extract(char *path, int section, int line)
{

    int fd = -1;
    fd = open(path, O_RDONLY);

    int version = 0;
    int no_of_sections = 0;

    int sect_offset = 0;
    int sect_size = 0;

    int headerSize = 0;
    char Magic;

    if (fd == -1)
    {
        printf("ERROR\n");
        printf("invalid file\n");
        return;
    }

    lseek(fd, -1, SEEK_END);
    read(fd, &Magic, 1);

    lseek(fd, -3, SEEK_END);
    read(fd, &headerSize, 2);

    lseek(fd, -headerSize, SEEK_END);
    read(fd, &version, 2);

    read(fd, &no_of_sections, 1);

    // VREF DACA SECTION <= nr of sections
    if (section > no_of_sections)
    {
        printf("ERROR\n");
        printf("invalid section\n");
        close(fd);
        return;
    }

    lseek(fd, ((section - 1) * 29) + 21, SEEK_CUR);

    read(fd, &sect_offset, 4);
    read(fd, &sect_size, 4);

    lseek(fd, sect_offset, SEEK_SET);
    // teoretic sunt la sectiune

    int i = 0;
    int linei = 0;

    char buffer[sect_size + 1];
    buffer[sect_size + 1] = '\0';

    read(fd, buffer, sect_size); // pun in buffer sectiunea

    for (linei = 1; linei < line; linei++)
    {
        if (i < sect_size)
        {
            while (buffer[i] != 0x0D && buffer[i + 1] != 0x0A)
            {
                i++;
            }
            i += 2;
        }

        if (i > sect_size)
        {
            printf("ERROR\n");
            printf("wrong line\n");
            close(fd);
            return;
        }
    }
    printf("SUCCESS\n");
    int pozin = i;

    while (buffer[i] != 0x0D && buffer[i + 1] != 0x0A && i < sect_size)
    {

        i++;
    }
    buffer[i] = '\0';

    printf("%s\n", buffer + pozin);
    close(fd);
    return;
}

int extractSFverf(char *path, int section, int line)
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
        printf("invalid file\n");
        return 1;
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
        printf("invalid file\n");
        close(fd);
        return 1;
    }

    if (version < 17 || version > 71)
    {
        printf("ERROR\n");
        printf("invalid file\n");
        close(fd);
        return 1;
    }
    if (no_of_sections < 8 || no_of_sections > 11)
    {
        printf("ERROR\n");
        printf("invalid file\n");
        close(fd);
        return 1;
    }
    i = no_of_sections;

    while (i > 0)
    {
        read(fd, sect_name, 17);
        read(fd, &sect_type, 4);

        if (sect_type != 52 && sect_type != 69 && sect_type != 81 && sect_type != 42 && sect_type != 89)
        {
            printf("ERROR\n");
            printf("invalid file\n");
            close(fd);
            return 1;
        }

        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
        sect_name[0] = '\0';

        i--;
    }

    extract(path, section, line);

    close(fd);
    return 0;
}

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
        return 1;
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

            return list_verf(recursive, size_greater_value, permission, path);
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
                return 1;
            }
            parsare(path);
            return 0;
        }

        if (strncmp(argv[1], "extract", 8) == 0)
        {
            int i = 1;
            char *path = NULL;
            int sectionNumber = 0;
            int lineNumber = 0;
            char *number = NULL;

            while (i < argc)
            {
                if (strncmp(argv[i], "path=", 5) == 0)
                {
                    char *substr = "path=";
                    path = argv[i] + strlen(substr);
                }

                if (strncmp(argv[i], "section=", 8) == 0)
                {
                    char *substr = "section=";

                    number = argv[i] + strlen(substr);
                    sectionNumber = atoi(number);
                }

                if (strncmp(argv[i], "line=", 5) == 0)
                {
                    char *substr = "line=";
                    number = argv[i] + strlen(substr);
                    lineNumber = atoi(number);
                }

                i++;
            }
            return extractSFverf(path, sectionNumber, lineNumber);
        }
        if (strncmp(argv[1], "findall", 8) == 0)
        {
            char *path = NULL;
            char *substr = "path=";
            path = argv[2] + strlen(substr);
            findall(path, 0);
        }
    }
    return 0;
}
