#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>
#include <WinUser.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int main(int agrc, char *argv[])
{
	LPMSG msg = NULL;
	while (GetMessage(msg, NULL, 0, 0))
	{
		printf("asd\n");
	}
	return 1;
}