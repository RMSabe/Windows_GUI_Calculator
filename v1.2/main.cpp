/*
	Windows GUI Calculator
	Version 1.2

	Author: Rafael Sabe
	Email: rafaelmsabe@gmail.com
*/

#include "globldef.h"
#include "strdef.h"
#include "math_handler.h"
#include "strdef.hpp"

#define MAINWND_WNDCLASS_NAME TEXT("__MAINWNDCLASS__")
#define MAINWND_CAPTION TEXT("Calculator")

#define RUNTIME_STATUS_INIT 0
#define RUNTIME_STATUS_IDLE 1
#define RUNTIME_STATUS_MAIN0 2
#define RUNTIME_STATUS_MAIN1 3
#define RUNTIME_STATUS_MAIN2 4
#define RUNTIME_STATUS_ENTERFIRST 5
#define RUNTIME_STATUS_ENTERFINAL 6
#define RUNTIME_STATUS_RESULT 7

#define CUSTOMCOLOR_BLACK 0x00000000
#define CUSTOMCOLOR_WHITE 0x00ffffff
#define CUSTOMCOLOR_LTGRAY 0x00c0c0c0

#define CUSTOMBRUSHINDEX_TRANSPARENT 0U
#define CUSTOMBRUSHINDEX_SOLID_BLACK 1U
#define CUSTOMBRUSHINDEX_SOLID_WHITE 2U
#define CUSTOMBRUSHINDEX_SOLID_LTGRAY 3U

#define PP_CUSTOMBRUSH_LENGTH 4U
#define PP_CUSTOMBRUSH_SIZE (PP_CUSTOMBRUSH_LENGTH*sizeof(VOID*))

#define CUSTOMFONT_TITLE_CHARSET DEFAULT_CHARSET
#define CUSTOMFONT_TITLE_WIDTH 20
#define CUSTOMFONT_TITLE_HEIGHT 35
#define CUSTOMFONT_TITLE_WEIGHT FW_NORMAL

#define CUSTOMFONT_TEXT_CHARSET DEFAULT_CHARSET
#define CUSTOMFONT_TEXT_WIDTH 10
#define CUSTOMFONT_TEXT_HEIGHT 20
#define CUSTOMFONT_TEXT_WEIGHT FW_NORMAL

#define CUSTOMFONTINDEX_TITLE 0U
#define CUSTOMFONTINDEX_TEXT 1U

#define PP_CUSTOMFONT_LENGTH 2U
#define PP_CUSTOMFONT_SIZE (PP_CUSTOMFONT_LENGTH*sizeof(VOID*))

#define MAINWND_BKCOLOR CUSTOMCOLOR_LTGRAY
#define MAINWND_BRUSHINDEX CUSTOMBRUSHINDEX_SOLID_LTGRAY

#define TEXTWND_TEXTCOLOR CUSTOMCOLOR_BLACK
#define TEXTWND_BKCOLOR MAINWND_BKCOLOR
#define TEXTWND_BRUSHINDEX CUSTOMBRUSHINDEX_TRANSPARENT

#define TEXTBOXWND_TEXTCOLOR CUSTOMCOLOR_BLACK
#define TEXTBOXWND_BKCOLOR CUSTOMCOLOR_WHITE
#define TEXTBOXWND_BRUSHINDEX CUSTOMBRUSHINDEX_SOLID_WHITE

INT runtime_status = -1;
INT prev_status = -1;

VOID **pp_custombrush = nullptr;
VOID **pp_customfont = nullptr;

HWND p_mainwnd = nullptr;
HWND p_text1 = nullptr;
HWND p_text2 = nullptr;
HWND p_textbox1 = nullptr;
HWND p_button1 = nullptr;
HWND p_button2 = nullptr;
HWND p_button3 = nullptr;
HWND p_button4 = nullptr;
HWND p_button5 = nullptr;
HWND p_button6 = nullptr;

WORD mainwnd_wndclass_id = 0u;

extern BOOL WINAPI app_init(VOID);
extern VOID WINAPI app_deinit(VOID);

extern BOOL WINAPI custom_gdiobj_init(VOID);
extern VOID WINAPI custom_gdiobj_deinit(VOID);

extern BOOL WINAPI register_mainwnd_wndclass(VOID);
extern BOOL WINAPI create_mainwnd(VOID);
extern BOOL WINAPI create_ctrls(VOID);

extern INT WINAPI app_get_ref_status(VOID);

extern VOID WINAPI runtime_loop(VOID);

extern VOID WINAPI paintscreen_main(VOID);
extern VOID WINAPI paintscreen_enternumber(VOID);
extern VOID WINAPI paintscreen_result(VOID);

extern VOID WINAPI text_choose_font(VOID);
extern VOID WINAPI text_align(VOID);
extern VOID WINAPI textbox_align(VOID);
extern VOID WINAPI button_align(VOID);
extern VOID WINAPI ctrls_setup(BOOL text_font_setup);

extern BOOL WINAPI window_get_dimensions(HWND p_wnd, INT *p_xpos, INT *p_ypos, INT *p_width, INT *p_height, INT *p_centerx, INT *p_centery);
extern BOOL WINAPI textbox1_capture_text(VOID);

