#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>
#include <windows.h>

#include "Reserve_Record.h"

// typedef struct {
//     char logtime[64];
//     char operate[64];
//     char action[64];
//     unsigned int id;
//     char seat_type[64];
//     char subscriber[64];
//     char period_date[64];
//     char period_time_start[64];
//     char period_time_end[64];
//     unsigned int amount;
//     char order_id[64];
// } LogEntry;



// 平替strptime函数
int my_strptime(const char *s, const char *format, struct tm *tm) {
    // 目前仅支持 "%Y/%m/%d %H:%M:%S" 这种格式
    if (strcmp(format, "%Y/%m/%d %H:%M:%S") == 0) {
        return sscanf(s, "%d/%d/%d %d:%d:%d", 
                        &tm->tm_year, &tm->tm_mon, &tm->tm_mday, 
                        &tm->tm_hour, &tm->tm_min, &tm->tm_sec) == 6;
    } else {
        // 如果需要，可以添加对其他格式的支持
        return 0; // 返回0表示格式不支持或解析失败
    }
}
/*
此自定义函数my_strptime仅支持一种日期时间格式"%Y/%m/%d %H:%M:%S"。
使用sscanf从字符串中解析年、月、日、小时、分钟和秒，然后填充到struct tm结构中。
您可以根据需要扩展此函数以支持其他格式。
在使用此函数之后，确保调整年份和月份的值，因为struct tm的tm_year字段从1900年开始计算，tm_mon字段从0开始计算。
通过替换项目中所有strptime的调用为my_strptime，您可以在Windows平台上实现类似的功能。
*/


// 生成24位订单号
void generate_order_id(char* order_id) {
    // 获取当前时间
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);

    // 随机数种子
    srand((unsigned)time(NULL));

    // 格式化时间并生成随机数
    snprintf(order_id, 24, "%04d%02d%02d%02d%02d%02d%04d",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, rand() % 10000);
}


// 写入日志
void write_log(const char* filename, const LogEntry* entry) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    // 写入CSV格式
    fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
        entry->logtime,
        entry->operate,
        entry->action,
        entry->id,
        entry->seat_type,
        entry->subscriber,
        entry->period_date,
        entry->period_time_start,
        entry->period_time_end,
        entry->amount,
        entry->order_id);
    fclose(file);
}

// 写入日志-实时时间
void write_log_realtime(const char* filename, const LogEntry* entry) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // 获取当前时间并格式化
    time_t now;
    time(&now);
    struct tm *tm_now = localtime(&now);
    char formatted_time[64];
    strftime(formatted_time, sizeof(formatted_time), "%Y/%m/%d %H:%M:%S", tm_now);

    // 写入CSV格式，使用自动填写的当前时间
    fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
        formatted_time,
        entry->operate,
        entry->action,
        entry->id,
        entry->seat_type,
        entry->subscriber,
        entry->period_date,
        entry->period_time_start,
        entry->period_time_end,
        entry->amount,
        entry->order_id);

    fclose(file);
}


// 检查时间冲突辅助函数
int is_time_conflict(const LogEntry* entries, int count, const LogEntry* new_entry) {
    for (int i = 0; i < count; i++) {
        if (entries[i].id == new_entry->id && strcmp(entries[i].action, "cancel") != 0) {
            // 检查时间冲突
            if (strcmp(entries[i].period_date, new_entry->period_date) == 0 &&
                strcmp(entries[i].period_time_end, new_entry->period_time_start) > 0 &&
                strcmp(entries[i].period_time_start, new_entry->period_time_end) < 0) {
                return 1; // 发现时间冲突
            }
        }
    }
    return 0; // 没有冲突
}


// 写入日志-实时时间-检查冲突
int write_log_realtime_conflict(const char* filename, const LogEntry* entry) {
    // 首先检查是否有时间冲突
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        printf("读取日志文件失败或文件为空。\n");
        return -1;
    }

    if (is_time_conflict(entries, count, entry)) {
        printf("时间冲突，无法预订。\n");
        free(entries);
        return -2;
    }

    // 没有冲突时，写入日志

    // 是否能开启文件
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        printf("Error opening file\n");
        free(entries);
        return -3;
    }

    // 定位到文件末尾，检查最后一个字符是否是换行符
    fseek(file, -1, SEEK_END);
    char lastChar = fgetc(file);
    if (lastChar != '\n') {
        fprintf(file, "\n"); // 如果不是换行符，先写入一个换行符
    }

    // 获取当前时间并格式化
    time_t now;
    time(&now);
    struct tm *tm_now = localtime(&now);
    char formatted_time[64];
    strftime(formatted_time, sizeof(formatted_time), "%Y/%m/%d %H:%M:%S", tm_now);

    fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
            formatted_time,
            entry->operate,
            entry->action,
            entry->id,
            entry->seat_type,
            entry->subscriber,
            entry->period_date,
            entry->period_time_start,
            entry->period_time_end,
            entry->amount,
            entry->order_id);

    fclose(file);
    free(entries);

    printf("写入记录成功\n");

    return 0;
}


