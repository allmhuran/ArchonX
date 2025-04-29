#ifndef AXDRAWABLE_H
#define AXDRAWABLE_H

#include "AXFrameMap.h"
#include "AXError.h"
#include "AXUtility.h"

typedef D3DXVECTOR2 dxv;

///////////////////////////////////////////////////////////////////////////////////
// ABC Drawable ... all visible graphics must inherit from this
///////////////////////////////////////////////////////////////////////////////////
class AXDrawable
   {
   public:
      AXDrawable(const AXFrameMap* map, const char* sequenceName);
      AXDrawable(const AXDrawable& rhs);
      AXDrawable& operator=(const AXDrawable& rhs);
      virtual ~AXDrawable() = 0 {}

      virtual void draw(ID3DXSprite& dxs);
      AXDrawable&  sequence(const char* s)        { _sq = _map->sequence(s);  }
      
      IDirect3DTexture8*   srcImg()               { return _map->texture(); }     
      virtual const AXFrame* pcf()                { return &(_map->frame(_frameIndex)); }

      virtual const dxv*   getPosition()          { return &_position; };
      virtual const dxv*   getScaling()           { return &_scaling; }
      virtual const dxv*   getPivot()             { return &_pivot; }         
      virtual float        getRotation()          { return _rotation; }
      virtual D3DCOLOR&    getColour()            { return _colour; }

      virtual void position(const dxv& p)         { _position = p;  }
      virtual void scaling(const dxv& s)          { _scaling = s;  }
      virtual void pivot(const dxv& p)            { _pivot = p;  }
      virtual void rotation(const float& radians) { _rotation = radians; }
      virtual void colour(const D3DCOLOR& c)      { _colour = c;  }

      virtual void scaleX(const float& x)         { _scaling.x = x;  }
      virtual void scaleY(const float& y)         { _scaling.y = y;  }
      virtual void scaleByX(const float& dx)      { _scaling.x *= dx;  }
      virtual void scaleByY(const float& dy)      { _scaling.y *= dy;  }
      virtual void scaleBy(const float& factor)   { _scaling *= factor;  }

      virtual void posX(const float& x)           { _position.x = x;  }      
      virtual void posY(const float& y)           { _position.y = y;  }
      virtual void moveX(const float& dx)         { _position.x += dx;  }
      virtual void moveY(const float& dy)         { _position.y += dy;  }
      virtual void move(const dxv& xy)            { _position += xy;  }

      virtual void mirrorX();                      
      virtual void mirrorY();

      virtual void visible(bool b)                { _visible = b; }
      virtual bool isVisible()                    { return _visible; }
   
   protected:
      const AXFrameMap* _map;
      const AXSequence* _sq;
      dxv               _position;
      dxv               _scaling;
      dxv               _pivot;
      float             _rotation;
      D3DCOLOR          _colour;
      unsigned          _frameIndex;
      bool              _visible;

   private:
      const RECT* srcRect()                      { return &(pcf()->_rect); }      
   };

inline void AXDrawable::mirrorX()
   {
   _position.x += ((pcf()->_rect.right - pcf()->_rect.left) * _scaling.x);
   _scaling.x *= -1;
   }

//---------------------------------------------------------------------------------
inline void AXDrawable::mirrorY()
   {
   _position.y += ((pcf()->_rect.bottom - pcf()->_rect.top) * _scaling.y);
   _scaling.y *= -1;
   }


///////////////////////////////////////////////////////////////////////////////////
// AXPicture: for static drawables (fixed size, no animation)
///////////////////////////////////////////////////////////////////////////////////
class AXPicture : public AXDrawable
   {
   public:
      AXPicture(const AXFrameMap* map, const char* sequenceName) : AXDrawable(map, sequenceName) {}
      ~AXPicture() {}
      
      void height(unsigned pixels) { _scaling.y = (float)pixels / (float)(pcf()->_rect.bottom - pcf()->_rect.top); };
      void width(unsigned pixels)  { _scaling.x = (float)pixels / (float)(pcf()->_rect.right - pcf()->_rect.left); };
      unsigned height(void)        { return (pcf()->_rect.bottom - pcf()->_rect.top) * _scaling.y; }
      unsigned width(void)         { return (pcf()->_rect.right - pcf()->_rect.left) * _scaling.x; }
   };


///////////////////////////////////////////////////////////////////////////////////
// AXText: for drawing text onto the screen
///////////////////////////////////////////////////////////////////////////////////
class AXText : public AXDrawable
   {
   public:
      AXText(const AXFrameMap* map, const char* sequenceName, int buf_sz = 100, const char firstChar = ' ', unsigned extent = 100);
      AXText(const AXText& rhs);
      AXText& operator=(const AXText& rhs);
      ~AXText() { delete[] _buffer; }

      // overrides
      void draw(ID3DXSprite& dxs);
      const dxv* getPosition()            { return &_charPos; }

      const unsigned& extent() const      { return _extent; }      
      void extent(unsigned pixels)        { _extent = pixels;  }      
      void resizeBuffer(unsigned new_sz);
      const char* buffer()                { return _buffer; }
      void clear()                        { _charIndex = 0; _buffer[0] = '\0'; }
      void backspace(int numChars = 1);


      void operator=(const char* s)                       { strncpy(_buffer, s, _buf_sz); _charIndex = 0; }
      void operator=(const double& f)                     { sprintf(_buffer, "%16.4f\0", f); _charIndex = 0; }
      AXText& operator<<(const char& c);
      AXText& operator<<(const char* s);
      AXText& operator<<(const int& i);
      AXText& operator<<(const double& f);

   private:      
      void readyFirstChar();
      void readyNextChar();

      char*       _buffer;
      unsigned    _buf_sz;
      char        _cz; // charzero, first char in the sequence on the image
      unsigned    _extent;
      unsigned    _charIndex;
      dxv         _charPos;
   };

//---------------------------------------------------------------------------------
inline void AXText::backspace(int numChars /* = 1 */)
   {
   for(int i = 0; i < numChars && _charIndex >= 0; i++)
      {
      _buffer[_charIndex--] = '\0';
      }
   }


///////////////////////////////////////////////////////////////////////////////////
// AXSprite for animated images
///////////////////////////////////////////////////////////////////////////////////
class AXSprite : public AXDrawable
   {
   public:      
      enum Mode { SINGLE = 0, REPEAT, RETURN };
      enum Dir  { FWD = 0, REV };

      // structors
      AXSprite(const AXFrameMap* map, const char* sequenceName);
      ~AXSprite() {};
      AXSprite& operator=(const AXSprite& rhs);

      void play();                      // creates an event callback which will advance the frame and call play again
      void stop();                               
      
      AXSprite& mode(Mode m)    { _mode = m;  }
      const Mode& mode() const  { return _mode; }
      
      AXSprite& dir(Dir d)      { _dir = d;  }     
      const Dir& dir() const    { return _dir; }
   
   protected:
      const RECT*  srcRect()    { return &(pcf()->_rect); }
      
   private:
      void  advanceFrame();
      void  lastSequence();
      
      MMRESULT             _frameTimer;    
      Mode                 _mode;
      Dir                  _dir;
      const AXSequence*    _lastSq;
      Mode                 _lastMode;
      Dir                  _lastDir;
      unsigned             _lastFrame;       

      static const unsigned timerRes;
      static void CALLBACK timeproc(UINT, UINT, DWORD, DWORD, DWORD);   
   };

#endif

