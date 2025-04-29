#include "AXEngine.h"

AXEngine* AXEngine::_instance = 0;
bool AXEngine::constructed = false;

extern AXText* txt;

//---------------------------------------------------------------------------------
AXEngine::AXEngine(HINSTANCE hi, const char* title)
:  X32(hi, MsgProc, title, WS_EX_OVERLAPPEDWINDOW, WS_OVERLAPPEDWINDOW, "AXEngine"),
   _graphics(0), _audio(0), _input(0), _active(false), _paused(false), _running(false), _dtLast(0), _fps(0)
   {
   // nothing
   }

//---------------------------------------------------------------------------------
void AXEngine::construct(HINSTANCE hi, const char* title)
   {
   if(!_instance) 
      {
      _instance = new AXEngine(hi, title);
      constructed = true;
      }
   }

//---------------------------------------------------------------------------------
AXEngine& AXEngine::initInput(DWORD genre, DIACTION* actions, const char* mapName, DWORD buf_sz, long axisMin, long axisMax)
   {
   _input = new AXInput(wndClass().hInstance, pWindow(), genre, actions, mapName, buf_sz, axisMin, axisMax);
   return *this;
   }
   
//---------------------------------------------------------------------------------
AXEngine& AXEngine::initConsole(const AXFrameMap* backSrc, const char* backSequence, AXFrameMap* textSrc, const char* textSequence, const unsigned sz_history)
   {
   _console = new AXConsole(backSrc, backSequence, textSrc, textSequence, sz_history);
   return *this;
   }

//---------------------------------------------------------------------------------
int AXEngine::run()
   {
   int val;

   _running = true;
   AXE.activate();
   initLoopTimer();

   while(_running)
      {
      // if not active, then use GetMessage to pause until we start to get messages again
      // at that point, try to reactivate
      while(!_active && (val = GetMessage(pMsg(), 0, 0, 0)) != 0)    
         {
         TranslateMessage(pMsg());
         DispatchMessage(pMsg());     
         
         try { activate(); } catch(XDeviceLost&) { deactivate(); Sleep(500); }
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
            calcLoopTime();
            if(_trackFPS) calcFPS();            
            if(_graphics) _graphics->render();
            if(_input) _input->processInput();
            }   
         catch(XDeviceLost&) { deactivate(); }      
         }  
      }
      
   return msg().wParam;
   }

//---------------------------------------------------------------------------------
void AXEngine::activate()
   {
   if(_graphics) _graphics->activate();  
   if(_input) _input->activate();
   if(_audio) _audio->activate();
   _active = true;
   }

//---------------------------------------------------------------------------------
void AXEngine::deactivate()
   {
   if(_graphics) _graphics->deactivate();
   if(_input) _input->deactivate();
   if(_audio) _audio->deactivate();
   _active = false;
   }

//---------------------------------------------------------------------------------
void AXEngine::initLoopTimer()
   {  
   LARGE_INTEGER val;
   QueryPerformanceFrequency(&val);
   _countsPerSecond = val.QuadPart;
   QueryPerformanceCounter(&val);
   _startCount = val.QuadPart;
   }

//---------------------------------------------------------------------------------
void AXEngine::calcLoopTime()
   {
   LARGE_INTEGER val;
   QueryPerformanceCounter(&val);
   _dtLast = (double)(val.QuadPart  - _startCount) / (double)_countsPerSecond;
   _startCount = val.QuadPart;
   }

//---------------------------------------------------------------------------------
void AXEngine::calcFPS()
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
LRESULT CALLBACK AXEngine::MsgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
   {
   switch(msg)
      {
      case WM_DESTROY:
         {
         if(AXEngine::constructed)
            AXEngine::engine().stop();

         PostQuitMessage(0);
         return 0;
         }

      case WM_CHAR:
         {
         if(wp == VK_SPACE)
            {
            AXE.graphics().printScreen();
            return 0;
            }
         
         if(wp == VK_ESCAPE)
            {
            if(AXEngine::constructed)
               AXEngine::engine().stop();

            PostQuitMessage(0);
            }

         return 0;
         }
      }

   return DefWindowProc(hw, msg, wp, lp);
   }

