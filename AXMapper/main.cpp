#undef AX_DEBUGGING

#include "AXFrameMapFactory.h"
#include <iostream>

using namespace std;

int main()
   {
   char c;

   AXFrameMapFactory f;

   cout << "****************************" << endl
        << "AXMapper: Builds AXFrameMaps" << endl
        << endl;
   do {
      cout << endl
           << "Make a selection:" << endl
           << "d = new descriptor file" << endl
           << "m = new map file" << endl
           << "q = quit" << endl;

      cin >> c;

      if(c == 'd')
         f.createDescriptor();

      if(c == 'm')
         f.createMap();
      
      } while(c != 'q');

   return 0;
   }

