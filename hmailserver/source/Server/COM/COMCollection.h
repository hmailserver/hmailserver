// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

template <class T, class P>
class COMCollectionItem
{
public:

   void AttachItem(shared_ptr<T> pObject)
   {
      object_ = pObject;
   }

   void AttachParent(shared_ptr<P> pParentColl, bool bObjectIsInParent)
   {
      parent_collection_ = pParentColl;
      m_bObjectIsInCollection = bObjectIsInParent;
   }

   void AddToParentCollection()
   {
      if (m_bObjectIsInCollection || !parent_collection_)
         return; 

      parent_collection_->AddItem(object_);
      m_bObjectIsInCollection = true;
   }

   shared_ptr<P> parent_collection_;
   shared_ptr<T> object_;


private:

   bool m_bObjectIsInCollection;
};

