// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The results an SPF evaluation can produce, RFC 7208 section 2.6.
   //
   // Internal only: never persisted, and never passed over COM. What the COM API
   // exposes is whether SPF is used and what it scores, not what a check came to.
   //
   // Its own file, rather than a member of SPF, because SPF is the service's
   // entry point and drags in the singleton and the wide string type with it.
   // The evaluator, the record locator and the conformance suite are built by the
   // portable build as well, and they all have to be able to name a result.
   enum class SPFResult
   {
      // The domain publishes no SPF record, or no record could be looked up
      // because the domain is malformed or does not exist.
      None,

      // A record was found but it makes no assertion about the client.
      Neutral,

      // The client is authorized to send mail on behalf of the domain.
      Pass,

      // The client is not authorized to send mail on behalf of the domain.
      Fail,

      // The client is not authorized, but the domain asks that the message is
      // accepted rather than rejected.
      SoftFail,

      // The evaluation could not be completed, typically because a DNS lookup
      // failed. Evaluating the same message later may give another result.
      TempError,

      // The record was found but could not be evaluated, because it is
      // syntactically invalid or exceeds a processing limit.
      PermError
   };
}
