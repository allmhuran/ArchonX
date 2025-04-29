#include "X32.h"


//---------------------------------------------------------------------------------
// x32 constructor sets up default values for the extended window class
//---------------------------------------------------------------------------------
X32::X32(HINSTANCE hi, HANDLER handler, const char* title, const DWORD& exStyle,
         const DWORD& style, const char* className)
:  _title(title), _exStyle(exStyle), _style(style)
   {    
   _wcx.cbSize        = sizeof(WNDCLASSEX);
   _wcx.style         = 0; 
   _wcx.lpfnWndProc   = handler;
   _wcx.cbClsExtra    = 0;
   _wcx.cbWndExtra    = 0;
   _wcx.hInstance     = hi;
   _wcx.hIcon         = (HICON)LoadImage(hi, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
   _wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
   _wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   _wcx.lpszMenuName  = NULL;
   _wcx.lpszClassName = className;
   _wcx.hIconSm       = NULL; //(HICON)LoadImage(hi, "IDI_ICON1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
   }

//---------------------------------------------------------------------------------
// x32 create registers and shows the window
//---------------------------------------------------------------------------------
void X32::showWindow()
   {
   RegisterClassEx(&_wcx);

   _hw = CreateWindowEx(_exStyle, _wcx.lpszClassName, _title, _style, 
                        100, 100, 200, 200, 
                        GetDesktopWindow(),
                        0, _wcx.hInstance, 0);

   ShowWindow(_hw, SW_SHOWNORMAL);
   UpdateWindow(_hw);
   };
