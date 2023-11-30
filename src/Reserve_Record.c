#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "main.h"

typedef struct {
    char logtime[64];
    char operate[64];
    char action[64];
    unsigned int id;
    char seat_type[64];
    char subscriber[64];
    char period_date[64];
    char period_time_start[64];
    char period_time_end[64];
    unsigned int amount;
    char order_id[64];
} LogEntry;


// д����־
void write_log(const char* filename, const LogEntry* entry) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    // д��CSV��ʽ
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

// ��ȡ��־
LogEntry* read_logs(const char* filename, int* count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        *count = 0;
        return NULL;
    }

    LogEntry* entries = NULL;
    int entryCount = 0;
    char buffer[512]; // ���ڴ洢��ȡ��ÿ������

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

// ɾ����־
void delete_log(const char* filename, unsigned int id) {
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        return; // ��ȡʧ�ܻ��ļ�Ϊ��
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(entries);
        return;
    }

    for (int i = 0; i < count; i++) {
        if (entries[i].id != id) {
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
}


// �������ں���λ���Ͳ����ѱ�Ԥ��ϯλ
LogEntry* get_booked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
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


// �����������ж�ʱ���Ƿ��ѹ���
int is_time_expired(const char* time_str) {
    struct tm time_info = {0};
    strptime(time_str, "%Y/%m/%d %H:%M:%S", &time_info);
    time_t time_val = mktime(&time_info);
    time_t now;
    time(&now);

    return now > time_val; // �����ǰʱ������ʱ������������
}

// ��ȡ�ض����ں���λ���͵�δԤ��ʱ���
LogEntry* get_unbooked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    int log_count = 0;
    LogEntry* entries = read_logs(filename, &log_count);
    LogEntry* unbookedEntries = NULL;

    // �˴�ʡ���˷���ʱ��εĸ����߼�
    // ����������һ�ַ�����ȷ����Щʱ�����δ��Ԥ����
    /*????????????????????????????????????????????????????*/

    for (int i = 0; i < log_count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && strncmp(entries[i].period_date, date, 10) == 0) {
            // ����Ƿ����
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


// ����Ԥ����ͳ��Ԥ��ϯλ-����
LogEntry* get_booked_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
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


// ����Ԥ����ͳ����ȡ��ϯλ-����
LogEntry* get_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
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


// ����Ԥ����ͳ����ЧԤ��ϯλ-δ����
LogEntry* get_valid_booked_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // ��ȡ��ǰʱ��

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validBookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

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


// ����Ԥ����ͳ����Чȡ��ϯλ-δ����
LogEntry* get_valid_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // ��ȡ��ǰʱ��

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validCancelledEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

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


// ͳ��Ԥ��������
void count_bookings_and_revenue(const char* filename, const char* date, int* totalBookings, unsigned int* totalRevenue) {
    *totalBookings = 0;
    *totalRevenue = 0;
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);

    if (entries == NULL) {
        return; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            (*totalBookings)++;
            *totalRevenue += entries[i].amount;
        }
    }

    free(entries);
}


