#ifndef AXCOMPONENT_H
#define AXCOMPONENT_H

class AXComponent
   {
   public:
      AXComponent() {}
      virtual ~AXComponent() {}

   protected:
      virtual void activate() = 0;
      virtual void deactivate() = 0;
   };

#endif