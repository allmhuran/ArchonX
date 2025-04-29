#ifndef AXERROR_H
#define AXERROR_H

#include <windows.h>
#include <string>
#include <fstream>
#include <dxerr8.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//---------------------------------------------------------------------------------
#define AX_DEBUGGING
//---------------------------------------------------------------------------------
#ifdef AX_DEBUGGING
extern std::fstream trace;
#define START_DEBUG_TRACE trace.open("trace.log", std::ios::out);
#define DEBUG_FILE_TRACE trace << __FILE__ " : " << __LINE__ << std::endl;
#define DEBUG_TRACE(X) trace << X << std::endl;
#define END_DEBUG_TRACE trace.close();
#else
#define START_DEBUG_TRACE
#define DEBUG_FILE_TRACE
#define DEBUG_TRACE(X)
#define END_DEBUG_TRACE
#endif

#define XARGS __FILE__, __LINE__

//---------------------------------------------------------------------------------
// exceptions
//---------------------------------------------------------------------------------
class X
   {
   public:
      X(const char* name = "XAX", const char* file = "unspecified", int line = 0, const char* msg = "") { DEBUG_TRACE(name << " in " << file << " at " << line << ':\n' << '\t' << msg) }
      virtual ~X() {}  
   };

class XNoObj : public X       
   { 
   public: 
      XNoObj(const char* file, int line, const char* msg = "") : X( "XNoObj",file, line, msg) {} 
   };

class XParams : public X     
   { 
   public: 
      XParams(const char* file, int line, const char* msg = "") : X("XParams", file, line, msg) {} 
   };

class XInit : public X         
   { 
   public: 
      XInit(const char* file, int line, const char* msg = "") : X("XInit", file, line, msg) {} 
   };

class XCallback : public X 
   { 
   public: 
      XCallback(const char* file, int line, const char* msg = "") : X("XCallback", file, line, msg) {} 
   };

class XDeviceLost : public X   
   { 
   public: 
      XDeviceLost(const char* file, int line, const char* msg = "") : X("XDeviceLost", file, line, msg) {} 
   };

#endif

