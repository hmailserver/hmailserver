// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


namespace HM
{

   template <typename T>
   struct CachedObject
   {
      CachedObject() {}

      CachedObject(std::shared_ptr<T> object) :
         object_(object),
         id_(object->GetID()),
         name_(object->GetName()),
         creation_time_(GetTickCount())
      {

      }

      int
      CachedObject::SecondsOld()
      {
         int iCurrentTime = GetTickCount();

         // Convert from milliseconds to seconds.
         int iAge = (iCurrentTime - creation_time_) / 1000;

         if (iAge < 0)
            iAge = 60 * 60 * 24;

         return iAge;
      }

      __int64 id_;
      std::wstring name_;
      int creation_time_;

      std::shared_ptr<T> object_;
   };

}
