// DelNotifyIcon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <afx.h>
#include <windows.h>
#include <process.h>
#include <TlHelp32.h>
#include "Commctrl.h"
#include "shellapi.h"
#include <tchar.h>


struct TRAYDATA
{
	HWND hWnd;
	UINT uID;
	UINT uCallbackMessage;
	DWORD Reserved1[2];
	HICON hIcon;
	DWORD Reserved2[3];
	TCHAR szExePath[MAX_PATH];
	TCHAR szTip[128]; 
};


int main(int argc, char* argv[])
{

	//获取托盘可见区域句柄
	HWND hWnd = NULL;
	hWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	hWnd = ::FindWindowEx(hWnd, NULL, _T("TrayNotifyWnd"), NULL);
	//hWnd = ::FindWindowEx(hWnd, NULL, _T("ReBarWindow32"), NULL);
	//hWnd = ::FindWindowEx(hWnd, NULL, _T("CiceroUIWndFrame"), NULL);
	hWnd = ::FindWindowEx(hWnd, NULL, _T("SysPager"), NULL);
	//hWnd = ::FindWindowEx(hWnd, NULL, _T("Button"), NULL);
	hWnd = ::FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);
 
	//获取进程 ID
	DWORD dwPID = 0;
	::GetWindowThreadProcessId(hWnd, &dwPID);
	DWORD dwCount = ::SendMessage(hWnd, TB_BUTTONCOUNT, NULL, NULL);
	printf("托盘可见区域当前存在 %u 个图标！PID = %u\n", dwCount, dwPID);
 
	//获取托盘溢出区域句柄
	//hWnd = ::FindWindow(_T("NotifyIconOverflowWindow"), NULL);
	//hWnd = ::FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);
 
	//::GetWindowThreadProcessId(hWnd, &dwPID);
	//dwCount = ::SendMessage(hWnd, TB_BUTTONCOUNT, NULL, NULL);
	//printf("托盘溢出区域当前存在 %u 个图标！PID = %u\n", dwCount, dwPID);
 
	//打开进程
	HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
 
	//申请内存
	LPVOID pTB = ::VirtualAllocEx(hProc, NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
	printf("申请地址 0x%08X \n\n", pTB);

	TBBUTTON tb;
	TRAYDATA td;
	NOTIFYICONDATA nid;
	for (DWORD i = 0; i < dwCount; i++)
	{
		::SendMessage(hWnd, TB_GETBUTTON, i, (LPARAM)pTB);
		printf("获取按钮 %u 信息...\n", i);
		
		::ReadProcessMemory(hProc, pTB, &tb, sizeof(TBBUTTON), NULL);
		printf("托盘图标 %u 信息地址：0x%08X\n", i, tb.dwData);
 
		//获取按钮文本
 
		//获取 TRAYDATA 信息
		//::ReadProcessMemory(hProc, (LPVOID)tb.dwData, &td, sizeof(TRAYDATA), NULL);
		//printf("句柄：0x%08X，ID：%u，回调消息：0x%08X，图标句柄：0x%08X\n图标提示：\n%s\n\n", td.hWnd, td.uID, td.uCallbackMessage, td.hIcon, td.szTip);
		
		//在屏幕上打印图标
		//printf("尝试在屏幕上打印图标...\n");
		//::InvalidateRect(::GetForegroundWindow(), &CRect(48 * (i / 8) + 10, 48 * (i % 8) + 10, 48, 48), TRUE);
		//::DrawIcon(::GetDC(::GetForegroundWindow()), 48 * (i / 8) + 10, 48 * (i % 8) + 10, td.hIcon);
		//::Sleep(2000);
 
		//显示 / 隐藏图标
		//方法一：发送消息至托盘工具条
		//printf("尝试隐藏托盘图标...\n");
		//::SendMessage(hWnd, TB_HIDEBUTTON, tb.idCommand, MAKELONG(TRUE, 0)); //隐藏图标(留下空白的占用位)
		//::Sleep(2000);
		//printf("尝试恢复托盘图标...\n");
		//::SendMessage(hWnd, TB_HIDEBUTTON, tb.idCommand, MAKELONG(FALSE, 0)); //显示图标
 
		//方法二：填充 NOTIFYICONDATA 结构体，调用 Shell_NotifyIcon 函数
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = td.hWnd;
		nid.uID = td.uID;
		nid.uCallbackMessage = td.uCallbackMessage;
		nid.hIcon = td.hIcon;
		memcpy(nid.szTip, td.szTip, sizeof(nid.szTip));
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		//printf("尝试删除托盘图标...\n");
		//::Shell_NotifyIcon(NIM_DELETE, &nid); //删除图标(注意这里是删除而不是隐藏)
		//::Sleep(2000);
		//printf("尝试恢复托盘图标...\n");
		//::Shell_NotifyIcon(NIM_ADD, &nid); //显示图标
	}
 
	//释放内存
	::VirtualFreeEx(hProc, pTB, sizeof(TBBUTTON), MEM_FREE);
 
	//关闭进程句柄
	::CloseHandle(hProc);
 
	system("Pause");
	return 0;

}

