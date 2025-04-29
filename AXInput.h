#ifndef AXINPUT_H
#define AXINPUT_H

#include "AXComponent.h"
#include <dinput.h>

typedef void (*inputHandler)(const DIDEVICEOBJECTDATA& data);

//---------------------------------------------------------------------------------
class AXInput : public AXComponent
   {
   friend class AXEngine;

   public:
      void configureDevices();

   private:
      // structors
      AXInput(const HINSTANCE& hi, HWND* hw, DWORD genre = DIVIRTUAL_ARCADE_SIDE2SIDE, DIACTION* actions = defActionGenerator(), const char* mapName = "ActionMap", DWORD buf_sz = 10, long axisMin = -100, long axisMax = 100);
      virtual ~AXInput();
      AXInput(const AXInput&);               // no impl
      AXInput& operator=(const AXInput&);    // no impl

      void processInput();
      void activate();                       // acquire devices
      void deactivate();                     // unacquire devices
      void getDevices();                     // device enumeration
      void addDevice(IDirectInputDevice8* dev);   
   
      IDirectInput8*         _din;       
      IDirectInputDevice8**  _devices;        // enumerated
      unsigned               _numDevices;      // enumerated
      HWND*                  _hWnd;           
      DIACTION*              _actionMap;     
      DIACTIONFORMAT         _format;
      DIDEVICEOBJECTDATA*    _dataBuffer;     // recieves buffered data

      static DIACTION* defActionGenerator();
      static BOOL CALLBACK enumDIDevice(LPCDIDEVICEINSTANCE, LPDIRECTINPUTDEVICE8, DWORD, DWORD, LPVOID);
   };

#endif