extern BOOL WINAPI catch_messages(VOID);

extern LRESULT CALLBACK mainwnd_wndproc(HWND p_wnd, UINT msg, WPARAM wparam, LPARAM lparam);

extern LRESULT CALLBACK mainwnd_event_wmcommand(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmpaint(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmctlcolorstatic(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmctlcoloredit(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmsize(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmdestroy(HWND p_wnd, WPARAM wparam, LPARAM lparam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	p_instance = hInstance;

	if(!app_init())
	{
		app_deinit();
		return 1;
	}

	runtime_loop();

	app_deinit();
	return 0;
}

BOOL WINAPI app_init(VOID)
{
	if(p_instance == nullptr)
	{
		MessageBox(NULL, TEXT("Error: Invalid Instance."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	p_processheap = GetProcessHeap();
	if(p_processheap == nullptr)
	{
		MessageBox(NULL, TEXT("Error: Invalid Process Heap."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!custom_gdiobj_init())
	{
		MessageBox(NULL, TEXT("Error: GDIOBJ Init Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!register_mainwnd_wndclass())
	{
		MessageBox(NULL, TEXT("Error: Register WNDCLASS Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!create_mainwnd())
	{
		MessageBox(NULL, TEXT("Error: Create MAINWND Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!create_ctrls())
	{
		MessageBox(NULL, TEXT("Error: Create CTRLS Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	runtime_status = RUNTIME_STATUS_INIT;
	return TRUE;
}

VOID WINAPI app_deinit(VOID)
{
	if(p_mainwnd != nullptr) DestroyWindow(p_mainwnd);

	if(mainwnd_wndclass_id)
	{
		UnregisterClass(MAINWND_WNDCLASS_NAME, p_instance);
		mainwnd_wndclass_id = 0u;
	}

	custom_gdiobj_deinit();
	return;
}

BOOL WINAPI custom_gdiobj_init(VOID)
{
	SIZE_T n_obj = 0u;
	LOGFONT logfont;

	//Init custom brush

	pp_custombrush = (VOID**) HeapAlloc(p_processheap, HEAP_ZERO_MEMORY, PP_CUSTOMBRUSH_SIZE);
	if(pp_custombrush == nullptr) return FALSE;

	pp_custombrush[CUSTOMBRUSHINDEX_TRANSPARENT] = GetStockObject(HOLLOW_BRUSH);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_BLACK] = CreateSolidBrush(CUSTOMCOLOR_BLACK);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_WHITE] = CreateSolidBrush(CUSTOMCOLOR_WHITE);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_LTGRAY] = CreateSolidBrush(CUSTOMCOLOR_LTGRAY);

	for(n_obj = 0u; n_obj < PP_CUSTOMBRUSH_LENGTH; n_obj++) if(pp_custombrush[n_obj] == nullptr) return FALSE;

	//Init custom font

	pp_customfont = (VOID**) HeapAlloc(p_processheap, HEAP_ZERO_MEMORY, PP_CUSTOMFONT_SIZE);
	if(pp_customfont == nullptr) return FALSE;

	ZeroMemory(&logfont, sizeof(LOGFONT));

	logfont.lfCharSet = CUSTOMFONT_TITLE_CHARSET;
	logfont.lfWidth = CUSTOMFONT_TITLE_WIDTH;
	logfont.lfHeight = CUSTOMFONT_TITLE_HEIGHT;
	logfont.lfWeight = CUSTOMFONT_TITLE_WEIGHT;

	pp_customfont[CUSTOMFONTINDEX_TITLE] = CreateFontIndirect(&logfont);

	logfont.lfCharSet = CUSTOMFONT_TEXT_CHARSET;
	logfont.lfWidth = CUSTOMFONT_TEXT_WIDTH;
	logfont.lfHeight = CUSTOMFONT_TEXT_HEIGHT;
	logfont.lfWeight = CUSTOMFONT_TEXT_WEIGHT;

	pp_customfont[CUSTOMFONTINDEX_TEXT] = CreateFontIndirect(&logfont);

	for(n_obj = 0u; n_obj < PP_CUSTOMFONT_LENGTH; n_obj++) if(pp_customfont[n_obj] == nullptr) return FALSE;

	return TRUE;
}

VOID WINAPI custom_gdiobj_deinit(VOID)
{
	SIZE_T n_obj = 0u;

	if(p_processheap == nullptr) return;

	if(pp_custombrush != nullptr)
	{
		for(n_obj = 0u; n_obj < PP_CUSTOMBRUSH_LENGTH; n_obj++)
		{
			if(pp_custombrush[n_obj] != nullptr)
			{
				DeleteObject(pp_custombrush[n_obj]);
				pp_custombrush[n_obj] = nullptr;
			}
		}

		HeapFree(p_processheap, 0, pp_custombrush);
		pp_custombrush = nullptr;
	}

	if(pp_customfont != nullptr)
	{
		for(n_obj = 0u; n_obj < PP_CUSTOMFONT_LENGTH; n_obj++)
		{
			if(pp_customfont[n_obj] != nullptr)
			{
				DeleteObject(pp_customfont[n_obj]);
				pp_customfont[n_obj] = nullptr;
			}
		}

		HeapFree(p_processheap, 0, pp_customfont);
		pp_customfont = nullptr;
	}

	return;
}

BOOL WINAPI register_mainwnd_wndclass(VOID)
{
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));

	wndclass.style = CS_OWNDC;
	wndclass.lpfnWndProc = &mainwnd_wndproc;
	wndclass.hInstance = p_instance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) pp_custombrush[MAINWND_BRUSHINDEX];
	wndclass.lpszClassName = MAINWND_WNDCLASS_NAME;

	mainwnd_wndclass_id = RegisterClass(&wndclass);

	return (BOOL) mainwnd_wndclass_id;
}

BOOL WINAPI create_mainwnd(VOID)
{
	DWORD style = (WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);

	p_mainwnd = CreateWindow(MAINWND_WNDCLASS_NAME, MAINWND_CAPTION, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, NULL, NULL, p_instance, NULL);
	return (p_mainwnd != nullptr);
}

BOOL WINAPI create_ctrls(VOID)
{
	DWORD style = 0u;

	style = (WS_CHILD | SS_CENTER);
	p_text1 = CreateWindow(TEXT("STATIC"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_text2 = CreateWindow(TEXT("STATIC"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);

	style = (WS_CHILD | WS_TABSTOP | ES_CENTER);
	p_textbox1 = CreateWindow(TEXT("EDIT"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);

	style = (WS_CHILD | WS_TABSTOP | BS_CENTER | BS_VCENTER | BS_TEXT | BS_PUSHBUTTON);
	p_button1 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_button2 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_button3 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_button4 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_button5 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_button6 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);

	if(p_text1 == nullptr) return FALSE;
	if(p_text2 == nullptr) return FALSE;
	if(p_textbox1 == nullptr) return FALSE;
	if(p_button1 == nullptr) return FALSE;
	if(p_button2 == nullptr) return FALSE;
	if(p_button3 == nullptr) return FALSE;
	if(p_button4 == nullptr) return FALSE;
	if(p_button5 == nullptr) return FALSE;
	if(p_button6 == nullptr) return FALSE;

	return TRUE;
}

INT WINAPI app_get_ref_status(VOID)
{
	if(runtime_status == RUNTIME_STATUS_IDLE) return prev_status;

	return runtime_status;
}

VOID WINAPI runtime_loop(VOID)
{
	while(catch_messages())
	{
		switch(runtime_status)
		{
			case RUNTIME_STATUS_IDLE:
				Sleep(10);
				break;

			case RUNTIME_STATUS_INIT:
				text_choose_font();
				text_align();
				textbox_align();
				runtime_status = RUNTIME_STATUS_MAIN0;

			case RUNTIME_STATUS_MAIN0:
			case RUNTIME_STATUS_MAIN1:
			case RUNTIME_STATUS_MAIN2:

				paintscreen_main();
				break;

			case RUNTIME_STATUS_ENTERFIRST:
			case RUNTIME_STATUS_ENTERFINAL:

				paintscreen_enternumber();
				break;

			case RUNTIME_STATUS_RESULT:
				paintscreen_result();
				break;
		}

		if(runtime_status != RUNTIME_STATUS_IDLE)
		{
			prev_status = runtime_status;
			runtime_status = RUNTIME_STATUS_IDLE;
		}
	}

	return;
}

VOID WINAPI paintscreen_main(VOID)
{
	ShowWindow(p_text1, SW_HIDE);
	ShowWindow(p_text2, SW_HIDE);
	ShowWindow(p_textbox1, SW_HIDE);
	ShowWindow(p_button1, SW_HIDE);
	ShowWindow(p_button2, SW_HIDE);
	ShowWindow(p_button3, SW_HIDE);
	ShowWindow(p_button4, SW_HIDE);
	ShowWindow(p_button5, SW_HIDE);
	ShowWindow(p_button6, SW_HIDE);

	button_align();

	SendMessage(p_text1, WM_SETTEXT, 0, (LPARAM) TEXT("Choose An Operation"));

	SendMessage(p_button5, WM_SETTEXT, 0, (LPARAM) TEXT("<-- Prev Page"));
	SendMessage(p_button6, WM_SETTEXT, 0, (LPARAM) TEXT("Next Page -->"));

	switch(runtime_status)
	{
		case RUNTIME_STATUS_MAIN0:
			SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Sum"));
			SendMessage(p_button2, WM_SETTEXT, 0, (LPARAM) TEXT("Subtraction"));
			SendMessage(p_button3, WM_SETTEXT, 0, (LPARAM) TEXT("Multiplication"));
			SendMessage(p_button4, WM_SETTEXT, 0, (LPARAM) TEXT("Division"));

			ShowWindow(p_button6, SW_SHOW);
			break;

		case RUNTIME_STATUS_MAIN1:
			SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Power"));
			SendMessage(p_button2, WM_SETTEXT, 0, (LPARAM) TEXT("Root"));
			SendMessage(p_button3, WM_SETTEXT, 0, (LPARAM) TEXT("Logarithm"));
			SendMessage(p_button4, WM_SETTEXT, 0, (LPARAM) TEXT("Sine"));

			ShowWindow(p_button5, SW_SHOW);
			ShowWindow(p_button6, SW_SHOW);
			break;

		case RUNTIME_STATUS_MAIN2:
			SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Cosine"));
			SendMessage(p_button2, WM_SETTEXT, 0, (LPARAM) TEXT("Tangent"));
			SendMessage(p_button3, WM_SETTEXT, 0, (LPARAM) TEXT("Factorial"));
			SendMessage(p_button4, WM_SETTEXT, 0, (LPARAM) TEXT("Newton Binomial"));

			ShowWindow(p_button5, SW_SHOW);
			break;
	}

	ShowWindow(p_text1, SW_SHOW);
	ShowWindow(p_button1, SW_SHOW);
	ShowWindow(p_button2, SW_SHOW);
	ShowWindow(p_button3, SW_SHOW);
	ShowWindow(p_button4, SW_SHOW);

	return;
}

VOID WINAPI paintscreen_enternumber(VOID)
{
	ShowWindow(p_text1, SW_HIDE);
	ShowWindow(p_text2, SW_HIDE);
	ShowWindow(p_textbox1, SW_HIDE);
	ShowWindow(p_button1, SW_HIDE);
	ShowWindow(p_button2, SW_HIDE);
	ShowWindow(p_button3, SW_HIDE);
	ShowWindow(p_button4, SW_HIDE);
	ShowWindow(p_button5, SW_HIDE);
	ShowWindow(p_button6, SW_HIDE);

	button_align();

	if(ang_unit) __SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Current Unit: Degrees"));
	else __SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Current Unit: Radians"));

	SendMessage(p_button3, WM_SETTEXT, 0, (LPARAM) TEXT("Toggle Ang Unit"));
	SendMessage(p_text2, WM_SETTEXT, 0, (LPARAM) textbuf);

	SendMessage(p_textbox1, WM_SETTEXT, 0, (LPARAM) TEXT(""));

	switch(runtime_status)
	{
		case RUNTIME_STATUS_ENTERFIRST:
			switch(math_op)
			{
				case MATHOP_SUM:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Sum \"a + b\". Enter value of \"a\":"));
					break;

				case MATHOP_SUB:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Subtraction \"a - b\". Enter value of \"a\":"));
					break;

				case MATHOP_MULT:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Multiplication \"a * b\". Enter value of \"a\":"));
					break;

				case MATHOP_DIV:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Division \"a / b\". Enter value of \"a\":"));
					break;

				case MATHOP_POW:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Power \"b\" of \"a\". Enter value of \"a\":"));
					break;

				case MATHOP_ROOT:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Root \"b\" of \"a\". Enter value of \"a\":"));
					break;

				case MATHOP_LOG:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Logarithm \"b\" of \"a\". Enter value of \"a\":"));
					break;

				case MATHOP_BIN:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Newton Binomial \"a\" to \"b\". Enter value of \"a\":"));
					break;
			}
			break;

		case RUNTIME_STATUS_ENTERFINAL:
			switch(math_op)
			{
				case MATHOP_SUM:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Sum \"a + b\". Enter value of \"b\":"));
					break;

				case MATHOP_SUB:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Subtraction \"a - b\". Enter value of \"b\":"));
					break;

				case MATHOP_MULT:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Multiplication \"a * b\". Enter value of \"b\":"));
					break;

				case MATHOP_DIV:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Division \"a / b\". Enter value of \"b\":"));
					break;

				case MATHOP_POW:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Power \"b\" of \"a\". Enter value of \"b\":"));
					break;

				case MATHOP_ROOT:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Root \"b\" of \"a\". Enter value of \"b\":"));
					break;

				case MATHOP_LOG:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Logarithm \"b\" of \"a\". Enter value of \"b\":"));
					break;

				case MATHOP_SIN:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Sine of \"a\". Enter value of \"a\":"));
					break;

				case MATHOP_COS:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Cosine of \"a\". Enter value of \"a\":"));
					break;

				case MATHOP_TAN:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Tangent of \"a\". Enter value of \"a\":"));
					break;

				case MATHOP_FAC:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Factorial \"a!\". Enter value of \"a\":"));
					break;

				case MATHOP_BIN:
					__SPRINTF(textbuf, TEXTBUF_SIZE_CHARS, TEXT("Newton Binomial \"a\" to \"b\". Enter value of \"b\":"));
					break;
			}
			break;
	}

	SendMessage(p_text1, WM_SETTEXT, 0, (LPARAM) textbuf);
	SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Return"));
	SendMessage(p_button2, WM_SETTEXT, 0, (LPARAM) TEXT("Proceed"));

	ShowWindow(p_text1, SW_SHOW);
	ShowWindow(p_textbox1, SW_SHOW);
	ShowWindow(p_button1, SW_SHOW);
	ShowWindow(p_button2, SW_SHOW);

	if(math_op_uses_ang_unit(math_op))
	{
		ShowWindow(p_text2, SW_SHOW);
		ShowWindow(p_button3, SW_SHOW);
	}

	return;
}

