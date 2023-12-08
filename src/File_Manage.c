#include "File_Manage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <windows.h>

// #include "cJSON.h"

// 普通的读写

// // 读取文件内容到字符串
// char* readFileToString(const char* filename) {
//     FILE* file = fopen(filename, "rb"); // 使用 "rb" 模式打开文件
//     if (file == NULL) {
//         perror("Error opening file");
//         return NULL;
//     }

//     fseek(file, 0, SEEK_END);
//     long length = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     char* buffer = (char*)malloc(length + 1);
//     if (buffer) {
//         fread(buffer, 1, length, file);
//         buffer[length] = '\0'; // 确保以空字符结尾
//     }

//     fclose(file);
//     return buffer;
// }


// // 将字符串写入文件
// void writeStringToFile(const char* filename, const char* jsonStr) {
//     FILE* file = fopen(filename, "w"); // 或者 "wb" 用于二进制文件
//     if (file == NULL) {
//         perror("Error opening file");
//         return;
//     }

//     fputs(jsonStr, file);
//     fclose(file);
// }


// 针对JSON优化的函数

// 读取文件内容到字符串
// char* readFileToString(const char* filename) {
//     FILE* file = fopen(filename, "r");
//     if (file == NULL) {
//         perror("Error opening file");
//         return NULL;
//     }

//     fseek(file, 0, SEEK_END);
//     long length = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     char* buffer = (char*)malloc(length + 1);
//     if (buffer) {
//         size_t readLength = fread(buffer, 1, length, file);
//         if (readLength != length) {
//             perror("Error reading file");
//             free(buffer);
//             fclose(file);
//             return NULL;
//         }
//         buffer[length] = '\0';
//     }

//     fclose(file);
//     return buffer;
// }

char* readFileToString(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, file);
        buffer[length] = '\0';  // 确保字符串以 null 结尾
    } else {
        perror("Error allocating memory");
    }

    fclose(file);
    return buffer;
}


// 将字符串写入文件
void writeStringToFile(const char* filename, const char* jsonStr) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    if (fputs(jsonStr, file) == EOF) {
        perror("Error writing to file");
    }
    fclose(file);
}


// 检查文件存在并创建文件
int check_and_create_file(const char *file_name) {
    printf("Check File: '%s'\n", file_name);
    int file_exists = access(file_name, F_OK);
    if (file_exists == -1) {
        FILE *file = fopen(file_name, "w");
        if (file == NULL) {
            fprintf(stderr, "Fail to Creat File!\n");
            return -1;
        }
        fclose(file);
        printf("File: '%s' Success Create File!\n", file_name);
        return 1;
    }
    else {
        printf("File: '%s' File Already Exists\n", file_name);
        return 0;
    }
}


// 创建文件
int createFile(const char* filename) {
    // 首先尝试以读取模式打开文件，检查文件是否已存在
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file); // 文件已存在，关闭文件
        printf("File '%s' already exists.\n", filename);
        return -2; // 返回特定的错误码表示文件已存在
    }

    // 文件不存在，尝试创建文件
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error creating file");
        return -1; // 创建失败
    }

    fclose(file); // 关闭文件，创建完成
    printf("File '%s' created successfully.\n", filename);
    return 0; // 创建成功
}


// 删除文件
int deleteFile(const char* filename) {
    if (remove(filename) == 0) {
        printf("File '%s' deleted successfully.\n", filename);
        return 0; // 删除成功
    } else {
        perror("Error deleting file");
        return -1; // 删除失败
    }
}


// 重命名文件
int renameFile(const char* originalFilename, const char* newFilename) {
    if (rename(originalFilename, newFilename) == 0) {
        printf("File renamed from '%s' to '%s' successfully.\n", originalFilename, newFilename);
        return 0; // 重命名成功
    } else {
        perror("Error renaming file");
        return -1; // 重命名失败
    }
}



// 移动文件
int moveFile(const char* originalFilename, const char* newDirectory) {
    char newFilePath[256];
    sprintf(newFilePath, "%s\\%s", newDirectory, originalFilename); // Windows 路径格式
    // 对于 UNIX/Linux，使用 sprintf(newFilePath, "%s/%s", newDirectory, originalFilename);

    if (rename(originalFilename, newFilePath) == 0) {
        printf("File moved to '%s'\n", newFilePath);
        return 0; // 移动成功
    } else {
        perror("Error moving file");
        return -1; // 移动失败
    }
}



// // opfile打开文件后使用该函数获取文件句柄
// HANDLE getFileHandleFromFilePointer(FILE* file) {
//     if (file == NULL) {
//         return INVALID_HANDLE_VALUE;
//     }

//     int fileDescriptor = _fileno(file);
//     HANDLE fileHandle = (HANDLE)_get_osfhandle(fileDescriptor);

