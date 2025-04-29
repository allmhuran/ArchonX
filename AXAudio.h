#ifndef AXAUDIO_H
#define AXAUDIO_H

#include <dmusici.h>
#include "AXComponent.h"

class AXAudio : public AXComponent
   {
   friend class AXEngine;

   public:
      void loadWav(WCHAR* file, IDirectMusicSegment8** wav) const;
      void playWav(IDirectMusicSegment8* wav, unsigned startIn = 0, DWORD flags = DMUS_SEGF_SECONDARY) const;

   private:
      // structors
      AXAudio(const HWND& hw);
      virtual ~AXAudio();      
      AXAudio(const AXAudio&);            // no impl
      AXAudio& operator=(const AXAudio&); // no impl

      void activate() {}
      void deactivate() {}

      IDirectMusicLoader8*       _dml;
      IDirectMusicPerformance8*  _dmp;        
   };

#endif