VOID WINAPI paintscreen_result(VOID)
{
	ShowWindow(p_text1, SW_HIDE);
	ShowWindow(p_text2, SW_HIDE);
	ShowWindow(p_textbox1, SW_HIDE);
	ShowWindow(p_button1, SW_HIDE);
	ShowWindow(p_button2, SW_HIDE);
	ShowWindow(p_button3, SW_HIDE);
	ShowWindow(p_button4, SW_HIDE);
	ShowWindow(p_button5, SW_HIDE);
	ShowWindow(p_button6, SW_HIDE);

	button_align();

	SendMessage(p_text1, WM_SETTEXT, 0, (LPARAM) TEXT("Result:"));
	SendMessage(p_text2, WM_SETTEXT, 0, (LPARAM) textbuf);
	SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Return"));

	ShowWindow(p_text1, SW_SHOW);
	ShowWindow(p_text2, SW_SHOW);
	ShowWindow(p_button1, SW_SHOW);

	return;
}

VOID WINAPI text_choose_font(VOID)
{
	INT mainwnd_width = 0;
	INT mainwnd_height = 0;

	window_get_dimensions(p_mainwnd, nullptr, nullptr, &mainwnd_width, &mainwnd_height, nullptr, nullptr);

	if((mainwnd_width < 640) || (mainwnd_height < 480))
	{
		SendMessage(p_text1, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_TEXT], (LPARAM) TRUE);
		SendMessage(p_text2, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_TEXT], (LPARAM) TRUE);
		SendMessage(p_textbox1, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_TEXT], (LPARAM) TRUE);
	}
	else
	{
		SendMessage(p_text1, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_TITLE], (LPARAM) TRUE);
		SendMessage(p_text2, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_TITLE], (LPARAM) TRUE);
		SendMessage(p_textbox1, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_TITLE], (LPARAM) TRUE);
	}

	return;
}

