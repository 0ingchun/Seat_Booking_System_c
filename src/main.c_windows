#include <windows.h>  // 引入 Windows API 的头文件
#include <tchar.h>    // 引入 Unicode 和 ANSI 兼容的函数宏
#include <stdio.h>

// #include "../include/main.h"
// #include "../include/cJSON.h"

#include "main.h"
#include "cJSON.h"
#include "Login_User.h"

// User_Login_t User_Login;

// 全局变量
HINSTANCE hInst;      // 应用程序实例句柄
HWND hwndEdit, hwndEdit2, hwndButton, hwndLabel, hwndLabel2, hwndOpenNewWindowButton;  // 分别为编辑框、按钮和标签的窗口句柄

// 声明第二个窗口的窗口过程函数
LRESULT CALLBACK SecondWndProc(HWND, UINT, WPARAM, LPARAM);

// 声明窗口过程函数
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain：应用程序入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc;       // 窗口类结构
    HWND hwnd;         // 主窗口句柄
    MSG Msg;           // 消息结构

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

    // 窗口类的设置
    wc.style = 0;                          // 窗口样式
    wc.lpfnWndProc = WndProc;              // 指定窗口过程函数
    wc.cbClsExtra = 0;                     // 窗口类额外内存
    wc.cbWndExtra = 0;                     // 窗口额外内存
    wc.hInstance = hInstance;              // 应用程序实例句柄
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // 加载图标
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);    // 加载光标
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // 背景刷
    wc.lpszMenuName = NULL;                // 菜单名称
    wc.lpszClassName = _T("MyWindowClass"); // 窗口类名称

    // 注册窗口类
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("窗口类注册失败！"), _T("错误"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 创建主窗口
    hwnd = CreateWindow(
        _T("MyWindowClass"),               // 窗口类名称
        _T("v1 带控件的窗口"),                // 窗口标题
        WS_OVERLAPPEDWINDOW,               // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,      // 窗口位置
        240, 240,                          // 窗口大小
        NULL, NULL, hInstance, NULL);      // 其他参数

    ShowWindow(hwnd, nCmdShow);           // 显示窗口
    UpdateWindow(hwnd);                   // 更新窗口

    // 消息循环
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);           // 翻译消息
        DispatchMessage(&Msg);            // 分发消息
    }
    return (int)Msg.wParam;
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: // 窗口创建消息
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
                _T("复制"),                // 按钮文本
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

            if (LOWORD(wParam) == 2) {    // 检查是哪个控件发出的消息
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

        case WM_CLOSE: // 窗口关闭消息
            DestroyWindow(hwnd);          // 销毁窗口
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
                _T("复制"),                // 按钮文本
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

int main() {
    printf("Hello World!\n");
    Login_Show();

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
}
