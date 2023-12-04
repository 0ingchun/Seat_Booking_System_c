#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "Seat_Info.h"


// 查看座位
void viewSeat(const char* jsonStr) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return;
    }

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
        cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
        
        printf("ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
        printf("Type: %s\n", cJSON_IsString(typeObj) ? typeObj->valuestring : "null");
        printf("Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
    }

    cJSON_Delete(jsonObj);
}


// 更新座位
char* updateSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
            // 更新座位信息
            cJSON_ReplaceItemInObject(seatObj, "type", cJSON_CreateString(type));
            cJSON_ReplaceItemInObject(seatObj, "amount", cJSON_CreateNumber(amount));
            break;
        }
    }

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}



// // 查找订阅者的座位
// void findSeatsBySubscriber(const char* jsonStr, const char* subscriber) {
//     cJSON* jsonObj = cJSON_Parse(jsonStr);
//     if (jsonObj == NULL) {
//         printf("Failed to parse JSON string\n");
//         return;
//     }

//     cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
//     if (!cJSON_IsArray(seatsArray)) {
//         printf("Seats field is not an array\n");
//         cJSON_Delete(jsonObj);
//         return;
//     }

//     int seatCount = 0;
//     printf("Seats booked by %s:\n", subscriber);

//     cJSON* seatObj;
//     cJSON_ArrayForEach(seatObj, seatsArray) {
//         cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");
//         if (cJSON_IsString(subscriberObj) && strcmp(subscriberObj->valuestring, subscriber) == 0) {
//             cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
//             cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
//             cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
//             cJSON* datetimeObj = cJSON_GetObjectItem(seatObj, "datetime");

//             printf("  Type: %s\n", cJSON_IsString(typeObj) ? typeObj->valuestring : "null");
//             printf("  ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
//             printf("  Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
//             printf("  Datetime: %s\n", cJSON_IsString(datetimeObj) ? datetimeObj->valuestring : "null");
//             printf("\n");

//             seatCount++;
//         }
//     }

//     printf("Total seats booked by %s: %d\n", subscriber, seatCount);

//     cJSON_Delete(jsonObj);
// }



// 查看座位数量-根据类型，并返回座位ID数组
int countSeatsByType(const char* jsonStr, const char* type, int** seatIds) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return 0;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return 0;
    }

    int totalSeats = 0;
    *seatIds = NULL; // 初始化为NULL

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
        if (cJSON_IsString(typeObj) && strcmp(typeObj->valuestring, type) == 0) {
            totalSeats++;
        }
    }

    if (totalSeats > 0) {
        *seatIds = malloc(totalSeats * sizeof(int)); // 分配内存
        int index = 0;

        cJSON_ArrayForEach(seatObj, seatsArray) {
            cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
            cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
            if (cJSON_IsString(typeObj) && strcmp(typeObj->valuestring, type) == 0) {
                (*seatIds)[index++] = cJSON_IsNumber(idObj) ? idObj->valueint : -1;
            }
        }
    }

    cJSON_Delete(jsonObj);
    return totalSeats; // 返回找到的座位数量
}


// 功能：从JSON中提取所有不同的座位类型
// 参数：jsonStr - JSON字符串，types - 指向字符串数组的指针，用于存储结果
// 返回：找到的不同座位类型的数量
int getSeatTypes(const char* jsonStr, char*** types) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return 0;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return 0;
    }

    int typesCount = 0;
    *types = NULL; // 初始化为NULL
    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
        if (cJSON_IsString(typeObj)) {
            // 检查此类型是否已存在于数组中
            int exists = 0;
            for (int i = 0; i < typesCount; i++) {
                if (strcmp((*types)[i], typeObj->valuestring) == 0) {
                    exists = 1;
                    break;
                }
            }

            // 如果此类型不存在于数组中，则添加它
            if (!exists) {
                *types = realloc(*types, (typesCount + 1) * sizeof(char*));
                (*types)[typesCount] = strdup(typeObj->valuestring);
                typesCount++;
            }
        }
    }

    cJSON_Delete(jsonObj);
    return typesCount; // 返回找到的不同类型的数量
}


// 输入id查找amount
int findAmountById(const char* jsonStr, unsigned int id) {
    // 解析JSON字符串
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return -1; // 返回-1表示查找失败
    }

    // 获取seats数组
    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return -1;
    }

    // 遍历数组，查找对应ID的amount
    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");

        if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
            if (cJSON_IsNumber(amountObj)) {
                int amount = amountObj->valueint;
                cJSON_Delete(jsonObj);
                return amount; // 返回找到的amount
            }
        }
    }

    cJSON_Delete(jsonObj);
    return -1; // 未找到指定ID，返回-1
}


// 添加座位
char* addSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    // 检查给定的id是否已存在
    cJSON* existingSeat = NULL;
    cJSON_ArrayForEach(existingSeat, seatsArray) {
        cJSON* idItem = cJSON_GetObjectItem(existingSeat, "id");
        if (cJSON_IsNumber(idItem) && (unsigned int)idItem->valuedouble == id) {
            cJSON_Delete(jsonObj);
            return ""; // 返回空字符串
        }
    }

    cJSON* newSeatObj = cJSON_CreateObject();
    cJSON_AddItemToObject(newSeatObj, "id", cJSON_CreateNumber(id));
    cJSON_AddItemToObject(newSeatObj, "type", cJSON_CreateString(type));
    cJSON_AddItemToObject(newSeatObj, "amount", cJSON_CreateNumber(amount));

    cJSON_AddItemToArray(seatsArray, newSeatObj);

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}
// char* addSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount) {
//     cJSON* jsonObj = cJSON_Parse(jsonStr);
//     if (jsonObj == NULL) {
//         printf("Failed to parse JSON string\n");
//         return NULL;
//     }

