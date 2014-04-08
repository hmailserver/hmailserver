// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Math.h"
#include <Math.h>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Math::Math()
   {

   }

   Math::~Math()
   {

   }

   float 
   Math::Round(const float &number, const int num_digits)
   {
      float doComplete5i, doComplete5(number * powf(10.0f, (float) (num_digits + 1)));

      if(number < 0.0f)
         doComplete5 -= 5.0f;
      else
         doComplete5 += 5.0f;

      doComplete5 /= 10.0f;
      modff(doComplete5, &doComplete5i);

      return doComplete5i / powf(10.0f, (float) num_digits);
   }
}