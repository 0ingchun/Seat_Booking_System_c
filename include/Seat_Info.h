typedef struct {
    char name[64];  // 座位类型
    unsigned int id;    // 座位id
    unsigned int amount;    // 金额
    char subscriber[64];    // 预订者
    char datetime [64];    // 预订时间
} Seat_Info_t;