// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The results an SPF evaluation can produce, RFC 7208 section 2.6. Internal
   // only: never persisted, never passed over COM. Its own header so the portable
   // build can name one - see README.md.
   enum class SPFResult
   {
      // No record, or no record could be looked up.
      None,

      // A record was found but it makes no assertion about the client.
      Neutral,

      // The client is authorized to send mail on behalf of the domain.
      Pass,

      // The client is not authorized to send mail on behalf of the domain.
      Fail,

      // Not authorized, but the domain asks that the message is accepted.
      SoftFail,

      // Could not be completed, typically a failed lookup. Trying again may differ.
      TempError,

      // Found but not evaluable: invalid, or past a processing limit.
      PermError
   };
}