// 读取日志
LogEntry* read_logs(const char* filename, int* count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        *count = 0;
        return NULL;
    }

    LogEntry* entries = NULL;
    int entryCount = 0;
    char buffer[512]; // 用于存储读取的每行数据

    while (fgets(buffer, sizeof(buffer), file)) {
        entries = realloc(entries, sizeof(LogEntry) * (entryCount + 1));
        sscanf(buffer, "%[^,],%[^,],%[^,],%u,%[^,],%[^,],%[^,],%[^,],%[^,],%u,%s",
            entries[entryCount].logtime,
            entries[entryCount].operate,
            entries[entryCount].action,
            &entries[entryCount].id,
            entries[entryCount].seat_type,
            entries[entryCount].subscriber,
            entries[entryCount].period_date,
            entries[entryCount].period_time_start,
            entries[entryCount].period_time_end,
            &entries[entryCount].amount,
            entries[entryCount].order_id);
        entryCount++;
    }

    fclose(file);
    *count = entryCount;
    return entries;
}

// 删除日志-按预订日期
void delete_entries_by_date(const char* filename, const char* target_date) {
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        printf("读取失败或文件为空\n");
        return; // 读取失败或文件为空
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(entries);
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].period_date, target_date, strlen(target_date)) != 0) {
            // 将非目标日期的记录写回文件
            fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
                entries[i].logtime,
                entries[i].operate,
                entries[i].action,
                entries[i].id,
                entries[i].seat_type,
                entries[i].subscriber,
                entries[i].period_date,
                entries[i].period_time_start,
                entries[i].period_time_end,
                entries[i].amount,
                entries[i].order_id);
        }
    }

    fclose(file);
    free(entries);
    printf("OK to Delete\n");
    return;
}


// 删除日志-按订单号
int delete_entries_by_orderid(const char* filename, const char* target_orderid) {
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        printf("读取失败或文件为空\n");
        return -1; // 读取失败或文件为空
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(entries);
        printf("Error opening file\n");
        return -2;
    }

    int match_found = 0; // Flag to check if there is a match for target_orderid

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].order_id, target_orderid, strlen(target_orderid)) != 0) {
            // 将非目标日期的记录写回文件
            fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
                entries[i].logtime,
                entries[i].operate,
                entries[i].action,
                entries[i].id,
                entries[i].seat_type,
                entries[i].subscriber,
                entries[i].period_date,
                entries[i].period_time_start,
                entries[i].period_time_end,
                entries[i].amount,
                entries[i].order_id);
        }
        else {
            match_found = 1;
        }
    }

    fclose(file);
    free(entries);
    // free(filename);

    if (!match_found) {
        printf("No match found for target_orderid\n");
        return -3;
    }
    else {
        printf("OK to Delete\n");
        return 1;
    }
}


// 根据ID和座位类型查找已被预订席位
LogEntry* get_booked_id_slots(const char* filename, const char* seat_type, unsigned int id, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && entries[i].id == id && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// 根据日期和座位类型查找已被预订席位
LogEntry* get_booked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// 根据日期和预订者查找已被预订席位
LogEntry* get_subscriber_booked_time_slots(const char* filename, const char* subscriber, const char* date, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// 辅助函数：判断时间是否已过期
int is_time_expired(const char* time_str) {
    struct tm time_info = {0};
    // strptime(time_str, "%Y/%m/%d %H:%M:%S", &time_info);
    my_strptime(time_str, "%Y/%m/%d %H:%M:%S", &time_info);
    time_t time_val = mktime(&time_info);
    time_t now;
    time(&now);

    return now > time_val; // 如果当前时间晚于时间参数，则过期
}

// 获取特定日期和座位类型的未预订时间段
LogEntry* get_unbooked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    int log_count = 0;
    LogEntry* entries = read_logs(filename, &log_count);
    LogEntry* unbookedEntries = NULL;

    // 此处省略了分析时间段的复杂逻辑
    // 假设我们有一种方法来确定哪些时间段是未被预订的
    /*????????????????????????????????????????????????????*/

    for (int i = 0; i < log_count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && strncmp(entries[i].period_date, date, 10) == 0) {
            // 检查是否过期
            if (!is_time_expired(entries[i].period_time_end)) {
                unbookedEntries = realloc(unbookedEntries, sizeof(LogEntry) * (total + 1));
                unbookedEntries[total] = entries[i];
                total++;
            }
        }
    }

    free(entries);
    *count = total;
    return unbookedEntries;
}