VOID WINAPI text_align(VOID)
{
	INT mainwnd_width = 0;
	INT mainwnd_height = 0;

	INT text1_xpos = 0;
	INT text1_ypos = 0;
	INT text1_width = 0;
	INT text1_height = 0;

	INT text2_xpos = 0;
	INT text2_ypos = 0;
	INT text2_width = 0;
	INT text2_height = 0;

	BOOL small_wnd = FALSE;

	window_get_dimensions(p_mainwnd, nullptr, nullptr, &mainwnd_width, &mainwnd_height, nullptr, nullptr);
	small_wnd = ((mainwnd_width < 640) || (mainwnd_height < 480));

	if(small_wnd)
	{
		text1_xpos = 20;
		text1_ypos = 20;
		text1_height = CUSTOMFONT_TEXT_HEIGHT;

		text2_xpos = 20;
		text2_ypos = text1_ypos + CUSTOMFONT_TEXT_HEIGHT + 10;
		text2_height = CUSTOMFONT_TEXT_HEIGHT;

	}
	else
	{
		text1_xpos = 40;
		text1_ypos = 40;
		text1_height = CUSTOMFONT_TITLE_HEIGHT;

		text2_xpos = 40;
		text2_ypos = text1_ypos + CUSTOMFONT_TITLE_HEIGHT + 10;
		text2_height = CUSTOMFONT_TITLE_HEIGHT;
	}

	text1_width = mainwnd_width - 2*text1_xpos;
	text2_width = mainwnd_width - 2*text2_xpos;

	SetWindowPos(p_text1, NULL, text1_xpos, text1_ypos, text1_width, text1_height, 0);
	SetWindowPos(p_text2, NULL, text2_xpos, text2_ypos, text2_width, text2_height, 0);
	return;
}