//     if (fileHandle == INVALID_HANDLE_VALUE) {
//         printf("Unable to get file handle.\n");
//     }

//     return fileHandle;
// }



// // UNIX文件锁用
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/file.h>
// #include <unistd.h>
// // 文件锁定函数
// int lockFile(FILE* file) {
//     if (flock(fileno(file), LOCK_EX) == -1) {
//         perror("Error locking file");
//         return -1;
//     }
//     return 0;
// }

// // 文件解锁函数
// int unlockFile(FILE* file) {
//     if (flock(fileno(file), LOCK_UN) == -1) {
//         perror("Error unlocking file");
//         return -1;
//     }
//     return 0;
// }


// Windows文件锁用

// 文件锁定函数
int lockFile(HANDLE file) {
    OVERLAPPED overlapped = { 0 };
    if (!LockFile(file, 0, 0, 1, 0)) {
        printf("Error locking file: %lu\n", GetLastError());
        return -1;
    }
    return 0;
}

// 文件解锁函数
int unlockFile(HANDLE file) {
    OVERLAPPED overlapped = { 0 };
    if (!UnlockFile(file, 0, 0, 1, 0)) {
        printf("Error unlocking file: %lu\n", GetLastError());
        return -1;
    }
    return 0;
}


// // 使用WindowsAPI创建文件并取得文件句柄
// HANDLE createFileAndGetHandle(const char* filename) {
//     HANDLE fileHandle = CreateFile(
//         filename,                           // 文件名
//         GENERIC_READ | GENERIC_WRITE,       // 打开文件进行读写
//         0,                                  // 不共享文件
//         NULL,                               // 默认安全属性
//         OPEN_ALWAYS,                        // 打开文件，如果文件不存在则创建
//         FILE_ATTRIBUTE_NORMAL,              // 普通文件
//         NULL);                              // 没有模板文件

//     if (fileHandle == INVALID_HANDLE_VALUE) {
//         printf("Unable to open or create file '%s'. Error: %lu\n", filename, GetLastError());
//         return NULL; // 打开或创建失败
//     }

//     // 检查文件是否是新创建的
//     if (GetLastError() == ERROR_ALREADY_EXISTS) {
//         printf("File '%s' already exists.\n", filename);
//         CloseHandle(fileHandle); // 关闭句柄
//         return NULL; // 文件重名
//     }

//     return fileHandle; // 返回有效的文件句柄
// }


// // 使用WindowsAPI打开文件并获得句柄
// HANDLE openFileAndGetHandle(const char* filename) {
//     HANDLE fileHandle = CreateFile(
//         filename,               // 文件名
//         GENERIC_READ,           // 打开文件进行读取
//         FILE_SHARE_READ,        // 允许其他进程读取文件
//         NULL,                   // 默认安全属性
//         OPEN_EXISTING,          // 打开现有文件
//         FILE_ATTRIBUTE_NORMAL,  // 普通文件
//         NULL);                  // 没有模板文件

//     if (fileHandle == INVALID_HANDLE_VALUE) {
//         printf("Unable to open file '%s'. Error: %lu\n", filename, GetLastError());
//         return NULL; // 打开失败
//     }

//     return fileHandle; // 返回有效的文件句柄
// }


// 库示例
// int main() {

//     const char* inputFilename = "input.txt";

//     // 创建文件
//     int result = createFile(inputFilename);
//     if (result == 0) {
//         printf("File successfully created.\n");
//     } else if (result == -2) {
//         printf("File already exists, no need to create.\n");
//     } else {
//         printf("Failed to create file.\n");
//     }


//     // const char* inputFilename = "input.txt";
//     const char* outputFilename = "output.txt";

//     // 读取文件内容到字符串
//     char* jsonStr = readFileToString(inputFilename);
//     if (jsonStr != NULL) {
//         printf("File content:\n%s\n", jsonStr);

//         // 将字符串内容写入另一个文件
//         writeStringToFile(outputFilename, jsonStr);

//         // 释放动态分配的内存
//         free(jsonStr);
//     }


//     const char* filename = "test.txt";
//     const char* newContent = "New content for the file.";
//     // const char* newDirectory = "C:\\path\\to\\new\\directory"; // Windows 目录格式
//     const char* newDirectory = "..\\..\\"; // Windows 目录格式

//     // 删除文件
//     deleteFile(filename);

//     const char* originalFilename = "old_filename.txt";
//     const char* newFilename = "new_filename.txt";

//     // 重命名文件
//     result = renameFile(originalFilename, newFilename);
//     if (result == 0) {
//         printf("File successfully renamed.\n");
//     } else {
//         printf("Failed to rename file.\n");
//     }

//     // 移动文件
//     moveFile(filename, newDirectory);

//     system("pause");

//     return 0;
// }
