#include "AXConsole.h"
#include "AXConsoleCommands.h"
#include "AXEngine.h"

AXConsole::AXConsole(const AXFrameMap* backSrc, const char* backSequence, AXFrameMap* textSrc, const char* textSequence, const unsigned sz_history)
:  _background(backSrc, backSequence), 
   _history(textSrc, textSequence),
   _inputLine(textSrc, textSequence),
   _cursorPos(0), _bufIndex(0)
   {
   _history.resizeBuffer(sz_history);
   addCommand("quit", ConsoleCommands::quit);
   addCommand("screenShot", ConsoleCommands::screenShot);
   }

//---------------------------------------------------------------------------------
void AXConsole::position(const dxv& pos)
   { 
   _background.position(pos); 
   _history.position(pos);
   
   unsigned dy = (_background.pcf()->_rect.bottom - _background.pcf()->_rect.top) * _background.getScaling()->y;
   _inputLine.posX(_background.getPosition()->x);
   _inputLine.posY(_background.getPosition()->y + dy);
   }

//---------------------------------------------------------------------------------
void AXConsole::height(unsigned pixels)
   {
   unsigned dy = (_inputLine.pcf()->_rect.bottom - _inputLine.pcf()->_rect.top)  * _inputLine.getScaling()->y;

   _background.height(pixels);
   _inputLine.posY(_background.getPosition()->y + pixels - dy);
   }
//---------------------------------------------------------------------------------
AXConsole& AXConsole::operator<<(const char& c)
   {
   if(c == '\n') 
      {
      executeCommand(_inputLine.buffer());   
      _cursorPos = 0;
      }
   else
      {
      _inputLine << c;
      _cursorPos++;
      }

   return *this;
   }

//---------------------------------------------------------------------------------
void AXConsole::executeCommand(const char* cmd)
   {
   _history << _inputLine.buffer();
   UC index = AXUtility::hash<UC>(cmd);
   if(!_commands[index])
      {
      _history << "Unknown command\n";
      }
   else if(!_commands[index]())
      {
      _history << cmd << " failed\n";
      }  
   }

//---------------------------------------------------------------------------------
void AXConsole::addCommand(const char* name, AXConsoleCommand cmd)
   {
   UC index = AXUtility::hash<UC>(name);
   if(_commands[index] != 0) throw XParams(XARGS, "hash collision");
   _commands[index] = cmd;
   }

//---------------------------------------------------------------------------------
void AXConsole::activate()
   {
   }

//---------------------------------------------------------------------------------
void AXConsole::deactivate()
   {
   }