VOID WINAPI textbox_align(VOID)
{
	INT mainwnd_width = 0;
	INT mainwnd_height = 0;

	INT text2_ypos = 0;
	INT text2_height = 0;

	INT textbox1_xpos = 0;
	INT textbox1_ypos = 0;
	INT textbox1_width = 0;
	INT textbox1_height = 0;

	BOOL small_wnd = FALSE;

	window_get_dimensions(p_mainwnd, nullptr, nullptr, &mainwnd_width, &mainwnd_height, nullptr, nullptr);
	window_get_dimensions(p_text2, nullptr, &text2_ypos, nullptr, &text2_height, nullptr, nullptr);

	small_wnd = ((mainwnd_width < 640) || (mainwnd_height < 480));

	if(small_wnd)
	{
		textbox1_xpos = 20;
		textbox1_height = CUSTOMFONT_TEXT_HEIGHT;
	}
	else
	{
		textbox1_xpos = 40;
		textbox1_height = CUSTOMFONT_TITLE_HEIGHT;
	}

	textbox1_ypos = text2_ypos + text2_height + 10;
	textbox1_width = mainwnd_width - 2*textbox1_xpos;

	SetWindowPos(p_textbox1, NULL, textbox1_xpos, textbox1_ypos, textbox1_width, textbox1_height, 0);
	return;
}

