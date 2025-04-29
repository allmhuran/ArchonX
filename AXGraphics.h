//---------------------------------------------------------------------------------
// AXGraphics encapsulates the interfaces for 2D graphic display in direct3d as
// well as streamlining directx initialisation
//---------------------------------------------------------------------------------

#ifndef AXGRAPHICS_H
#define AXGRAPHICS_H

#include "AXComponent.h"
#include "AXError.h"
#include <d3d8.h>
#include <d3dx8.h>

typedef void (*Drawer)();

class AXGraphics : public AXComponent
   {
   friend class AXEngine;
   
   public:
      void printScreen();                       // fullscreen screenshot
      int  screenWidth()             { return _d3ddm.Width; }
      int  screenHeight()            { return _d3ddm.Height; }
      
      ID3DXSprite& dxSprite()        { return *_d3ds; }
      IDirect3DDevice8& dxDevice()   { return *_d3dd; }
      void setDrawFunction(Drawer d) { _drawScene = d; }

   private:
      // structors
      AXGraphics(const HWND& hw, Drawer d = 0, bool windowed = false);
      ~AXGraphics();
      AXGraphics(const AXGraphics&);            // no impl
      AXGraphics& operator=(const AXGraphics&); // no impl

      void activate();
      void deactivate();  
      void render();

      Drawer                  _drawScene;        // custom rendering function
      ID3DXSprite*            _d3ds;             // sprite interface  
      IDirect3DDevice8*       _d3dd;             // d3d device interface
      IDirect3D8*             _d3do;             // d3d object interface
      D3DDISPLAYMODE          _d3ddm;            // display mode
      D3DPRESENT_PARAMETERS   _d3dpp;            // d3d behaviour            
   };

//---------------------------------------------------------------------------------
inline void AXGraphics::render()
   {
   // Clear the backbuffer to a blue colour 
   _d3dd->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0, 0);

   if(_d3ds->Begin() == D3D_OK)
      {
      // call the custom drawing function, end the scene and flip
      if(_drawScene) _drawScene();

      _d3ds->End();

      if(_d3dd->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST)
         throw XDeviceLost(XARGS);
      }
   }


#endif

