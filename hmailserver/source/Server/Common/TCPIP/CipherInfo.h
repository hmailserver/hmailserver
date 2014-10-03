// Copyright (c) 2014 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class CipherInfo
   {
   public:
      CipherInfo(const AnsiString &name, const AnsiString &version, int bits) :
         name_(name),
         version_(version),
         bits_(bits),
         initialized_(true)
      {

      }

      CipherInfo() : 
         CipherInfo("", "", 0)
      {
         initialized_ = false;
      }

      AnsiString GetName() const 
      { 
         ThrowIfNotInitialized_();
         return name_; 
      }

      AnsiString GetVersion() const 
      { 
         ThrowIfNotInitialized_();
         return version_; 
      }
      
      int GetBits() const 
      { 
         ThrowIfNotInitialized_();
         return bits_; 
      }

   private:

      void ThrowIfNotInitialized_() const
      {
         if (!initialized_)
            throw new std::logic_error("cipherinfo instance is not initialized.");
      }

	   AnsiString name_;
      AnsiString version_;
      int bits_;
      bool initialized_;

   };
}