VOID WINAPI button_align(VOID)
{
	const INT BUTTON_HEIGHT = 20;

	INT mainwnd_width = 0;
	INT mainwnd_height = 0;
	INT mainwnd_centerx = 0;
	INT mainwnd_centery = 0;

	INT ref_status = -1;
	INT margin_bottom = 0;

	INT *p_button_dimensions = nullptr;

	BOOL small_wnd = FALSE;

	ref_status = app_get_ref_status();
	window_get_dimensions(p_mainwnd, nullptr, nullptr, &mainwnd_width, &mainwnd_height, &mainwnd_centerx, &mainwnd_centery);

	small_wnd = ((mainwnd_width < 640) || (mainwnd_height < 480));

	p_button_dimensions = (INT*) HeapAlloc(p_processheap, HEAP_ZERO_MEMORY, 24*sizeof(INT));

	if(small_wnd) margin_bottom = 60;
	else margin_bottom = 100;

	switch(ref_status)
	{
		case RUNTIME_STATUS_MAIN0:
		case RUNTIME_STATUS_MAIN1:
		case RUNTIME_STATUS_MAIN2:

			//buttons width
			p_button_dimensions[2] = 120;
			p_button_dimensions[6] = 120;
			p_button_dimensions[10] = 120;
			p_button_dimensions[14] = 120;
			p_button_dimensions[18] = 120;
			p_button_dimensions[22] = 120;

			//buttons height
			p_button_dimensions[3] = BUTTON_HEIGHT;
			p_button_dimensions[7] = BUTTON_HEIGHT;
			p_button_dimensions[11] = BUTTON_HEIGHT;
			p_button_dimensions[15] = BUTTON_HEIGHT;
			p_button_dimensions[19] = BUTTON_HEIGHT;
			p_button_dimensions[23] = BUTTON_HEIGHT;

			p_button_dimensions[17] = mainwnd_height - margin_bottom - BUTTON_HEIGHT;	//button 5 ypos
			p_button_dimensions[21] = p_button_dimensions[17];				//button 6 ypos

			p_button_dimensions[9] = p_button_dimensions[17] - BUTTON_HEIGHT - 10;		//button 3 ypos
			p_button_dimensions[13] = p_button_dimensions[9];				//button 4 ypos

			if(small_wnd)
			{
				p_button_dimensions[0] = mainwnd_centerx - p_button_dimensions[2] - 10;			//button 1 xpos = centerx - button 1 width - 10
				p_button_dimensions[4] = mainwnd_centerx + 10;						//button 2 xpos = centerx + 10

				p_button_dimensions[8] = p_button_dimensions[0];					//button 3 xpos = button 1 xpos
				p_button_dimensions[12] = p_button_dimensions[4];					//button 4 xpos = button 2 xpos

				p_button_dimensions[1] = p_button_dimensions[9] - BUTTON_HEIGHT - 10;			//button 1 ypos = button 3 ypos - 30
				p_button_dimensions[5] = p_button_dimensions[1];					//button 2 ypos = button 1 ypos
			}
			else
			{
				p_button_dimensions[4] = mainwnd_centerx - p_button_dimensions[6] - 10;			//button 2 xpos = centerx - button 2 width - 10
				p_button_dimensions[8] = mainwnd_centerx + 10;						//button 3 xpos = centerx + 10

				p_button_dimensions[0] = p_button_dimensions[4] - p_button_dimensions[2] - 20;		//button 1 xpos = button 2 xpos - button 1 width - 20
				p_button_dimensions[12] = p_button_dimensions[8] + p_button_dimensions[10] + 20;	//button 4 xpos = button 3 xpos + button 3 width + 20

				p_button_dimensions[1] = p_button_dimensions[9];					//button 1 ypos = button 3 & button 4 ypos
				p_button_dimensions[5] = p_button_dimensions[1];					//button 2 ypos = button 1 ypos
			}

			switch(ref_status)
			{
				case RUNTIME_STATUS_MAIN0:
					p_button_dimensions[20] = mainwnd_centerx - p_button_dimensions[22]/2;		//button 6 xpos = centerx - button 6 width/2
					break;

				case RUNTIME_STATUS_MAIN1:
					p_button_dimensions[16] = mainwnd_centerx - p_button_dimensions[18] - 10;	//button 5 xpos = centerx - button 5 width - 10
					p_button_dimensions[20] = mainwnd_centerx + 10;					//button 6 xpos = centerx + 10
					break;

				case RUNTIME_STATUS_MAIN2:
					p_button_dimensions[16] = mainwnd_centerx - p_button_dimensions[18]/2;		//button 5 xpos = centerx - button 5 width/2
					break;
			}
			break;

		case RUNTIME_STATUS_ENTERFIRST:
		case RUNTIME_STATUS_ENTERFINAL:
			p_button_dimensions[3] = BUTTON_HEIGHT;
			p_button_dimensions[7] = BUTTON_HEIGHT;
			p_button_dimensions[11] = BUTTON_HEIGHT;

			if(math_op_uses_ang_unit(math_op))
			{
				p_button_dimensions[2] = 120;
				p_button_dimensions[6] = 120;
				p_button_dimensions[10] = 120;

				p_button_dimensions[9] = mainwnd_height - margin_bottom - BUTTON_HEIGHT;
				p_button_dimensions[5] = p_button_dimensions[9] - BUTTON_HEIGHT - 10;
				p_button_dimensions[1] = p_button_dimensions[5] - BUTTON_HEIGHT - 10;
			}
			else
			{
				p_button_dimensions[2] = 80;
				p_button_dimensions[6] = 80;

				p_button_dimensions[5] = mainwnd_height - margin_bottom - BUTTON_HEIGHT;
				p_button_dimensions[1] = p_button_dimensions[5] - BUTTON_HEIGHT - 10;
			}

			p_button_dimensions[0] = mainwnd_centerx - p_button_dimensions[2]/2;
			p_button_dimensions[4] = mainwnd_centerx - p_button_dimensions[6]/2;
			p_button_dimensions[8] = mainwnd_centerx - p_button_dimensions[10]/2;
			break;

		case RUNTIME_STATUS_RESULT:
			p_button_dimensions[2] = 100;
			p_button_dimensions[3] = BUTTON_HEIGHT;

			p_button_dimensions[0] = mainwnd_centerx - p_button_dimensions[2]/2;
			p_button_dimensions[1] = mainwnd_height - margin_bottom - BUTTON_HEIGHT;
			break;
	}

	SetWindowPos(p_button1, NULL, p_button_dimensions[0], p_button_dimensions[1], p_button_dimensions[2], p_button_dimensions[3], 0);
	SetWindowPos(p_button2, NULL, p_button_dimensions[4], p_button_dimensions[5], p_button_dimensions[6], p_button_dimensions[7], 0);
	SetWindowPos(p_button3, NULL, p_button_dimensions[8], p_button_dimensions[9], p_button_dimensions[10], p_button_dimensions[11], 0);
	SetWindowPos(p_button4, NULL, p_button_dimensions[12], p_button_dimensions[13], p_button_dimensions[14], p_button_dimensions[15], 0);
	SetWindowPos(p_button5, NULL, p_button_dimensions[16], p_button_dimensions[17], p_button_dimensions[18], p_button_dimensions[19], 0);
	SetWindowPos(p_button6, NULL, p_button_dimensions[20], p_button_dimensions[21], p_button_dimensions[22], p_button_dimensions[23], 0);

	if(p_button_dimensions != nullptr)
	{
		HeapFree(p_processheap, 0, p_button_dimensions);
		p_button_dimensions = nullptr;
	}

	return;
}

