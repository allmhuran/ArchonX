#ifndef AXFRAMEMAP_H
#define AXFRAMEMAP_H

#include "AXGraphics.h"
#include "AXError.h"
#include "AXUtility.h"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////
// a frame of animation
///////////////////////////////////////////////////////////////////////////////////
struct AXFrame 
   {
   int  _interval;
   RECT _rect;      
   };

///////////////////////////////////////////////////////////////////////////////////
// first and last frame index in a sequence
///////////////////////////////////////////////////////////////////////////////////
struct AXSequence
   {
   unsigned _ff; // index of first frame in sequence
   unsigned _lf; // index of last frame in sequence
   };

///////////////////////////////////////////////////////////////////////////////////
// an AXFrameMap OWNS the direct3d texture and the frame descriptors
///////////////////////////////////////////////////////////////////////////////////
class AXFrameMap
   {
   public:
      // structors     
      AXFrameMap(IDirect3DDevice8& device, const char* sprFile, const D3DCOLOR& key = 0xff000000);
      ~AXFrameMap();
              
      IDirect3DTexture8* texture() const                       { return _texture; } 
      const AXFrame&     operator[](unsigned frame) const;
      const AXFrame&     operator()(const char* sequenceName, unsigned frame) const;
      const AXFrame&     frame(unsigned n) const               { return operator[](n); }
      const AXSequence*  sequence(const char* sequenceName) const;

      enum { MAX_SEQUENCES = 0xff };

   private:                    
      AXSequence*        _sequences[MAX_SEQUENCES];
      AXFrame*           _frames;
      unsigned           _numFrames;
      IDirect3DTexture8* _texture;
      
      // no copy or assignment
      AXFrameMap(const AXFrameMap&);
      AXFrameMap& operator=(const AXFrameMap&);      
   };

//---------------------------------------------------------------------------------
inline const AXFrame& AXFrameMap::operator[](unsigned frame) const
   {
   if(frame > _numFrames) throw XParams(XARGS); 
   return _frames[frame];
   }

//---------------------------------------------------------------------------------
inline const AXFrame& AXFrameMap::operator()(const char* sequenceName, unsigned frame) const 
   {
   if(frame > _numFrames) throw XParams(XARGS);
   return _frames[_sequences[AXUtility::hash<UC>(sequenceName)]->_ff + frame]; 
   }

#endif

//---------------------------------------------------------------------------------
inline const AXSequence* AXFrameMap::sequence(const char* sequenceName) const
   {
   AXSequence* sp = _sequences[AXUtility::hash<UC>(sequenceName)];
   if(!sp) throw XParams(XARGS);
   return sp;
   }


