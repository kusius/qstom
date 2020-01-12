#ifndef __UNIX_QSTOM_H_
#define __UNIX_QSTOM_H_

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

struct platform_file_data
{
    char* Memory;
    unsigned int FileSize;
};

internal void
PlatformFreeFileMemory(platform_file_data* File)
{
    free(File->Memory);
    *File = {};
    return;
}

internal void
PlatformReadFile(const char* FileName, platform_file_data* FileInfoOut)
{
    struct stat FileStats;
    FILE* f;
    if(stat(FileName, &FileStats) == 0)
    {
        unsigned int FileSize = FileStats.st_size;
        FileInfoOut->Memory = (char*)malloc(FileSize + 1);
        if(FileInfoOut->Memory)
        {
            f = fopen(FileName, "r");
            if(f)
            {
                size_t BytesRead = fread(FileInfoOut->Memory,
                                         sizeof(char),
                                         FileSize,
                                         f);
                if(BytesRead == FileSize)
                {
                    // NOTE: null terminate the memory
                    FileInfoOut->Memory[FileSize] = '\0';
                    FileInfoOut->FileSize = FileSize;
                    fclose(f); //NOTE: we dont check if close fails
                }
                else
                {
                    //TODO: Logging (incomplete read)
                }
            }
            else
            {
                //TODO: Logging (could not open file)
            }
        }
        else
        {
            //TODO: Logging (could not allocate memory)
        }
    }
    else
    {
        //TODO: Logging (could not stat)
    }
    return;
}

#endif // __UNIX_QSTOM_H_
