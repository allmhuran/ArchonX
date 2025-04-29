#ifndef AXAPP_H
#define AXAPP_H

#include "X32.h"
#include "AXGraphics.h"
#include "AXAudio.h"
#include "AXInput.h"
#include "AXDrawable.h"
#include "AXError.h"


#define APP AXApp::app()

// singleton class for global data and interfaces

class AXApp : public X32
   {
   public:
      static void construct(HINSTANCE hi, const char* title = "AXApp Window");
      static AXApp& app() { if(!_instance) throw XNoObj(XARGS); return *_instance; }

      AXApp& initGraphics(bool windowed = false) { _graphics = new AXGraphics(window(), 0, windowed); return *this; }
      AXApp& initAudio()                         { _audio = new AXAudio(window()); return *this; }
      AXApp& initInput(DWORD genre = DIVIRTUAL_ARCADE_SIDE2SIDE, DIACTION* actions = AXInput::defActionGenerator(), const char* mapName = "ActionMap", DWORD buf_sz = 10, long axisMin = -100, long axisMax = 100);

      int run();  // begins main process
      void stop() { _active = false; _running = false; } // ends main process
      
      void activate();   // ready DirectX interfaces
      void deactivate(); // disable DirectX interfaces

      AXGraphics& graphics() { return *_graphics; }
      AXAudio& audio()       { return *_audio; }
      AXInput& input()       { return *_input; }

      const float& dt() const       { return _dtLast; }
      const float  fps()            { return _fps; }

      HRESULT hr;

   protected:
      AXApp(HINSTANCE hi, const char* title);
      ~AXApp() {};
      AXApp(const AXApp&);
      AXApp& operator=(const AXApp&);
      AXApp* operator&();
      
   private:
      void initLoopTimer();
      void calcLoopTime();
      void calcFPS();
      
      AXGraphics* _graphics;
      AXAudio*    _audio;
      AXInput*    _input;

      bool  _active;
      bool  _paused;
      bool  _running;
      
      float      _dtLast;
      float      _fps;
      LONGLONG   _startCount;
      LONGLONG   _countsPerSecond;
   
      static bool constructed;
      static LRESULT CALLBACK MsgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);
      static AXApp* _instance;
   };

#endif