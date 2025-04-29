//---------------------------------------------------------------------------------
// X32 encapsulates and streamlines window creation and window setup
// With X32, creating a window means creating an X32 instance, adding any custom
// settings through X32 members, and then calling X32::create
//---------------------------------------------------------------------------------

#ifndef X32_H
#define X32_H

#include <windows.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

typedef LRESULT (CALLBACK* HANDLER)(HWND, UINT, WPARAM, LPARAM);

class X32
   {
   public:
      X32(HINSTANCE hi, HANDLER handler, const char* title = "X32", 
          const DWORD& exStyle = WS_EX_OVERLAPPEDWINDOW, 
          const DWORD& style = WS_OVERLAPPEDWINDOW,
          const char* className = "myclass");

      virtual ~X32() {}

      X32& icon(LPCSTR icon)      { _wcx.hIcon = (HICON)LoadImage(_wcx.hInstance, icon, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE); return *this; }
      X32& smallicon(LPCSTR icon) { _wcx.hIconSm = (HICON)LoadImage(_wcx.hInstance, icon, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE); return *this; }
      X32& cursor(LPCSTR cursor)  { _wcx.hCursor = (HCURSOR)LoadImage(_wcx.hInstance, cursor, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE); return *this; }
      X32& background(HBRUSH hb)  { _wcx.hbrBackground = hb; return *this; }
          
      void showWindow();        

      HWND&       window()    { return _hw; }
      MSG&        msg()       { return _msg; }
      WNDCLASSEX& wndClass()  { return _wcx; }
      HINSTANCE   inst()      { return _wcx.hInstance; }
      
      HWND*       pWindow()   { return &_hw; }
      MSG*        pMsg()      { return &_msg; }
      WNDCLASSEX* pWndClass() { return &_wcx; }
      HINSTANCE*  pInst()     { return &(_wcx.hInstance); }

   private:
      DWORD       _exStyle;
      DWORD       _style;
      const char* _title;

      HWND        _hw;
      MSG         _msg;
      WNDCLASSEX  _wcx;     

      X32(const X32&);
      X32& operator=(const X32&);
   };

#endif