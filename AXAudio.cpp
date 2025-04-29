#include "AXAudio.h"
#include "AXError.h"

//---------------------------------------------------------------------------------
AXAudio::AXAudio(const HWND& hw)
:  _dml(0), _dmp(0)
   {
   // start COM on the thread
   CoInitialize(NULL);

   CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,
                    IID_IDirectMusicLoader8, (void**)&_dml);


   CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                    IID_IDirectMusicPerformance8, (void**)&_dmp);      


   _dmp->InitAudio(NULL, NULL, hw, DMUS_APATH_SHARED_STEREOPLUSREVERB, 4, DMUS_AUDIOF_ALL, NULL);  
   }

//---------------------------------------------------------------------------------
AXAudio::~AXAudio()
   {
   _dmp->Stop(NULL, NULL, 0, 0);
   _dmp->CloseDown();

   _dml->Release();
   _dmp->Release();
   
   CoUninitialize();
   }

//---------------------------------------------------------------------------------
void AXAudio::loadWav(WCHAR* file, IDirectMusicSegment8** wav) const
   {
   _dml->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, file, reinterpret_cast<void**>(wav)); 
   (*wav)->Download(_dmp);  
   }

//---------------------------------------------------------------------------------
void AXAudio::playWav(IDirectMusicSegment8* wav, unsigned startIn, DWORD flags) const
   {
   _dmp->PlaySegment(wav, flags, startIn, NULL);
   }