VOID WINAPI ctrls_setup(BOOL text_font_setup)
{
	if(text_font_setup) text_choose_font();

	text_align();
	textbox_align();
	button_align();
	return;
}

BOOL WINAPI window_get_dimensions(HWND p_wnd, INT *p_xpos, INT *p_ypos, INT *p_width, INT *p_height, INT *p_centerx, INT *p_centery)
{
	RECT rect;

	if(p_wnd == nullptr) return FALSE;

	if(!GetWindowRect(p_wnd, &rect)) return FALSE;

	if(p_xpos != nullptr) *p_xpos = rect.left;
	if(p_ypos != nullptr) *p_ypos = rect.top;
	if(p_width != nullptr) *p_width = rect.right - rect.left;
	if(p_height != nullptr) *p_height = rect.bottom - rect.top;
	if(p_centerx != nullptr) *p_centerx = (rect.right - rect.left)/2;
	if(p_centery != nullptr) *p_centery = (rect.bottom - rect.top)/2;

	return TRUE;
}

BOOL WINAPI textbox1_capture_text(VOID)
{
	DOUBLE *p_double = nullptr;
	__string str = TEXT("");

	switch(prev_status)
	{
		case RUNTIME_STATUS_ENTERFIRST:
			p_double = &first_input;
			break;

		case RUNTIME_STATUS_ENTERFINAL:
			p_double = &final_input;
			break;
	}

	SendMessage(p_textbox1, WM_GETTEXT, (WPARAM) TEXTBUF_SIZE_CHARS, (LPARAM) textbuf);
	textbuf[TEXTBUF_SIZE_CHARS - 1u] = (TCHAR) '\0';

	str = textbuf;

	try
	{
		*p_double = std::stod(str);
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL WINAPI catch_messages(VOID)
{
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT) return FALSE;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

LRESULT CALLBACK mainwnd_wndproc(HWND p_wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_COMMAND:
			return mainwnd_event_wmcommand(p_wnd, wparam, lparam);

		case WM_PAINT:
			return mainwnd_event_wmpaint(p_wnd, wparam, lparam);

		case WM_CTLCOLORSTATIC:
			return mainwnd_event_wmctlcolorstatic(p_wnd, wparam, lparam);

		case WM_CTLCOLOREDIT:
			return mainwnd_event_wmctlcoloredit(p_wnd, wparam, lparam);

		case WM_SIZE:
			return mainwnd_event_wmsize(p_wnd, wparam, lparam);

		case WM_DESTROY:
			return mainwnd_event_wmdestroy(p_wnd, wparam, lparam);
	}

	return DefWindowProc(p_wnd, msg, wparam, lparam);
}

LRESULT CALLBACK mainwnd_event_wmcommand(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	if(p_wnd == nullptr) return 0;
	if(lparam == 0) return 0;

	if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button1))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_MAIN0:
				math_op = MATHOP_SUM;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN1:
				math_op = MATHOP_POW;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN2:
				math_op = MATHOP_COS;
				runtime_status = RUNTIME_STATUS_ENTERFINAL;
				break;

			case RUNTIME_STATUS_ENTERFINAL:
				if(math_op_uses_second_input(math_op)) runtime_status = RUNTIME_STATUS_ENTERFIRST;
				else runtime_status = RUNTIME_STATUS_MAIN0;
				break;

			case RUNTIME_STATUS_ENTERFIRST:
			case RUNTIME_STATUS_RESULT:

				runtime_status = RUNTIME_STATUS_MAIN0;
				break;
		}
	}
	else if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button2))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_MAIN0:
				math_op = MATHOP_SUB;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN1:
				math_op = MATHOP_ROOT;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN2:
				math_op = MATHOP_TAN;
				runtime_status = RUNTIME_STATUS_ENTERFINAL;
				break;

			case RUNTIME_STATUS_ENTERFIRST:
				if(!textbox1_capture_text())
				{
					MessageBox(NULL, TEXT("Invalid Value Entered."), TEXT("ERROR"), (MB_ICONEXCLAMATION | MB_OK));
					runtime_status = RUNTIME_STATUS_ENTERFIRST;
				}
				else runtime_status = RUNTIME_STATUS_ENTERFINAL;
				break;

			case RUNTIME_STATUS_ENTERFINAL:
				if(!textbox1_capture_text())
				{
					MessageBox(NULL, TEXT("Invalid Value Entered."), TEXT("ERROR"), (MB_ICONEXCLAMATION | MB_OK));
					runtime_status = RUNTIME_STATUS_ENTERFINAL;
				}
				else
				{
					calculate();
					runtime_status = RUNTIME_STATUS_RESULT;
				}
				break;
		}
	}
	else if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button3))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_MAIN0:
				math_op = MATHOP_MULT;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN1:
				math_op = MATHOP_LOG;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN2:
				math_op = MATHOP_FAC;
				runtime_status = RUNTIME_STATUS_ENTERFINAL;
				break;

			case RUNTIME_STATUS_ENTERFINAL:
				ang_unit = !ang_unit;
				runtime_status = RUNTIME_STATUS_ENTERFINAL;
				break;
		}
	}
	else if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button4))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_MAIN0:
				math_op = MATHOP_DIV;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;

			case RUNTIME_STATUS_MAIN1:
				math_op = MATHOP_SIN;
				runtime_status = RUNTIME_STATUS_ENTERFINAL;
				break;

			case RUNTIME_STATUS_MAIN2:
				math_op = MATHOP_BIN;
				runtime_status = RUNTIME_STATUS_ENTERFIRST;
				break;
		}
	}
	else if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button5))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_MAIN1:
				runtime_status = RUNTIME_STATUS_MAIN0;
				break;

			case RUNTIME_STATUS_MAIN2:
				runtime_status = RUNTIME_STATUS_MAIN1;
				break;
		}
	}
	else if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button6))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_MAIN0:
				runtime_status = RUNTIME_STATUS_MAIN1;
				break;

			case RUNTIME_STATUS_MAIN1:
				runtime_status = RUNTIME_STATUS_MAIN2;
				break;
		}
	}

	return 0;
}

