#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"


// 读取并查看用户信息
void viewUser(const char* jsonStr) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return;
    }

    cJSON* usersArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(usersArray)) {
        printf("Users field is not an array\n");
        cJSON_Delete(jsonObj);
        return;
    }

    cJSON* userObj;
    cJSON_ArrayForEach(userObj, usersArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        cJSON* passwdObj = cJSON_GetObjectItem(userObj, "passwd");
        cJSON* authObj = cJSON_GetObjectItem(userObj, "auth");
        cJSON* balanceObj = cJSON_GetObjectItem(userObj, "balance");

        printf("Username: %s\n", cJSON_IsString(usernameObj) ? usernameObj->valuestring : "null");
        printf("Password: %s\n", cJSON_IsString(passwdObj) ? passwdObj->valuestring : "null");
        printf("Auth: %d\n", cJSON_IsNumber(authObj) ? authObj->valueint : -1);
        printf("Balance: %u\n", cJSON_IsNumber(balanceObj) ? balanceObj->valueint : 0);
    }

    cJSON_Delete(jsonObj);
}


// 查找用户并返回其他信息
int findUser(const char* jsonStr, const char* username, char* outPasswd, short* outAuth, unsigned int* outBalance) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return 0; // 表示用户未找到
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return 0; // 表示用户未找到
    }

    cJSON* userObj;
    cJSON_ArrayForEach(userObj, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        cJSON* passwdObj = cJSON_GetObjectItem(userObj, "passwd");
        cJSON* authObj = cJSON_GetObjectItem(userObj, "auth");
        cJSON* balanceObj = cJSON_GetObjectItem(userObj, "balance");

        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, username) == 0) {
            if (outPasswd != NULL && cJSON_IsString(passwdObj)) {
                strcpy(outPasswd, passwdObj->valuestring);
            }
            if (outAuth != NULL && cJSON_IsNumber(authObj)) {
                *outAuth = authObj->valueint;
            }
            if (outBalance != NULL && cJSON_IsNumber(balanceObj)) {
                *outBalance = balanceObj->valueint;
            }
            cJSON_Delete(jsonObj);
            return 1; // 表示用户找到
        }
    }

    cJSON_Delete(jsonObj);
    return 0; // 表示用户未找到
}


// 写入并修改用户信息
char* modifyUser(const char* jsonStr, const char* newUsername, const char* newPasswd, short newAuth) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    cJSON* userObj = NULL;
    cJSON_ArrayForEach(userObj, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, newUsername) == 0) {
            cJSON_ReplaceItemInObject(userObj, "passwd", cJSON_CreateString(newPasswd));
            cJSON_ReplaceItemInObject(userObj, "auth", cJSON_CreateNumber(newAuth));
            // 不修改 balance
            char* modifiedJsonStr = cJSON_Print(jsonObj);
            cJSON_Delete(jsonObj);
            return modifiedJsonStr;
        }
    }

    printf("Username not found.\n");
    cJSON_Delete(jsonObj);
    return NULL;
}


// 添加新用户
char* addUser(const char* jsonStr, const char* username, const char* passwd, short auth) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    // 检查用户名是否已存在
    cJSON* existingUser = NULL;
    cJSON_ArrayForEach(existingUser, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(existingUser, "username");
        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, username) == 0) {
            printf("Username already exists.\n");
            cJSON_Delete(jsonObj);
            return NULL;
        }
    }

    // 添加新用户
    cJSON* newUserObj = cJSON_CreateObject();
    cJSON_AddItemToObject(newUserObj, "username", cJSON_CreateString(username));
    cJSON_AddItemToObject(newUserObj, "passwd", cJSON_CreateString(passwd));
    cJSON_AddItemToObject(newUserObj, "auth", cJSON_CreateNumber(auth));
    cJSON_AddItemToObject(newUserObj, "balance", cJSON_CreateNumber(0)); // 初始余额设置为0
    cJSON_AddItemToArray(userArray, newUserObj);

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


// 删除已存在的用户
char* deleteUser(const char* jsonStr, const char* username) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    int index = 0;
    cJSON* userObj;
    cJSON_ArrayForEach(userObj, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, username) == 0) {
            cJSON_DeleteItemFromArray(userArray, index);
            break;
        }
        index++;
    }

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


// int main() {
//     // 更新 JSON 字符串，包含 balance 字段
//     const char* jsonStr = "{\"users\":[{\"username\":\"user1\",\"passwd\":\"pass1\",\"auth\":1,\"balance\":1000},{\"username\":\"user2\",\"passwd\":\"pass2\",\"auth\":0,\"balance\":500}]}";

//     printf("原始用户信息：\n");
//     viewUser(jsonStr);
//     printf("\n");

//     // 查找用户信息
//     char passwd[64];
//     short auth;
//     unsigned int balance;
//     int userFound = findUser(jsonStr, "user1", passwd, &auth, &balance);
//     printf("查找用户信息：\n");
//     if (userFound) {
//         printf("Password: %s\n", passwd);
//         printf("Auth: %d\n", auth);
//         printf("Balance: %u\n", balance);
//     } else {
//         printf("User not found.\n");
//     }
//     printf("\n");

//     // 修改用户信息
//     char* modifiedJsonStr = modifyUser(jsonStr, "user1", "newpass", 2);
//     if (modifiedJsonStr != NULL) {
//         printf("修改用户信息后：\n");
//         viewUser(modifiedJsonStr);
//         free(modifiedJsonStr);
//     }
//     printf("\n");

//     // 添加新用户
//     char* updatedJsonStr = addUser(jsonStr, "user3", "pass3", 1);
//     if (updatedJsonStr != NULL) {
//         printf("添加新用户后：\n");
//         viewUser(updatedJsonStr);
//         free(updatedJsonStr);
//     }
//     printf("\n");

//     // 删除已存在的用户
//     char* deletedJsonStr = deleteUser(jsonStr, "user2");
//     if (deletedJsonStr != NULL) {
//         printf("删除用户后：\n");
//         viewUser(deletedJsonStr);
//         free(deletedJsonStr);
//     }

//     system("pause");
//     return 0;
// }

