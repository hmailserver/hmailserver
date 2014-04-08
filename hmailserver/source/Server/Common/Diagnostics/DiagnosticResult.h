// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class DiagnosticResult
   {
   public:
      
      DiagnosticResult(String name, String description, bool Success, String Details);
      DiagnosticResult();

      void SetName(const String &name) {_name = name;}
      void SetDescription(const String &description) {_description = description;}
      void SetDetails(const String &details) {_details = details;}
      void SetSuccess(bool success) {_success = success;}

      String GetName() const 
      {
         return _name;
      }

      String GetDescription() const
      {
         return _description;
      }

      String GetDetails() const
      {
         return _details;
      }

      bool GetSuccess() const
      {
         return _success;
      }

   private:
      
      String _name;
      String _description;
      bool _success;
      String _details;

   };


}
