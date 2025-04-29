#ifndef AXCONSOLE_H
#define AXCONSOLE_H

#include "AXComponent.h"
#include "AXDrawable.h"
#include "AXUtility.h"

#define N_CONSOLE_LINES 5
#define CONSOLE_WIDTH   20

typedef bool (*AXConsoleCommand)(void);

///////////////////////////////////////////////////////////////////////////////////
// AXConsole : general purpose engine console for text input 
///////////////////////////////////////////////////////////////////////////////////
class AXConsole : public AXComponent
   {
   friend class AXEngine;

   public:
      AXConsole& operator<<(const char& c);    

      void executeCommand(const char* cmd);
      void addCommand(const char* name, AXConsoleCommand cmd);

      
      const dxv* getPosition()         { return _background.getPosition(); }
      void position(const dxv& pos);
      
      void backspace()                 { _inputLine.backspace(); }
      void clearInput()                { _inputLine.clear(); }
      void clearHistory()              { _history.clear(); }
      bool active()                    { return _active; }
      void activate();
      void deactivate();
      void width(unsigned pixels)  { _background.width(pixels), _history.extent(pixels), _inputLine.extent(pixels); }
      void height(unsigned pixels);

      enum { MAX_COMMANDS = 0xff };
      
   private:
      AXConsole(const AXFrameMap* backSrc, const char* backSequence, AXFrameMap* textSrc, const char* textSequence, const unsigned sz_history = 300);
      ~AXConsole() {}

      AXConsoleCommand  _commands[MAX_COMMANDS];

      AXPicture   _background;
      AXText      _history;
      AXText      _inputLine;
      bool        _active;
      unsigned    _bufIndex;
      unsigned    _cursorPos;   

      // no implementation
      AXConsole(const AXConsole&);
      AXConsole& operator=(const AXConsole&);
   };

#endif