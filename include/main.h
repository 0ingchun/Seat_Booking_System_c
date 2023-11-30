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
