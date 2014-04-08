// Header file for using SPF routines for Windows
//
// Author: Roger Moser (roger.moser@pamho.net)
// Version 1.10
//
// License: You can do with this source whatever you want.


// define SPFDLL before incuding this file if using a DLL
#ifdef SPFDLL
#define SPFAPI __stdcall
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#else //SPFDLL
#define SPFAPI
#endif //SPFDLL

#ifndef SPFEXP
#define SPFEXP
#endif //SPFEXP


// values returned by SPFQuery()
enum
 {
  SPF_Pass, // + (permitted to send mail)
  SPF_SoftFail, // ~ (not permitted to send mail, but do not reject mail)
  SPF_Fail, // - (not permitted to send mail)
  SPF_Neutral, // ? (neither permitted not denied to send mail)
  SPF_None, // no SPF record
  SPF_TempError, // temporary error (error retrieving data from DNS)
  SPF_Error=SPF_TempError, // (obsolete)
  SPF_PermError, // permanent error (unknown mechanism or syntax error)
  SPF_Unknown=SPF_PermError // (obsolete)
 };
#define SPF_ResultMask 0xF // mask for result

enum
 {
  SPF_NoRecord=0x00, // no SPF record published
  SPF_BadDomain=0x10, // malformed domain
  SPF_NoDomain=0x20, // domain does not exist
  SPF_Literal=0x30 // domain is an address literal
 };
#define SPF_ReasonMask 0xF0 // mask for reason

// values for parameter 'flags' of SPFInit()
#define SPF_Multithread 1 // more than one thread may call SPF
#define SPF_CacheShared 2 // more than one application uses the cache file
#define SPF_CompressCache 4 // LZ compress the data in the cache file 


// SPFInit
// -------
// Initializes the SPF routines (loads the DNSAPI.DLL and opens the cache).
// Calling this function is optional if no cache is desired.
// Call it in one thread only and before calling any other SPF function.
// Return values:
//   0 if successful, 1: cannot load DNSAPI.DLL, 2: cannot open cache
// Parameters:
//   cachename: filename for the cache, or NULL if no cache file is desired
//   cachesize: size of cache in memory (ignored if cachename is not NULL)
//   flags: SPF_Multithread, SPF_CacheShared, SPF_CompressCache
// Comments:
//   Without SPF_CacheShared accessing the cache is a little faster.
//   With SPF_CompressCache the routine is about four times slower.
//   SPF_CacheShared and SPF_CompressCache are ignored is cachename is NULL.

SPFEXP int SPFAPI SPFInit(const char* cachename, unsigned cachesize,
  int flags);


// SPFExit
// -------
// Terminates the SPF routines (unloads the DNSAPI.DLL and closes the cache).
// Calling this function is optional if no cache is desired.
// Call it in one thread only.

SPFEXP void SPFAPI SPFExit(void);


// SPFSetWhiteList
// ---------------
// Sets the whitelist policy that is included before '-all', '~all' or '?all'
// if there is at least one mechanism with no prefix or '+', or if there
// is no SPF record at all.
// This function is not thread-safe, call it only while no thread is
// executing the SPFQuery() function.
// Parameter:
//   policy: SPF record (including the version string) or NULL
// Return value: zero if successful, otherwise non-zero

SPFEXP int SPFAPI SPFSetWhiteList(const char* policy);


// SPFSetBestGuess
// ---------------
// Sets the best-guess policy that is used if there is no SPF record.
// This function is not thread-safe, call it only while no thread is
// executing the SPFQuery() function.
// Parameter:
//   policy: NULL or SPF record (including the version string)
//   policy_a: NULL or SPF record for cases where there is only an A record
// Return value: zero if successful, otherwise non-zero
// Comment:
//   If 'policy_a' is NULL, then always 'policy' is taken, also for cases
//   where there is only an A record.
//   Using best-guess policies is not recommended.

SPFEXP int SPFAPI SPFSetBestGuess(const char* policy, const char* policy_a);


// SPFSetOurDomain
// ---------------
// Sets the receiving domain name for the 'r' macro.
// This function is not thread-safe, call it only while no thread is
// executing the SPFQuery() function.
// Parameter:
//   domain: receiving domain name or NULL for "unknown"
// Return value: zero if successful, otherwise non-zero

SPFEXP int SPFAPI SPFSetOurDomain(const char* domain);


// SPFSetFallBack
// --------------
// Sets the fall-back policy for the specified domain. The fall-back
// policy is used if for the domain no SPF record is published.
// Parameters:
//   domain: domain name, or NULL to remove all fall-back policies
//   policy: SPF record (including the version string), or NULL to remove it
//   ttl: time to live in seconds, or zero for 'forever'
// Return value: zero if successful, otherwise non-zero
// Comments:
//   SPFCleanupCache() also removes expires fall-back policies.
//   If a cache file is used, SPFSetFallBack(NULL,NULL,0) to remove all
//   fall-back policies may take a long time. So call it in a separate
//   thread.

SPFEXP int SPFAPI SPFSetFallBack(const char* domain, const char* policy,
  long ttl);


