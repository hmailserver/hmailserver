// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // A domain whose ARC seal may override a DMARC failure. See SpamTestDMARC.
   class ARCTrustedSealer : public BusinessObject<ARCTrustedSealer>
   {
   public:
      ARCTrustedSealer(void);
      ~ARCTrustedSealer(void);

      String GetName() const {return domain_; }

      // The d= of the ARC-Seal, e.g. google.com.
      String GetDomain() const {return domain_; }
      void SetDomain(const String &domain) {domain_ = domain; }

      String GetDescription() const {return description_; }
      void SetDescription(const String &description) {description_ = description; }

      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true; }

   private:

      String domain_;
      String description_;
   };
}
