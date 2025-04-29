#ifndef AXFRAMEMAPFACTORY_H
#define AXFRAMEMAPFACTORY_H

#include "..\AXFrameMap.h"
///////////////////////////////////////////////////////////////////////////////////
// FrameMapFactory helps users create frames
///////////////////////////////////////////////////////////////////////////////////

class AXFrameMapFactory
   {
   public:
      AXFrameMapFactory() {}
      ~AXFrameMapFactory() {}

      void createDescriptor();
      void createMap();

   private:
      void writeDescriptor();
      void writeSequence(std::fstream& in, std::fstream& out);
      void writeFrame(std::fstream& in, std::fstream& out);
      void writeImage(std::fstream& in, std::fstream& out); 
   };

#endif