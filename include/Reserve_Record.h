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