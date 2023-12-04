#include <windows.h>  // 引入 Windows API 的头文件
#include <tchar.h>    // 引入 Unicode 和 ANSI 兼容的函数宏
#include <stdio.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "User_Lib.h"
#include "File_Manage.h"
#include "Login_User.h"
#include "Seat_Info.h"
#include "Reserve_Record.h"

#include "cJSON.h"
#include "main.h"


#define USER_INFO_DATABASE "UserInfo.json"
#define SEAT_INFO_DATABASE "SeatInfo.json"
#define RESERVE_RECORD_LOG "ReserveLog.csv"

// 主要服务全局变量
Login_User_t Login_User;

boolean Login_Flag = 0;
short Auth_Flag = 0;

// 字符串转换
void ConvertTCharToChar(const TCHAR* tcharStr, char* charStr, int charStrSize) {
    if (tcharStr == NULL || charStr == NULL) return;

#ifdef UNICODE
    // 如果使用 Unicode 字符集
    WideCharToMultiByte(CP_UTF8, 0, tcharStr, -1, charStr, charStrSize, NULL, NULL);
#else
    // 如果使用多字节字符集
    strncpy(charStr, tcharStr, charStrSize);
#endif
}

// 字符串转换
void ConvertCharToTChar(const char* charStr, TCHAR* tcharStr, int tcharStrSize) {
    if (charStr == NULL || tcharStr == NULL) {
        return;
    }

#if defined(UNICODE) || defined(_UNICODE)
    // Unicode 环境下，将 char 转换为 wchar_t
    size_t length = mbstowcs(NULL, charStr, 0); // 获取所需的 wchar_t 长度
    if (length == (size_t)(-1) || length >= tcharStrSize) {
        return;
    }
    mbstowcs(tcharStr, charStr, tcharStrSize);
#else
    // 非 Unicode 环境下，char 和 TCHAR 是相同的
    strncpy(tcharStr, charStr, tcharStrSize);
    tcharStr[tcharStrSize - 1] = '\0'; // 确保字符串以 null 结尾
#endif
}

// int main() {
//     TCHAR szText_username[100]; // 假设这是您获取到的 TCHAR 类型字符串
//     // ... 您的代码，填充 szText_username ...

//     const int bufferSize = 100;
//     char username[bufferSize];

//     ConvertTCharToChar(szText_username, username, bufferSize);

//     // 现在 username 变量包含了转换后的字符
// }

// 刷新窗口，但没啥用，不能重新生成控件
void RefreshWindow(HWND hwnd) {
    InvalidateRect(hwnd, NULL, TRUE);  // 将整个窗口标记为无效区域
    UpdateWindow(hwnd);                // 立即重绘窗口
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);   // 重绘窗口
}

// User_Login_t User_Login;

// 全局变量
HINSTANCE hInst;      // 应用程序实例句柄

HWND hwndEdit, hwndEdit2, hwndButton, hwndLabel, hwndLabel2, hwndOpenNewWindowButton;  // 分别为编辑框、按钮和标签的窗口句柄

// 声明主窗口过程函数
LRESULT CALLBACK Main_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明第二个窗口的窗口过程函数
LRESULT CALLBACK SecondWndProc(HWND, UINT, WPARAM, LPARAM);

// 声明登录窗口的窗口过程函数
LRESULT CALLBACK Login_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明注册窗口的窗口过程函数
LRESULT CALLBACK Signup_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明用户信息窗口的窗口过程函数
LRESULT CALLBACK User_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明服务窗口的窗口过程函数
LRESULT CALLBACK Service_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明订单窗口的窗口过程函数
LRESULT CALLBACK Order_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明管理窗口的窗口过程函数
LRESULT CALLBACK Admin_WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd_Main_Window, hwnd_Login_Window, hwnd_Signup_Window, hwnd_User_Window, hwnd_Service_Window, hwnd_Order_Window, hwnd_Admin_Window;         // 主窗口句柄

