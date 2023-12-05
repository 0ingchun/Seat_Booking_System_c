# Seat_Booking_System_c

可能是全网第一个被迫用c来应付的座位预订系统小组作业

# 简介

- 调用win32API生成gui

- vscode码字，cmake编译

- 编译后的exe需要和如下3个文本数据文件放在一个目录方可运行：json格式进行用户和座位数据管理（UserInfo.json & SeatInfo.json），csv格式进行预约记录（ReserveLog.csv）。你也可以直接编辑这些文件

- 调用的四个自产自销用户库：File_Manage（文件管理），Login_User（操作用户数据），Seat_Info（操作座位信息），Reserve_Record（操作预订记录）

- 调用的外来库：win32API（Creat很多WindowsGUI），cJSON（操作json格式）

- 主要的窗口：主窗口MainWin（负责分发其他窗口服务），登录窗口LoginWin，注册窗口SingupWin，用户中心窗口UserWin，查询与预约服务窗口ServiceWin，订单查询与取消窗口OrderWin，管理员操作窗口AdminWin

# “软件版本特性”

- 预约时间判断是跑不成的

- 取消预约删除日志时会把用户json数据写入预约记录csv

# MORE

- by 

- Thanks: （GPT）4公主 & （Codeium）3.5哥哥

- *Just to commemorate the completion of my CPT109 Course Work.* **Who can say no to a piece of shit that can run?**