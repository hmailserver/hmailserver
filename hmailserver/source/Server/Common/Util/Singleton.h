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

   static T * Instance()
   {
      if (instance_ == nullptr)
      {
         boost::lock_guard<boost::recursive_mutex> guard(create_instance_mutex_);

         instance_ = new T();
      }

      

      return instance_;
   }

private:

   static T * instance_;
   static boost::recursive_mutex create_instance_mutex_;

};

template <class T> T *Singleton<T>::instance_ = nullptr;
template <class T> boost::recursive_mutex Singleton<T>::create_instance_mutex_;