// 根据预订者统计预订席位-所有
LogEntry* get_booked_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    LogEntry* bookedEntries = NULL;

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// 根据预订者统计已取消席位-所有
LogEntry* get_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    LogEntry* cancelledEntries = NULL;

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "cancel") == 0) {
            cancelledEntries = realloc(cancelledEntries, sizeof(LogEntry) * (total + 1));
            cancelledEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return cancelledEntries;
}


// 根据预订者统计有效预订席位-未过期
LogEntry* get_valid_booked_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // 获取当前时间

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validBookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        // strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);
        my_strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

        time_t end_time_t = mktime(&end_time);
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "appoint") == 0 && difftime(end_time_t, now) > 0) {
            validBookedEntries = realloc(validBookedEntries, sizeof(LogEntry) * (total + 1));
            validBookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return validBookedEntries;
}


// 根据预订者统计有效取消席位-未过期
LogEntry* get_valid_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // 获取当前时间

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validCancelledEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        // strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);
        my_strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

        time_t end_time_t = mktime(&end_time);
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "cancel") == 0 && difftime(end_time_t, now) > 0) {
            validCancelledEntries = realloc(validCancelledEntries, sizeof(LogEntry) * (total + 1));
            validCancelledEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return validCancelledEntries;
}


// 统计预订和收入
void count_bookings_and_revenue(const char* filename, const char* date, int* totalBookings, unsigned int* totalRevenue) {
    *totalBookings = 0;
    *totalRevenue = 0;
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);

    if (entries == NULL) {
        return; // 文件读取失败或文件为空
    }

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            (*totalBookings)++;
            *totalRevenue += entries[i].amount;
        }
    }

    free(entries);
}


// 根据订单号获取日志信息
LogEntry* get_log_by_order_id(const char* filename, const char* order_id, int* count) {
    *count = 0;
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* foundEntry = NULL;

    if (entries == NULL) {
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].order_id, order_id) == 0) {
            foundEntry = malloc(sizeof(LogEntry));
            *foundEntry = entries[i];
            total = 1;
            break;
        }
    }

    free(entries);
    *count = total;
    return foundEntry;
}


// // 库示例
// int main() {

//     LogEntry entry;
//     // 填充日志条目数据
//     strcpy(entry.logtime, "2023/12/01 10:00:00");
//     strcpy(entry.operate, "系统");
//     strcpy(entry.action, "appoint");
//     entry.id = 1;
//     strcpy(entry.seat_type, "普通座");
//     strcpy(entry.subscriber, "张三");
//     strcpy(entry.period_date, "2023/12/04");
//     strcpy(entry.period_time_start, "2023/12/01 10:00:00");
//     strcpy(entry.period_time_end, "2023/12/01 12:00:00");
//     entry.amount = 100;
//     strcpy(entry.order_id, "order127");
//     // 写入日志
//     write_log_realtime("log.csv", &entry);


//     // 读取所有日志
//     int count0;
//     LogEntry* entries = read_logs("log.csv", &count0);
//     if (entries != NULL) {
//         for (int i = 0; i < count0; i++) {
//             printf("Log Entry %d: %s, %s, %s, %u, %s, %s, %s, %s, %s, %u, %s\n",
//                     i + 1,
//                     entries[i].logtime,
//                     entries[i].operate,
//                     entries[i].action,
//                     entries[i].id,
//                     entries[i].seat_type,
//                     entries[i].subscriber,
//                     entries[i].period_date,
//                     entries[i].period_time_start,
//                     entries[i].period_time_end,
//                     entries[i].amount,
//                     entries[i].order_id);
//         }
//         free(entries); // 释放内存
//     } else {
//         printf("无法读取日志或日志文件为空。\n");
//     }


//     // 删除指定日期的所有日志条目
//     delete_entries_by_date("log.csv", "2023/12/04");


//     const char *datetime_str = "2023/12/1 12:30:45";
//     struct tm time_info = {0};

//     if (my_strptime(datetime_str, "%Y/%m/%d %H:%M:%S", &time_info)) {
//         time_info.tm_year -= 1900; // 年份从1900年开始
//         time_info.tm_mon -= 1;     // 月份从0开始

//         time_t time_val = mktime(&time_info); // 转换为time_t
//         // 使用 time_val ...
//     } else {
//         printf("日期时间解析失败。\n");
//     }


