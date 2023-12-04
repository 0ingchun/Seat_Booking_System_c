#ifndef RESERVE_RECORD_H
#define RESERVE_RECORD_H

#include <time.h>

typedef struct {
    char logtime[64];    // 日志记录操作时间 年/月/日/ 时:分:秒:毫秒
    char operate[64];    // 操作者
    char action[64];    // 操作类型：appoint / cancel

    unsigned int id;    // 座位id
    char seat_type[64];  // 座位类型
    char subscriber[64];    // 预订者
    char period_date [64];    // 预订的时间段日期
    char period_time_start [64];    // 预订起始时间
    char period_time_end [64];    // 预订结束时间

    unsigned int amount;    // 金额
    char order_id[64];    // 订单编号

} Reserve_Record_t;


// LogEntry 结构体定义
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

// 函数原型声明
int my_strptime(const char *s, const char *format, struct tm *tm);

void generate_order_id(char* order_id);

void write_log(const char* filename, const LogEntry* entry);

void write_log_realtime(const char* filename, const LogEntry* entry);

int is_time_conflict(const LogEntry* entries, int count, const LogEntry* new_entry);

int write_log_realtime_conflict(const char* filename, const LogEntry* entry);

LogEntry* read_logs(const char* filename, int* count);

void delete_entries_by_date(const char* filename, const char* target_date);

int delete_entries_by_orderid(const char* filename, const char* target_orderid);

LogEntry* get_booked_id_slots(const char* filename, const char* seat_type, unsigned int id, int* count);

LogEntry* get_booked_time_slots(const char* filename, const char* seat_type, const char* date, int* count);

LogEntry* get_subscriber_booked_time_slots(const char* filename, const char* subscriber, const char* date, int* count);

int is_time_expired(const char* time_str);

LogEntry* get_unbooked_time_slots(const char* filename, const char* seat_type, const char* date, int* count);

LogEntry* get_booked_seats(const char* filename, const char* subscriber, int* count);

LogEntry* get_cancelled_seats(const char* filename, const char* subscriber, int* count);

LogEntry* get_valid_booked_seats(const char* filename, const char* subscriber, int* count);

LogEntry* get_valid_cancelled_seats(const char* filename, const char* subscriber, int* count);

void count_bookings_and_revenue(const char* filename, const char* date, int* totalBookings, unsigned int* totalRevenue);

LogEntry* get_log_by_order_id(const char* filename, const char* order_id, int* count);

#endif

// 我的座位预订系统日志记录功能需要记录以下这些元素
//     char logtime[64];    // 日志记录操作时间 年/月/日/时/分/秒/毫秒
//     char operate[64];    // 操作者
//     char action[64];    // 操作类型：appoint / cancel
//     unsigned int id;    // 座位id
//     char seat_type[64];  // 座位类型
//     char subscriber[64];    // 预订者
//     char period_date [64];    // 预订的时间段日期
//     char period_time_start [64];    // 预订起始时间
//     char period_time_end [64];    // 预订结束时间
//     unsigned int amount;    // 金额
//     char order_id[64];    // 订单编号

// 我想要一个读写csv格式日志的库，这个库可以访问实时时间，并且有写入日志的函数，读取日志信息的函数，删除某条日志的函数，

// 根据座位类型查找统计某天的已预订时间段和未预订的时间段（未被预订席位的结束时间段需要与当前实时时间对比后确认有效的才给予输出，防止输出过期的时间段）的函数，

// 根据预订者查找统计该预订者的已预订席位的函数（对比实时时间和座位预订结束时间，发现过期的不给予输出），

// 根据预订者查找统计该预订者的取消预订席位的函数（对比实时时间和座位预订结束时间，发现过期的不给予输出），

// 根据预订者查找统计该预订者所有的已预订席位的函数，

// 根据预订者查找统计该预订者所有的取消预订席位的函数，

// 统计某日被预订席位总数和某日总收入的函数，

// 输入订单编号查看日志信息的函数