// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

template <class T, class P>
class COMCollectionItem
{
public:

   void AttachItem(std::shared_ptr<T> pObject)
   {
      object_ = pObject;
   }

   void AttachParent(std::shared_ptr<P> pParentColl, bool bObjectIsInParent)
   {
      parent_collection_ = pParentColl;
      object_is_in_collection_ = bObjectIsInParent;
   }

   void AddToParentCollection()
   {
      if (object_is_in_collection_ || !parent_collection_)
         return; 

      parent_collection_->AddItem(object_);
      object_is_in_collection_ = true;
   }

   std::shared_ptr<P> parent_collection_;
   std::shared_ptr<T> object_;


private:

   bool object_is_in_collection_;
};

