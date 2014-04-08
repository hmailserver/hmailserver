// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

template <class T>
class Singleton
{
public:

   virtual ~Singleton<T>()
   {

   }

   static void CreateInstance()
   {
      pInstance = new T;
   }

   static void DeleteInstance()
   {
      delete pInstance;
      pInstance = 0;
   }

   static T * Instance()
   {
      return pInstance;
   }

private:

   static T * pInstance;
};

template <class T> T *Singleton<T>::pInstance = NULL;