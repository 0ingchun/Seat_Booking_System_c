#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"

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
        cJSON* nameObj = cJSON_GetObjectItem(seatObj, "name");
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
        cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");
        cJSON* datetimeObj = cJSON_GetObjectItem(seatObj, "datetime");

        printf("Name: %s\n", cJSON_IsString(nameObj) ? nameObj->valuestring : "null");
        printf("ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
        printf("Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
        printf("Subscriber: %s\n", cJSON_IsString(subscriberObj) ? subscriberObj->valuestring : "null");
        printf("Datetime: %s\n", cJSON_IsString(datetimeObj) ? datetimeObj->valuestring : "null");
    }

    cJSON_Delete(jsonObj);
}


// 查找订阅者的座位
void findSeatsBySubscriber(const char* jsonStr, const char* subscriber) {
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

    int seatCount = 0;
    printf("Seats booked by %s:\n", subscriber);

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");
        if (cJSON_IsString(subscriberObj) && strcmp(subscriberObj->valuestring, subscriber) == 0) {
            cJSON* nameObj = cJSON_GetObjectItem(seatObj, "name");
            cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
            cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
            cJSON* datetimeObj = cJSON_GetObjectItem(seatObj, "datetime");

            printf("  Name: %s\n", cJSON_IsString(nameObj) ? nameObj->valuestring : "null");
            printf("  ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
            printf("  Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
            printf("  Datetime: %s\n", cJSON_IsString(datetimeObj) ? datetimeObj->valuestring : "null");
            printf("\n");

            seatCount++;
        }
    }

    printf("Total seats booked by %s: %d\n", subscriber, seatCount);

    cJSON_Delete(jsonObj);
}



// 查看座位数量
void countSeatsByName(const char* jsonStr, const char* name) {
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

    int totalSeats = 0;
    int bookedSeats = 0;
    int availableSeats = 0;

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* nameObj = cJSON_GetObjectItem(seatObj, "name");
        cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");
        cJSON* datetimeObj = cJSON_GetObjectItem(seatObj, "datetime");

        if (cJSON_IsString(nameObj) && strcmp(nameObj->valuestring, name) == 0) {
            printf("  Name: %s\n", cJSON_IsString(nameObj) ? nameObj->valuestring : "null");
            printf("  Datetime: %s\n", cJSON_IsString(datetimeObj) ? datetimeObj->valuestring : "null");
            printf("\n");

            totalSeats++;
            if (cJSON_IsString(subscriberObj) && strlen(subscriberObj->valuestring) > 0) {
                bookedSeats++;
            } else {
                availableSeats++;
            }
        }
    }

    printf("Total seats of type '%s': %d\n", name, totalSeats);
    printf("Booked seats of type '%s': %d\n", name, bookedSeats);
    printf("Available seats of type '%s': %d\n", name, availableSeats);

    cJSON_Delete(jsonObj);
}



// 添加座位
char* addSeat(const char* jsonStr, const char* name, unsigned int id, unsigned int amount, const char* subscriber, const char* datetime) {
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

    cJSON* newSeatObj = cJSON_CreateObject();
    cJSON_AddItemToObject(newSeatObj, "name", cJSON_CreateString(name));
    cJSON_AddItemToObject(newSeatObj, "id", cJSON_CreateNumber(id));
    cJSON_AddItemToObject(newSeatObj, "amount", cJSON_CreateNumber(amount));
    cJSON_AddItemToObject(newSeatObj, "subscriber", cJSON_CreateString(subscriber));
    cJSON_AddItemToObject(newSeatObj, "datetime", cJSON_CreateString(datetime));
    cJSON_AddItemToArray(seatsArray, newSeatObj);

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


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

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


// 库示例
// int main() {
//     const char* jsonStr = "{\"seats\":[{\"name\":\"VIP\",\"id\":101,\"amount\":1000,\"subscriber\":\"Alice\",\"datetime\":\"2023-04-01 09:00:00\"},{\"name\":\"VIP\",\"id\":102,\"amount\":500,\"subscriber\":\"\",\"datetime\":\"2023-04-01 10:00:00\"},{\"name\":\"Economy\",\"id\":103,\"amount\":300,\"subscriber\":\"Alice\",\"datetime\":\"2023-04-01 11:00:00\"}]}";

//     printf("原始座位信息：\n");
//     viewSeat(jsonStr);
//     printf("\n");

//     findSeatsBySubscriber(jsonStr, "Alice");
//     printf("\n");

//     countSeatsByName(jsonStr, "VIP");
//     printf("\n");

//     // 添加新座位
//     char* updatedJsonStr = addSeat(jsonStr, "Regular", 104, 500, "Bob", "2023-04-02 12:00:00");
//     if (updatedJsonStr != NULL) {
//         printf("添加新座位后：\n");
//         viewSeat(updatedJsonStr);
//         free(updatedJsonStr);
//     }
//     printf("\n");

//     // 删除座位
//     char* deletedJsonStr = deleteSeat(jsonStr, 101);
//     if (deletedJsonStr != NULL) {
//         printf("删除座位后：\n");
//         viewSeat(deletedJsonStr);
//         free(deletedJsonStr);
//     }

//     system("pause");
//     return 0;
// }