//     int countBooked, countCancelled;
//     LogEntry* bookedSeats;
//     LogEntry* cancelledSeats;

//     // 获取所有 "张三" 预订的席位
//     bookedSeats = get_booked_seats("log.csv", "ZhangSan", &countBooked);
//     if (bookedSeats != NULL) {
//         printf("张三已预订的席位数量: %d\n", countBooked);
//         for (int i = 0; i < countBooked; i++) {
//             printf("席位ID: %u, 预订日期: %s\n", bookedSeats[i].id, bookedSeats[i].period_date);
//         }
//         free(bookedSeats); // 释放内存
//     } else {
//         printf("没有找到已预订的席位或读取日志文件失败。\n");
//     }

//     // 获取所有 "张三" 取消预订的席位
//     cancelledSeats = get_cancelled_seats("log.csv", "ZhangSan", &countCancelled);
//     if (cancelledSeats != NULL) {
//         printf("张三已取消预订的席位数量: %d\n", countCancelled);
//         for (int i = 0; i < countCancelled; i++) {
//             printf("席位ID: %u, 取消日期: %s\n", cancelledSeats[i].id, cancelledSeats[i].period_date);
//         }
//         free(cancelledSeats); // 释放内存
//     } else {
//         printf("没有找到已取消预订的席位或读取日志文件失败。\n");
//     }


//     int countValidBooked, countValidCancelled;
//     LogEntry* validBookedSeats;
//     LogEntry* validCancelledSeats;

//     // 获取所有 "张三" 未过期的已预订的席位
//     validBookedSeats = get_valid_booked_seats("log.csv", "ZhangSan", &countValidBooked);
//     if (validBookedSeats != NULL) {
//         printf("张三未过期的已预订席位数量: %d\n", countValidBooked);
//         for (int i = 0; i < countValidBooked; i++) {
//             printf("席位ID: %u, 预订日期: %s, 预订结束时间: %s\n", validBookedSeats[i].id, validBookedSeats[i].period_date, validBookedSeats[i].period_time_end);
//         }
//         free(validBookedSeats); // 释放内存
//     } else {
//         printf("没有找到未过期的已预订席位或读取日志文件失败。\n");
//     }

//     // 获取所有 "张三" 未过期的已取消预订的席位
//     validCancelledSeats = get_valid_cancelled_seats("log.csv", "ZhangSan", &countValidCancelled);
//     if (validCancelledSeats != NULL) {
//         printf("张三未过期的已取消预订席位数量: %d\n", countValidCancelled);
//         for (int i = 0; i < countValidCancelled; i++) {
//             printf("席位ID: %u, 取消日期: %s, 预订结束时间: %s\n", validCancelledSeats[i].id, validCancelledSeats[i].period_date, validCancelledSeats[i].period_time_end);
//         }
//         free(validCancelledSeats); // 释放内存
//     } else {
//         printf("没有找到未过期的已取消预订席位或读取日志文件失败。\n");
//     }


//     int count1;
//     LogEntry* bookedSlots = get_booked_time_slots("log.csv", "Normal", "2023/12/1", &count1);
//     // ... 输出或处理 bookedSlots 中的数据
//     if (bookedSlots) {
//         for (int i = 0; i < count1; i++) {
//             printf("已预订时间段：%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
//         }
//     }
//     free(bookedSlots); // 释放内存


//     int count2;
//     LogEntry* unbookedSlots = get_unbooked_time_slots("log.csv", "Normal", "2023/12/1", &count2);

//     if (unbookedSlots) {
//         for (int i = 0; i < count2; i++) {
//             printf("未预订时间段：%s - %s\n", unbookedSlots[i].period_time_start, unbookedSlots[i].period_time_end);
//         }
//         free(unbookedSlots);
//     } else {
//         printf("没有找到未预订的时间段。\n");
//     }


//     int totalBookings;
//     unsigned int totalRevenue;

//     count_bookings_and_revenue("log.csv", "2023/12/1", &totalBookings, &totalRevenue);
//     printf("2023年12月1日的预订席位总数: %d, 总收入: %u\n", totalBookings, totalRevenue);


//     int count;
//     LogEntry* logEntry = get_log_by_order_id("log.csv", "order123", &count);

//     if (logEntry != NULL && count > 0) {
//         printf("订单编号: %s, 预订者: %s, 预订金额: %u\n", logEntry->order_id, logEntry->subscriber, logEntry->amount);
//         free(logEntry);
//     } else {
//         printf("没有找到对应订单编号的日志记录。\n");
//     }


//     system("pause");
//     return 0;
// }