LRESULT CALLBACK mainwnd_event_wmpaint(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	HDC p_wnddc = nullptr;
	PAINTSTRUCT paintstruct;

	if(p_wnd == nullptr) return 0;

	p_wnddc = BeginPaint(p_wnd, &paintstruct);
	if(p_wnddc == nullptr) return 0;

	FillRect(paintstruct.hdc, &paintstruct.rcPaint, (HBRUSH) pp_custombrush[MAINWND_BRUSHINDEX]);
	EndPaint(p_wnd, &paintstruct);

	return 0;
}

LRESULT CALLBACK mainwnd_event_wmctlcolorstatic(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	if(p_wnd == nullptr) return 0;
	if(wparam  == 0) return 0;
	if(lparam == 0) return 0;

	SetTextColor((HDC) wparam, TEXTWND_TEXTCOLOR);
	SetBkColor((HDC) wparam, TEXTWND_BKCOLOR);

	return (LRESULT) pp_custombrush[TEXTWND_BRUSHINDEX];
}

LRESULT CALLBACK mainwnd_event_wmctlcoloredit(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	if(p_wnd == nullptr) return 0;
	if(wparam == 0) return 0;
	if(lparam == 0) return 0;

	SetTextColor((HDC) wparam, TEXTBOXWND_TEXTCOLOR);
	SetBkColor((HDC) wparam, TEXTBOXWND_BKCOLOR);

	return (LRESULT) pp_custombrush[TEXTBOXWND_BRUSHINDEX];
}

LRESULT CALLBACK mainwnd_event_wmsize(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	if(p_wnd == nullptr) return 0;

	ctrls_setup(TRUE);
	RedrawWindow(p_wnd, NULL, NULL, (RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN));
	return 0;
}

LRESULT CALLBACK mainwnd_event_wmdestroy(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	p_mainwnd = nullptr;
	p_text1 = nullptr;
	p_text2 = nullptr;
	p_textbox1 = nullptr;
	p_button1 = nullptr;
	p_button2 = nullptr;
	p_button3 = nullptr;
	p_button4 = nullptr;
	p_button5 = nullptr;
	p_button6 = nullptr;

	PostQuitMessage(0);
	return 0;
}
