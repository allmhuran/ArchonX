#include "AXConsoleCommands.h"

namespace ConsoleCommands
   {

   bool screenShot()
      {
      AXE.graphics().printScreen();
      return true;
      }

   bool quit()
      {
      AXE.stop();
      return true;
      }
   }


