#include <Windows.h>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv\cvaux.h>

#include "OpenFileDialog.h"
#include "Utils.h"


Utils::Utils(void)
{
}


Utils::~Utils(void)
{
}

void Utils::SetWindow(char* name, HWND hParent, int x, int y) 
{
	HWND hVideo = (HWND)cvGetWindowHandle(name);
	HWND hVideoParent = GetParent(hVideo);
		
	if(hVideoParent != NULL)
	{
		SetWindowLong(hVideoParent,GWL_STYLE,((GetWindowLong(hVideoParent,GWL_STYLE) & ~WS_POPUP) & ~WS_OVERLAPPEDWINDOW ) | WS_CHILD );
		//SetWindowLong(hVideoParent,GWL_EXSTYLE,GetWindowLong(hVideoParent,GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE  & ~WS_EX_WINDOWEDGE  );
		POINT point;
		point.x = x;
		point.y = y;
		bool result = ScreenToClient(hParent, &point);
		if(result)
		{
			SetWindowPos(hVideoParent,HWND_TOP, point.x, point.y, 0, 0, SWP_SHOWWINDOW );

		}
		SetParent(hVideoParent, hParent);

		//SetWindowPos(hVideoParent,HWND_TOP, 0, 0, 100, 100, SWP_SHOWWINDOW);
	}
}

char* Utils::GetNativeFile() 
{
	char currentDir[100];
	GetCurrentDirectory(100, currentDir);
	char* fileName;
	OpenFileDialog* openFileDialog = new OpenFileDialog();		
	openFileDialog->FilterIndex = 1;

	openFileDialog->Title = "Open Video File";

	if (openFileDialog->ShowDialog())
	{
		fileName = openFileDialog->FileName;
		if (fileName == 0)
		{
			MessageBox(NULL, "Not Open file", "Warning", MB_OK);
			SetCurrentDirectory(currentDir);
			return 0;
		}				
		else 
		{
			SetCurrentDirectory(currentDir);
			return fileName;
		}
	}
	else
	{
		MessageBox(NULL, "Not Open file", "Error", MB_OK);
		SetCurrentDirectory(currentDir);
		return 0;
	}
}

BOOL Utils::SaveBitmapToFile(LPCTSTR lpszFilePath, HBITMAP hBm)
{
    //  定义位图文件表头
    BITMAPFILEHEADER bmfh;
    //  定义位图信息表头
    BITMAPINFOHEADER bmih;

    //  调色板长度
    int nColorLen = 0;
    //  调色表大小
    DWORD dwRgbQuadSize = 0;
    //  位图大小
    DWORD dwBmSize = 0;
    //  分配内存的指针
    HGLOBAL    hMem = NULL;

    LPBITMAPINFOHEADER     lpbi;

    BITMAP bm;

    HDC hDC;

    HANDLE hFile = NULL;

    DWORD dwWritten;

    GetObject(hBm, sizeof(BITMAP), &bm);

    bmih.biSize    = sizeof(BITMAPINFOHEADER);    // 本结构所占的字节
    bmih.biWidth    = bm.bmWidth;            // 位图宽
    bmih.biHeight    = bm.bmHeight;            // 位图高
    bmih.biPlanes    = 1;
    bmih.biBitCount        = bm.bmBitsPixel;    // 每一图素的位数
    bmih.biCompression    = BI_RGB;            // 不压缩
    bmih.biSizeImage        = 0;  //  位图大小
    bmih.biXPelsPerMeter    = 0;
    bmih.biYPelsPerMeter    = 0;
    bmih.biClrUsed        = 0;
    bmih.biClrImportant    = 0;

    //  计算位图图素数据区大小 
    dwBmSize = 4 * ((bm.bmWidth * bmih.biBitCount + 31) / 32) * bm.bmHeight;

    //  如果图素位 <= 8bit，则有调色板
    if (bmih.biBitCount <= 8)
    {
        nColorLen = (1 << bm.bmBitsPixel);
    }

    //  计算调色板大小
    dwRgbQuadSize = nColorLen * sizeof(RGBQUAD);

    //  分配内存
    hMem = GlobalAlloc(GHND, dwBmSize + dwRgbQuadSize + sizeof(BITMAPINFOHEADER));

    if (NULL == hMem)
    {
        return FALSE;
    }

    //  锁定内存
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hMem);

    //  将bmih中的内容写入分配的内存中
    *lpbi = bmih;


    hDC= GetDC(NULL);

    //  将位图中的数据以bits的形式放入lpData中。
    GetDIBits(hDC, hBm, 0, (DWORD)bmih.biHeight, (LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwRgbQuadSize, (BITMAPINFO *)lpbi, (DWORD)DIB_RGB_COLORS);
    
    bmfh.bfType = 0x4D42;  // 位图文件类型：BM
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize + dwBmSize;  // 位图大小
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwRgbQuadSize;  // 位图数据与文件头部的偏移量

    //  把上面的数据写入文件中

    hFile = CreateFile(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return FALSE;
    }

    //  写入位图文件头
    WriteFile(hFile, (LPSTR)&bmfh, sizeof(BITMAPFILEHEADER), (DWORD *)&dwWritten, NULL);
    //  写入位图数据
    WriteFile(hFile, (LPBITMAPINFOHEADER)lpbi, bmfh.bfSize - sizeof(BITMAPFILEHEADER), (DWORD *)&dwWritten, NULL);

    GlobalFree(hMem);
    CloseHandle(hFile);

    return TRUE;
}



