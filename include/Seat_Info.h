#ifndef SEAT_INFO__H
#define SEAT_INFO__H

typedef struct {
    unsigned int id;    // 座位id
    char type[64];  // 座位类型
    unsigned int amount;    // 金额
    // char subscriber[64];    // 预订者
    // char datetime [64];    // 预订时间
} Seat_Info_t;


void viewSeat(const char* jsonStr);

char* updateSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount);

// void findSeatsBySubscriber(const char* jsonStr, const char* subscriber);

int countSeatsByType(const char* jsonStr, const char* type, int** seatIds);

int getSeatTypes(const char* jsonStr, char*** types);

int findAmountById(const char* jsonStr, unsigned int id);

char* addSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount);

char* deleteSeat(const char* jsonStr, unsigned int id);

#endif