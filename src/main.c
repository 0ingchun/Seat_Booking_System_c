#include <windows.h>  // 引入 Windows API 的头文件
#include <tchar.h>    // 引入 Unicode 和 ANSI 兼容的函数宏
#include <stdio.h>

#include "File_Manage.h"
#include "Login_User.h"
#include "Seat_Info.h"
#include "cJSON.h"
#include "main.h"

#define USER_INFO_DATABASE "UserInfo.json"
#define SEAT_INFO_DATABASE "SeatInfo.json"

Login_User_t Login_User;

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

// 声明第二个窗口的窗口过程函数
LRESULT CALLBACK SecondWndProc(HWND, UINT, WPARAM, LPARAM);

// 声明登录窗口的窗口过程函数
LRESULT CALLBACK Login_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明注册窗口的窗口过程函数
LRESULT CALLBACK Signup_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明服务窗口的窗口过程函数
LRESULT CALLBACK Service_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明管理窗口的窗口过程函数
LRESULT CALLBACK Admin_WndProc(HWND, UINT, WPARAM, LPARAM);

// 声明主窗口过程函数
LRESULT CALLBACK Main_WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd_Main_Window, hwnd_Login_Window, hwnd_Signup_Window, hwnd_Service_Window, hwnd_Admin_Window;         // 主窗口句柄

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
        MessageBox(NULL, _T("第二个窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户登录窗口类
    WNDCLASS wc_Login = {0};
    wc_Login.lpfnWndProc = Login_WndProc;
    wc_Login.hInstance = hInstance;
    wc_Login.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Login.lpszClassName = _T("LoginWindowClass");
    if (!RegisterClass(&wc_Login)) {
        MessageBox(NULL, _T("用户登录窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户注册窗口类
    WNDCLASS wc_Signup = {0};
    wc_Signup.lpfnWndProc = Signup_WndProc;
    wc_Signup.hInstance = hInstance;
    wc_Signup.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Signup.lpszClassName = _T("SignupWindowClass");
    if (!RegisterClass(&wc_Signup)) {
        MessageBox(NULL, _T("用户注册窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册用户服务窗口类
    WNDCLASS wc_Service = {0};
    wc_Service.lpfnWndProc = Service_WndProc;
    wc_Service.hInstance = hInstance;
    wc_Service.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Service.lpszClassName = _T("ServiceWindowClass");
    if (!RegisterClass(&wc_Service)) {
        MessageBox(NULL, _T("用户服务窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 注册管理员窗口类
    WNDCLASS wc_Admin = {0};
    wc_Admin.lpfnWndProc = Admin_WndProc;
    wc_Admin.hInstance = hInstance;
    wc_Admin.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Admin.lpszClassName = _T("AdminWindowClass");
    if (!RegisterClass(&wc_Admin)) {
        MessageBox(NULL, _T("管理员窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
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
        MessageBox(NULL, _T("窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 创建主窗口
    hwnd_Main_Window = CreateWindow(
        _T("MainWindowClass"),               // 窗口类名称
        _T("v1 Makerspace Seat Booking System"),                // 窗口标题
        WS_OVERLAPPEDWINDOW,               // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,      // 窗口位置
        240, 240,                          // 窗口大小
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


boolean Login_Flag = 0;
short Auth_Flag = 0;

HWND hwndButton_Login_Window, hwndButton_Service_Window, hwndButton_Admin_Window;

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

            // 创建静态文本
            hwndLabel = CreateWindow(
                _T("STATIC"),              // 控件类型：静态文本
                _T("USER"),                    // 默认文本为空
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                10, 40, 100, 20,           // 位置和大小
                hwnd, (HMENU)3, hInst, NULL); // 父窗口和其他参数

            // 创建按钮
            hwndButton_Login_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("LOGIN"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                10, 10, 50, 20,           // 位置和大小
                hwnd, (HMENU)1, hInst, NULL); // 父窗口和其他参数

            hwndButton_Service_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("SERVICE"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                80, 10, 50, 20,           // 位置和大小
                hwnd, (HMENU)2, hInst, NULL); // 父窗口和其他参数

            hwndButton_Admin_Window = CreateWindow(
                _T("BUTTON"),              // 控件类型：按钮
                _T("ADMIN"),                // 按钮文本
                WS_CHILD | WS_VISIBLE,     // 样式：子窗口、可见
                80, 40, 50, 20,           // 位置和大小
                hwnd, (HMENU)3, hInst, NULL); // 父窗口和其他参数

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

            

            if (LOWORD(wParam) == 1) { // 判断是否点击了打开登录窗口的按钮
                printf("Press Login Button: %d\n", Login_Flag);
                if (Login_Flag == 0) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示第二个窗口
                    hwnd_Login_Window = CreateWindow(
                        _T("LoginWindowClass"),
                        _T("Login System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Login_Window, SW_SHOW);
                    UpdateWindow(hwnd_Login_Window);
                }
                else if (Login_Flag == 1) {
                    // 打开用户信息与余额充值
                    MessageBox(NULL, "Already Login", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == 2) { // 判断是否点击了打开服务窗口的按钮
                printf("Press Service Button: %d\n", Login_Flag);
                if (Login_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示第二个窗口
                    hwnd_Service_Window = CreateWindow(
                        _T("ServiceWindowClass"),
                        _T("Service System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Service_Window, SW_SHOW);
                    UpdateWindow(hwnd_Service_Window);
                }
                else {
                    MessageBox(NULL, "Please Login First", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == 3) { // 判断是否点击了打开管理窗口的按钮
                printf("Press Admin Button: %d\n", Login_Flag);
                if (Auth_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // 隐藏主窗口

                    // 创建并显示第二个窗口
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
                    MessageBox(NULL, "Not a Admin", "Error", MB_ICONINFORMATION | MB_OK);
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
    // const int ID_LABEL_LOGIN_USERNAME = 1, ID_LABEL_LOGIN_PASSWORD = 2, ID_EDIT_LOGIN_USERNAME = 3, ID_EDIT_LOGIN_PASSWORD = 4, ID_BUTTON_LOGIN = 5, ID_BUTTON_SGINUP = 6;

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
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
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


// 服务窗口过程函数
LRESULT CALLBACK Service_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // 窗口创建消息
                printf("Service Window WM_CREATE\n");
                
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

                char* UserInfoFilename = "UserInfo.json";
                char* jsonStr = readFileToString(UserInfoFilename); // 读取文件内容到字符串
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

                if (strcmp(password, Login_User.passwd) == 0 && Login_User.username[0] != '\0' && Login_User.auth != 0 && Login_User.passwd != 0)  // 检查密码是否正确
                {
                    Auth_Flag = Login_User.auth;

                    Login_Flag = 1;

                    printf("Login Success!\n");

                    RefreshWindow(hwnd_Main_Window);
                    // ShowWindow(hwnd, SW_HIDE);  // 隐藏窗口
                    ShowWindow(hwnd_Main_Window, SW_SHOW);  // 隐藏窗口

                    DestroyWindow(hwnd);    // 销毁登录窗口
                }
                else
                {
                    Login_Flag = 0;
                    printf("Login Failed!\n");
                }

                free(jsonStr); // 释放内存

            }

            if (LOWORD(wParam) == ID_BUTTON_SGINUP) { // 判断是否点击了打开新窗口的按钮
                // 创建并显示第二个窗口
                HWND hwndSecond = CreateWindow(
                    _T("SecondWindowClass"), _T("第二个窗口"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwndSecond, SW_SHOW);
                UpdateWindow(hwndSecond);
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

// 管理员窗口过程函数
LRESULT CALLBACK Admin_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // 窗口创建消息
                printf("Service Window WM_CREATE\n");
                
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

                char* UserInfoFilename = "UserInfo.json";
                char* jsonStr = readFileToString(UserInfoFilename); // 读取文件内容到字符串
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

                if (strcmp(password, Login_User.passwd) == 0 && Login_User.username[0] != '\0' && Login_User.auth != 0 && Login_User.passwd != 0)  // 检查密码是否正确
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
                else
                {
                    Login_Flag = 0;
                    printf("Login Failed!\n");
                }

                free(jsonStr); // 释放内存

            }

            if (LOWORD(wParam) == ID_BUTTON_SGINUP) { // 判断是否点击了打开新窗口的按钮
                // 创建并显示第二个窗口
                HWND hwndSecond = CreateWindow(
                    _T("SecondWindowClass"), _T("第二个窗口"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwndSecond, SW_SHOW);
                UpdateWindow(hwndSecond);
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



int main() {
    printf("Hello World!\n");
    // Login_Show();

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
}