// WinMain：应用程序入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {    

    hInst = hInstance; // 设置全局变量 hInst

    // 注册第二个窗口类
    WNDCLASS wcSecond = {0};
    wcSecond.lpfnWndProc = SecondWndProc;
    wcSecond.hInstance = hInstance;
    wcSecond.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcSecond.lpszClassName = _T("SecondWindowClass");
    if (!RegisterClass(&wcSecond)) {
        MessageBox(NULL, _T("第二个窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户登录窗口类
    WNDCLASS wc_Login = {0};
    wc_Login.lpfnWndProc = Login_WndProc;
    wc_Login.hInstance = hInstance;
    wc_Login.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Login.lpszClassName = _T("LoginWindowClass");
    if (!RegisterClass(&wc_Login)) {
        MessageBox(NULL, _T("用户登录窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户注册窗口类
    WNDCLASS wc_Signup = {0};
    wc_Signup.lpfnWndProc = Signup_WndProc;
    wc_Signup.hInstance = hInstance;
    wc_Signup.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Signup.lpszClassName = _T("SignupWindowClass");
    if (!RegisterClass(&wc_Signup)) {
        MessageBox(NULL, _T("用户注册窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户信息窗口类
    WNDCLASS wc_User = {0};
    wc_User.lpfnWndProc = User_WndProc;
    wc_User.hInstance = hInstance;
    wc_User.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_User.lpszClassName = _T("UserWindowClass");
    if (!RegisterClass(&wc_User)) {
        MessageBox(NULL, _T("用户信息窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户服务窗口类
    WNDCLASS wc_Service = {0};
    wc_Service.lpfnWndProc = Service_WndProc;
    wc_Service.hInstance = hInstance;
    wc_Service.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Service.lpszClassName = _T("ServiceWindowClass");
    if (!RegisterClass(&wc_Service)) {
        MessageBox(NULL, _T("用户服务窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户订单窗口类
    WNDCLASS wc_Order = {0};
    wc_Order.lpfnWndProc = Order_WndProc;
    wc_Order.hInstance = hInstance;
    wc_Order.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Order.lpszClassName = _T("OrderWindowClass");
    if (!RegisterClass(&wc_Order)) {
        MessageBox(NULL, _T("用户订单窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册管理员窗口类
    WNDCLASS wc_Admin = {0};
    wc_Admin.lpfnWndProc = Admin_WndProc;
    wc_Admin.hInstance = hInstance;
    wc_Admin.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Admin.lpszClassName = _T("AdminWindowClass");
    if (!RegisterClass(&wc_Admin)) {
        MessageBox(NULL, _T("管理员窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 窗口类的设置
    WNDCLASS wc;       // 窗口类结构
    MSG Msg;           // 消息结构
    wc.style = 0;                          // 窗口样式
    wc.lpfnWndProc = Main_WndProc;              // 指定窗口过程函数
    wc.cbClsExtra = 0;                     // 窗口类额外内存
    wc.cbWndExtra = 0;                     // 窗口额外内存
    wc.hInstance = hInstance;              // 应用程序实例句柄
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // 加载图标
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);    // 加载光标
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // 背景刷
    wc.lpszMenuName = NULL;                // 菜单名称
    wc.lpszClassName = _T("MainWindowClass"); // 窗口类名称
    // 注册窗口类
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("主窗口类注册失败！"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 创建主窗口
    hwnd_Main_Window = CreateWindow(
        _T("MainWindowClass"),               // 窗口类名称
        _T("Makerspace Seat Booking System"),                // 窗口标题
        WS_OVERLAPPEDWINDOW,               // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,      // 窗口位置
        210, 120,                          // 窗口大小
        NULL, NULL, hInstance, NULL);      // 其他参数

    ShowWindow(hwnd_Main_Window, nCmdShow);           // 显示窗口
    UpdateWindow(hwnd_Main_Window);                   // 更新窗口

    // 消息循环
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);           // 翻译消息
        DispatchMessage(&Msg);            // 分发消息
    }
    return (int)Msg.wParam;
}


HWND hwndButton_Login_Window, hwndButton_Service_Window, hwndButton_Order_Window, hwndButton_Admin_Window;
#define ID_BUTTON_MAIN_LOGIN 1
#define ID_BUTTON_MAIN_SERVICE 2
#define ID_BUTTON_MAIN_ORDER 3
#define ID_BUTTON_MAIN_ADMIN 4
// 主窗口过程函数
LRESULT CALLBACK Main_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: // 窗口创建消息
            printf("Main Window WM_CREATE\n");

            //         printf("%d\n", Login_Flag);
            // if (Login_Flag) // 已登录
            // {
            //     printf("已登录\n");

            //     printf("%d\n", Auth_Flag);
            //     if (!Auth_Flag) // 普通用户
            //     {
            //         printf("普通用户\n");
            //     }
            //     else if (Auth_Flag)    // 管理员
            //     {
            //         printf("管理员\n");
            //     }
            // }
            // else if (!Login_Flag)   // 未登录
            // {

            //     printf("未登录\n");
                
            //     ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

            //     // 创建并显示第二个窗口
            //     hwnd_Login_Window = CreateWindow(
            //         _T("LoginWindowClass"),
            //         _T("Login System"),
            //         WS_OVERLAPPEDWINDOW,
            //         CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
            //         NULL, NULL, hInst, NULL);
            //     ShowWindow(hwnd_Login_Window, SW_SHOW);
            //     UpdateWindow(hwnd_Login_Window);

            // }

            // 创建按钮
            hwndButton_Login_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("LOGIN"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                10, 10, 75, 20,           // 位置和大小
                hwnd, (HMENU)ID_BUTTON_MAIN_LOGIN, hInst, NULL); // 父窗口和其他参数

            hwndButton_Service_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("SERVICE"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                100, 10, 75, 20,           // 位置和大小
                hwnd, (HMENU)ID_BUTTON_MAIN_SERVICE, hInst, NULL); // 父窗口和其他参数

            hwndButton_Order_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("ORDER"),                    // 默认文本为空
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                10, 40, 75, 20,           // 位置和大小
                hwnd, (HMENU)ID_BUTTON_MAIN_ORDER, hInst, NULL); // 父窗口和其他参数

            hwndButton_Admin_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("ADMIN"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                100, 40, 75, 20,           // 位置和大小
                hwnd, (HMENU)ID_BUTTON_MAIN_ADMIN, hInst, NULL); // 父窗口和其他参数

            break;

        // case WM_PAINT: // 重绘消息
        // // printf("WM_PAINT\n");

        // if (Login_Flag){
        //     SetWindowText(hwndButton, Login_User.username);     // 设置静态文本显示
        // }
        // else if (!Login_Flag){
        //     SetWindowText(hwndButton, _T("LOGIN"));     // 设置静态文本显示
        // }
            
        // break;

        case WM_ACTIVATE: // 激活消息
        // printf("WM_ACTIVATE\n");

        if (Login_Flag){
            SetWindowText(hwndButton_Login_Window, Login_User.username);     // 设置静态文本显示
        }
        else if (!Login_Flag){
            SetWindowText(hwndButton_Login_Window, _T("LOGIN"));     // 设置静态文本显示
        }

        break;

        case WM_SHOWWINDOW: // 隐藏消息

        break;

        case WM_COMMAND: // 命令消息

            

            if (LOWORD(wParam) == ID_BUTTON_MAIN_LOGIN) { // 判断是否点击了打开登录窗口的按钮
                printf("Press Login Button: %d\n", Login_Flag);
                if (Login_Flag == 0) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示第二个窗口
                    hwnd_Login_Window = CreateWindow(
                        _T("LoginWindowClass"),
                        _T("Login System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 210, 170,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Login_Window, SW_SHOW);
                    UpdateWindow(hwnd_Login_Window);
                }
                else if (Login_Flag == 1) { // 如果用户已登录
                    // 打开用户信息与余额充值
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示第二个窗口
                    hwnd_User_Window = CreateWindow(
                        _T("UserWindowClass"),
                        _T("User Info"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 210, 170,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_User_Window, SW_SHOW);
                    UpdateWindow(hwnd_User_Window);


                    // MessageBox(NULL, "Already Login", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == ID_BUTTON_MAIN_SERVICE) { // 判断是否点击了打开服务窗口的按钮
                printf("Press Service Button: %d\n", Login_Flag);
                if (Login_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示服务窗口
                    hwnd_Service_Window = CreateWindow(
                        _T("ServiceWindowClass"),
                        _T("Service System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 650, 600,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Service_Window, SW_SHOW);
                    UpdateWindow(hwnd_Service_Window);
                }
                else {
                    MessageBox(NULL, "Please Login First", "SERVICE Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == ID_BUTTON_MAIN_ORDER) { // 判断是否点击了打开订单窗口的按钮
                printf("Press Service Button: %d\n", Login_Flag);
                if (Login_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示订单窗口
                    hwnd_Order_Window = CreateWindow(
                        _T("OrderWindowClass"),
                        _T("Order System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 1150, 500,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Order_Window, SW_SHOW);
                    UpdateWindow(hwnd_Order_Window);
                }
                else {
                    MessageBox(NULL, "Please Login First", "ORDER Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == ID_BUTTON_MAIN_ADMIN) { // 判断是否点击了打开管理窗口的按钮
                printf("Press Admin Button: %d\n", Login_Flag);
                if (Auth_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示管理窗口
                    hwnd_Admin_Window = CreateWindow(
                        _T("AdminWindowClass"),
                        _T("Admin System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Admin_Window, SW_SHOW);
                    UpdateWindow(hwnd_Admin_Window);
                }
                else {
                    MessageBox(NULL, "Not an Admin", "ADMIN Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            break;

        case WM_CLOSE: // 窗口关闭消息
            DestroyWindow(hwnd);          // 销毁窗口
            // ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口
            break;

        case WM_DESTROY: // 窗口销毁消息
            PostQuitMessage(0);           // 发送退出消息
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam); // 默认处理
    }
    return 0;
}

// 第二个窗口的窗口过程函数
LRESULT CALLBACK SecondWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

            case WM_CREATE: // 窗口创建消息
                printf("Second Window WM_CREATE\n");

            // 创建编辑框
            hwndEdit = CreateWindow(
                _T("EDIT"),                // 控件类型：编辑框
                _T(""),                    // 默认文本为空
                WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
                10, 10, 100, 20,           // 位置和大小
                hwnd, (HMENU)1, hInst, NULL); // 父窗口和其他参数

            // 创建编辑框
            hwndEdit2 = CreateWindow(
                _T("EDIT"),                // 控件类型：编辑框
                _T(""),                    // 默认文本为空
                WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
                10, 80, 100, 20,           // 位置和大小
                hwnd, (HMENU)4, hInst, NULL); // 父窗口和其他参数

            // 创建按钮
            hwndButton = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("COPY"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                120, 10, 50, 20,           // 位置和大小
                hwnd, (HMENU)2, hInst, NULL); // 父窗口和其他参数

            // 创建静态文本
            hwndLabel = CreateWindow(
                _T("STATIC"),              // 控件类型：静态文本
                _T("7"),                    // 默认文本为空
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                10, 40, 100, 20,           // 位置和大小
                hwnd, (HMENU)3, hInst, NULL); // 父窗口和其他参数

            // 创建静态文本
            hwndLabel2 = CreateWindow(
                _T("STATIC"),              // 控件类型：静态文本
                _T("666"),                    // 默认文本为空
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                10, 100, 200, 20,           // 位置和大小
                hwnd, (HMENU)3, hInst, NULL); // 父窗口和其他参数

            hwndOpenNewWindowButton = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("打开新窗口"),          // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                120, 80, 100, 30,          // 位置和大小
                hwnd, (HMENU)5, hInst, NULL); // 父窗口和其他参数

            break;

        case WM_COMMAND: // 命令消息
        
            if (LOWORD(wParam) == 4) {    // 检查是哪个控件发出的消息
                TCHAR szText[100];        // 文本缓冲区
                GetWindowText(hwndEdit, szText, 100); // 获取编辑框文本
                GetWindowText(hwndEdit2, szText, 100); // 获取编辑框文本
                SetWindowText(hwndLabel, szText);     // 设置静态文本显示
                SetWindowText(hwndLabel2, szText);     // 设置静态文本显示
            }

            if (LOWORD(wParam) == 5) { // 判断是否点击了打开新窗口的按钮
                // 创建并显示第二个窗口
                HWND hwndSecond = CreateWindow(
                    _T("SecondWindowClass"), _T("第二个窗口"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwndSecond, SW_SHOW);
            }

            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND hwndLabel_Login_Username, hwndLabel_Login_Password, hwndEdit_Login_Username, hwndEdit_Login_Password, hwndButton_Login, hwndButton_SginUp;
#define ID_LABEL_LOGIN_USERNAME 1
#define ID_LABEL_LOGIN_PASSWORD 2
#define ID_EDIT_LOGIN_USERNAME 3
#define ID_EDIT_LOGIN_PASSWORD 4
#define ID_BUTTON_LOGIN 5
#define ID_BUTTON_SGINUP 6
// 登录窗口过程函数
LRESULT CALLBACK Login_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // 窗口创建消息
                    printf("Login Window WM_CREATE\n");
                
                // 创建静态文本
                hwndLabel_Login_Username = CreateWindow(
                    _T("STATIC"),              // 控件类型：静态文本
                    _T("USERNAME"),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    10, 10, 100, 20,           // 位置和大小
                    hwnd, (HMENU)ID_LABEL_LOGIN_USERNAME, hInst, NULL); // 父窗口和其他参数

                // 创建静态文本
                hwndLabel_Login_Password = CreateWindow(
                    _T("STATIC"),              // 控件类型：静态文本
                    _T("PASSWORD"),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    10, 70, 100, 20,           // 位置和大小
                    hwnd, (HMENU)ID_LABEL_LOGIN_PASSWORD, hInst, NULL); // 父窗口和其他参数

                // 创建编辑框
                hwndEdit_Login_Username = CreateWindow(
                    _T("EDIT"),                // 控件类型：编辑框
                    _T(""),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
                    10, 40, 100, 20,           // 位置和大小
                    hwnd, (HMENU)ID_EDIT_LOGIN_USERNAME, hInst, NULL); // 父窗口和其他参数

                // 创建编辑框
                hwndEdit_Login_Password = CreateWindow(
                    _T("EDIT"),                // 控件类型：编辑框
                    _T(""),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
                    10, 100, 100, 20,           // 位置和大小
                    hwnd, (HMENU)ID_EDIT_LOGIN_PASSWORD, hInst, NULL); // 父窗口和其他参数

                // 创建按钮
                hwndButton_Login = CreateWindow(
                    _T("BUTTON"),              // 控件类型：按钮
                    _T("LOGIN"),                // 按钮文本
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    120, 20, 60, 30,           // 位置和大小
                    hwnd, (HMENU)ID_BUTTON_LOGIN, hInst, NULL); // 父窗口和其他参数

                hwndButton_SginUp = CreateWindow(
                    _T("BUTTON"),              // 控件类型：按钮
                    _T("SIGNUP"),          // 按钮文本
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    120, 80, 60, 30,          // 位置和大小
                    hwnd, (HMENU)ID_BUTTON_SGINUP, hInst, NULL); // 父窗口和其他参数

            break;

        case WM_COMMAND: // 命令消息
                if (LOWORD(wParam) == ID_BUTTON_LOGIN) {    // 检查是哪个控件发出的消息

                // 登录
                TCHAR szText_username[100];  // 文本缓冲区
                TCHAR szText_passwd[100];    // 文本缓冲区
                GetWindowText(hwndEdit_Login_Username, szText_username, 100); // 获取用户名
                GetWindowText(hwndEdit_Login_Password, szText_passwd, 100);   // 获取密码

                // char username[100];
                char password[100];
                ConvertTCharToChar(szText_username, Login_User.username, 100); // 转换用户名
                ConvertTCharToChar(szText_passwd, password, 100);   // 转换密码

                // char* UserInfoFilename = "UserInfo.json";
                char* jsonStr = readFileToString(USER_INFO_DATABASE); // 读取文件内容到字符串
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // char passwd[64];
                // short auth;
                // unsigned int balance;
                int userFound = findUser(jsonStr, Login_User.username, Login_User.passwd, &Login_User.auth, &Login_User.balance); // 查找用户信息
                if (userFound) {
                    printf("Password: %s\n", Login_User.passwd);
                    printf("Auth: %d\n", Login_User.auth);
                    printf("Balance: %u\n", Login_User.balance);
                } else {
                    printf("User not found.\n");
                }
                // printf("File content:\n%s\n", jsonStr);

                if (strcmp(password, Login_User.passwd) == 0 && Login_User.username[0] != '\0' && Login_User.passwd[0] != 0)  // 检查密码是否正确
                {
                    Auth_Flag = Login_User.auth;

                    Login_Flag = 1;

                    printf("Login Success!\n");

                    RefreshWindow(hwnd_Main_Window);
                    // ShowWindow(hwnd, SW_HIDE);  // 隐藏窗口
                    ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口

                    DestroyWindow(hwnd);    // 销毁登录窗口

                    // // 创建并显示第二个窗口
                    // HWND hwndSecond = CreateWindow(
                    //     _T("SecondWindowClass"), _T("第二个窗口"),
                    //     WS_OVERLAPPEDWINDOW,
                    //     CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    //     NULL, NULL, hInst, NULL);
                    // ShowWindow(hwndSecond, SW_SHOW);
                    // // UpdateWindow(hwndSecond);
                }
                // 登录报错
                else if (szText_username[0] == '\0' && szText_passwd[0] == '\0') {
                    Login_Flag = 0;
                    printf("Login Failed!\n");
                    MessageBox(NULL, "Please Type User Info", "Error", MB_ICONINFORMATION | MB_OK);
                }
                else if (!userFound || szText_username[0] == '\0') {
                    Login_Flag = 0;
                    printf("Username Failed!\n");
                    MessageBox(NULL, "Please Check Username", "Error", MB_ICONINFORMATION | MB_OK);
                }
                else if (strcmp(password, Login_User.passwd) != 0 && Login_User.username[0] != '\0' && Login_User.passwd[0] != '\0') {
                    Login_Flag = 0;
                    printf("Password Failed!\n");
                    MessageBox(NULL, "Please Check Password", "Error", MB_ICONINFORMATION | MB_OK);
                }

                free(jsonStr); // 释放内存

            }

            if (LOWORD(wParam) == ID_BUTTON_SGINUP) { // 判断是否点击了打开新窗口的按钮
                // 创建并显示注册窗口
                hwnd_Signup_Window = CreateWindow(
                    _T("SignupWindowClass"), _T("Sign Up"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 210, 170,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwnd_Signup_Window, SW_SHOW);
                UpdateWindow(hwnd_Signup_Window);
                ShowWindow(hwnd, SW_HIDE);  // 隐藏窗口
            }

            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Main_Window);
            // ShowWindow(hwnd, SW_HIDE);  // 隐藏窗口
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND hwndLabel_Signup_Username, hwndLabel_Signup_Password, hwndEdit_Signup_Username, hwndEdit_Signup_Password, hwndButton_Signup;
    #define ID_LABEL_SIGNUP_USERNAME 1
    #define ID_LABEL_SIGNUP_PASSWORD 2
    #define ID_EDIT_SIGNUP_USERNAME 3
    #define ID_EDIT_SIGNUP_PASSWORD 4
    #define ID_BUTTON_SIGNUP 5
// 用户注册窗口过程函数
LRESULT CALLBACK Signup_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // 窗口创建消息
                    printf("Login Window WM_CREATE\n");
                
                // 创建静态文本
                hwndLabel_Signup_Username = CreateWindow(
                    _T("STATIC"),              // 控件类型：静态文本
                    _T("USERNAME"),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    10, 10, 100, 20,           // 位置和大小
                    hwnd, (HMENU)1, hInst, NULL); // 父窗口和其他参数

                // 创建静态文本
                hwndLabel_Signup_Password = CreateWindow(
                    _T("STATIC"),              // 控件类型：静态文本
                    _T("PASSWORD"),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    10, 70, 100, 20,           // 位置和大小
                    hwnd, (HMENU)2, hInst, NULL); // 父窗口和其他参数

                // 创建编辑框
                hwndEdit_Signup_Username = CreateWindow(
                    _T("EDIT"),                // 控件类型：编辑框
                    _T(""),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
                    10, 40, 100, 20,           // 位置和大小
                    hwnd, (HMENU)3, hInst, NULL); // 父窗口和其他参数

                // 创建编辑框
                hwndEdit_Signup_Password = CreateWindow(
                    _T("EDIT"),                // 控件类型：编辑框
                    _T(""),                    // 默认文本为空
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
                    10, 100, 100, 20,           // 位置和大小
                    hwnd, (HMENU)4, hInst, NULL); // 父窗口和其他参数

                // 创建按钮
                hwndButton_Signup = CreateWindow(
                    _T("BUTTON"),              // 控件类型：按钮
                    _T("SIGN UP"),                // 按钮文本
                    WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                    120, 20, 60, 30,           // 位置和大小
                    hwnd, (HMENU)5, hInst, NULL); // 父窗口和其他参数

            break;

        case WM_COMMAND: // 命令消息
                if (LOWORD(wParam) == 5) {    // 检查是哪个控件发出的消息

                // 注册
                TCHAR szText_username[100];  // 文本缓冲区
                TCHAR szText_passwd[100];    // 文本缓冲区
                GetWindowText(hwndEdit_Signup_Username, szText_username, 100); // 获取用户名
                GetWindowText(hwndEdit_Signup_Password, szText_passwd, 100);   // 获取密码

                char username[100];
                char password[100];
                ConvertTCharToChar(szText_username, username, 100); // 转换用户名
                ConvertTCharToChar(szText_passwd, password, 100);   // 转换密码

                if (username[0] != 0 && password[0] != 0) {

                    char* UserInfoFilename = "UserInfo.json";
                    char* jsonStr = readFileToString(UserInfoFilename); // 读取文件内容到字符串
                    if (jsonStr != NULL) {
                        printf("File content:\n%s\n", jsonStr);
                    }
                    else printf("Error reading user info file.\n");

                    // 添加新用户
                    char* updatedJsonStr = addUser(jsonStr, username, password, 0);
                    if (updatedJsonStr != NULL) {
                        printf("添加新用户后：\n");
                        viewUser(updatedJsonStr);
                        writeStringToFile(USER_INFO_DATABASE, updatedJsonStr);

                        free(jsonStr);         // 释放内存
                        free(updatedJsonStr);   // 释放内存

                        // 返回登录窗口
                        MessageBox(NULL, "OK to Sign Up \n ", "OK", MB_ICONINFORMATION | MB_OK);
                        RefreshWindow(hwnd_Login_Window);
                        ShowWindow(hwnd_Login_Window, SW_SHOW);  // 隐藏窗口
                        DestroyWindow(hwnd);
                    }
                    else {  // 如果用户名重复
                        printf("添加新用户失败。\n");
                        MessageBox(NULL, "Error to Sign Up\nPlease try again\nUsernames May be Duplicated", "Error", MB_ICONINFORMATION | MB_OK);
                    }
                }
                else { // 如果用户有一项未输入
                    MessageBox(NULL, "Please enter username and password", "Error", MB_ICONINFORMATION | MB_OK);
                }

            }

            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Login_Window);
            ShowWindow(hwnd_Login_Window, SW_SHOW);  // 隐藏窗口
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 用户信息窗口过程函数
LRESULT CALLBACK User_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // 窗口创建消息
                    printf("Login Window WM_CREATE\n");

            break;

        case WM_COMMAND: // 命令消息

            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Main_Window);
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


// 添加行，到列表视图,服务窗口辅助函数
void AddItemsToListView_Service(HWND hwndListView, LogEntry* logEntry_p, int logEntryCount) {
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    ListView_DeleteAllItems(hwndListView); // 清空旧列表显示
    lvItem.iSubItem = 0;

    if (logEntry_p) {
        for (int i = 0; i < logEntryCount; i++) {
            char pszTextStr[50]; // 增加缓冲区大小以避免溢出
            sprintf(pszTextStr, "<%d> %s", i+1, logEntry_p[i].period_date);
            lvItem.iItem = i;
            lvItem.pszText = pszTextStr;
            ListView_InsertItem(hwndListView, &lvItem);
            ListView_SetItemText(hwndListView, i, 1, logEntry_p[i].period_time_start);
            ListView_SetItemText(hwndListView, i, 2, logEntry_p[i].period_time_end);

            printf("i=%d\n", i);
        }
    }
    else {
        lvItem.iItem = 0;
        lvItem.pszText = "<NULL>";
    }
}
// 添加列，到列表视图，服务窗口辅助函数
void AddColumnsToListView_Service(HWND hWndListView) {
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // 第一列
    lvColumn.pszText = "DATE";
    lvColumn.cx = 150;
    ListView_InsertColumn(hWndListView, 0, &lvColumn);

    // 第二列
    lvColumn.pszText = "START";
    lvColumn.cx = 150;
    ListView_InsertColumn(hWndListView, 1, &lvColumn);

    // 第三列
    lvColumn.pszText = "END";
    lvColumn.cx = 150;
    ListView_InsertColumn(hWndListView, 2, &lvColumn);
}

HWND hwndComboBox_Service_Type, hwndComboBox_Service_Id, hwndLabel_Service_Amount, hWndListView_Service_Output, hwndEditText_Service_Output;
#define ID_COMBOBOX_SERVICE_TYPE 1
#define ID_COMBOBOX_SERVICE_ID 2
#define ID_LABEL_SERVICE_AMOUNT 3
#define ID_BUTTON_SERVICE_CHECK 4
#define ID_DATEPICKER_SERVICE_DATE 5
#define ID_TIMEPICKER_SERVICE_START 6
#define ID_TIMEPICKER_SERVICE_END 7
#define ID_BUTTON_SERVICE_RESERVE 8
#define ID_LISTVIEW_SERVICE_OUTPUT 9
// #define ID_EDITTEXT_SERVICE_OUTPUT 10
// 服务窗口过程函数
LRESULT CALLBACK Service_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    SYSTEMTIME st;
    // char buffer[128];

    switch (msg) {
        case WM_CREATE: // 窗口创建消息
            printf("Service Window WM_CREATE\n");

            // 创建第一个下拉框
            hwndComboBox_Service_Type = CreateWindow(WC_COMBOBOX, "", 
                            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                            10, 10, 120, 150,  // 位置和大小
                            hwnd, (HMENU)ID_COMBOBOX_SERVICE_TYPE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            SendMessage(hwndComboBox_Service_Type, CB_SELECTSTRING, -1, (LPARAM)"TYPE");

            // 创建第二个下拉框
            hwndComboBox_Service_Id = CreateWindow(WC_COMBOBOX, "", 
                            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                            10, 60, 120, 150,  // 位置和大小
                            hwnd, (HMENU)ID_COMBOBOX_SERVICE_ID,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            SendMessage(hwndComboBox_Service_Id, CB_SELECTSTRING, -1, (LPARAM)"ID");

            // 创建静态文本显示价格
            hwndLabel_Service_Amount = CreateWindow(
                            _T("STATIC"),              // 控件类型：静态文本
                            _T("AMOUNT"),                    // 默认文本为空
                            WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                            10, 110, 100, 20,           // 位置和大小
                            hwnd, (HMENU)2, hInst, NULL); // 父窗口和其他参数

            // 创建检查按钮
            CreateWindowEx(0, "BUTTON", "Check Reservation",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 160, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_SERVICE_CHECK,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建日期选择
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 210, 140, 25,
                            hwnd, (HMENU)ID_DATEPICKER_SERVICE_DATE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建开始时间选择
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | DTS_TIMEFORMAT | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 260, 140, 25,
                            hwnd, (HMENU)ID_TIMEPICKER_SERVICE_START,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建结束时间选择
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | DTS_TIMEFORMAT | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 310, 140, 25,
                            hwnd, (HMENU)ID_TIMEPICKER_SERVICE_END,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建预约按钮
            CreateWindowEx(0, "BUTTON", "Make Reservation",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 360, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_SERVICE_RESERVE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建列表视图
            hWndListView_Service_Output = CreateWindow(WC_LISTVIEW, "",
                            WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL,
                            180, 10, 450, 700,  // 位置和大小
                            hwnd, (HMENU)ID_LISTVIEW_SERVICE_OUTPUT,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            AddColumnsToListView_Service(hWndListView_Service_Output);  // 列表视图添加列


            // hwndEditText_Service_Output = CreateWindowEx(0, "EDIT", "",
            //                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            //                 300, 50, 200, 100, // 位置和大小
            //                 hwnd, (HMENU)ID_EDITTEXT_SERVICE_OUTPUT,
            //                 ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 设置编辑控件内容
            // SetWindowText(hwndEditText_Service_Output, "这里是长文本内容...\n可以包含多行\n以展示滚动效果。\n这里是更多的文本内容\n这里是长文本内容...\n可以包含多行\n以展示滚动效果。\n这里是长文本内容...\n可以包含多行\n以展示滚动效果。\n");

            break;

        case WM_COMMAND:

            if (LOWORD(wParam) == ID_COMBOBOX_SERVICE_TYPE) {    // type下拉框获取type
                // 步骤1：获取当前选中的内容
                TCHAR selectedType[50];
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                SendMessage(hwndComboBox_Service_Type, CB_RESETCONTENT, 0, 0);  // 清空下拉框
                SendMessage(hwndComboBox_Service_Id, CB_RESETCONTENT, 0, 0);

                char* jsonStr = readFileToString(SEAT_INFO_DATABASE); // 读取文件内容到字符串
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // 获取所有座位类型
                char** seatTypes;
                int countType = getSeatTypes(jsonStr, &seatTypes);
                printf("Found %d different seat types\n", countType);
                for (int i = 0; i < countType; i++) {
                    printf("Seat Type: %s\n", seatTypes[i]);
                    SendMessage(hwndComboBox_Service_Type, CB_ADDSTRING, 0, (LPARAM)seatTypes[i]);
                    free(seatTypes[i]); // 释放每个字符串
                }
                free(seatTypes); // 释放字符串数组
                free(jsonStr);

                // 步骤4：恢复先前选中的内容
                SendMessage(hwndComboBox_Service_Type, CB_SELECTSTRING, -1, (LPARAM)selectedType);

            }

            if (LOWORD(wParam) == ID_COMBOBOX_SERVICE_ID) {    // id下拉框获取id
                // 读取type
                TCHAR selectedType[50];
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                // 读取已选择的ID
                TCHAR selectedId[50];
                int itemIndexId = SendMessage(hwndComboBox_Service_Id, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Id, CB_GETLBTEXT, itemIndexId, (LPARAM)selectedId);


                SendMessage(hwndComboBox_Service_Id, CB_RESETCONTENT, 0, 0);    // 清空下拉框

                char* jsonStr = readFileToString(SEAT_INFO_DATABASE); // 读取文件内容到字符串
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // 统计类型数量
                int* seatIds;
                int count = countSeatsByType(jsonStr, selectedType, &seatIds);
                printf("Found %d seats of type %s\n", count, selectedType);
                for (int i = 0; i < count; i++) {
                    char seatIdsStr[10]; // 声明一个足够大的字符数组来存储转换后的字符串
                    sprintf(seatIdsStr, "%d", seatIds[i]);
                    SendMessage(hwndComboBox_Service_Id, CB_ADDSTRING, 0, (LPARAM)seatIdsStr);
                    printf("Seat ID: %d\n", seatIds[i]);
                }
                free(seatIds); // 释放内存

                // 步骤4：恢复先前选中的内容
                SendMessage(hwndComboBox_Service_Id, CB_SELECTSTRING, -1, (LPARAM)selectedId);
                
                char str_amount[10] = {'\0'};
                sprintf(str_amount, "%d", findAmountById(jsonStr, atoi(selectedId)));
                SetWindowText(hwndLabel_Service_Amount, str_amount);     // 设置价格文本显示

                free(jsonStr); // 释放内存
                printf("\n");

            }

            if (LOWORD(wParam) == ID_BUTTON_SERVICE_CHECK) {    // 搜索已预订记录
                // 读取type
                TCHAR selectedType[50] = {'\0'};
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                // 读取已选择的ID
                TCHAR selectedId[50] = {'\0'};
                int itemIndexId = SendMessage(hwndComboBox_Service_Id, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Id, CB_GETLBTEXT, itemIndexId, (LPARAM)selectedId);

                if (selectedId[0] != 0 && selectedType[0] != 0) { // 检查type和id是否已填       
        
                    int count1;
                    LogEntry* bookedSlots = get_booked_id_slots(RESERVE_RECORD_LOG, selectedType, atoi(selectedId), &count1);
                    // ... 输出或处理 bookedSlots 中的数据
                    if (bookedSlots) {
                        for (int i = 0; i < count1; i++) {
                            printf("已预订时间段：%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                        }
                    }
                    else printf("没有找到已预订席位或读取日志文件失败。\n");

                    AddItemsToListView_Service(hWndListView_Service_Output, bookedSlots, count1); // 添加行到列表

                    free(bookedSlots); // 释放内存

                }
                else {
                    printf("请填写type和id\n");
                    MessageBox(NULL, _T("Please Fill Type and ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                }

            }

            if (LOWORD(wParam) == ID_BUTTON_SERVICE_RESERVE) {

                LogEntry LogEntry_s_temp;
                char buffer[128];
                char buffer_temp[25];

                HWND hDatePicker = GetDlgItem(hwnd, ID_DATEPICKER_SERVICE_DATE);
                HWND hTimePicker_Start = GetDlgItem(hwnd, ID_TIMEPICKER_SERVICE_START);
                HWND hTimePicker_End = GetDlgItem(hwnd, ID_TIMEPICKER_SERVICE_END);

                // 读取已选择的type
                TCHAR selectedType[50] = {'\0'};
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                // 读取已选择的ID
                TCHAR selectedId[50] = {'\0'};
                int itemIndexId = SendMessage(hwndComboBox_Service_Id, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Id, CB_GETLBTEXT, itemIndexId, (LPARAM)selectedId);

                printf("Selected type: %s\n", selectedType);
                printf("Selected id: %s\n", selectedId);

                if (selectedId[0] != 0 && selectedType[0] != 0) { // 检查type和id是否已填

                    char* jsonStr_seat = readFileToString(SEAT_INFO_DATABASE); // 读取文件内容到字符串
                    if (jsonStr_seat != NULL) {
                        printf("File content:\n%s\n", jsonStr_seat);
                    }
                    else printf("Error reading user info file.\n");

                    int amount = findAmountById(jsonStr_seat, atoi(selectedId)); // 获取价格

                    printf("Price: %d\n", amount);
                    printf("Balance: %d\n", Login_User.balance);

                    free(jsonStr_seat);

                    if (Login_User.balance >= amount) {

                        LogEntry_s_temp.amount = amount;

                        // 生成订单号
                        char order_id[24];
                        generate_order_id(order_id);
                        strcpy(LogEntry_s_temp.order_id, order_id);

                        // 获取日期
                        DateTime_GetSystemtime(hDatePicker, &st);
                        sprintf(buffer_temp, "%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);
                        strcpy(LogEntry_s_temp.period_date, buffer_temp);

                        sprintf(buffer, "Date: %04d-%02d-%02d\n", st.wYear, st.wMonth, st.wDay);
                        
                        // 获取时间
                        DateTime_GetSystemtime(hTimePicker_Start, &st);
                        sprintf(buffer_temp, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
                        strcpy(LogEntry_s_temp.period_time_start, buffer_temp);

                        sprintf(buffer + strlen(buffer), "Time: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);

                        // 获取时间
                        DateTime_GetSystemtime(hTimePicker_End, &st);
                        sprintf(buffer_temp, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
                        strcpy(LogEntry_s_temp.period_time_end, buffer_temp);

                        sprintf(buffer + strlen(buffer), "Time: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
                        
                        printf("%s", buffer);   //  打印日期，时间

                        strcpy(LogEntry_s_temp.operate, Login_User.username);
                        strcpy(LogEntry_s_temp.action, "appoint");
                        LogEntry_s_temp.id = atoi(selectedId);
                        strcpy(LogEntry_s_temp.seat_type, selectedType);

                        strcpy(LogEntry_s_temp.subscriber, Login_User.username);

                        // 写入预订日志
                        if (write_log_realtime_conflict(RESERVE_RECORD_LOG, &LogEntry_s_temp) == 0)
                        {
                            MessageBox(NULL, _T("Success to Reserve"), _T("OK"), MB_ICONEXCLAMATION | MB_OK);
                            
                            Login_User.balance = Login_User.balance - amount;   // 扣费
                            char* jsonStr_user = readFileToString(USER_INFO_DATABASE); // 读取文件内容到字符串
                            if (jsonStr_user != NULL) {
                                printf("File content:\n%s\n", jsonStr_user);
                            }
                            else printf("Error reading user info file.\n");
                            char* modifiedJsonStr = modifyUser(jsonStr_user, Login_User.username, Login_User.passwd, Login_User.auth, Login_User.balance);
                            if (modifiedJsonStr != NULL) {
                                printf("修改用户信息后：\n");
                                viewUser(modifiedJsonStr);
                                writeStringToFile(USER_INFO_DATABASE, modifiedJsonStr); // 写入
                            }
                            printf("\n");
                            free(modifiedJsonStr);
                            free(jsonStr_user);
                        }
                        else MessageBox(NULL, _T("Fail to Reserve"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    }
                    else {
                        printf("余额不足\n");
                        MessageBox(NULL, _T("Not Sufficient Balance"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    }

                    // 刷新列表视图
                    int count1;
                    LogEntry* bookedSlots = get_booked_id_slots(RESERVE_RECORD_LOG, selectedType, atoi(selectedId), &count1);
                    // ... 输出或处理 bookedSlots 中的数据
                    if (bookedSlots) {
                        for (int i = 0; i < count1; i++) {
                            printf("已预订时间段：%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                        }
                    }
                    else printf("没有找到已预订席位或读取日志文件失败。\n");

                    AddItemsToListView_Service(hWndListView_Service_Output, bookedSlots, count1); // 添加行到列表

                    free(bookedSlots); // 释放内存

                }
                else {
                    printf("请填写type和id\n");
                    MessageBox(NULL, _T("Please Fill Type and ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                }

            }
            break;

        case WM_CLOSE:
            // 在这里添加其他关闭窗口前的处理

            RefreshWindow(hwnd_Main_Window);
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            // PostQuitMessage(0);
            return 0;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


// 添加行，到列表视图,服务窗口辅助函数
void AddItemsToListView_Order(HWND hwndListView, LogEntry* logEntry_p, int logEntryCount) {
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    ListView_DeleteAllItems(hwndListView); // 清空旧列表显示
    lvItem.iSubItem = 0;

    if (logEntry_p) {
        for (int i = 0; i < logEntryCount; i++) {
            // char pszTextStr[50]; // 增加缓冲区大小以避免溢出
            // sprintf(pszTextStr, "<%d> %s", i+1, logEntry_p[i].logtime);
            lvItem.iItem = i;
            // lvItem.pszText = pszTextStr;
            lvItem.pszText = logEntry_p[i].order_id;
            ListView_InsertItem(hwndListView, &lvItem);
            ListView_SetItemText(hwndListView, i, 1, logEntry_p[i].seat_type);
            char str_id[12];
            sprintf(str_id, "%d", logEntry_p[i].id);
            ListView_SetItemText(hwndListView, i, 2, str_id);
            ListView_SetItemText(hwndListView, i, 3, logEntry_p[i].period_date);
            ListView_SetItemText(hwndListView, i, 4, logEntry_p[i].period_time_start);
            ListView_SetItemText(hwndListView, i, 5, logEntry_p[i].period_time_end);
            char str_amount[12];
            sprintf(str_amount, "%d", logEntry_p[i].amount);
            ListView_SetItemText(hwndListView, i, 6, str_amount);
            char pszTextStr[50]; // 增加缓冲区大小以避免溢出
            sprintf(pszTextStr, "%s <%d>", logEntry_p[i].logtime, i+1);
            ListView_SetItemText(hwndListView, i, 7, pszTextStr);

            printf("i=%d\n", i);
        }
    }
    else {
        lvItem.iItem = 0;
        lvItem.pszText = "<NULL>";
    }
}
// 添加列，到列表视图，服务窗口辅助函数
void AddColumnsToListView_Order(HWND hWndListView) {
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // 第0列
    lvColumn.pszText = "ORDER-ID";
    lvColumn.cx = 180;
    ListView_InsertColumn(hWndListView, 0, &lvColumn);

    // 第1列
    lvColumn.pszText = "TYPE";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 1, &lvColumn);

    // 第2列
    lvColumn.pszText = "ID";
    lvColumn.cx = 50;
    ListView_InsertColumn(hWndListView, 2, &lvColumn);

    // 第3列
    lvColumn.pszText = "DATE";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 3, &lvColumn);

    // 第4列
    lvColumn.pszText = "START";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 4, &lvColumn);

    // 第5列
    lvColumn.pszText = "END";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 5, &lvColumn);

    // 第6列
    lvColumn.pszText = "AMOUNT";
    lvColumn.cx = 70;
    ListView_InsertColumn(hWndListView, 6, &lvColumn);

    // 第7列
    lvColumn.pszText = "LogTime";
    lvColumn.cx = 200;
    ListView_InsertColumn(hWndListView, 7, &lvColumn);
}

HWND hWndListView_Order_Output, hwndComboBox_Order_OrderId;
// hwndEditText_Order_OrderId, 
#define ID_BUTTON_ORDER_CHECKALL 1
#define ID_DATEPICKER_ORDER_DATE 2
#define ID_BUTTON_ORDER_CHECKDATE 3
#define ID_LISTVIEW_ORDER_OUTPUT 4
// #define ID_EDITTEXT_ORDER_ORDERID 5
#define ID_COMBOBOX_ORDER_ORDERID 5
#define ID_BUTTON_ORDER_CANCEL 6
// 订单窗口过程函数
LRESULT CALLBACK Order_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SYSTEMTIME st;

    switch (msg) {
        case WM_CREATE: // 窗口创建消息
            printf("Order Window WM_CREATE\n");

            // 创建订单号下拉框
            hwndComboBox_Order_OrderId = CreateWindow(WC_COMBOBOX, "", 
                            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                            10, 310, 150, 20,  // 位置和大小
                            hwnd, (HMENU)ID_COMBOBOX_ORDER_ORDERID,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            SendMessage(hwndComboBox_Order_OrderId, CB_ADDSTRING, 0, (LPARAM)"Fuck");

            // 创建检查所有按钮
            CreateWindowEx(0, "BUTTON", "All Reservation",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 10, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_ORDER_CHECKALL,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建日期选择
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 60, 140, 25,
                            hwnd, (HMENU)ID_DATEPICKER_ORDER_DATE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建按日期检查按钮
            CreateWindowEx(0, "BUTTON", "Check by Date",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 110, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_ORDER_CHECKDATE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // 创建列表视图
            hWndListView_Order_Output = CreateWindow(WC_LISTVIEW, "",
                            WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL,
                            180, 10, 900, 450,  // 位置和大小
                            hwnd, (HMENU)ID_LISTVIEW_ORDER_OUTPUT,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            AddColumnsToListView_Order(hWndListView_Order_Output);  // 列表视图添加列

            // // 创建编辑框
            // hwndEditText_Order_OrderId = CreateWindow(
            //                 _T("EDIT"),                // 控件类型：编辑框
            //                 _T(""),                    // 默认文本为空
            //                 WS_CHILD | WS_VISIBLE | WS_BORDER, // 样式：子窗口、可见、带边框
            //                 10, 160, 150, 20,           // 位置和大小
            //                 hwnd, (HMENU)ID_EDITTEXT_ORDER_ORDERID, hInst, NULL); // 父窗口和其他参数

            // 创建取消预约按钮
            CreateWindowEx(0, "BUTTON", "CANCEL ORDER",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 210, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_ORDER_CANCEL,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            break;

        case WM_COMMAND:

            if (LOWORD(wParam) == ID_BUTTON_ORDER_CHECKALL)
            {
                // 检查所有预约
                int count1;
                LogEntry* bookedSlots = get_booked_seats(RESERVE_RECORD_LOG, Login_User.username, &count1);
                // ... 输出或处理 bookedSlots 中的数据
                if (bookedSlots) {
                    for (int i = 0; i < count1; i++) {
                        printf("已预订时间段：%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                    }
                }
                else printf("没有找到已预订席位或读取日志文件失败。\n");

                AddItemsToListView_Order(hWndListView_Order_Output, bookedSlots, count1); // 添加行到列表

                free(bookedSlots); // 释放内存
            }

            if (LOWORD(wParam) == ID_BUTTON_ORDER_CHECKDATE)
            {
                // 按日期检查预约
                char buffer_date[25];
                HWND hDatePicker = GetDlgItem(hwnd, ID_DATEPICKER_ORDER_DATE);
                // 获取日期
                DateTime_GetSystemtime(hDatePicker, &st);
                sprintf(buffer_date, "%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);

                int count1;
                LogEntry* bookedSlots = get_subscriber_booked_time_slots(RESERVE_RECORD_LOG, Login_User.username, buffer_date, &count1);
                // ... 输出或处理 bookedSlots 中的数据
                if (bookedSlots) {
                    for (int i = 0; i < count1; i++) {
                        printf("已预订时间段：%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                    }
                }
                else printf("没有找到已预订席位或读取日志文件失败。\n");

                AddItemsToListView_Order(hWndListView_Order_Output, bookedSlots, count1); // 添加行到列表

                free(bookedSlots); // 释放内存
            }

            if (LOWORD(wParam) == ID_COMBOBOX_ORDER_ORDERID) {  // 下拉框获取订单号
                printf("下拉框获取订单号\n");
                // 步骤1：获取当前选中的内容
                TCHAR selectedOrderId[100] = {'\0'};
                int itemIndexOrderId = SendMessage(hwndComboBox_Order_OrderId, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Order_OrderId, CB_GETLBTEXT, itemIndexOrderId, (LPARAM)selectedOrderId);

                SendMessage(hwndComboBox_Order_OrderId, CB_RESETCONTENT, 0, 0);  // 清空下拉框

                // 检查所有预约
                int count1;
                LogEntry* bookedSlots = get_booked_seats(RESERVE_RECORD_LOG, Login_User.username, &count1);
                // ... 输出或处理 bookedSlots 中的数据
                if (bookedSlots) {
                    for (int i = 0; i < count1; i++) {
                        printf("订单号：%s\n", bookedSlots[i].order_id);
                        SendMessage(hwndComboBox_Order_OrderId, CB_ADDSTRING, 0, (LPARAM)bookedSlots[i].order_id);
                    }
                }
                else printf("没有找到已预订席位或读取日志文件失败。\n");

                free(bookedSlots);

                // 步骤4：恢复先前选中的内容
                SendMessage(hwndComboBox_Order_OrderId, CB_SELECTSTRING, -1, (LPARAM)selectedOrderId);
            }

            if (LOWORD(wParam) == ID_BUTTON_ORDER_CANCEL)
            {
                // 取消预约

                // 获取订单号
                // TCHAR szText_orderid[30];  // 文本缓冲区
                // GetWindowText(hwndEditText_Order_OrderId, szText_orderid, 30);
                // 步骤1：获取当前选中的内容
                TCHAR selectedOrderId[50];
                int itemIndexOrderId = SendMessage(hwndComboBox_Order_OrderId, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Order_OrderId, CB_GETLBTEXT, itemIndexOrderId, (LPARAM)selectedOrderId);

                char str_orderid[30];
                ConvertTCharToChar(selectedOrderId, str_orderid, 30); // 转换订单号

                printf ("CANCEL订单号：%s\n", str_orderid);

                if (str_orderid[0] != '\0') {

                    int count2;
                    LogEntry* bookedSlots = get_log_by_order_id(RESERVE_RECORD_LOG, str_orderid, &count2);

                    if (bookedSlots) {
                        int delete_flag = delete_entries_by_orderid(RESERVE_RECORD_LOG, str_orderid);
                        if (delete_flag == 1) {

                            // 返款
                            Login_User.balance += bookedSlots[0].amount;
                            char* jsonStr_user = readFileToString(USER_INFO_DATABASE); // 读取文件内容到字符串
                            if (jsonStr_user != NULL) {
                                printf("File content:\n%s\n", jsonStr_user);
                            }
                            else printf("Error reading user info file.\n");
                            char* modifiedJsonStr = modifyUser(jsonStr_user, Login_User.username, Login_User.passwd, Login_User.auth, Login_User.balance);
                            if (modifiedJsonStr != NULL) {
                                printf("修改用户信息后：\n");
                                viewUser(modifiedJsonStr);
                                writeStringToFile(USER_INFO_DATABASE, modifiedJsonStr); // 写入
                            }
                            printf("\n");
                            free(modifiedJsonStr);
                            free(jsonStr_user);

                            ListView_DeleteAllItems(hWndListView_Order_Output); // 清空旧列表显示
                            MessageBox(NULL, _T("Success to Cancel"), _T("OK"), MB_ICONEXCLAMATION | MB_OK);
                        }
                        else if (delete_flag == -3) {
                            printf("CanNot Match Order-ID, Please Check Order-ID\n");
                            MessageBox(NULL, _T("CanNot Match Order-ID\nPlease Check Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                        }
                        else MessageBox(NULL, _T("Fail to Cancel\nPlease Check Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    
                    }
                    else {
                        printf("没有找到订单号\n");
                        MessageBox(NULL, _T("CanNot Match Order-ID\nPlease Check Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    }

                    free(bookedSlots); // 释放内存
                }
                else MessageBox(NULL, _T("Please Input Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);

            }

            break;

        case WM_CLOSE:
            // 在这里添加其他关闭窗口前的处理

            RefreshWindow(hwnd_Main_Window);
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            // PostQuitMessage(0);
            return 0;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 管理员窗口过程函数
LRESULT CALLBACK Admin_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // 窗口创建消息
                printf("Admin Window WM_CREATE\n");
                

            break;

        case WM_COMMAND: // 命令消息
                
            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Main_Window);
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}



int main() {
    // Login_Flag = 1;
    // HWND hWnd = GetConsoleWindow();
    // ShowWindow(hWnd, SW_HIDE);
    printf("Hello World!\n");
    // Login_Show();

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
}
