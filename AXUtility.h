#ifndef AXUTILITY_H
#define AXUTILITY_H

typedef unsigned char UC;

namespace AXUtility
   {

   template<class T>
   T hash(const char* index)
      {
      T* p = (T*)index;
      unsigned char res = 0;

      for(int i = 0; i < strlen(index); i++)
         {
         res = res << 1 ^ *p++;
         }

      return res;
      }
   }


#endif