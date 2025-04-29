#include "AXApp.h"

AXApp* AXApp::_instance = 0;
bool AXApp::constructed = false;

extern AXText* txt;

//---------------------------------------------------------------------------------
AXApp::AXApp(HINSTANCE hi, const char* title)
:  X32(hi, MsgProc, title, WS_EX_OVERLAPPEDWINDOW, WS_OVERLAPPEDWINDOW, "AXApp"),
   _graphics(0), _audio(0), _input(0), _active(false), _paused(false), _running(false), _dtLast(0), _fps(0)
   {
   // nothing
   }

//---------------------------------------------------------------------------------
void AXApp::construct(HINSTANCE hi, const char* title)
   {
   if(!_instance) 
      {
      _instance = new AXApp(hi, title);
      constructed = true;
      }
   }

//---------------------------------------------------------------------------------
AXApp& AXApp::initInput(DWORD genre, DIACTION* actions, const char* mapName, DWORD buf_sz, long axisMin, long axisMax)
   {
   _input = new AXInput(wndClass().hInstance, pWindow(), genre, actions, mapName, buf_sz, axisMin, axisMax);
   return *this;
   }
   
//---------------------------------------------------------------------------------
int AXApp::run()
   {
   int val;
   int i = 0;

   _running = true;
   APP.activate();
   initLoopTimer();

   while(_running)
      {
      // if not active, then use GetMessage to pause until we start to get messages again
      // at that point, try to reactivate
      while(!_active && (val = GetMessage(pMsg(), 0, 0, 0)) != 0)    
         {
         TranslateMessage(pMsg());
         DispatchMessage(pMsg());     
         
         try { activate(); } catch(XDeviceLost&) { deactivate(); }
         }

      while(_active)       
         {
         while(PeekMessage(pMsg(), 0, 0, 0, PM_REMOVE))   
            {
            TranslateMessage(pMsg());
            DispatchMessage(pMsg());
            }
      
         // main process here
         try        
            {
            if(_graphics) _graphics->render();
            if(_input) _input->processInput();
            calcLoopTime();
            calcFPS();            
            (*txt)(fps(), "%6.2f");
            }   
         catch(XDeviceLost&) { deactivate(); }      
         }  
      }
      
   return msg().wParam;
   }

//---------------------------------------------------------------------------------
void AXApp::activate()
   {
   if(_graphics) _graphics->activate();  
   if(_input) _input->activate();
   if(_audio) _audio->activate();
   _active = true;
   }

//---------------------------------------------------------------------------------
void AXApp::deactivate()
   {
   if(_graphics) _graphics->deactivate();
   if(_input) _input->deactivate();
   if(_audio) _audio->deactivate();
   _active = false;
   }

//---------------------------------------------------------------------------------
void AXApp::initLoopTimer()
   {  
   LARGE_INTEGER val;
   QueryPerformanceFrequency(&val);
   _countsPerSecond = val.QuadPart;
   QueryPerformanceCounter(&val);
   _startCount = val.QuadPart;
   }

//---------------------------------------------------------------------------------
void AXApp::calcLoopTime()
   {
   LARGE_INTEGER val;
   QueryPerformanceCounter(&val);
   _dtLast = (double)(val.QuadPart  - _startCount) / (double)_countsPerSecond;
   _startCount = val.QuadPart;
   }

//---------------------------------------------------------------------------------
void AXApp::calcFPS()
   {
   static int i = 0;
   static float elapsed;

   if(++i == 1000)
      {
      _fps = i / elapsed;
      i = 0;
      elapsed = 0;
      }
   else
      {
      elapsed += _dtLast;
      }
   }

//---------------------------------------------------------------------------------
LRESULT CALLBACK AXApp::MsgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
   {
   switch(msg)
      {
      case WM_DESTROY:
         {
         if(AXApp::constructed)
            AXApp::app().stop();

         PostQuitMessage(0);
         return 0;
         }

      case WM_CHAR:
         {
         if(wp == VK_SPACE)
            {
            APP.graphics().printScreen();
            return 0;
            }
         
         if(wp == VK_ESCAPE)
            {
            if(AXApp::constructed)
               AXApp::app().stop();

            PostQuitMessage(0);
            }

         return 0;
         }
      }

   return DefWindowProc(hw, msg, wp, lp);
   }

