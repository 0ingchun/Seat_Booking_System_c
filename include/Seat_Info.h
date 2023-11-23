typedef struct {
    char name[64];  // 座位类型
    unsigned int id;    // 座位id
    unsigned int amount;    // 金额
    char subscriber[64];    // 预订者
    char datetime [64];    // 预订时间
} Seat_Info_t;


void viewSeat(const char* jsonStr);

char* updateSeat(const char* jsonStr, const char* name, unsigned int id, unsigned int amount, const char* subscriber, const char* datetime);

void findSeatsBySubscriber(const char* jsonStr, const char* subscriber);

void countSeatsByName(const char* jsonStr, const char* name);

char* addSeat(const char* jsonStr, const char* name, unsigned int id, unsigned int amount, const char* subscriber, const char* datetime);

char* deleteSeat(const char* jsonStr, unsigned int id);

