#include "AXInput.h"
#include "AXError.h"

//---------------------------------------------------------------------------------
AXInput::AXInput(const HINSTANCE& hi, HWND* hw, DWORD genre, DIACTION* actions, const char* mapName, DWORD buf_sz, long axisMin, long axisMax)
:  _hWnd(hw),
   _din(0),
   _actionMap(0),
   _devices(0),
   _numDevices(0)
   {
   DirectInput8Create(hi, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_din, NULL);

   // unique GUID for this app's action map
   GUID mapGUID = { 0x508b7cd1, 0x5673, 0x4f2f, {0xa0, 0x2c, 0x3b, 0x7a, 0xaf, 0xcd, 0x03, 0x72} };

   _actionMap = actions;
   _dataBuffer = new DIDEVICEOBJECTDATA[buf_sz];
   ZeroMemory(_dataBuffer, buf_sz * sizeof(DIDEVICEOBJECTDATA));
    
   _format.dwSize        = sizeof(DIACTIONFORMAT);
   _format.dwActionSize  = sizeof(DIACTION);
   _format.dwNumActions  = sizeof(_actionMap) / sizeof(DIACTION);
   _format.dwDataSize    = _format.dwNumActions * sizeof(DWORD);
   _format.guidActionMap = mapGUID;
   _format.dwGenre       = genre;
   _format.rgoAction     = _actionMap;
   _format.dwBufferSize  = buf_sz;
   _format.lAxisMin      = axisMin;
   _format.lAxisMax      = axisMax;
   strcpy(_format.tszActionMap, mapName); 

   getDevices();
   }

//---------------------------------------------------------------------------------
AXInput::~AXInput()
   {  
   deactivate();
   
   if(_din)
      _din->Release();

   delete[] _actionMap;
   delete[] _devices;
   delete[] _dataBuffer;
   }

//---------------------------------------------------------------------------------
// enumerates input devices on the system (fires callbacks)
//---------------------------------------------------------------------------------
void AXInput::getDevices()
   {
   _din->EnumDevicesBySemantics(NULL, &_format, enumDIDevice, (void*)this, 0);
   }

//---------------------------------------------------------------------------------
// adds a predefined device to the list in the AXInput object
//---------------------------------------------------------------------------------
void AXInput::addDevice(IDirectInputDevice8* dev)
   {
   // make more room for this device
   IDirectInputDevice8** tmp = new IDirectInputDevice8*[_numDevices + 1];
   memcpy(tmp, _devices, _numDevices * sizeof(IDirectInputDevice8*));
   delete[] _devices;
   _devices = tmp;

   // set up the device
   dev->SetCooperativeLevel(*_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
   dev->BuildActionMap(&_format, NULL, DIDBAM_PRESERVE);   
   dev->SetActionMap(&_format, NULL, DIDSAM_DEFAULT);

   // add it to the list
   _devices[_numDevices] = dev;
   _numDevices++;
   }

//---------------------------------------------------------------------------------
void AXInput::deactivate()
   {
   for(unsigned i = 0; i < _numDevices; i++)
      _devices[i]->Unacquire();
   }

//---------------------------------------------------------------------------------
void AXInput::activate()
   {
   HRESULT hr;
   for(unsigned i = 0; i < _numDevices; i++)
      {
      hr = _devices[i]->Acquire();

      if(hr != DI_OK && hr != S_FALSE) 
         throw XDeviceLost(XARGS);
      }
   }

//---------------------------------------------------------------------------------
// displays the inbuilt DirectX Input configuration page for action mapping
//---------------------------------------------------------------------------------
void AXInput::configureDevices()
   {
   DICONFIGUREDEVICESPARAMS params;   

   ZeroMemory(&params, sizeof(params));
   params.dwSize = sizeof(params);
   params.dwcUsers       = 1;
   params.lptszUserNames = NULL;
   params.dwcFormats     = 1;
   params.lprgFormats    = &_format;
   params.hwnd           = *_hWnd;
   params.lpUnkDDSTarget = NULL;
   params.dics.dwSize    = sizeof(DICOLORSET);

   deactivate();
   _din->ConfigureDevices(NULL, &params, DICD_EDIT, NULL);
   activate();
   }

//---------------------------------------------------------------------------------
void AXInput::processInput()
   {
   DWORD n_data = _format.dwBufferSize;

   for(unsigned i = 0; i < _numDevices; i++)
      {
      if(_devices[i]->Poll() == DIERR_INPUTLOST)
         throw XDeviceLost(XARGS);
      
      _devices[i]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), _dataBuffer, &n_data, 0);

      for(unsigned j = 0; j < n_data; j++)
         {
         // uAppData is a pointer to the game defined function to handle the type of input received
         (reinterpret_cast<inputHandler>(_dataBuffer[j].uAppData))(_dataBuffer[j]);         
         }
      }
   }

//---------------------------------------------------------------------------------
// fires for each device found during enumeration, adds it to the AXInput object
//---------------------------------------------------------------------------------
BOOL CALLBACK AXInput::enumDIDevice(LPCDIDEVICEINSTANCE dInst, LPDIRECTINPUTDEVICE8 dev, DWORD flags, DWORD nRemaining, LPVOID appData)
   {  
   AXInput* axi = reinterpret_cast<AXInput*>(appData);

   // dont care about devices of type devicectrl 
   if(GET_DIDEVICE_TYPE(dInst->dwDevType) == DI8DEVTYPE_DEVICECTRL)
      return DIENUM_CONTINUE;

   // COM stuff: we cant copy a COM so add a ref to the object created 
   // by windows for the callback otherwise it will be auto destroyed
   dev->AddRef();

   // add to the AXInput object
   axi->addDevice(dev);

   return DIENUM_CONTINUE;
   }

//---------------------------------------------------------------------------------
DIACTION* AXInput::defActionGenerator()
   {
   DIACTION actions[] = { 
      { 0,         DIKEYBOARD_ESCAPE,     0, "Quit" },
      { 0,         DIKEYBOARD_SYSRQ,      0, "Screenshot" } 
      };

   DIACTION* ret = new DIACTION[sizeof(actions) / sizeof(DIACTION)];
   memcpy(ret, actions, sizeof(actions));
   return ret;
   };


