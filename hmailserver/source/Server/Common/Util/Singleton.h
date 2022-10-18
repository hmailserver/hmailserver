// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

template <class T>
class Singleton
{
public:

   static T * Instance()
   {
      static T t;
      return &t;
   }
};