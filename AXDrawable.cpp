#include "AXDrawable.h"

///////////////////////////////////////////////////////////////////////////////////
// AXDrawable 
///////////////////////////////////////////////////////////////////////////////////

AXDrawable::AXDrawable(const AXFrameMap* map, const char* sequenceName)
:  _map(map), 
   _sq(map->sequence(sequenceName)),
   _position(0, 0),
   _scaling(1.0, 1.0),
   _pivot(0, 0),
   _rotation(0),
   _colour(0xFFFFFFFF),
   _frameIndex(_sq->_ff),
   _visible(true)
   {
   // nothing
   }

//---------------------------------------------------------------------------------
AXDrawable::AXDrawable(const AXDrawable& rhs)
:  _map(rhs._map),
   _sq(rhs._sq),
   _position(rhs._position),
   _scaling(rhs._scaling),
   _rotation(rhs._rotation),
   _pivot(rhs._pivot),
   _colour(rhs._colour)
   {
   // nothing
   }

//---------------------------------------------------------------------------------
AXDrawable& AXDrawable::operator=(const AXDrawable& rhs)
   {
   if(this == &rhs) return *this; 
   _map = rhs._map;
   _sq = rhs._sq;
   _position = rhs._position;
   _scaling = rhs._scaling;
   _rotation = rhs._rotation;
   _pivot = rhs._pivot;
   _colour = rhs._colour;

   return *this;
   }

//---------------------------------------------------------------------------------
void AXDrawable::draw(ID3DXSprite& dxs)
   {     
   if(_visible)
      if(dxs.Draw(srcImg(), srcRect(), getScaling(), getPivot(), getRotation(), getPosition(), getColour()) != D3D_OK)
         throw XParams(XARGS);
   }

///////////////////////////////////////////////////////////////////////////////////
// AXText
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
AXText::AXText(const AXFrameMap* map, const char* sequenceName, int buf_sz, const char firstChar, unsigned extent)
 : AXDrawable(map, sequenceName), 
   _buffer(0),
   _buf_sz(buf_sz),
   _cz(firstChar),
   _charIndex(0)
   {
   _buffer = new char[_buf_sz];
   memset((void*)_buffer, 0, _buf_sz);
   readyFirstChar();
   }

//---------------------------------------------------------------------------------
AXText::AXText(const AXText& rhs)
:  AXDrawable(rhs),
   _buf_sz(rhs._buf_sz),
   _cz(rhs._cz),
   _extent(rhs._extent)
   {
   _buffer = new char[_buf_sz];
   memcpy((void*)_buffer, rhs._buffer, _buf_sz);
   readyFirstChar();
   }

//---------------------------------------------------------------------------------
AXText& AXText::operator=(const AXText& rhs)
   {
   if(this == &rhs) return *this;

   AXDrawable::operator=(rhs);
   _buf_sz = rhs._buf_sz;
   _cz = rhs._cz;
   _extent = rhs._extent;

   delete[] _buffer;
   _buffer = new char[_buf_sz];
   memcpy((void*)_buffer, rhs._buffer, _buf_sz);
   readyFirstChar();

   return *this;
   }

//---------------------------------------------------------------------------------
void AXText::resizeBuffer(unsigned new_sz)
   {
   char* tmp = new char[_buf_sz];
   strncpy(tmp, _buffer, new_sz);
   delete[] _buffer;
   _buffer = new char[new_sz];
   this->operator<<(tmp);
   delete[] tmp;
   }

//---------------------------------------------------------------------------------
void AXText::readyFirstChar()
   {
   _pivot.x = _pivot.y = 0;
   _charPos = _position;   
   }

//---------------------------------------------------------------------------------
void AXText::readyNextChar()
   {  
   const RECT& rect = pcf()->_rect;
   unsigned dx = (rect.right - rect.left) * _scaling.x;
   unsigned dy = (rect.bottom - rect.top) * _scaling.y;
   
   if(_buffer[++_charIndex] == '\n')
      {
      _charPos.x = _position.x;
      _charPos.y += dy;
      }
   else
      {
      _charPos.x += dx;

      if(abs(_charPos.x + dx - _position.x) > _extent)
         {
         _charPos.x = _position.x;
         _charPos.y += dy;
         }
      }

   _pivot = _position - _charPos;
   }
   
