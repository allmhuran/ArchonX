#include "AXFrameMap.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////
// AXFrameMap
///////////////////////////////////////////////////////////////////////////////////
AXFrameMap::AXFrameMap(IDirect3DDevice8& device, const char* file, const D3DCOLOR& key)
   {
   
   using namespace std;

   memset((void*)&_sequences, 0, sizeof(_sequences));

   unsigned nSequences;
   unsigned nFrames;

   fstream in(file, ios::in | ios::binary);
   in.read((char*)&nSequences, sizeof nSequences);
   in.read((char*)&_numFrames, sizeof _numFrames);

   _frames = new AXFrame[_numFrames];

   unsigned char index;
   AXSequence* sp;

   for(int i = 0; i < nSequences; i++)
      {
      in.read((char*)&index, sizeof index);

      if(_sequences[index] != 0)
         throw XParams(XARGS);

      sp = new AXSequence;      
      in.read((char*)sp, sizeof(AXSequence));

      nFrames = sp->_lf - sp->_ff + 1;

      for(int j = 0; j < nFrames; j++)
         in.read((char*)&_frames[sp->_ff + j], sizeof(AXFrame));

      _sequences[index] = sp;
      }

   size_t sz_data;
   in.read((char*)&sz_data, sizeof sz_data);

   char* data = new char[sz_data];
   in.read(data, sz_data * sizeof(char));

   D3DXCreateTextureFromFileInMemoryEx(&device,  // device
                                       data,                 // src data address
                                       sz_data,              // src data size
                                       0, 0, 1, 0,           // width, height, miplevels, usage
                                       D3DFMT_UNKNOWN,       // format
                                       D3DPOOL_MANAGED,      // pool
                                       D3DX_FILTER_NONE,     // filter  
                                       D3DX_FILTER_NONE,     // mipfilter
                                       key,                  // colorkey
                                       NULL, NULL,           // srcinfo, palette
                                      &_texture);            // destination
                                      
                                      
   }

//---------------------------------------------------------------------------------
AXFrameMap::~AXFrameMap()
   {
   for(int i = 0; i < MAX_SEQUENCES; i++)
      if(_sequences[i]) delete _sequences[i];

   delete[] _frames;
   _texture->Release();
   }