// ���ݶ����Ż�ȡ��־��Ϣ
LogEntry* get_log_by_order_id(const char* filename, const char* order_id, int* count) {
    *count = 0;
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* foundEntry = NULL;

    if (entries == NULL) {
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
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


// ��ʾ��
// int main() {


//     int countBooked, countCancelled;
//     LogEntry* bookedSeats;
//     LogEntry* cancelledSeats;

//     // ��ȡ���� "����" Ԥ����ϯλ
//     bookedSeats = get_booked_seats("log.csv", "����", &countBooked);
//     if (bookedSeats != NULL) {
//         printf("������Ԥ����ϯλ����: %d\n", countBooked);
//         for (int i = 0; i < countBooked; i++) {
//             printf("ϯλID: %u, Ԥ������: %s\n", bookedSeats[i].id, bookedSeats[i].period_date);
//         }
//         free(bookedSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ���Ԥ����ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }

//     // ��ȡ���� "����" ȡ��Ԥ����ϯλ
//     cancelledSeats = get_cancelled_seats("log.csv", "����", &countCancelled);
//     if (cancelledSeats != NULL) {
//         printf("������ȡ��Ԥ����ϯλ����: %d\n", countCancelled);
//         for (int i = 0; i < countCancelled; i++) {
//             printf("ϯλID: %u, ȡ������: %s\n", cancelledSeats[i].id, cancelledSeats[i].period_date);
//         }
//         free(cancelledSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ���ȡ��Ԥ����ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }


//     int countValidBooked, countValidCancelled;
//     LogEntry* validBookedSeats;
//     LogEntry* validCancelledSeats;

//     // ��ȡ���� "����" δ���ڵ���Ԥ����ϯλ
//     validBookedSeats = get_valid_booked_seats("log.csv", "����", &countValidBooked);
//     if (validBookedSeats != NULL) {
//         printf("����δ���ڵ���Ԥ��ϯλ����: %d\n", countValidBooked);
//         for (int i = 0; i < countValidBooked; i++) {
//             printf("ϯλID: %u, Ԥ������: %s, Ԥ������ʱ��: %s\n", validBookedSeats[i].id, validBookedSeats[i].period_date, validBookedSeats[i].period_time_end);
//         }
//         free(validBookedSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ�δ���ڵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }

//     // ��ȡ���� "����" δ���ڵ���ȡ��Ԥ����ϯλ
//     validCancelledSeats = get_valid_cancelled_seats("log.csv", "����", &countValidCancelled);
//     if (validCancelledSeats != NULL) {
//         printf("����δ���ڵ���ȡ��Ԥ��ϯλ����: %d\n", countValidCancelled);
//         for (int i = 0; i < countValidCancelled; i++) {
//             printf("ϯλID: %u, ȡ������: %s, Ԥ������ʱ��: %s\n", validCancelledSeats[i].id, validCancelledSeats[i].period_date, validCancelledSeats[i].period_time_end);
//         }
//         free(validCancelledSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ�δ���ڵ���ȡ��Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }


//     int count1;
//     LogEntry* bookedSlots = get_booked_time_slots("log.csv", "��ͨ��", "2023/11/30", &count1);

//     // ... ������� bookedSlots �е�����
//     if (bookedSlots) {
//         for (int i = 0; i < count1; i++) {
//             printf("��Ԥ��ʱ��Σ�%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
//         }
//     free(bookedSlots); // �ͷ��ڴ�

//     int count2;
//     LogEntry* unbookedSlots = get_unbooked_time_slots("log.csv", "��ͨ��", "2023/11/30", &count2);

//     if (unbookedSlots) {
//         for (int i = 0; i < count2; i++) {
//             printf("δԤ��ʱ��Σ�%s - %s\n", unbookedSlots[i].period_time_start, unbookedSlots[i].period_time_end);
//         }
//         free(unbookedSlots);
//     } else {
//         printf("û���ҵ�δԤ����ʱ��Ρ�\n");
//     }


//     int totalBookings;
//     unsigned int totalRevenue;

//     count_bookings_and_revenue("log.csv", "2023/11/30", &totalBookings, &totalRevenue);
//     printf("2023��11��30�յ�Ԥ��ϯλ����: %d, ������: %u\n", totalBookings, totalRevenue);


//     int count;
//     LogEntry* logEntry = get_log_by_order_id("log.csv", "�������", &count);

//     if (logEntry != NULL && count > 0) {
//         printf("�������: %s, Ԥ����: %s, Ԥ�����: %u\n", logEntry->order_id, logEntry->subscriber, logEntry->amount);
//         free(logEntry);
//     } else {
//         printf("û���ҵ���Ӧ������ŵ���־��¼��\n");
//     }


//     system("pause");
//     return 0;
// }