//---------------------------------------------------------------------------------
void AXText::draw(ID3DXSprite& dxs)
   {
   if(!_visible) return;
   
   while(_buffer[_charIndex] != '\0')
      {
      _frameIndex = _sq->_ff + _buffer[_charIndex] - _cz;
      AXDrawable::draw(dxs);
      readyNextChar();
      }  

   readyFirstChar();
   }

//---------------------------------------------------------------------------------
AXText& AXText::operator<<(const char& c)  
   { 
   if(_charIndex + 1 == _buf_sz) _charIndex = 0;  
   sprintf(_buffer + _charIndex++, "%c\0", c); return *this; 
   }

//---------------------------------------------------------------------------------
AXText& AXText::operator<<(const char* s)   
   { 
   for(int i = 0; s[i] != '\0'; i++)
      this->operator<<(s[i]);

   return *this;
   }

//---------------------------------------------------------------------------------
AXText& AXText::operator<<(const double& f)                  
   { 
   char tmp[20];
   _gcvt(f, 16, tmp);   
   return this->operator<<(tmp); 
   }

///////////////////////////////////////////////////////////////////////////////////
// AXSprite
///////////////////////////////////////////////////////////////////////////////////
const unsigned AXSprite::timerRes = 25;

//---------------------------------------------------------------------------------
AXSprite::AXSprite(const AXFrameMap* map, const char* sequenceName)
:  AXDrawable(map, sequenceName)
   {
   // nothing
   }

//---------------------------------------------------------------------------------
AXSprite& AXSprite::operator=(const AXSprite& rhs)
   {
   if(this == &rhs)
      return *this;

   return *this;
   }

//---------------------------------------------------------------------------------
// intelligently advances the frame for the current sequence
//---------------------------------------------------------------------------------
void AXSprite::advanceFrame()
   {
   if(_dir == FWD && ++_frameIndex == _sq->_lf)
      {
      switch(_mode)
         {
         case SINGLE:   
            { 
            stop();
            break;
            }

         case REPEAT:
            { 
            _frameIndex = _sq->_ff;  
            break; 
            }

         case RETURN:   
            { 
            lastSequence();
            break;
            }
         }
      }
   else if(--_frameIndex == _sq->_ff)
      {
      switch(_mode) 
         {
         case SINGLE:   
            { 
            stop();             
            break; 
            }

         case REPEAT:   
            { 
            _frameIndex = _sq->_lf;  
            break; 
            }

         case RETURN:   
            { 
            lastSequence();
            break; 
            }
         }
      }
   }

//---------------------------------------------------------------------------------
void AXSprite::lastSequence()
   {
   stop();

   if(!_lastSq)
      return;

   _sq = _lastSq;
   _mode = _lastMode;
   _dir = _lastDir;
   _frameIndex = _lastFrame;
   _lastSq = 0;
   }

//---------------------------------------------------------------------------------
// requests a callback if this frame has a limited lifespan (non zero interval)
//---------------------------------------------------------------------------------
void AXSprite::play()
   {
   unsigned delay = pcf()->_interval;

   if(delay > 0)
      _frameTimer = timeSetEvent(delay, AXSprite::timerRes, timeproc, (ULONG)this, TIME_ONESHOT);
   }

//---------------------------------------------------------------------------------
// kills the callback if pending
//---------------------------------------------------------------------------------
void AXSprite::stop()
   {
   if(_frameTimer)
      {
      timeKillEvent(_frameTimer);
      _frameTimer = 0;
      }
   }

//---------------------------------------------------------------------------------
// fired for limited lifespan frames to advance the animation
//---------------------------------------------------------------------------------
void CALLBACK AXSprite::timeproc(UINT uid, UINT msg, DWORD data, DWORD dw1, DWORD dw2)
   {
   AXSprite* sp = reinterpret_cast<AXSprite*>(data);

   if(!sp)
      return;
   
   sp->_frameTimer = 0;
   sp->advanceFrame();
   sp->play();
   }


