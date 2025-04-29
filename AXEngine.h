#ifndef AXENGINE_H
#define AXENGINE_H

#include "X32.h"
#include "AXGraphics.h"
#include "AXAudio.h"
#include "AXInput.h"
#include "AXConsole.h"
#include "AXDrawable.h"
#include "AXError.h"


#define AXE AXEngine::engine()

// singleton class for global data and interfaces

class AXEngine : public X32
   {
   public:
      static void construct(HINSTANCE hi, const char* title = "AXEngine Window");
      static AXEngine& engine() { if(!_instance) throw XNoObj(XARGS); return *_instance; }

      AXEngine& initGraphics(bool windowed = false) { _graphics = new AXGraphics(window(), 0, windowed); return *this; }
      AXEngine& initAudio()                         { _audio = new AXAudio(window()); return *this; }
      AXEngine& initInput(DWORD genre = DIVIRTUAL_ARCADE_SIDE2SIDE, DIACTION* actions = AXInput::defActionGenerator(), const char* mapName = "ActionMap", DWORD buf_sz = 10, long axisMin = -100, long axisMax = 100);
      AXEngine& initConsole(const AXFrameMap* backSrc, const char* backSequence, AXFrameMap* textSrc, const char* textSequence, const unsigned sz_history = 300);

      void releaseGraphics() { delete _graphics; }
      void releaseAudio()    { delete _audio; }
      void releaseInput()    { delete _input; }
      void releaseConsole()  { delete _console; }
      
      int run();  // begins main process
      void stop() { _active = false; _running = false; } // ends main process
      
      void activate();   // ready DirectX interfaces
      void deactivate(); // disable DirectX interfaces

      AXGraphics& graphics() { return *_graphics; }
      AXAudio& audio()       { return *_audio; }
      AXInput& input()       { return *_input; }
      AXConsole& console()   { return *_console; }

      const float& dt() const       { return _dtLast; }
      const float  fps()            { return _fps; }
      void  trackFPS(bool track)    { _trackFPS = track; }

   protected:
      AXEngine(HINSTANCE hi, const char* title);
      ~AXEngine() {};
      AXEngine(const AXEngine&);
      AXEngine& operator=(const AXEngine&);
      AXEngine* operator&();
      
   private:
      void initLoopTimer();
      void calcLoopTime();
      void calcFPS();
      
      AXGraphics* _graphics;
      AXAudio*    _audio;
      AXInput*    _input;
      AXConsole*  _console;

      bool  _active;
      bool  _paused;
      bool  _running;
      bool  _trackFPS;
      
      float      _dtLast;
      float      _fps;
      LONGLONG   _startCount;
      LONGLONG   _countsPerSecond;
   
      static bool constructed;
      static LRESULT CALLBACK MsgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);
      static AXEngine* _instance;
   };

#endif