//     cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
//     if (!cJSON_IsArray(seatsArray)) {
//         cJSON_Delete(jsonObj);
//         return NULL;
//     }

//     cJSON* newSeatObj = cJSON_CreateObject();
//     cJSON_AddItemToObject(newSeatObj, "id", cJSON_CreateNumber(id));
//     cJSON_AddItemToObject(newSeatObj, "type", cJSON_CreateString(type));
//     cJSON_AddItemToObject(newSeatObj, "amount", cJSON_CreateNumber(amount));

//     cJSON_AddItemToArray(seatsArray, newSeatObj);

//     char* updatedJsonStr = cJSON_Print(jsonObj);
//     cJSON_Delete(jsonObj);
//     return updatedJsonStr;
// }


// 删除座位
char* deleteSeat(const char* jsonStr, unsigned int id) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    int index = 0;
    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
            cJSON_DeleteItemFromArray(seatsArray, index);
            break;
        }
        index++;
    }

    char* updatedJsonStr;
    if (index == cJSON_GetArraySize(seatsArray)) {
        // ID not found, return empty string
        updatedJsonStr = strdup("");
    } else {
        updatedJsonStr = cJSON_Print(jsonObj);
    }

    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}
// char* deleteSeat(const char* jsonStr, unsigned int id) {
//     cJSON* jsonObj = cJSON_Parse(jsonStr);
//     if (jsonObj == NULL) {
//         printf("Failed to parse JSON string\n");
//         return NULL;
//     }

//     cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
//     if (!cJSON_IsArray(seatsArray)) {
//         cJSON_Delete(jsonObj);
//         return NULL;
//     }

//     int index = 0;
//     cJSON* seatObj;
//     cJSON_ArrayForEach(seatObj, seatsArray) {
//         cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
//         if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
//             cJSON_DeleteItemFromArray(seatsArray, index);
//             break;
//         }
//         index++;
//     }

//     char* updatedJsonStr = cJSON_Print(jsonObj);
//     cJSON_Delete(jsonObj);
//     return updatedJsonStr;
// }


// // // 库示例
// int main() {
//     const char* jsonStr = "{\"seats\":[{\"type\":\"VIP\",\"id\":101,\"amount\":1000,\"subscriber\":\"Alice\",\"datetime\":\"2023-04-01 09:00:00\"},{\"type\":\"VIP\",\"id\":102,\"amount\":500,\"subscriber\":\"\",\"datetime\":\"2023-04-01 10:00:00\"},{\"type\":\"Economy\",\"id\":103,\"amount\":300,\"subscriber\":\"Alice\",\"datetime\":\"2023-04-01 11:00:00\"}]}";

//     printf("原始座位信息：\n");
//     viewSeat(jsonStr);
//     printf("\n");

//     char* updatedJsonStr = updateSeat(jsonStr, "VIP", 101, 1500);
//     if (updatedJsonStr != NULL) {
//         printf("更新后的座位信息：\n");
//         viewSeat(updatedJsonStr);
//         free(updatedJsonStr);
//     } else {
//         printf("座位更新失败。\n");
//     }


//     // findSeatsBySubscriber(jsonStr, "Alice");
//     // printf("\n");

//     // 统计类型数量
//     int* seatIds;
//     int count = countSeatsByType(jsonStr, "VIP", &seatIds);
//     printf("Found %d seats of type 'VIP'\n", count);
//     for (int i = 0; i < count; i++) {
//         printf("Seat ID: %d\n", seatIds[i]);
//     }
//     free(seatIds); // 释放内存
//     printf("\n");


//     // 获取所有座位类型
//     char** seatTypes;
//     int count2 = getSeatTypes(jsonStr, &seatTypes);
//     printf("Found %d different seat types\n", count2);
//     for (int i = 0; i < count2; i++) {
//         printf("Seat Type: %s\n", seatTypes[i]);
//         free(seatTypes[i]); // 释放每个字符串
//     }
//     free(seatTypes); // 释放字符串数组


// unsigned int idToFind = 102; // 要查找的座位ID
//     int amount = findAmountById(jsonStr, idToFind);

//     if (amount != -1) {
//         printf("Amount for seat ID %u is %d\n", idToFind, amount);
//     } else {
//         printf("Seat with ID %u not found.\n", idToFind);
//     }


//     // 添加新座位
//     char* updatedJsonStr2 = addSeat(jsonStr, "Regular", 104, 500);
//     if (updatedJsonStr2 != NULL) {
//         printf("添加新座位后：\n");
//         viewSeat(updatedJsonStr2);
//         free(updatedJsonStr2);
//     }
//     printf("\n");


//     // 删除座位
//     char* deletedJsonStr3 = deleteSeat(jsonStr, 101);
//     if (deletedJsonStr3 != NULL) {
//         printf("删除座位后：\n");
//         viewSeat(deletedJsonStr3);
//         free(deletedJsonStr3);
//     }

//     system("pause");
//     return 0;
// }