// SPFQuery
// --------
// Checks if the host is designated to send mail from the 'sender'
// by using the SPF records.
// Parameters:
//   family: IP address family, AF_INET (2) or AF_INET6 (23)
//   ipaddr: pointer to IP address of SMTP client (in network byte order)
//   sender: email address of sender (recommended) or domain of sender
//   policy: if not NULL, use this policy instead of getting if from the DNS
//   helo: HELO / EHLO string (optional, can be NULL)
//   client: host name of SMTP client (optional, can be NULL)
//   explain: will receive pointer to allocated string with explanation
//     if specified, otherwise will be NULL, must be freed by SPFFree()
//     (optional, can be NULL)
// Return values:
//   SPF_Pass: + (permitted to send mail)
//   SPF_SoftFail: ~ (not permitted to send mail, but do not reject mail)
//   SPF_Fail: - (not permitted to send mail)
//   SPF_Neutral: ? (neither permitted not denied to send mail)
//   SPF_None: no SPF record
//   SPF_None|SPF_BadDomain: no SPF record because of malformed domain
//   SPF_None|SPF_NoDomain: no SPF record because the domain does not exist
//   SPF_None|SPF_Literal: no SPF record and domain is an address literal
//   SPF_TempError: temporary error (error retrieving data from DNS)
//   SPF_PermError: permanent error (unknown mechanism or syntax error)
// Comments:
//   If 'sender' is NULL or empty, the function uses 'helo' as the sender.
//   If 'client' is given, then it should be the verified host name, that
//   means that the A record should point back to the IP address.

SPFEXP int SPFAPI SPFQuery(int family, const void* ipaddr,
  const char* sender, const char* policy, const char* helo,
  const char* client, const char** explain);


// SPFFree
// -------
// Frees a memory block allocated by an SPF function.
// Use it the free the buffer with the explanation.

SPFEXP void SPFAPI SPFFree(const char* s);


// SPFCleanupCache
// ---------------
// Removes the expired records from the cache.
// Parameter:
//   ttl: the routine removes the records whose TTL is less than 'ttl'
//        e.g. SPFCleanupCache(0) removes all expired records and
//        SPFCleanupCache(LONG_MAX) removes all records.
// Comment:
//   This function may take a long time. So call it in a separate thread.
//   It also removes expires fall-back policies.
//   If the cache is in the memory, then this function does not clean it up
//   because that is not required.

SPFEXP void SPFAPI SPFCleanupCache(long ttl);


// SPFGetHostName
// --------------
// Gets the host name from the IP address and tests if the A/AAAA record
// points back to the IP address. Otherwise the returned host name is
// "Unknown".
// Parameters:
//   family: IP address family, AF_INET (2) or AF_INET6 (23)
//   ipaddr: pointer to IP address in network byte order
//   host: pointer to buffer for returned host name
//   size: size of buffer for returned host name
//   domain: if not NULL, the function returns this domain if among the host
//     names, otherwise a subdomain of it, otherwise the first host name
// Return value: zero if the PTR entry exists and the A/AAAA record
//   points back to the IP address, non-zero otherwise

SPFEXP int SPFAPI SPFGetHostName(int family, const void* ipaddr, char* host,
  int size, const char* domain);


// SPFStringToAddr
// ---------------
// Converts text representation of IP address to binary form
// Parameters:
//   str: text representation of IP address
//   family: IP address family, AF_INET (2) or AF_INET6 (23)
//   ipaddr: pointer for result in network byte order
// Return value: pointer to character after the string, or NULL if invalid

SPFEXP const char* SPFAPI SPFStringToAddr(const char* str, int family,
  void* ipaddr);


// SPFGetAddress
// -------------
// Looks up the A record of the domain and returns the IP address.
// You may use this function for RBL (black list) lookup since the Windows
// version of gethostbyname() is not so useful.
// Parameter:
//   domain: domain name
// Return values: IP address in network byte order
//                0x00000000 if the domain does not exist or has no A record
//                0xffffffff if the lookup (temporarily) failed

SPFEXP unsigned long SPFAPI SPFGetAddress(const char* domain);


// SPFHasMX
// --------
// Checks if the domain exists and it has an MX or A record.
// Parameter:
//   address: email address or domain name
// Return values: SPF_Pass if an MX record exists
//                SPF_None if only an A record exists
//                SPF_SoftFail if an MX record exists but it is an IP address
//                SPF_Fail if the domain exists but has no MX or A record
//                SPF_TempError if the lookup (temporarily) failed
//                SPF_PermError if the domain does not exist
//                SPF_Neutral if the 'address' is NULL or empty

SPFEXP int SPFAPI SPFHasMX(const char* address);


// SPF result as string
// --------------------
// Parameter:
//   result: value returned by SPFQuery()
// Return value: pointer to string, or "unknown" if 'result' was invalid
// Comment:
//   Masking of 'result' with SPF_ResultMask is done by the function.

SPFEXP const char* SPFAPI SPFResultString(int result);

#ifdef SPFDLL
#ifdef __cplusplus
 }
#endif /*__cplusplus*/
#endif //SPFDLL
