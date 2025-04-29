#include "AXEngine.h"
#include "X32.h"
#include "AXGraphics.h"
#include "AXAudio.h"
#include "AXInput.h"
#include "AXError.h"
#include "AXDrawable.h"

AXFrameMap* map = 0;
AXText*     txt = 0;
AXSprite*   sprite = 0;

void customDraw()
   {
   if(txt) txt->draw(AXE.graphics().dxSprite());
   if(sprite) sprite->draw(AXE.graphics().dxSprite());
   }

//---------------------------------------------------------------------------------
// AXE ENTRY POINT
//---------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR cmd, int show)
   {  
   START_DEBUG_TRACE

   int i;
 
   AXEngine::construct(hi, "AXApp");
   AXE.showWindow(); 
   AXE.initGraphics(false);

   AXE.graphics().setDrawFunction(customDraw);
      
   map = new AXFrameMap(AXE.graphics().dxDevice(), "e:\\dev\\res\\courier.spr");
   txt = new AXText(map, "courier", 100, ' ');
   txt->position(dxv(AXE.graphics().screenWidth() / 2 - 50, AXE.graphics().screenHeight() / 2));
   txt->rotation(D3DXToRadian(0));
   txt->extent(700);
   i = AXE.run();

   delete txt;
   delete map;
   delete sprite;
   
   END_DEBUG_TRACE
      
   return i;
   }





