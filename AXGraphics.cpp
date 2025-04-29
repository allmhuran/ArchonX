#include "AXGraphics.h"
#include <sstream>
#include <iomanip>

//---------------------------------------------------------------------------------
AXGraphics::AXGraphics(const HWND& hw, Drawer d, bool windowed)
: _d3do(0), _d3dd(0), _d3ds(0), _drawScene(d)
   {
   _d3do = Direct3DCreate8(D3D_SDK_VERSION);
   _d3do->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &_d3ddm);

   ZeroMemory(&_d3dpp, sizeof(_d3dpp));
   _d3dpp.Windowed = windowed;
   _d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
   _d3dpp.BackBufferWidth = _d3ddm.Width;
   _d3dpp.BackBufferHeight = _d3ddm.Height;
   _d3dpp.BackBufferFormat = _d3ddm.Format;  

   if(windowed)
      {
      _d3dpp.FullScreen_RefreshRateInHz = 0;
      _d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
      }
   else
      {
      _d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
      _d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
      }
      
   _d3do->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
                      hw, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                      &_d3dpp, &_d3dd);

   D3DXCreateSprite(_d3dd, &_d3ds);
   }
//---------------------------------------------------------------------------------
void AXGraphics::deactivate()
   {
   if(_d3ds) _d3ds->OnLostDevice();
   }

//---------------------------------------------------------------------------------
void AXGraphics::activate()
   {
   _d3dd->Reset(&_d3dpp);
   _d3ds->OnResetDevice();

   if(_d3dd) 
      if(_d3dd->Reset(&_d3dpp) != S_OK) 
         throw XDeviceLost(XARGS); 

   if(_d3ds) 
      if(_d3ds->OnResetDevice() != S_OK)
         throw XDeviceLost(XARGS);

   }

//---------------------------------------------------------------------------------
AXGraphics::~AXGraphics()
   {
   if(_d3ds) _d3ds->Release();
   if(_d3dd) _d3dd->Release();
   if(_d3do) _d3do->Release();
   }


//---------------------------------------------------------------------------------
void AXGraphics::printScreen()
   {
   using namespace std;
   static int lastScreen = 0;

   IDirect3DSurface8* screen;   
   _d3dd->CreateImageSurface(_d3ddm.Width, _d3ddm.Height, D3DFMT_A8R8G8B8, &screen);
   _d3dd->GetFrontBuffer(screen);

   stringstream fname;
   fname << setw(8) << setfill('0') << ++lastScreen << ".bmp";

   D3DXSaveSurfaceToFile(fname.str().c_str(), D3DXIFF_BMP, screen, NULL, NULL);
   screen->Release();
   }  