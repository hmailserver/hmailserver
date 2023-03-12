// SPF routines for Windows
//
// Author: Roger Moser (roger.moser@pamho.net)
// Version 1.10
//
// License: You can do with this source whatever you want.

#define STRICT

#include "StdAfx.h"

#undef _UNICODE
#undef UNICODE

#include <windns.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// define SPFDLL if creating a DLL
#ifdef SPFDLL
#define SPFEXP __declspec(dllexport)
#endif //SPFDLL

#include "rmspf.h"

// if required, define custom functions
#ifndef spfmalloc
#define spfmalloc malloc
#define spfrealloc realloc
#define spffree free
#endif //spfmalloc

// define this if the cache in a file should be implemented
// #define SPFFILECACHE

// define this if the cache in the memory should be implemented
//#define SPFMEMCACHE

// define one of following for the case where there is no SPF record
#define SPFZONECUT 0 // return SPF_None
// #define SPFZONECUT 1 // use the SPF record at the zone cut instead
//#define SPFZONECUT 2 // redirect to the zone cut (which becomes <domain>)

// define this if DNSAPI.DLL is supported by the compiler
// and it should be automatically loaded when the program starts
//#define DNSAPI_SUPP

// define this if the RamaDB library is linked and the RAMADB32.DLL should
// not be manually loaded (by calling LoadLibrary).
//#define RAMADB_LINKED


/****************************************************************************/
/* DNS API interface */

#ifdef DNSAPI_SUPP

#define pDnsQuery DnsQuery_A
#define pDnsRecordListFree DnsRecordListFree

#else //DNSAPI_SUPP

// from winerror.h
#define DNS_ERROR_RCODE_NAME_ERROR	9003L
#define DNS_INFO_NO_RECORDS		9501L

#ifdef __cplusplus
extern "C"
#endif //__cplusplus
typedef DNS_STATUS WINAPI typDnsQuery_A(PCSTR pszName, WORD wType,
DWORD Options, PIP4_ARRAY aipServers, PDNS_RECORD* ppQueryResults,
PVOID* pReserved);

#ifdef __cplusplus
extern "C"
#endif //__cplusplus
typedef VOID WINAPI typDnsRecordListFree(PDNS_RECORD pRecordList,
DNS_FREE_TYPE FreeType);

static HMODULE hmodDnsApi; // module handle of DNSAPI.DLL
static typDnsQuery_A* pDnsQuery; // pointer to DnsQuery_A() function
static typDnsRecordListFree* pDnsRecordListFree; // DnsRecordListFree() func.

#endif //DNSAPI_SUPP


#ifdef SPFFILECACHE

/****************************************************************************/
/* RamaDB interface */

#include "ramadb.h"

#ifdef RAMADB_LINKED

#define pIsamCreate IsamCreate
#define pIsamOpen IsamOpen
#define pIsamLock IsamLock
#define pIsamUnlock IsamUnlock
#define pIsamClose IsamClose
#define pIsamGet IsamGet
#define pIsamPut IsamPut
#define pIsamDelete IsamDelete

#else //RAMADB_LINKED

static int iRamaDB; // 0: RAMADB.DLL not loaded, 1: loaded, -1: error
static HMODULE hmodRamaDB; // module handle of RAMADB.DLL
static typIsamCreate* pIsamCreate;
static typIsamOpen* pIsamOpen;
static typIsamLock* pIsamLock;
static typIsamUnlock* pIsamUnlock;
static typIsamClose* pIsamClose;
static typIsamGet* pIsamGet;
static typIsamPut* pIsamPut;
static typIsamDelete* pIsamDelete;

#endif //RAMADB_LINKED

#endif //SPFFILECACHE


/****************************************************************************/
/* defines and enums */

#define SPF_Version "v=spf1"
#define SPF_VersionLen 6

#define false 0
#define true 1

#define SPFDOMSIZE 256 // max size of expanded domain name
#define SPFARECSIZE 256 // size of buffer for data record with IP addresses
#define SPFNRECSIZE 1024 // size of buffer for data record with domain names
#define SPFKEYSIZE 255 // size of buffer for search key
#define SPFMAXLOOKUPS 10 // max number of lookups

#if defined(SPFFILECACHE) || defined(SPFMEMCACHE)
#define SPFCACHE
#endif //defined(SPFFILECACHE) || defined(SPFMEMCACHE)

#ifdef SPFCACHE
// default TTL if DNS lookup failed or returned no records
#define SPFDEFTTL 7200 // 2 hours
#endif //SPFCACHE

#ifdef SPFMEMCACHE
#define SPFMAXDATALEN (8*255) // max data length+1 in memory cache
#endif //SPFMEMCACHE

// maximum and minimum time (dependent on implementation)
#define TIME_MAX ((time_t)~0/2!=0?(time_t)~0:~((time_t)1<<(sizeof(time_t)*8-1)))
#define TIME_MIN ((time_t)~0/2!=0?(time_t)0:((time_t)1<<(sizeof(time_t)*8-1)))
#ifndef LONG_MAX
#define LONG_MAX (~((long)1<<(sizeof(long)*8-1)))
#endif //LONG_MAX

#define SPF_NoMatch (SPF_PermError+1)

#ifdef SPFCACHE
#define SPF_NoMemory (SPF_PermError+2)
#define SPFEXPSIZE sizeof(time_t) // sizeof(dns_expire)
#else //SPFCACHE
#define SPF_NoMemory SPF_TempError
#define SPFEXPSIZE 0
#endif //SPFCACHE

// from winsock.h
#ifndef AF_INET
#define AF_INET 2
#endif //AF_INET
#ifndef AF_INET6
#define AF_INET6 23
#endif //AF_INET6

#ifdef SPFCACHE
// record types in cache
enum
{
   SPF_Type_A,
   SPF_Type_AAAA,
   SPF_Type_MX,
   SPF_Type_SPF,
   SPF_Type_MS,
   SPF_Type_EXP,
   SPF_Type_FallBack,
#if SPFZONECUT
   SPF_Type_ZoneCut,
#endif //SPFZONECUT
   SPF_Type_PTR // (must be last element)
};
#endif //SPFCACHE


/****************************************************************************/
/* typedefs */

typedef int spfbool;
typedef unsigned char uchar;
typedef unsigned short uint16;
typedef unsigned long uint32;

typedef struct
{
   int spf_level; // subquery counter (0x0101: include, 0x0100: redirect)
   int spf_lookups; // number of mechanisms that did a DNS lookup
   spfbool spf_ipv6; // IP address is IPv6
   spfbool spf_testhelo; // testing the HELO domain (because no sender)
   time_t spf_time; // current time (Unix time)
   const char* spf_policy; // SPF policy override
   const uchar* spf_ipaddr; // IP address of SMTP client
   const char* spf_sender; // responsible sender (NULL if no local part)
   const char* spf_domain; // domain of responsible sender
   const char* spf_helo; // HELO/EHLO domain (or NULL)
   const char* spf_client; // SMTP client domain name (or NULL)
   const char* spf_expdom; // domain that has explanation in TXT record
   const char** spf_explain; // non-zero if explanation is desired
} spfrec;

typedef struct
{
#ifdef SPFCACHE
   time_t dns_expire; // Unix time when data expires
#endif //SPFCACHE
   char dns_data[1]; // data
} dnsrec;

typedef struct
{
   int lst_keylen; // key length
   int lst_datlen; // data length
#ifndef SPFCACHE
   time_t lst_expire; // Unix time when data expires
#endif //SPFCACHE
   char lst_data[1]; // data
} lstrec;


/****************************************************************************/
/* variables and constants */

extern const char* SPFResults[] =
{
   "Pass",
   "SoftFail",
   "Fail",
   "Neutral",
   "None",
   "TempError",
   "PermError"
};

static const char hexdigit[] = "0123456789ABCDEF";
static const char unknownstr[] = "unknown";

static int initialized; // >0:library is initialized, <0:could not initialize
static spfbool multithread; // library is used by multithreaded application
static CRITICAL_SECTION listmutex; // semaphore for accessing fall-back list

#ifdef SPFCACHE
static CRITICAL_SECTION cachemutex; // semaphore for accessing cache
static spfbool withcache; // with cache
#endif //SPFCACHE

#ifdef SPFFILECACHE
static HISAM hCache; // handle for cache file
#endif //SPFFILECACHE

#ifdef SPFMEMCACHE
static unsigned cachesize; // size of cache buffer
static uchar* cachebuf; // cache buffer
static uchar* cacheend; // end of cache buffer (cachebuf+cachesize)
static uint32 crc32tbl[256]; // CRC32 table (for hash function)
#endif //SPFMEMCACHE

static lstrec** listRec; // list of fallbacks (if no cache file is used)
static int listn; // number of fallbacks
static int listm; // max number of fallbacks

static const char* whitelist; // whitelist policy (included before the 'all')
static const char* bestguess; // best-guess policy (if no SPF record)
static const char* bestguess_a; // best-guess policy (if only A record)
static const char* ourdomain; // receiving domain (only for 'r' macro)

static const uchar localhost4[] = { 127, 0, 0, 0 };
static const uchar localhost6[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
static const uchar ipv4mapped[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };


/****************************************************************************/
/* character types (instead of ctype of the C library) */

#define EMP 0x01
#define DIG 0x02
#define HEX 0x04
#define UPP 0x08
#define LOW 0x10
#define URL 0x20
#define NAM 0x40
#define DLM 0x80
#define UPX (HEX|UPP)
#define LOX (HEX|LOW)

static const uchar chtype[256] =
{
   EMP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   EMP, URL, 0, 0, 0, 0, 0, URL, URL, URL, URL, DLM, DLM, NAM, NAM, DLM,
   DIG, DIG, DIG, DIG, DIG, DIG, DIG, DIG, DIG, DIG, 0, 0, 0, DLM, 0, 0,
   0, UPX, UPX, UPX, UPX, UPX, UPX, UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP,
   UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP, 0, 0, 0, 0, NAM,
   0, LOX, LOX, LOX, LOX, LOX, LOX, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW,
   LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, 0, 0, 0, URL, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define ISEMPTY(c) (chtype[(uchar)(c)]&EMP)
#define ISDIGIT(c) (chtype[(uchar)(c)]&DIG)
#define ISXDIGIT(c) (chtype[(uchar)(c)]&(DIG|HEX))
#define ISALPHA(c) (chtype[(uchar)(c)]&(UPP|LOW))
#define ISALNUM(c) (chtype[(uchar)(c)]&(DIG|UPP|LOW))
#define ISNAMEC(c) (chtype[(uchar)(c)]&(DIG|UPP|LOW|NAM))
#define ISLOWER(c) (chtype[(uchar)(c)]&LOW)
#define ISUPPER(c) (chtype[(uchar)(c)]&UPP)
#define ISDELIM(c) (chtype[(uchar)(c)]&(DLM|NAM))
#define ISURIC(c) (chtype[(uchar)(c)]&(DIG|UPP|LOW|NAM|URL))
#define TOLOWER(c) spftolower((uchar)(c))


static int
spftolower(int c)
{
   if (c >= 'A' && c <= 'Z')
      return c - 'A' + 'a';
   return c;
}


/****************************************************************************/
/* functions */


static int check_host(spfrec* spfp, const char* domain);


// duplicate string
static char*
spfstrdup(const char* s)
{
   int len;
   char* cp;

   len = (int)strlen(s) + 1;
   cp = (char*)spfmalloc(len);
   if (cp != NULL)
      memcpy(cp, s, len);
   return cp;
}


// initialize the library
static spfbool
initspf(int multithreaded)
{
   HANDLE hMutex;

   if (initialized > 0) // already initialized
      return true;
   if (initialized < 0) // could not be initialized
      return false;

   if (multithreaded > 0) // not called by SPFInit()
   {
      // only one thread should initialize the library
      hMutex = CreateMutex(NULL, false, _T("RMSPFMutex"));
      if (hMutex == NULL)
         return false;

      switch (WaitForSingleObject(hMutex, 10000))
      {
      case WAIT_OBJECT_0:
      case WAIT_ABANDONED:
         break;
      default:
         return false;
      }

      // (must test it again)
      if (initialized > 0) // already initialized
      {
         ReleaseMutex(hMutex);
         return true;
      }
      if (initialized < 0) // could not be initialized
      {
         ReleaseMutex(hMutex);
         return false;
      }
   }

#ifndef DNSAPI_SUPP
   // load DNSAPI library functions
   hmodDnsApi = LoadLibrary(_T("DNSAPI.DLL"));
   if (hmodDnsApi == NULL)
   {
      initialized = -1;
      if (multithreaded > 0)
         ReleaseMutex(hMutex);
      return false;
   }
   pDnsQuery = (typDnsQuery_A*)GetProcAddress(hmodDnsApi, "DnsQuery_A");
   pDnsRecordListFree = (typDnsRecordListFree*)GetProcAddress(hmodDnsApi,
      "DnsRecordListFree");
   if (pDnsQuery == NULL || pDnsRecordListFree == NULL)
   {
      FreeLibrary(hmodDnsApi);
      initialized = -1;
      if (multithreaded > 0)
         ReleaseMutex(hMutex);
      return false;
   }
#endif //DNSAPI_SUPP

   if (multithreaded == 0)
      multithread = false;
   else
   {
#ifdef SPFCACHE
      InitializeCriticalSection(&cachemutex);
#endif //SPFCACHE
      InitializeCriticalSection(&listmutex);
      multithread = true;
   }

   initialized = 1;
   if (multithreaded > 0)
      ReleaseMutex(hMutex);
   return true;
}


#ifdef SPFFILECACHE
#ifndef RAMADB_LINKED
// load the RAMADB.DLL
static spfbool
loadramadb(void)
{
   if (iRamaDB>0) // already loaded
      return true;
   if (iRamaDB<0) // could not be loaded
      return false;

   // load RamaDB library functions
   hmodRamaDB=LoadLibrary("RAMADB32.DLL");
   if (hmodRamaDB==NULL)
   {
      iRamaDB=-1;
      return false;
   }
   pIsamCreate=(typIsamCreate*)GetProcAddress(hmodRamaDB,"_IsamCreate@24");
   pIsamOpen=(typIsamOpen*)GetProcAddress(hmodRamaDB,"_IsamOpen@16");
   pIsamLock=(typIsamLock*)GetProcAddress(hmodRamaDB,"_IsamLock@4");
   pIsamUnlock=(typIsamUnlock*)GetProcAddress(hmodRamaDB,"_IsamUnlock@4");
   pIsamClose=(typIsamClose*)GetProcAddress(hmodRamaDB,"_IsamClose@4");
   pIsamGet=(typIsamGet*)GetProcAddress(hmodRamaDB,"_IsamGet@40");
   pIsamPut=(typIsamPut*)GetProcAddress(hmodRamaDB,"_IsamPut@20");
   pIsamDelete=(typIsamDelete*)GetProcAddress(hmodRamaDB,"_IsamDelete@12");
   if (pIsamCreate==NULL
      || pIsamOpen==NULL
      || pIsamLock==NULL
      || pIsamUnlock==NULL
      || pIsamClose==NULL
      || pIsamGet==NULL
      || pIsamPut==NULL
      || pIsamDelete==NULL)
   {
      FreeLibrary(hmodRamaDB);
      iRamaDB=-1;
      return false;
   }
   iRamaDB=1;
   return true;
}
#endif //RAMADB_LINKED
#endif //SPFFILECACHE


// exit SPF (optional if no cache)
void
SPFExit(void)
{
#ifdef SPFFILECACHE
   if (hCache!=NULL)
   {
      pIsamClose(hCache);
      hCache=NULL;
   }
#ifndef RAMADB_LINKED
   if (iRamaDB>0)
      FreeLibrary(hmodRamaDB);
   iRamaDB=0;
#endif //RAMADB_LINKED
#endif //SPFFILECACHE

#ifdef SPFMEMCACHE
   if (cachebuf!=NULL)
   {
      spffree((void*)cachebuf);
      cachebuf=NULL;
   }
#endif //SPFMEMCACHE

#ifdef SPFCACHE
   withcache=false;
#endif //SPFCACHE

   if (whitelist != NULL)
   {
      spffree((void*)whitelist);
      whitelist = NULL;
   }
   if (bestguess != NULL)
   {
      spffree((void*)bestguess);
      bestguess = NULL;
   }
   if (bestguess_a != NULL)
   {
      spffree((void*)bestguess_a);
      bestguess_a = NULL;
   }
   if (ourdomain != NULL)
   {
      spffree((void*)ourdomain);
      ourdomain = NULL;
   }

   if (initialized > 0)
   {
#ifndef DNSAPI_SUPP
      FreeLibrary(hmodDnsApi);
#endif //DNSAPI_SUPP
      if (multithread)
      {
#ifdef SPFCACHE
         DeleteCriticalSection(&cachemutex);
#endif //SPFCACHE
         DeleteCriticalSection(&listmutex);
         multithread = false;
      }
   }
   initialized = 0;
}


#ifdef SPFMEMCACHE

// make the CRC32 table
static void
makecrc32(void)
{
   int i;
   int c;
   unsigned long crc;

   for (c=0; c<256; c++)
   {
      crc=c;
      for (i=8; i>0; i--)
         crc=crc&1?(crc>>1)^0xEDB88320L:crc>>1;
      crc32tbl[c]=crc;
   }
}

#endif //SPFMEMCACHE


// initialize SPF (optional if no cache), return zero if successful
int
SPFInit(const char* cachename, unsigned cachesiz, int flags)
{
#ifdef SPFFILECACHE
   ISAMERROR isamerr;
   unsigned isamflags;
#endif //SPFFILECACHE

   SPFExit(); // in case SPFInit() was already called

   if (!initspf(flags&SPF_Multithread ? -1 : 0))
   {
      SPFExit();
      return 1;
   }

#ifdef SPFFILECACHE
   if (cachename!=NULL && *cachename!=0)
   {
#ifndef RAMADB_LINKED
      if (!loadramadb())
      {
         SPFExit();
         return 2;
      }
#endif //RAMADB_LINKED
      isamflags=ISAM_KeyCompress;
      if (flags&SPF_CacheShared)
         isamflags|=ISAM_Shared;
      if (flags&SPF_CompressCache)
         isamflags|=ISAM_LZCompress;
      isamerr=pIsamOpen(&hCache,cachename,isamflags,NULL);
      if (isamerr==ISAM_FileNotFound)
         isamerr=pIsamCreate(&hCache,cachename,isamflags,2048,48,NULL);
      if (isamerr!=ISAM_Ok)
      {
         SPFExit();
         return 3;
      }
      withcache=true;
   }
#ifdef SPFMEMCACHE
   else
#endif //SPFMEMCACHE
#else //SPFFILECACHE
   cachename = cachename;
#endif //SPFFILECACHE

#ifdef SPFMEMCACHE
   if (cachesiz!=0)
   {
      cachebuf=(uchar*)spfmalloc(cachesiz);
      if (cachebuf==NULL)
      {
         SPFExit();
         return 4;
      }
      memset(cachebuf,0,cachesiz);
      cachesize=cachesiz;
      cacheend=cachebuf+cachesiz;
      makecrc32();
      withcache=true;
   }
#else //SPFMEMCACHE
   cachesiz = cachesiz;
#endif //SPFMEMCACHE

   return 0;
}


// check version of SPF record
static spfbool
checkversion(const char* s)
{
   return _memicmp(s, SPF_Version, SPF_VersionLen) == 0
      && ISEMPTY(s[SPF_VersionLen]);
}


// set the whitelist policy, return zero if successful
int
SPFSetWhiteList(const char* policy)
{
   if (whitelist != NULL)
   {
      spffree((void*)whitelist);
      whitelist = NULL;
   }
   if (policy != NULL && *policy != 0)
   {
      if (!checkversion(policy))
         return SPF_PermError;
      whitelist = spfstrdup(policy);
      if (whitelist == NULL)
         return SPF_TempError;
   }
   return 0;
}


// set the best-guess policy, return zero if successful
int
SPFSetBestGuess(const char* policy, const char* policy_a)
{
   if (bestguess != NULL)
   {
      spffree((void*)bestguess);
      bestguess = NULL;
   }
   if (policy != NULL && *policy != 0)
   {
      if (!checkversion(policy))
         return SPF_PermError;
      bestguess = spfstrdup(policy);
      if (bestguess == NULL)
         return SPF_TempError;
   }

   if (bestguess_a != NULL)
   {
      spffree((void*)bestguess_a);
      bestguess_a = NULL;
   }
   if (policy_a != NULL && *policy_a != 0)
   {
      if (!checkversion(policy_a))
         return SPF_PermError;
      bestguess_a = spfstrdup(policy_a);
      if (bestguess_a == NULL)
         return SPF_TempError;
   }

   return 0;
}


// set the receiving domain name, return zero if successful
int
SPFSetOurDomain(const char* domain)
{
   if (ourdomain != NULL)
   {
      spffree((void*)ourdomain);
      ourdomain = NULL;
   }
   if (domain != NULL && *domain != 0)
   {
      ourdomain = spfstrdup(domain);
      if (ourdomain == NULL)
         return SPF_TempError;
   }
   return 0;
}


// free memory
void
SPFFree(const char* s)
{
   if (s != NULL)
      spffree((void*)s);
}


// convert integer to decimal string
static int
numtostr(char* buf, unsigned val)
{
   int i;
   int len;
   char s[3 * sizeof(val)];

   for (i = sizeof(s) - 1; i >= 0; i--)
   {
      s[i] = (char)(val % 10 + '0');
      val /= 10;
      if (val == 0)
         break;
   }
   for (len = 0; i < sizeof(s); i++, len++)
      buf[len] = s[i];
   buf[len] = 0;
   return len;
}


// convert time to decimal string
static int
timetostr(char* buf, time_t val)
{
   int i;
   int len;
   char s[3 * sizeof(val)];

   for (i = sizeof(s) - 1; i >= 0; i--)
   {
      s[i] = (char)(val % 10 + '0');
      val /= 10;
      if (val == 0)
         break;
   }
   for (len = 0; i < sizeof(s); i++, len++)
      buf[len] = s[i];
   buf[len] = 0;
   return len;
}


// convert integer to hexadecimal string
static int
numtohexstr(char* buf, unsigned val)
{
   int i;
   int len;
   char s[2 * sizeof(val)];

   for (i = sizeof(s) - 1; i >= 0; i--)
   {
      s[i] = hexdigit[val & 15];
      val >>= 4;
      if (val == 0)
         break;
   }
   for (len = 0; i < sizeof(s); i++, len++)
      buf[len] = s[i];
   buf[len] = 0;
   return len;
}


// convert IPv4 string to binary form, return pointer to next char, or NULL
static const char*
aton4(const char* s, uchar* addr, int term)
{
   int n;
   int val;
   const char* cp;

   n = 0;
   for (cp = s;; cp++)
   {
      if (n >= 4 || !ISDIGIT(*cp))
         return NULL;
      val = *cp - '0';
      for (cp++; ISDIGIT(*cp); cp++)
      {
         val = val * 10 + *cp - '0';
         if (val > 255)
            return NULL;
      }
      addr[n++] = (uchar)val;
      if (*cp != '.')
         break;
   }
   if (n != 4 || !ISEMPTY(*cp) && *cp != term)
      return NULL;
   return cp;
}


// convert IPv6 string to binary form, return pointer to next char, or NULL
static const char*
aton6(const char* s, uchar* addr, int term)
{
   int i;
   int n;
   int val;
   int nulls;
   const char* cp;
   const char* cp1;

   cp = s;

   nulls = 16 + 2;
   if (*cp == ':')
   {
      cp++;
      if (*cp != ':')
         return NULL;
   }

   n = 0;
   for (;; cp++)
   {
      if (*cp == ':')
      {
         if (nulls < 16)
            return NULL;
         nulls = n;
         continue;
      }

      cp1 = cp;
      if (n >= 16)
         return NULL;
      if (!ISXDIGIT(*cp))
      {
         if (nulls == n) // was just "::"
         {
            i = 16;
            break;
         }
         return NULL;
      }
      val = ISDIGIT(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10;
      for (cp++; ISXDIGIT(*cp); cp++)
      {
         val = val * 16 + (ISDIGIT(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10);
         if (val > 0xffff)
            return NULL;
      }

      if (*cp == '.')
      {
         cp = aton4(cp1, addr + 12, term);
         if (cp == NULL)
            return NULL;
         i = 12;
         break;
      }

      addr[n++] = (uchar)(val >> 8);
      addr[n++] = (uchar)val;
      if (*cp != ':')
      {
         i = 16;
         break;
      }
   }

   if (nulls < i)
   {
      if (n >= i)
         return NULL;
      for (i--, n--; n >= nulls; i--, n--)
         addr[i] = addr[n];
      for (; i >= nulls; i--)
         addr[i] = 0;
   }
   else if (nulls == i || n != i)
      return NULL;

   if (!ISEMPTY(*cp) && *cp != term)
      return NULL;
   return cp;
}


// convert IP string to binary form, return pointer to next char, or NULL
const char*
SPFStringToAddr(const char* str, int family, void* ipaddr)
{
   if (str != NULL && ipaddr != NULL)
   {
      switch (family)
      {
      case AF_INET:
         return aton4(str, (uchar*)ipaddr, 0);
      case AF_INET6:
         return aton6(str, (uchar*)ipaddr, 0);
      }
   }
   return NULL;
}


// convert IP address to string, return length of string
static int
ntoa(spfbool ipv6, const uchar* addr, char* s, spfbool fordomain)
{
   int i;
   int k;
   int i1;
   int n1;
   int len;
   char* cp;

   if (!ipv6)
   {
      len = 0;
      for (i = 0;; i++)
      {
         len += numtostr(s + len, addr[i]);
         if (i == 3)
            break;
         s[len++] = '.';
      }
      s[len] = 0;
      return len;
   }

   // if string will be a domain name, use nibble format
   if (fordomain)
   {
      cp = s;
      for (i = 0; i < 16; i++)
      {
         *cp++ = hexdigit[addr[i] >> 4];
         *cp++ = '.';
         *cp++ = hexdigit[addr[i] & 15];
         *cp++ = '.';
      }
      cp[-1] = 0;
      return 63;
   }

   // find out where to write "::"
   i1 = 16;
   n1 = 0;
   for (i = 0; i < 16; i += 2)
   {
      if (*(uint16*)(addr + i) == 0)
      {
         for (k = i + 2; k < 16 && *(uint16*)(addr + k) == 0; k += 2)
            ;
         if (n1 < k - i)
         {
            n1 = k - i;
            i1 = i;
         }
      }
   }

   len = 0;
   i = 0;
   while (i < 16)
   {
      if (i == 12 && i1 == 0 && (n1 >= 12 || n1 == 10 && *(uint16*)(addr + 10) == 0xffff))
         return len + ntoa(false, addr + 12, s + len, false);
      if (i == i1)
      {
         s[len++] = ':';
         s[len++] = ':';
         i += n1;
      }
      else
      {
         len += numtohexstr(s + len, (addr[i] << 8) | addr[i + 1]);
         i += 2;
         if (i != i1 && i != 16)
            s[len++] = ':';
      }
   }
   s[len] = 0;

   return len;
}


// check if 'subdomain' is the 'domain' or a subdomain of it
static spfbool
indomain(const char* subdomain, const char* domain)
{
   int i;
   int len;

   len = (int)strlen(domain);
   i = (int)strlen(subdomain) - len;
   if (i < 0 || i>0 && subdomain[i - 1] != '.')
      return false;
   if (_memicmp(domain, subdomain + i, len) != 0)
      return false;
   return true;
}


// DNS query, return codes: -1: ok, >0: result (never returns 0)
static int
dnsquery(const char* domain, int type, DNS_RECORD** dnsrespp)
{
   DNS_STATUS rc;

   *dnsrespp = NULL;
   rc = pDnsQuery(domain, (WORD)type, DNS_QUERY_TREAT_AS_FQDN, NULL, dnsrespp, NULL);
   if (rc == DNS_INFO_NO_RECORDS) // (bug in Microsoft's DLL)
   {
      rc = pDnsQuery(domain, (WORD)type,
         DNS_QUERY_BYPASS_CACHE | DNS_QUERY_TREAT_AS_FQDN, NULL, dnsrespp, NULL);
   }
   switch (rc)
   {
   case NO_ERROR:
      return -1;
   case DNS_ERROR_RCODE_NAME_ERROR: // NXDOMAIN
      return SPF_PermError;
   case DNS_INFO_NO_RECORDS:
      return SPF_None;
   }
   return SPF_TempError;
}


#ifdef SPFMEMCACHE

// read record from memory cache
static spfbool
getrecord(const uchar* key, unsigned keylen,
uchar* dat, unsigned* datlenp, unsigned datsize)
{
   unsigned i;
   unsigned len;
   unsigned datlen;
   unsigned long crc;
   uchar* cp;

   // hash key
   crc=0xffffffffU;
   for (i=0; i<keylen; i++)
      crc=crc32tbl[(uchar)crc^key[i]]^(crc>>8);
   cp=cachebuf+crc%cachesize;

   // records must start with 255
   if (*cp!=(uchar)255)
      return false;

   // check if it is the correct key
   if (++cp>=cacheend)
      cp=cachebuf;
   if (*cp!=(uchar)(keylen-1))
      return false;
   for (i=0; i<keylen; i++)
   {
      if (++cp>=cacheend)
         cp=cachebuf;
      switch (*cp)
      {
      case 255: // overwritten
         return false;
      case 254: // escaped 254 or 255
         if (++cp>=cacheend)
            cp=cachebuf;
         if (*cp==(uchar)255) // overwritten
            return false;
         if (key[i]!=(uchar)(254|*cp))
            return false;
         break;
      default:
         if (key[i]!=*cp)
            return false;
      }
   }

   // get data length
   if (++cp>=cacheend)
      cp=cachebuf;
   datlen=*cp;
   if (datlen>=(255-SPFMAXDATALEN/255))
   {
      if (datlen==255) // overwritten
         return false;
      if (++cp>=cacheend)
         cp=cachebuf;
      if (*cp==(uchar)255) // overwritten
         return false;
      datlen=(datlen-(255-SPFMAXDATALEN/255))*255+*cp;
   }

   // get data
   i=0;
   len=datlen;
   if (dat!=NULL)
   {
      if (len>datsize)
         len=datsize;

      for ( ; i<len; i++)
      {
         if (++cp>=cacheend)
            cp=cachebuf;
         switch (*cp)
         {
         case 255: // overwritten
            return false;
         case 254: // escaped 254 or 255
            if (++cp>=cacheend)
               cp=cachebuf;
            if (*cp==(uchar)255) // overwritten
               return false;
            dat[i]=(uchar)(254|*cp);
            break;
         default:
            dat[i]=*cp;
         }
      }
   }

   // check if rest of data is not overwritten
   while (i<datlen)
   {
      if (++cp>=cacheend)
         cp=cachebuf;
      switch (*cp)
      {
      case 255: // overwritten
         return false;
      case 254: // escaped 254 or 255
         break;
      default:
         i++;
      }
   }

   if (datlenp!=NULL)
      *datlenp=len;
   return true;
}


// write record to memory cache, return true if successful
static spfbool
putrecord(const uchar* key, unsigned keylen,
const uchar* dat, unsigned datlen)
{
   unsigned i;
   unsigned long crc;
   uchar* cp;

   if (datlen>=SPFMAXDATALEN) // don't put long records in cache
      return false;

   // hash key
   crc=0xffffffffU;
   for (i=0; i<keylen; i++)
      crc=crc32tbl[(uchar)crc^key[i]]^(crc>>8);
   cp=cachebuf+crc%cachesize;

   // records start with 255
   *cp=255;

   // put key
   if (++cp>=cacheend)
      cp=cachebuf;
   *cp=(uchar)(keylen-1);
   for (i=0; i<keylen; i++)
   {
      if (++cp>=cacheend)
         cp=cachebuf;
      if (key[i]<(uchar)254)
         *cp=key[i];
      else
      {
         *cp=254;
         if (++cp>=cacheend)
            cp=cachebuf;
         *cp=(uchar)(key[i]&1);
      }
   }

   // put data length
   if (++cp>=cacheend)
      cp=cachebuf;
   if (datlen<(255-SPFMAXDATALEN/255))
      *cp=(uchar)datlen;
   else
   {
      *cp=(uchar)((255-SPFMAXDATALEN/255)+datlen/255);
      if (++cp>=cacheend)
         cp=cachebuf;
      *cp=(uchar)(datlen%255);
   }

   // put data
   for (i=0; i<datlen; i++)
   {
      if (++cp>=cacheend)
         cp=cachebuf;
      if (dat[i]<(uchar)254)
         *cp=dat[i];
      else
      {
         *cp=254;
         if (++cp>=cacheend)
            cp=cachebuf;
         *cp=(uchar)(dat[i]&1);
      }
   }

   return true;
}

#endif //SPFMEMCACHE


#ifdef SPFCACHE

// read record from cache, return length, or 0 (if not in cache), or -result
static int
readcache(const char* key, int keylen, dnsrec** datapp, int datasize,
time_t currtime)
{
   int datalen;
   unsigned datlen;
#ifdef SPFFILECACHE
   ISAMERROR isamerr;
#endif //SPFFILECACHE
   dnsrec* datap;

   if (keylen==0)
      return 0;
   if (multithread)
      EnterCriticalSection(&cachemutex);
#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
   if (hCache!=NULL)
#endif //SPFMEMCACHE
   {
      isamerr=pIsamLock(hCache);
      if (isamerr!=ISAM_Ok)
      {
         if (multithread)
            LeaveCriticalSection(&cachemutex);
         return 0;
      }
   }
#endif //SPFFILECACHE
   datalen=0;

   for (;;) // (only once, trick to avoid 'goto')
   {
      datap=*datapp; // (is NULL if datasize==0)
#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
      if (hCache!=NULL)
#endif //SPFMEMCACHE
      {
         isamerr=pIsamGet(hCache,ISAM_This,key,keylen,NULL,NULL,0,
            datap,&datlen,datasize);
         if (isamerr!=ISAM_Ok) // not found or error
            break;
      }
#endif //SPFFILECACHE
#ifdef SPFMEMCACHE
#ifdef SPFFILECACHE
      else
#endif //SPFFILECACHE
      {
         if (!getrecord((uchar*)key,keylen,(uchar*)datap,&datlen,datasize))
            break;
      }
#endif //SPFMEMCACHE

      // allocate buffer if it was too small
      if (datlen>=datasize)
      {
         if (datasize>0) // (already done above if datasize==0)
         {
#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
            if (hCache!=NULL)
#endif //SPFMEMCACHE
            {
               isamerr=pIsamGet(hCache,ISAM_Same,key,keylen,NULL,NULL,0,
                  NULL,&datlen,0);
               if (isamerr!=ISAM_Ok) // (should not happen)
                  break;
            }
#endif //SPFFILECACHE
#ifdef SPFMEMCACHE
#ifdef SPFFILECACHE
            else
#endif //SPFFILECACHE
            {
               if (!getrecord((uchar*)key,keylen,NULL,&datlen,0))
                  break;
            }
#endif //SPFMEMCACHE
         }
         if (datlen>datasize)
         {
            datap=(dnsrec*)spfmalloc(datlen);
            if (datap==NULL)
               break;
#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
            if (hCache!=NULL)
#endif //SPFMEMCACHE
            {
               isamerr=pIsamGet(hCache,ISAM_Same,key,keylen,NULL,NULL,0,
                  datap,&datlen,datlen);
               if (isamerr!=ISAM_Ok) // (should not happen)
               {
                  spffree(datap);
                  break;
               }
            }
#endif //SPFFILECACHE
#ifdef SPFMEMCACHE
#ifdef SPFFILECACHE
            else
#endif //SPFFILECACHE
            {
               if (!getrecord((uchar*)key,keylen,(uchar*)datap,&datlen,datlen))
               {
                  spffree(datap);
                  break;
               }
            }
#endif //SPFMEMCACHE
         }
      }

      // ignore expired and delete invalid records
      if (datlen<=SPFEXPSIZE || datap->dns_expire<currtime)
      {
#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
         if (hCache!=NULL)
#endif //SPFMEMCACHE
            if (datlen<=SPFEXPSIZE)
               pIsamDelete(hCache,key,keylen);
#endif //SPFFILECACHE
         if (datlen>datasize)
            spffree(datap);
         break;
      }

      // cache says that there is no data
      if (datlen==SPFEXPSIZE+1)
      {
         datalen=-datap->dns_data[0];
         if (datlen>datasize)
            spffree(datap);
         break;
      }

      *datapp=datap;
      datalen=datlen;
      break;
   }

#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
   if (hCache!=NULL)
#endif //SPFMEMCACHE
      pIsamUnlock(hCache);
#endif //SPFFILECACHE
   if (multithread)
      LeaveCriticalSection(&cachemutex);
   return datalen;
}


// write data to cache, return true if successful
static spfbool
writecache(const char* key, unsigned keylen, dnsrec* datap, int datalen,
time_t currtime)
{
   spfbool ok;

   if (keylen==0)
      return false;
   if (datap->dns_expire<=1)
      return true; // don't cache if TTL is 1 sec or less
   if (datap->dns_expire==LONG_MAX)
      datap->dns_expire=TIME_MAX;
   else
   {
      datap->dns_expire+=currtime;
      if (datap->dns_expire<currtime) // overflow
         datap->dns_expire=TIME_MAX;
   }
   if (multithread)
      EnterCriticalSection(&cachemutex);
#ifdef SPFFILECACHE
#ifdef SPFMEMCACHE
   if (hCache!=NULL)
#endif //SPFMEMCACHE
      ok=pIsamPut(hCache,key,keylen,datap,datalen)==ISAM_Ok;
#endif //SPFFILECACHE
#ifdef SPFMEMCACHE
#ifdef SPFFILECACHE
   else
#endif //SPFFILECACHE
      ok=putrecord((uchar*)key,keylen,(uchar*)datap,datalen);
#endif //SPFMEMCACHE

   if (multithread)
      LeaveCriticalSection(&cachemutex);
   return ok;
}


// write error code into cache
static void
writecacheerror(const char* key, unsigned keylen, int error, time_t currtime)
{
   dnsrec data;

   if (withcache && keylen!=0 && error!=SPF_NoMemory)
   {
      data.dns_expire=SPFDEFTTL;
      data.dns_data[0]=(char)error;
      writecache(key,keylen,&data,SPFEXPSIZE+1,currtime);
   }
}

#endif //SPFCACHE


// find record in list, return index to record, or ~index to insert point
static int
findlist(const char* key, int keylen)
{
   int l, m, r;
   int co, co1;
   int len;
   lstrec* lstp;

   l = 0;
   r = listn - 1;
   while (l <= r)
   {
      m = (l + r) >> 1;
      lstp = listRec[m];
      len = lstp->lst_keylen;
      co1 = keylen - len;
      if (co1 < 0)
         len = keylen;
      co = memcmp(key, lstp->lst_data, len);
      if (co == 0)
      {
         if (co1 == 0)
            return m;
         co = co1;
      }
      if (co > 0)
         l = m + 1;
      else
         r = m - 1;
   }
   return ~l;
}


// read record from list, return length, or 0 (if not in list)
static int
readlist(const char* key, int keylen, dnsrec** datapp, unsigned int datasize,
time_t currtime)
{
   int i;
   int datalen;
   unsigned datlen;
   lstrec* lstp;
   dnsrec* datp;
   dnsrec* datap;

   if (keylen == 0)
      return 0;
   if (multithread)
      EnterCriticalSection(&listmutex);
   datalen = 0;

   for (;;) // (only once, trick to avoid 'goto')
   {
      i = findlist(key, keylen);
      if (i < 0) // not found
         break;

      lstp = listRec[i];
      datp = (dnsrec*)(lstp->lst_data + keylen);

      // delete expired records
#ifdef SPFCACHE
      if (datp->dns_expire<currtime)
#else //SPFCACHE
      if (lstp->lst_expire < currtime)
#endif //SPFCACHE
      {
         spffree(lstp);
         listn--;
         memmove(listRec + i, listRec + i + 1, (listn - i)*sizeof(lstrec*));
         break;
      }

      datlen = lstp->lst_datlen;
      datap = *datapp; // (is NULL if datasize==0)

      // allocate buffer if it was too small
      if (datlen > datasize || datap == NULL)
      {
         datap = (dnsrec*)spfmalloc(datlen);
         if (datap == NULL)
            break;
         *datapp = datap;
      }

      memcpy(datap, datp, datlen);
      datalen = datlen;
      break;
   }

   if (multithread)
      LeaveCriticalSection(&listmutex);
   return datalen;
}


#ifdef SPFCACHE
#define writelist(k,kl,d,dl,t,e) writelist(k,kl,d,dl,t)
#define listexpire datap->dns_expire
#endif //SPFCACHE

// write data to list, return true if successful
static spfbool
writelist(const char* key, unsigned keylen, dnsrec* datap, int datalen,
time_t currtime, time_t listexpire)
{
   int i;
   spfbool ok;
   lstrec* lstp;
   lstrec** listp;

   if (keylen == 0)
      return false;
   ok = false;

   if (listexpire <= 1)
      return true; // don't store if TTL is 1 sec or less
   if (listexpire == LONG_MAX)
      listexpire = TIME_MAX;
   else
   {
      listexpire += currtime;
      if (listexpire < currtime) // overflow
         listexpire = TIME_MAX;
   }

   if (multithread)
      EnterCriticalSection(&listmutex);

   i = findlist(key, keylen);

   for (;;) // (only once, trick to avoid 'goto')
   {
      if (i >= 0) // already in list
      {
         lstp = listRec[i];
         if (datalen > lstp->lst_datlen) // increase buffer size if too small
         {
            lstp = (lstrec*)spfrealloc(lstp, sizeof(lstrec) - 1 + keylen + datalen);
            if (lstp == NULL)
            {
               spffree(listRec[i]);
               listn--;
               memmove(listRec + i, listRec + i + 1, (listn - i)*sizeof(lstrec*));
               break;
            }
            listRec[i] = lstp;
         }

         lstp->lst_datlen = datalen;
#ifndef SPFCACHE
         lstp->lst_expire = listexpire;
#endif //SPFCACHE
         memcpy(lstp->lst_data + keylen, datap, datalen);
         ok = true;
         break;
      }

      // not yet in list
      i = ~i;

      // increase list size if too small
      if (listn >= listm)
      {
         listm += 256;
         listp = (lstrec**)spfrealloc(listRec, listm*sizeof(lstrec*));
         if (listp == NULL)
         {
            listm -= 256;
            break;
         }
         listRec = listp;
      }

      lstp = (lstrec*)spfmalloc(sizeof(lstrec) - 1 + keylen + datalen);
      if (lstp == NULL)
         break;

      memmove(listRec + i + 1, listRec + i, (listn - i)*sizeof(lstrec*));
      listn++;

      listRec[i] = lstp;
      lstp->lst_keylen = keylen;
      lstp->lst_datlen = datalen;
#ifndef SPFCACHE
      lstp->lst_expire = listexpire;
#endif //SPFCACHE
      memcpy(lstp->lst_data, key, keylen);
      memcpy(lstp->lst_data + keylen, datap, datalen);
      ok = true;
      break;
   }

   if (multithread)
      LeaveCriticalSection(&listmutex);
   return ok;
}


// delete record in list
static void
deletelist(const char* key, unsigned keylen)
{
   int i;

   if (multithread)
      EnterCriticalSection(&listmutex);

   i = findlist(key, keylen);
   if (i >= 0)
   {
      spffree(listRec[i]);
      listn--;
      memmove(listRec + i, listRec + i + 1, (listn - i)*sizeof(lstrec*));
   }

   if (multithread)
      LeaveCriticalSection(&listmutex);
}


// compare 'len' bits of IP addresses
static spfbool
addrequal(const void* addr1, const void* addr2, int len)
{
   unsigned mask;

   while (len >= 32)
   {
      if (*(uint32*)addr1 != *(uint32*)addr2)
         return false;
      (*(uint32**)&addr1)++;
      (*(uint32**)&addr2)++;
      len -= 32;
   }
   if (len >= 16)
   {
      if (*(uint16*)addr1 != *(uint16*)addr2)
         return false;
      (*(uint16**)&addr1)++;
      (*(uint16**)&addr2)++;
      len -= 16;
   }
   if (len >= 8)
   {
      if (*(uchar*)addr1 != *(uchar*)addr2)
         return false;
      (*(uchar**)&addr1)++;
      (*(uchar**)&addr2)++;
      len -= 8;
   }
   if (len == 0)
      return true;
   mask = (unsigned)~0 << (8 - len);
   if (((*(uchar*)addr1)&mask) != ((*(uchar*)addr2)&mask))
      return false;
   return true;
}


// check if the IPv4 address is valid (may appear in the public Internet)
static spfbool
validaddr4(const uchar* addr)
{
   int i;
   static const struct
   {
      uchar addr[4];
      int len;
   } invaladdr[] =
   {
      { { 0, 0, 0, 0 }, 8 },
      { { 10, 0, 0, 0 }, 8 },
      { { 127, 0, 0, 0 }, 8 },
      { { 169, 254, 0, 0 }, 16 },
      { { 172, 16, 0, 0 }, 12 },
      { { 192, 168, 0, 0 }, 16 },
      { { 255, 255, 255, 255 }, 32 }
   };

   for (i = 0; i < sizeof(invaladdr) / sizeof(invaladdr[0]); i++)
   {
      if (addrequal(addr, invaladdr[i].addr, invaladdr[i].len))
         return false;
   }
   return true;
}


// check if the IPv6 address is valid (may appear in the public Internet)
static spfbool
validaddr6(const uchar* addr)
{
   int i;
   static const struct
   {
      uchar addr[16];
      int len;
   } invaladdr[] =
   {
      { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 127 },
      { { 0xFE, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 9 }
   };

   for (i = 0; i < sizeof(invaladdr) / sizeof(invaladdr[0]); i++)
   {
      if (addrequal(addr, invaladdr[i].addr, invaladdr[i].len))
         return false;
   }
   return true;
}


#ifndef SPFCACHE
#define makekey(k,d,t) makekey(k,d)
#endif //SPFCACHE

// make key for database lookup
static int
makekey(char* key, const char* domain, int type)
{
   int i;
   int k;
   int len;
   int keylen;

   // reverse parts in domain name
   i = (int)strlen(domain);
   if (i >= SPFKEYSIZE)
      return 0;
   keylen = 0;
   for (;;)
   {
      len = i;
      for (; i > 0; i--)
         if (domain[i - 1] == '.')
            break;
      for (k = i; k < len; k++)
         key[keylen++] = (char)TOLOWER(domain[k]);
      if (i == 0)
         break;
      key[keylen++] = '.';
      i--;
   }
#ifdef SPFCACHE
   key[keylen++]=(char)type;
#endif //SPFCACHE
   return keylen;
}


// get A/AAAA records from received DNS data, return 0, length, or -result
static int
get_arec(DNS_RECORD* dnsresp, const char* domain, spfbool ipv6,
dnsrec** datapp, int datasize)
{
   int dnstype;
   int addrlen;
   int datalen;
   DNS_RECORD* dnsp;
   dnsrec* datap;

   datap = *datapp;

   if (ipv6)
   {
      dnstype = DNS_TYPE_AAAA;
      addrlen = 16;
   }
   else
   {
      dnstype = DNS_TYPE_A;
      addrlen = 4;
   }

   for (;;)
   {
      datalen = SPFEXPSIZE;
      for (dnsp = dnsresp; dnsp != NULL; dnsp = dnsp->pNext)
      {
         if (dnsp->wType != dnstype)
            continue;
         if (domain != NULL) // additional section
         {
            if (dnsp->Flags.S.Section != DNSREC_ADDITIONAL)
               continue;
            if (_stricmp(dnsp->pName, domain) != 0)
               continue;
         }
         else // answer section
         {
            if (dnsp->Flags.S.Section != DNSREC_ANSWER)
               continue;
         }
#ifdef SPFCACHE
         if (datalen==SPFEXPSIZE || datap->dns_expire>dnsp->dwTtl)
            datap->dns_expire=dnsp->dwTtl;
#endif //SPFCACHE
         if (datalen + addrlen <= datasize)
         {
            if (ipv6)
               memcpy((char*)datap + datalen, &dnsp->Data.AAAA.Ip6Address, 16);
            else
               *(uint32*)((char*)datap + datalen) =
               *(uint32*)&dnsp->Data.A.IpAddress;
         }
         datalen += addrlen;
      }
      if (datalen <= datasize)
         break;
      // allocate memory if buffer is too small
      datap = (dnsrec*)spfmalloc(datalen);
      if (datap == NULL)
         return -SPF_TempError;
      *datapp = datap;
      datasize = datalen;
   }

   if (datalen == SPFEXPSIZE)
      return 0; // no records
   return datalen;
}


#ifndef SPFCACHE
#define lookup_addr(a,v,d,s,r,t) lookup_addr(a,v,d,s,r)
#endif //SPFCACHE

// lookup A/AAAA records of domain, return data length (>0), or -result
static int
lookup_addr(const char* domain, spfbool ipv6, dnsrec** datapp, int datasize,
DNS_RECORD* dnsresp1, time_t currtime)
{
   int result;
   int datalen;
   DNS_RECORD* dnsresp;
#ifdef SPFCACHE
   int keylen;
   char key[SPFKEYSIZE];
#endif //SPFCACHE

   // first look in additional section of DNS data just received
   if (dnsresp1 != NULL)
   {
      datalen = get_arec(dnsresp1, domain, ipv6, datapp, datasize);
      if (datalen != 0) // found or no memory
         return datalen; // (do not store in cache because TTL is wrong)
   }

#ifdef SPFCACHE
   // then search in the cache
   if (withcache)
   {
      keylen=makekey(key,domain,ipv6?SPF_Type_AAAA:SPF_Type_A);
      datalen=readcache(key,keylen,datapp,datasize,currtime);
      if (datalen!=0) // record found or error
         return datalen;
   }
#endif //SPFCACHE

   result = dnsquery(domain, ipv6 ? DNS_TYPE_AAAA : DNS_TYPE_A, &dnsresp);
   if (result > 0)
   {
#ifdef SPFCACHE
      writecacheerror(key,keylen,result,currtime);
#endif //SPFCACHE
      return -result;
   }

   datalen = get_arec(dnsresp, NULL, ipv6, datapp, datasize);
   pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);
   if (datalen < 0) // no memory
      return datalen;

   if (datalen == 0)
      datalen = -SPF_None;

#ifdef SPFCACHE
   if (withcache)
   {
      if (datalen<0) // no records
         writecacheerror(key,keylen,-datalen,currtime);
      else
         writecache(key,keylen,*datapp,datalen,currtime);
   }
#endif //SPFCACHE

   return datalen;
}


#ifndef SPFCACHE
#define findaddr(r,d,v,a,l,t) findaddr(r,d,v,a,l)
#endif //SPFCACHE

// find A/AAAA record for domain, return -1: not found, 0: found, >0: error
// MaKn: it only returns 0 if it matches the senders IP address.
static int
findaddr(DNS_RECORD* dnsresp, const char* domain, spfbool ipv6,
const uchar* addr, int cidrlen, time_t currtime)
{
   int i;
   int k;
   int datalen;
   int result;
   int addrlen;
   dnsrec* datap;
   char data[SPFARECSIZE];
   memset(data, 0, SPFARECSIZE);

   datap = (dnsrec*)data;
   datalen = lookup_addr(domain, ipv6, &datap, sizeof(data), dnsresp, currtime);
   if (datalen < 0)
      result = -datalen;
   else
   {
      result = -1;
      addrlen = ipv6 ? 16 : 4;
      for (i = SPFEXPSIZE, k = i + addrlen; k <= datalen; i = k, k += addrlen)
      {
         if (addr != NULL)
         {
            if (addrequal((uchar*)datap + i, addr, cidrlen))
            {
               result = 0;
               break;
            }
         }
         else if (ipv6 ? validaddr6((uchar*)datap + i) : validaddr4((uchar*)datap + i))
         {
            result = 0;
            break;
         }
      }
   }

   if (datap != (dnsrec*)data)
      spffree((void*)datap);

   return result;
}


#ifndef SPFCACHE
#define gethostname(v,a,h,s,p,f,t) gethostname(v,a,h,s,p,f)
#endif //SPFCACHE

// get validated host name from IP address, return non-zero if unknown
// flags: 8=for 'ptr' mechanism or 'p' macro
//        4=get 'parent' if in list of PTRs
//        2=get first host that is 'parent' or subdomain of 'parent'
//        1=get first host in list of PTRs
static int
gethostname(spfbool ipv6, const uchar* addr, char* host, int hostsize,
const char* parent, int flags, time_t currtime)
{
   int i;
   int n;
   int len;
   int bit;
   int datalen;
   int datasize;
   int result;
   char* cp;
   char* cp1;
   char domain[80];
   DNS_RECORD* dnsp;
   DNS_RECORD* dnsresp;
   dnsrec* datap;
   char data[SPFNRECSIZE];
#ifdef SPFCACHE
   unsigned keylen;
   char key[1+16];
#endif //SPFCACHE

   // look up PTR of IP address
   datap = (dnsrec*)data;

#ifdef SPFCACHE
   // first look up in cache
   if (!ipv6)
   {
      key[0]=0; // IPv4 key
      *(uint32*)(key+1)=*(uint32*)addr;
      keylen=1+4;
   }
   else
   {
      key[0]=1; // IPv6 key
      memcpy(key+1,addr,16);
      keylen=1+16;
   }

   if (!withcache)
      datalen=0;
   else
      datalen=readcache(key,keylen,&datap,sizeof(data),currtime);

   if (datalen!=0) // was in cache
      dnsresp=NULL;
   else
#endif //SPFCACHE
   {
      // create domain name for reverse DNS lookup
      cp = domain;
      int remaining = 80;
      if (!ipv6)
      {
         for (i = 3; i >= 0; i--)
         {
            cp += numtostr(cp, addr[i]);
            remaining--;
            *cp++ = '.';
            remaining--;
         }
         strcpy_s(cp, remaining, "in-addr.arpa");
      }
      else
      {
         for (i = 15; i >= 0; i--)
         {
            *cp++ = hexdigit[addr[i] & 15];
            remaining--;
            *cp++ = '.';
            remaining--;
            *cp++ = hexdigit[addr[i] >> 4];
            remaining--;
            *cp++ = '.';
            remaining--;
         }
         strcpy_s(cp, remaining, "ip6.arpa");
      }

      // look up DNS
      result = dnsquery(domain, DNS_TYPE_PTR, &dnsresp);
      if (result > 0)
      {
         dnsresp = NULL;
         datalen = -result;
      }
      else
      {
         // get the PTR records
         datasize = sizeof(data);
         for (;;)
         {
            datalen = SPFEXPSIZE;
            for (dnsp = dnsresp; dnsp != NULL; dnsp = dnsp->pNext)
            {
               if (dnsp->wType != DNS_TYPE_PTR)
                  continue;
               if (dnsp->Flags.S.Section != DNSREC_ANSWER)
                  continue;
#ifdef SPFCACHE
               if (datalen==SPFEXPSIZE || datap->dns_expire>dnsp->dwTtl)
                  datap->dns_expire = dnsp->dwTtl;
#endif //SPFCACHE
               cp = dnsp->Data.PTR.pNameHost;
               if (*cp != 0)
               {
                  len = (int)strlen(cp) + 1;
                  if (datalen + len <= datasize)
                     memcpy((char*)datap + datalen, cp, len);
                  datalen += len;
               }
            }

            if (datalen <= datasize)
               break;
            datap = (dnsrec*)spfmalloc(datalen);
            if (datap == NULL)
            {
               datap = (dnsrec*)data;
               datalen = -SPF_NoMemory; // so that result is not put into cache
               break;
            }
            datasize = datalen;
         }

         if (datalen == SPFEXPSIZE) // no records
            datalen = -SPF_None;
      }

#ifdef SPFCACHE
      if (withcache)
      {
         if (datalen<0)
            writecacheerror(key,keylen,-datalen,currtime);
         else
            writecache(key,keylen,datap,datalen,currtime);
   }
#endif //SPFCACHE
}

   // check the IP addresses of the retrieved domains
   if (datalen < 0)
   {
#ifdef SPFCACHE
      if (datalen==-SPF_NoMemory)
         result=SPF_TempError;
      else
#endif //SPFCACHE
         result = -datalen;
      if ((flags & 8) && result>0) // for 'ptr' treat errors as 'no match'
         result = -1;
   }
   else
   {
      result = -1;
      for (bit = 4; bit != 0; bit >>= 1)
      {
         if (!(bit&flags))
            continue;
         n = 0;
         cp1 = (char*)datap + datalen;
         for (cp = datap->dns_data; cp<cp1; cp = strchr(cp, 0) + 1)
         {
            if (*cp == (char)255) // already checked
               continue;
            switch (bit)
            {
            case 4: // host name must be 'parent'
               if (_stricmp(cp, parent) != 0)
                  continue;
               break;
            case 2: // host name must be 'parent' or subdomain of 'parent'
               if (!indomain(cp, parent))
                  continue;
               break;
            }
            if ((flags & 8) && ++n>SPFMAXLOOKUPS)
               break; // for 'ptr' limit number of lookups
            result = findaddr(dnsresp, cp, ipv6, addr, ipv6 ? 128 : 32, currtime);
            if (result == 0 || result > 0 && !(flags & 8)) // for 'ptr' ignore errors
               break;
            result = -1;
            *cp = (char)255; // don't check again
         }
         if (result >= 0)
            break;
      }
   }

   if (host != NULL)
   {
      if (result != 0) // PTR not found or A/AAAA does not match IP address
         cp = "Unknown";
      len = (int)strlen(cp);
      if (len > hostsize - 1)
         len = hostsize - 1;
      memcpy(host, cp, len);
      host[len] = 0;
   }

   if (dnsresp != NULL)
      pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);
   if (datap != (dnsrec*)data)
      spffree(datap);

   return result;
 }


// get validated host name from IP address, return non-zero if unknown
int
SPFGetHostName(int family, const void* ipaddr, char* host, int size,
const char* domain)
{
   spfbool ipv6;

   if (ipaddr == NULL || initialized <= 0 && !initspf(1))
      return SPF_PermError;

   switch (family)
   {
   case AF_INET:
      ipv6 = false;
      break;
   case AF_INET6:
      ipv6 = true;
      break;
   default:
      return SPF_PermError;
   }
   return gethostname(ipv6, (const uchar*)ipaddr, host, size, domain, domain ? 7 : 1,
      time(NULL));
}


// expand macro, alloc if *bufp==0, return values: -1: ok, >0: result (error)
static int
expand(spfrec* spfp, const char* s, const char* s1, const char* domain,
char** bufp, spfbool fordomain)
{
   int c;
   int i;
   int k;
   int len;
   int num; // max number of parts
   int macro; // what macro
   int buflen;
   int bufsize;
   spfbool reverse; // parts in reverse order
   spfbool urlencode; // must URL encode string
   char* buf;
   const char* cp;
   const char* cp1;
   const char* mac;
   char tmp[256];
   char delim[128]; // delimiters

   // macro for storing char in buf but only if buflen>=0 and buflen<bufsize
#define PUTBUF(c) \
      { \
    if ((unsigned)buflen<(unsigned)bufsize) \
      buf[buflen]=(char)(c); \
    buflen++; \
      }

   buf = *bufp;
   if (buf == NULL)
      bufsize = 0;
   else
      bufsize = SPFDOMSIZE;
   buflen = 0;

   for (;;)
   {
      for (cp = s; cp < s1; cp++)
      {
         if (*cp < 0x20 || *cp>0x7E)
            return SPF_PermError;

         if (*cp != '%' || cp + 1 >= s1)
         {
            PUTBUF(*cp);
            continue;
         }

         // '%' encountered:
         cp++;
         switch (*cp)
         {
         case '_':
            PUTBUF(' ');
            continue;
         case '-':
            PUTBUF('%');
            PUTBUF('2');
            PUTBUF('0');
            continue;
         case '{':
            break;
         default:
            if (*cp < 0x20 || *cp>0x7E)
               return SPF_PermError;
            PUTBUF('%');
            // fall thru
         case '%':
         case '/':
            PUTBUF(*cp);
            continue;
         }

         // '%{' encountered:
         if (++cp >= s1)
            return SPF_PermError;

         // get macro letter and URL encode flag
         macro = (uchar)*cp;
         if (ISLOWER(macro))
            urlencode = false;
         else if (ISUPPER(macro))
         {
            macro = TOLOWER(macro);
            urlencode = true;
         }
         else
            return SPF_PermError;
         if (++cp >= s1)
            return SPF_PermError;

         // get max number of parts
         num = 0;
         while (ISDIGIT(*cp))
         {
            if (num > (INT_MAX - (*cp - '0')) / 10)
                return SPF_PermError;

            num = num * 10 + *cp - '0';
            if (++cp >= s1)
               return SPF_PermError;
         }
         if (num == 0)
            num = 255;

         // get reverse flag
         if (*cp != 'r')
            reverse = false;
         else
         {
            reverse = true;
            if (++cp >= s1)
               return SPF_PermError;
         }

         // get delimiters
         memset(delim, 0, sizeof(delim));
         if (*cp == '}')
            delim['.'] = true;
         else
         {
            while (ISDELIM(*cp))
            {
               delim[(uchar)*cp] = true;
               if (++cp >= s1)
                  return SPF_PermError;
            }
            if (*cp != '}')
               return SPF_PermError;
         }

         switch (macro)
         {
         case 'l': // local-part of responsible-sender
         case 's': // responsible-sender
            mac = tmp;
            if (spfp->spf_sender == NULL) // no local part
            {
               strcpy_s(tmp, 256, "postmaster");
               len = 10;
            }
            else
            {
               for (len = 0; len < sizeof(tmp) && spfp->spf_sender[len] != '@'; len++)
                  tmp[len] = spfp->spf_sender[len];
            }
            if (macro == 'l') // only local part
               break;
            if (len < sizeof(tmp))
               tmp[len++] = '@';
            cp1 = spfp->spf_domain;
            while (len < sizeof(tmp) && *cp1 != 0)
               tmp[len++] = *cp1++;
            break;
         case 'o': // responsible-domain
            mac = spfp->spf_domain;
            len = (int)strlen(mac);
            break;
         case 'd': // current-domain
            mac = domain;
            len = (int)strlen(mac);
            break;
         case 'c': // SMTP client IP (for explanation)
            if (fordomain) // allow 'c' macro only in explanation records
               return SPF_PermError;
            // fall thru
         case 'i': // SMTP client IP (for domain name)
            mac = tmp;
            len = ntoa(spfp->spf_ipv6, spfp->spf_ipaddr, tmp, macro == 'i');
            break;
         case 't': // current timestamp in Unix time
            if (fordomain) // allow 't' macro only in explanation records
               return SPF_PermError;
            mac = tmp;
            len = timetostr(tmp, spfp->spf_time);
            break;
         case 'v': // client IP version string
            if (spfp->spf_ipv6)
            {
               mac = "ip6";
               len = 3;
            }
            else
            {
               mac = "in-addr";
               len = 7;
            }
            break;
         case 'p': // SMTP client domain name
            if (spfp->spf_client != NULL)
               mac = spfp->spf_client;
            else
            {
               mac = tmp;
               gethostname(spfp->spf_ipv6, spfp->spf_ipaddr, tmp, sizeof(tmp),
                  spfp->spf_domain, 7 | 8, spfp->spf_time);
            }
            len = (int)strlen(mac);
            break;
         case 'h': // HELO/EHLO domain
            if (spfp->spf_helo != NULL)
            {
               mac = spfp->spf_helo;
               len = (int)strlen(mac);
               break;
            }
            mac = unknownstr;
            len = 7;
            break;
         case 'r': // receiving domain
            if (fordomain) // allow 'r' macro only in explanation records
               return SPF_PermError;
            if (ourdomain != NULL)
            {
               mac = ourdomain;
               len = (int)strlen(mac);
               break;
            }
            mac = unknownstr;
            len = 7;
            break;
         default: // invalid macro letter
            return SPF_PermError;
         }

         if (!reverse)
         {
            for (i = len; i > 0; i--)
               if ((uchar)mac[i - 1] < 128 && delim[(uchar)mac[i - 1]] && --num == 0)
                  break;
            for (; i < len; i++)
            {
               c = (uchar)mac[i];
               if (c < 128 && delim[c])
                  c = '.';
               if (!urlencode || ISURIC(c))
               {
                  PUTBUF(c);
               }
               else
               {
                  PUTBUF('%');
                  PUTBUF(hexdigit[c >> 4]);
                  PUTBUF(hexdigit[c & 15]);
               }
            }
         }
         else
         {
            for (i = 0; i < len; i++)
               if ((uchar)mac[i] < 128 && delim[(uchar)mac[i]] && --num == 0)
                  break;
            for (;;)
            {
               len = i;
               for (; i > 0; i--)
                  if ((uchar)mac[i - 1] < 128 && delim[(uchar)mac[i - 1]])
                     break;
               for (k = i; k < len; k++)
               {
                  c = (uchar)mac[k];
                  if (!urlencode || ISURIC(c))
                  {
                     PUTBUF(c);
                  }
                  else
                  {
                     PUTBUF('%');
                     PUTBUF(hexdigit[c >> 4]);
                     PUTBUF(hexdigit[c & 15]);
                  }
               }
               if (i == 0)
                  break;
               PUTBUF('.');
               i--;
            }
         }
      }

      if (buf != NULL)
      {
         // string is too long
         if (buflen >= bufsize)
         {
            // string was too long in previous pass
            if (buflen == bufsize)
            {
               // go to char after first dot
               for (i = 1; buf[i - 1] != '.'; i++)
               {
                  if (i >= buflen) // there is no dot
                  {
                     i = 1;
                     break;
                  }
               }
               // move left rest of string
               for (k = 0; i < buflen; k++, i++)
                  buf[k] = buf[i];
               buf[k] = 0;
               break;
            }

            buflen = bufsize - buflen; // so that beginning is stripped
            continue; // try again
         }

         // string is not too long:
         if (fordomain && buflen > 0 && buf[buflen - 1] == '.') // remove trailing dot
            buflen--;
         buf[buflen] = 0;
         break;
      }

      // first pass and buf is NULL, allocate buffer
      if (!fordomain || buflen < SPFDOMSIZE)
      {
         bufsize = buflen + 1;
         buflen = 0;
      }
      else
      {
         bufsize = SPFDOMSIZE;
         buflen = bufsize - buflen;
      }
      buf = (char*)spfmalloc(bufsize);
      if (buf == NULL)
         return SPF_TempError;
   }

   *bufp = buf;
   return -1;
}


// get modifier, return values: -1: ok, >0: result (error)
static int
get_modifier(spfrec* spfp, const char** cpp, const char* domain,
char** bufp)
{
   const char* cp;
   const char* dom;

   *bufp = NULL;
   cp = *cpp + 1;
   dom = cp;
   while (!ISEMPTY(*cp))
      cp++;
   *cpp = cp;
   if (cp == dom) // empty modifier
      return -1;
   return expand(spfp, dom, cp, domain, bufp, true);
}


// get domain from directive, return values: -1: ok, >0: result (error)
static int
get_domain(spfrec* spfp, const char** cpp, int prefix, const char* domain,
char** bufp)
{
   const char* cp;
   const char* cp1;
   const char* dom;

   cp = *cpp;
   if (*cp != ':') // no domain specified
   {
      *bufp = (char*)domain;
      return -1;
   }

   cp++;
   dom = cp;
   while (!ISEMPTY(*cp))
      cp++;

   // strip dual-cidr-length at end
   for (cp1 = cp - 1; cp1 >= dom && ISDIGIT(*cp1); cp1--)
      ;
   if (cp1 >= dom && *cp1 == '/')
   {
      if (cp1 > dom && cp1[-1] == '/')
      {
         cp1--;
         cp = cp1;
         for (cp1--; cp1 >= dom && ISDIGIT(*cp1); cp1--)
            ;
         if (cp1 >= dom && *cp1 == '/')
            cp = cp1;
      }
      else
         cp = cp1;
   }

   *cpp = cp;

   if (prefix < 0) // do not expand if result will be ignored
   {
      *bufp = (char*)domain;
      return -1;
   }

   if (cp == dom)
      return SPF_PermError; // syntax error

   return expand(spfp, dom, cp, domain, bufp, true);
}


// get IP address length from directive, return -1 if error
static int
get_cidrlen(const char** cpp, spfbool second, int maxlen)
{
   int val;
   const char* cp;

   cp = *cpp;
   if (ISEMPTY(*cp)) // no IP address length specified
      return maxlen;
   if (*cp != '/') // syntax error
      return -1;

   cp++;
   if (second && *cp != '/')
   {
      while (ISDIGIT(*cp))
         cp++;
      if (*cp != '/' || cp[1] != '/') // no second ip address length specified
      {
         *cpp = cp;
         if (!ISEMPTY(*cp))
            return -1;
         return maxlen;
      }
      cp += 2;
   }

   if (!ISDIGIT(*cp))
      return -1;
   val = *cp - '0';
   for (cp++; ISDIGIT(*cp); cp++)
      val = val * 10 + *cp - '0';

   if (!second && *cp == '/' && cp[1] == '/')
   {
      cp += 2;
      while (ISDIGIT(*cp))
         cp++;
   }

   *cpp = cp;
   if (val > maxlen || !ISEMPTY(*cp))
      return -1;
   return val;
}


// parse 'include' mechanism, return codes: -1: no match, >=0: result
static int
parse_include(spfrec* spfp, const char** cpp, int prefix, const char* domain)
{
   int result;
   char* dom;
   char dombuf[SPFDOMSIZE];

   if (++spfp->spf_lookups > SPFMAXLOOKUPS)
      return SPF_PermError;
   if (**cpp != ':') // syntax error
      return SPF_PermError;
   dom = dombuf;
   result = get_domain(spfp, cpp, prefix, domain, &dom);
   if (result > 0) // syntax error or no memory
      return result;
   if (!ISEMPTY(**cpp)) // syntax error
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   if (_stricmp(dom, spfp->spf_domain) == 0 || _stricmp(dom, domain) == 0) // loop
      return SPF_PermError;

   spfp->spf_level += 0x101;
   result = check_host(spfp, dom);
   spfp->spf_level--;

   switch (result)
   {
   case SPF_Pass:
      return prefix;
   case SPF_None:
      return SPF_PermError; // a missing SPF record means 'permanent error'
   case SPF_TempError:
   case SPF_PermError:
      return result;
   }
   return -1;
}


// parse 'ip4' mechanism, return codes: -1: no match, >=0: result
static int
parse_ip4(spfrec* spfp, const char** cpp, int prefix)
{
   int cidrlen;
   const char* cp;
   uchar ipaddr[4];

   cp = *cpp;
   if (*cp != ':')
      return SPF_PermError;
   cp = aton4(cp + 1, ipaddr, '/');
   if (cp == NULL)
      return SPF_PermError;
   *cpp = cp;

   cidrlen = get_cidrlen(cpp, false, 32);
   if (cidrlen < 0)
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   if (!spfp->spf_ipv6 && addrequal(ipaddr, spfp->spf_ipaddr, cidrlen))
      return prefix;
   return -1;
}


// parse 'ip6' mechanism, return codes: -1: no match, >=0: result
static int
parse_ip6(spfrec* spfp, const char** cpp, int prefix)
{
   int cidrlen;
   const char* cp;
   uchar ipaddr[16];

   cp = *cpp;
   if (*cp != ':')
      return SPF_PermError;
   cp = aton6(cp + 1, ipaddr, '/');
   if (cp == NULL)
      return SPF_PermError;
   *cpp = cp;

   cidrlen = get_cidrlen(cpp, false, 128);
   if (cidrlen < 0)
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   // check if it is an IPv4-mapped IPv6 address
   if (cidrlen >= 96 && addrequal(ipaddr, ipv4mapped, 96))
   {
      if (!spfp->spf_ipv6 && addrequal(ipaddr + 12, spfp->spf_ipaddr, cidrlen - 96))
         return prefix;
   }
   else
   {
      if (spfp->spf_ipv6 && addrequal(ipaddr, spfp->spf_ipaddr, cidrlen))
         return prefix;
   }

   return -1;
}


// parse 'a' mechanism, return codes: -1: no match, >=0: result
static int
parse_a(spfrec* spfp, const char** cpp, int prefix, const char* domain)
{
   int i;
   int k;
   int datalen;
   int cidrlen;
   int addrlen;
   int result;
   char* dom;
   char dombuf[SPFDOMSIZE];
   dnsrec* datap;
   char data[SPFARECSIZE];

   if (++spfp->spf_lookups > SPFMAXLOOKUPS)
      return SPF_PermError;
   dom = dombuf;
   result = get_domain(spfp, cpp, prefix, domain, &dom);
   if (result > 0) // syntax error or no memory
      return result;
   cidrlen = get_cidrlen(cpp, spfp->spf_ipv6, spfp->spf_ipv6 ? 128 : 32);
   if (cidrlen < 0)
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   datap = (dnsrec*)data;
   datalen = lookup_addr(dom, spfp->spf_ipv6, &datap, sizeof(data), NULL,
      spfp->spf_time);
   if (datalen < 0)
   {
      if (datalen == -SPF_None || datalen == -SPF_PermError) // no records
         return -1;
      return -datalen;
   }

   result = -1;
   addrlen = spfp->spf_ipv6 ? 16 : 4;
   for (i = SPFEXPSIZE, k = i + addrlen; k <= datalen; i = k, k += addrlen)
   {
      if (addrequal((uchar*)datap + i, spfp->spf_ipaddr, cidrlen))
      {
         result = prefix;
         break;
      }
   }

   if (datap != (dnsrec*)data)
      spffree(datap);
   return result;
}


#ifndef SPFCACHE
#define lookup_mx(n,d,s,r,t) lookup_mx(n,d,s,r)
#endif //SPFCACHE

// lookup MX records, return length, or -result
static int
lookup_mx(const char* domain, dnsrec** datapp, int datasize,
DNS_RECORD** dnsrespp, time_t currtime)
{
   int i;
   int k;
   int len;
   int result;
   int mxcnt;
   int mxmax;
   int datalen;
   DNS_RECORD* dnsresp;
   DNS_RECORD* dnsp;
   DNS_RECORD** dnslistp;
   DNS_RECORD* dnslist[16];
   const char* cp;
   dnsrec* datap;
#ifdef SPFCACHE
   unsigned keylen;
   char key[SPFKEYSIZE];
#endif //SPFCACHE

   datap = *datapp;

#ifdef SPFCACHE
   // first look up in cache
   if (!withcache)
      datalen=0;
   else
   {
      keylen=makekey(key,domain,SPF_Type_MX);
      datalen=readcache(key,keylen,&datap,datasize,currtime);
   }

   if (datalen!=0) // was in cache
      dnsresp=NULL;
   else
#endif //SPFCACHE
   {
      // look up DNS
      result = dnsquery(domain, DNS_TYPE_MX, &dnsresp);
      if (result > 0)
      {
         dnsresp = NULL;
         datalen = -result;
      }
      else
      {
         // get the MX records
         mxmax = sizeof(dnslist) / sizeof(dnslist[0]);
         dnslistp = dnslist;
         for (;;)
         {
            mxcnt = 0;
            for (dnsp = dnsresp; dnsp != NULL; dnsp = dnsp->pNext)
            {
               if (dnsp->wType != DNS_TYPE_MX)
                  continue;
               if (dnsp->Flags.S.Section != DNSREC_ANSWER)
                  continue;
               if (mxcnt < mxmax)
                  dnslistp[mxcnt] = dnsp;
               mxcnt++;
            }
            if (mxcnt <= mxmax)
               break;
            dnslistp = (DNS_RECORD**)spfmalloc(mxcnt*sizeof(DNS_RECORD*));
            if (dnslistp == NULL)
               break;
            mxmax = mxcnt;
         }

         if (dnslistp == NULL)
            datalen = -SPF_NoMemory;
         else
         {
            // sort the MX records according to preference (by bubble sort)
            for (i = 0; i < mxcnt; i++)
            {
               for (k = mxcnt - 1; k > i; k--)
               {
                  if (dnslistp[k - 1]->Data.MX.wPreference > dnslistp[k]->Data.MX.wPreference)
                  {
                     dnsp = dnslistp[k - 1];
                     dnslistp[k - 1] = dnslistp[k];
                     dnslistp[k] = dnsp;
                  }
               }
            }

            // get the MX host names
            for (;;)
            {
               datalen = SPFEXPSIZE;
               for (i = 0; i < mxcnt; i++)
               {
#ifdef SPFCACHE
                  dnsp=dnslistp[i];
                  if (i==0 || datap->dns_expire>dnsp->dwTtl)
                     datap->dns_expire=dnsp->dwTtl;
                  cp=dnsp->Data.MX.pNameExchange;
#else //SPFCACHE
                  cp = dnslistp[i]->Data.MX.pNameExchange;
#endif //SPFCACHE
                  if (*cp != 0)
                  {
                     len = (int)strlen(cp) + 1;
                     if (datalen + len <= datasize)
                        memcpy((char*)datap + datalen, cp, len);

                     datalen += len;
                  }
            }

               if (datalen <= datasize)
                  break;
               datap = (dnsrec*)spfmalloc(datalen);
               if (datap == NULL)
               {
                  datap = *datapp;
                  datalen = -SPF_NoMemory;
                  break;
               }
               datasize = datalen;
         }

            if (dnslistp != dnslist)
               spffree((void*)dnslistp);

            if (datalen == SPFEXPSIZE) // no records
               datalen = -SPF_None;
      }
   }

#ifdef SPFCACHE
      if (withcache)
      {
         if (datalen<0)
            writecacheerror(key,keylen,-datalen,currtime);
         else
            writecache(key,keylen,datap,datalen,currtime);
}
#endif //SPFCACHE
   }

   if (dnsrespp != NULL)
      *dnsrespp = dnsresp;
   else if (dnsresp != NULL)
      pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);

   *datapp = datap;
   return datalen;
 }


// parse 'mx' mechanism, return codes: -1: no match, >=0: result
static int
parse_mx(spfrec* spfp, const char** cpp, int prefix, const char* domain)
{
   int n;
   int result;
   int cidrlen;
   int datalen;
   DNS_RECORD* dnsresp;
   const char* cp;
   const char* cp1;
   char* dom;
   char dombuf[SPFDOMSIZE];
   dnsrec* datap;
   char data[SPFNRECSIZE];

   if (++spfp->spf_lookups > SPFMAXLOOKUPS)
      return SPF_PermError;
   dom = dombuf;
   result = get_domain(spfp, cpp, prefix, domain, &dom);
   if (result > 0) // syntax error or no memory
      return result;
   cidrlen = get_cidrlen(cpp, spfp->spf_ipv6, spfp->spf_ipv6 ? 128 : 32);
   if (cidrlen < 0)
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   datap = (dnsrec*)data;
   datalen = lookup_mx(dom, &datap, sizeof(data), &dnsresp, spfp->spf_time);

   // get the IP addresses of the MX hosts
   if (datalen < 0)
   {
#ifdef SPFCACHE
      if (datalen==-SPF_NoMemory)
         result=SPF_TempError;
      else
#endif //SPFCACHE
         if (datalen == -SPF_PermError) // domain does not exist
            result = -1;
         else
            result = -datalen;
}
   else
   {
      n = 0;
      result = -1;
      cp1 = (char*)datap + datalen;
      for (cp = datap->dns_data; cp < cp1; cp = strchr(cp, 0) + 1)
      {
         if (++n > SPFMAXLOOKUPS) // limit number of lookups
         {
            result = SPF_PermError;
            break;
         }

         result = findaddr(dnsresp, cp, spfp->spf_ipv6, spfp->spf_ipaddr, cidrlen, spfp->spf_time);

         // Removed 2008-11-20
         // if (result==0)
         if (result >= 0)
         {
            // 2008-10-30 MaKn: The MX record doesn't resolve. Should still be OK to continue?
            //                  There may be other MX records which does resolve.
            // Removed:
            // if (result==SPF_PermError) // domain does not exist
            //  result=-1;

            // 2008-11-20 MaKn: Feedback from Roger Moser:
            // Removed:
            // break;

            if (result != SPF_PermError) // domain exists
               break;
         }
      }
   }

   if (dnsresp != NULL)
      pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);
   if (datap != (dnsrec*)data)
      spffree(datap);

   if (result == 0)
      return prefix;
   if (result == SPF_None)
      return -1;
   return result;
 }


// parse 'ptr' mechanism, return codes: -1: no match, >=0: result
static int
parse_ptr(spfrec* spfp, const char** cpp, int prefix, const char* domain)
{
   int result;
   char* dom;
   char dombuf[SPFDOMSIZE];

   if (++spfp->spf_lookups > SPFMAXLOOKUPS)
      return SPF_PermError;
   dom = dombuf;
   result = get_domain(spfp, cpp, prefix, domain, &dom);
   if (result > 0) // syntax error or no memory
      return result;
   if (!ISEMPTY(**cpp)) // syntax error
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   if (spfp->spf_client != NULL && indomain(spfp->spf_client, dom))
      return prefix;

   result = gethostname(spfp->spf_ipv6, spfp->spf_ipaddr, NULL, 0, dom, 2 | 8,
      spfp->spf_time);

   if (result == 0)
      return prefix;
   if (result == SPF_TempError)
      return result;
   return -1;
}


// parse 'exists' mechanism, return codes: -1: no match, >=0: result
static int
parse_exists(spfrec* spfp, const char** cpp, int prefix, const char* domain)
{
   int result;
   int datalen;
   char* dom;
   char dombuf[SPFDOMSIZE];
   dnsrec* datap;
   char data[SPFARECSIZE];

   if (++spfp->spf_lookups > SPFMAXLOOKUPS)
      return SPF_PermError;
   dom = dombuf;
   result = get_domain(spfp, cpp, prefix, domain, &dom);
   if (result > 0) // syntax error or no memory
      return result;
   if (!ISEMPTY(**cpp)) // syntax error
      return SPF_PermError;

   if (prefix < 0) // if result already determined, only check syntax
      return prefix;

   // look up domain
   datap = (dnsrec*)data;
   datalen = lookup_addr(dom, false, &datap, sizeof(data), NULL, spfp->spf_time);
   if (datalen < 0)
   {
      if (datalen == -SPF_TempError)
         return SPF_TempError;
      return -1;
   }
   if (datap != (dnsrec*)data)
      spffree(datap);
   return prefix;
}


#ifndef SPFCACHE
#define getspf(h,d,t) getspf(h,d)
#endif //SPFCACHE

// get the SPF TXT record, return -1: ok, >0: none or error
static int
getspf(const char* domain, const dnsrec** datapp, time_t currtime)
{
   unsigned int i;
   int len;
   int datalen;
   int result;
   const char* cp;
   DNS_RECORD* dnsresp;
   DNS_RECORD* dnsp;
   dnsrec* datap;
#ifdef SPFCACHE
   unsigned keylen;
   char key[SPFKEYSIZE];
#endif //SPFCACHE

#ifdef SPFCACHE
   // first look up in cache
   if (withcache)
   {
      datap=NULL; // (required)
      keylen=makekey(key,domain,SPF_Type_SPF);
      datalen=readcache(key,keylen,&datap,0,currtime);
      if (datalen<0) // no records
         return -datalen;
      if (datalen>0) // found
      {
         *datapp=datap;
         return -1;
      }
}
#endif //SPFCACHE

   // get the TXT records for the domain
   result = dnsquery(domain, DNS_TYPE_TEXT, &dnsresp);
   if (result > 0)
   {
#ifdef SPFCACHE
      writecacheerror(key, keylen, result, currtime);
#endif //SPFCACHE
      return result;
   }

   // choose the TXT record with the correct SPF version
   datap = NULL;
   result = SPF_None;
   for (dnsp = dnsresp; dnsp != NULL; dnsp = dnsp->pNext)
   {
      if (dnsp->wType != DNS_TYPE_TEXT)
         continue;
      if (dnsp->Flags.S.Section != DNSREC_ANSWER)
         continue;
      if (dnsp->Data.TXT.dwStringCount == 0)
         continue;

      cp = dnsp->Data.TXT.pStringArray[0];
      if (!checkversion(cp))
         continue;
      if (datap != NULL) // duplicate entry
      {
         spffree(datap);
         datap = NULL;
         result = SPF_PermError;
         break;
      }

      if (dnsp->Data.TXT.dwStringCount == 1)
      {
         datalen = (int)strlen(cp) + SPFEXPSIZE + 1;
         datap = (dnsrec*)spfmalloc(datalen);
         if (datap == NULL)
         {
            result = SPF_NoMemory;
            break;
         }
         memcpy(datap->dns_data, cp, datalen - SPFEXPSIZE);
      }
      else
      {
         // concatenate the strings
         datalen = SPFEXPSIZE + 1;
         for (i = 0; i < dnsp->Data.TXT.dwStringCount; i++)
            datalen += (int)strlen(dnsp->Data.TXT.pStringArray[i]);
         datap = (dnsrec*)spfmalloc(datalen);
         if (datap == NULL)
         {
            result = SPF_NoMemory;
            break;
         }
         datalen = SPFEXPSIZE;
         for (i = 0; i < dnsp->Data.TXT.dwStringCount; i++)
         {
            len = (int)strlen(dnsp->Data.TXT.pStringArray[i]);
            memcpy((char*)datap + datalen, dnsp->Data.TXT.pStringArray[i], len);
            datalen += len;
         }
         ((char*)datap)[datalen++] = 0;
      }
#ifdef SPFCACHE
      datap->dns_expire = dnsp->dwTtl;
#endif //SPFCACHE
      }

   pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);

   if (datap == NULL)
   {
#ifdef SPFCACHE
      if (result==SPF_NoMemory)
         return SPF_TempError;
      writecacheerror(key, keylen, result, currtime);
#endif //SPFCACHE
      return result;
   }

#ifdef SPFCACHE
   if (withcache)
      writecache(key, keylen, datap, datalen, currtime);
#endif //SPFCACHE

   *datapp = datap;
   return -1;
 }


// get the SPF fall-back record, return -1: ok, >0: none or error
static int
getfallback(const char* domain, const dnsrec** datapp, time_t currtime)
{
   int datalen;
   dnsrec* datap;
   unsigned keylen;
   char key[SPFKEYSIZE];

   // look up in cache or list
   datap = NULL; // (required)
   keylen=makekey(key,domain,SPF_Type_FallBack);
#ifdef SPFFILECACHE
   if (hCache!=NULL)
      datalen=readcache(key,keylen,&datap,0,currtime);
   else
#endif //SPFFILECACHE
      datalen = readlist(key, keylen, &datap, 0, currtime);
   if (datalen < 0) // no records
      return -datalen;
   if (datalen > 0) // found
   {
      *datapp = datap;
      return -1;
   }
   return SPF_None;
}


// check if the host name (level==0) / domain name (level!=0) is valid
static spfbool
validdomain(const char* domain, int level)
{
   int dot; // 1:prev char was dot, 2:was dash, 4:there was at least one dot
   const char* cp;
   uchar ipaddr[16];

   dot = 1;
   for (cp = domain; *cp != 0; cp++)
   {
      switch (*cp)
      {
      case '.':
         if (dot & 3)
            return false; // '.' after '.' or '-'
         dot = 1 | 4;
         break;
      case '-':
         if (level == 0)
         {
            if (dot & 1)
               return false; // '-' after '.'
            dot = (dot & 4) | 2;
            break;
         }
         // fall thru
      default:
         if (level == 0 ? !ISALNUM(*cp) : *cp < 0x21 || *cp>0x7E)
            return false; // neither letter nor digit / not a visible character
         dot &= 4;
      }
   }
   if (dot != 4)
      return false; // there was no '.' or last char is '.' or '-'
   if (aton4(domain, ipaddr, 0) != NULL)
      return false; // IPv4 address
   return true;
}


#if SPFZONECUT

#define SPF_ZoneCut 1 // (must not be 0, SPF_TempError or SPF_PermError)
#ifndef SPFCACHE
#define iszonecut(d,t) iszonecut(d)
#endif //SPFCACHE

// check if the domain is at a zone cut
static spfbool
iszonecut(const char* domain, time_t currtime)
{
   int datalen;
   DNS_RECORD* dnsresp;
   DNS_RECORD* dnsp;
   dnsrec data;
#ifdef SPFCACHE
   dnsrec* datap;
   unsigned keylen;
   char key[SPFKEYSIZE];
#endif //SPFCACHE

#ifdef SPFCACHE
   // first look up in cache
   if (withcache)
   {
      datap=&data;
      keylen=makekey(key,domain,SPF_Type_ZoneCut);
      datalen=readcache(key,keylen,&datap,sizeof(data),currtime);
      if (datalen!=0) // found
      {
         if (datap!=&data) // (should not happen)
            spffree((void*)datap);
         return datalen==-SPF_ZoneCut;
      }
   }
#endif //SPFCACHE

   // get the NS records for the domain
   datalen=-dnsquery(domain,DNS_TYPE_NS,&dnsresp);
   if (datalen>0) // there are NS records
   {
      datalen=-SPF_None;
      for (dnsp=dnsresp; dnsp!=NULL; dnsp=dnsp->pNext)
      {
         if (dnsp->wType!=DNS_TYPE_NS)
            continue;
         if (dnsp->Flags.S.Section!=DNSREC_ANSWER)
            continue;
#ifdef SPFCACHE
         data.dns_expire=dnsp->dwTtl;
#endif //SPFCACHE
         data.dns_data[0]=SPF_ZoneCut;
         datalen=SPFEXPSIZE+1;
         break;
      }
      pDnsRecordListFree(dnsresp,DnsFreeRecordListDeep);
   }

#ifdef SPFCACHE
   if (withcache)
   {
      if (datalen<0)
         writecacheerror(key,keylen,-datalen,currtime);
      else
         writecache(key,keylen,&data,datalen,currtime);
   }
#endif //SPFCACHE

   return datalen>0;
}


#ifndef SPFCACHE
#define findzonecut(d,t) findzonecut(d)
#endif //SPFCACHE

// find zone cut
static const char*
findzonecut(const char* domain, time_t currtime)
{
   const char* cp;
   const char* zonecut;

   for (zonecut=NULL; (cp=strchr(domain,'.'))!=NULL; zonecut=domain) 
   {
      if (iszonecut(domain,currtime))
         return zonecut;
      domain=cp+1;
   }
   return NULL;
}

#endif //SPFZONECUT


// check at 'domain' if 'ipaddr' is designated to send mail from 'sender'
static int
check_host(spfrec* spfp, const char* domain)
{
   int prefix = 0;
   int namelen = 0;
   int result = 0;
   int result1 = 0;
   int usewhitelist = 0; // 1: use whitelist before 'all', 2: there was no SPF
   const dnsrec* datap = 0;
   const char* cp = 0;
   const char* name = 0;
   char* redirect = 0;
   char* explain = 0;

   if (spfp->spf_level >= 10 * 0x100) // too many includes and redirects
      return SPF_PermError;

   redirect = NULL;
   explain = NULL;
   usewhitelist = 0;

   if (domain == NULL) // use whitelist policy
   {
      domain = spfp->spf_domain;
      cp = whitelist + SPF_VersionLen;
      datap = NULL;
      result = -1;
   }
   else if (!validdomain(domain, spfp->spf_level))
   {
      if (spfp->spf_level == 0)
         return SPF_None | SPF_BadDomain;
      return SPF_PermError;
   }
   else if (spfp->spf_policy == NULL || spfp->spf_level != 0)
   {
      result=getspf(domain,&datap,spfp->spf_time);
#if SPFZONECUT
      if (result==SPF_None && spfp->spf_level==0)
      {
         name=findzonecut(domain,spfp->spf_time);
         if (name!=NULL)
         {
            result1=getspf(name,&datap,spfp->spf_time);
            if (result1==SPF_None)
               result1=getfallback(name,&datap,spfp->spf_time);
            if (result1<0)
            {
               result=result1;
#if SPFZONECUT==2
               domain=name;
#endif //SPFZONECUT==2
            }
         }
   }
#endif //SPFZONECUT
      if (result == SPF_None && spfp->spf_level == 0)
         result = getfallback(domain, &datap, spfp->spf_time);
      if (result < 0)
         cp = datap->dns_data + SPF_VersionLen;
      else if (spfp->spf_level != 0)
         return result;
      else if (result != SPF_None)
      {
         if (result == SPF_PermError) // domain does not exist
            return SPF_None | SPF_NoDomain;
         return result;
      }
      else if (bestguess_a != NULL && !spfp->spf_testhelo
         && SPFHasMX(spfp->spf_domain) == SPF_None) // only A record
      {
         // use best-guess policy if there is no SPF record
         cp = bestguess_a + SPF_VersionLen;
         datap = NULL;
         result = -1;
      }
      else if (bestguess != NULL)
      {
         // use best-guess policy if there is no SPF record
         cp = bestguess + SPF_VersionLen;
         datap = NULL;
         result = -1;
      }
      else if (whitelist != NULL)
      {
         // use whitelist policy if there is no SPF record
         usewhitelist = 2;
         cp = "-all";
         datap = NULL;
         result = -1;
      }
      else
         return result;
}
   else
   {
      if (!checkversion(spfp->spf_policy))
         return SPF_PermError;
      cp = spfp->spf_policy + SPF_VersionLen;
      datap = NULL;
      result = -1;
   }

   // parse the SPF record
   while (result != SPF_TempError && result != SPF_PermError)
   {
      // go to next word
      while (*cp == ' ')
         cp++;

      if (*cp == 0)
         break;

      switch (*cp)
      {
      case '~':
         cp++;
         prefix = SPF_SoftFail;
         break;
      case '-':
         cp++;
         prefix = SPF_Fail;
         break;
      case '?':
         cp++;
         prefix = SPF_Neutral;
         break;
      case '+':
         cp++;
         prefix = SPF_Pass;
         break;
      default:
         prefix = SPF_None;
         break;
      }

      name = cp;
      if (ISALPHA(*cp))
      {
         do
            cp++;
         while (ISNAMEC(*cp));
      }
      namelen = (int)(cp - name);

      if (*cp == '=') // modifier
      {
         if (prefix != SPF_None)
         {
            result = SPF_PermError; // syntax error
            break;
         }

         // redirect
         if (namelen == 8 && _memicmp(name, "redirect", 8) == 0)
         {
            if (redirect != NULL) // already specified
            {
               spffree((void*)redirect);
               redirect = NULL;
               result = SPF_PermError;
               break;
            }
            result1 = get_modifier(spfp, &cp, domain, &redirect);
            if (result1 > 0) // syntax error or no memory
            {
               result = result1;
               break;
            }
            continue;
         }

         if ((spfp->spf_level & 0xFF) == 0) // not executing an 'include'
         {
            // exp
            if (namelen == 3 && spfp->spf_explain != NULL
               && _memicmp(name, "exp", 3) == 0)
            {
               if (explain != NULL) // already specified
               {
                  spffree((void*)explain);
                  explain = NULL;
               }
               result1 = get_modifier(spfp, &cp, domain, &explain);
               if (result1 > 0) // syntax error or no memory
               {
                  result = result1;
                  break;
               }
               continue;
            }
         }

         // ignore unknown modifiers
         while (!ISEMPTY(*cp))
            cp++;
         continue;
      }

      if (*cp != ':' && *cp != '/' && !ISEMPTY(*cp)) // syntax error
      {
         result = SPF_PermError;
         break;
      }
      if (result >= 0)
         prefix = -1; // this means: result already determined, only check syntax
      else if (prefix == SPF_None)
         prefix = SPF_Pass;

      // all
      if (namelen == 3 && _memicmp(name, "all", 3) == 0)
      {
         if (!ISEMPTY(*cp)) // syntax error
         {
            result = SPF_PermError;
            break;
         }
         if (result < 0) // if result already determined, only check syntax
         {
            if (prefix == SPF_Pass)
               result = prefix;
            else if ((spfp->spf_level & 0xFF) != 0) // executing an 'include'
               result = SPF_NoMatch;
            else
            {
               if (usewhitelist && whitelist != NULL)
               {
                  spfp->spf_level += 0x101;
                  result1 = check_host(spfp, NULL); // include whitelist policy
                  spfp->spf_level--;
                  if (result1 == SPF_Pass)
                  {
                     result = result1;
                     continue;
                  }
                  if (usewhitelist == 2) // there was no SPF record
                  {
                     result = SPF_None;
                     continue;
                  }
               }
               result = prefix;
            }
         }
         continue;
      }

      // use whitelist before 'all' if result is not yet determined
      // and mechanism could return 'pass'
      if (prefix == SPF_Pass)
         usewhitelist = 1;

      switch (namelen)
      {
      case 1:
         // a
         if (*name == 'a' || *name == 'A')
         {
            result1 = parse_a(spfp, &cp, prefix, domain);
            if (result1 >= 0)
               result = result1;
            continue;
         }
         break;

      case 2:
         // mx
         if ((*name == 'm' || *name == 'M') && (name[1] == 'x' || name[1] == 'X'))
         {
            result1 = parse_mx(spfp, &cp, prefix, domain);

            if (result1 >= 0)
               result = result1;

            continue;
         }
         break;

      case 3:
         // ip4
         if (_memicmp(name, "ip4", 3) == 0)
         {
            result1 = parse_ip4(spfp, &cp, prefix);
            if (result1 >= 0)
               result = result1;
            continue;
         }

         // ip6
         if (_memicmp(name, "ip6", 3) == 0)
         {
            result1 = parse_ip6(spfp, &cp, prefix);
            if (result1 >= 0)
               result = result1;
            continue;
         }

         // ptr
         if (_memicmp(name, "ptr", 3) == 0)
         {
            result1 = parse_ptr(spfp, &cp, prefix, domain);
            if (result1 >= 0)
               result = result1;
            continue;
         }
         break;

      case 6:
         // exists
         if (_memicmp(name, "exists", 6) == 0)
         {
            result1 = parse_exists(spfp, &cp, prefix, domain);
            if (result1 >= 0)
               result = result1;
            continue;
         }
         break;

      case 7:
         // include
         if (_memicmp(name, "include", 7) == 0)
         {
            result1 = parse_include(spfp, &cp, prefix, domain);
            if (result1 >= 0)
               result = result1;
            continue;
         }
         break;
      }

      // unknown mechanism
      if (result < 0) // abort if result not yet determined
      {
         result = SPF_PermError;
         break;
      }

      // ignore unknown mechanisms if result already determined
      while (!ISEMPTY(*cp))
         cp++;
   }

   if (result < 0) // not yet determined
   {
      if (redirect == NULL) // neither 'all' nor 'redirect' encountered
      {
         if ((spfp->spf_level & 0xFF) == 0) // not executing an 'include'
         {
            result = SPF_Neutral;
            if (usewhitelist && whitelist != NULL)
            {
               spfp->spf_level += 0x101;
               result1 = check_host(spfp, NULL); // include whitelist policy
               spfp->spf_level--;
               if (result1 == SPF_Pass)
                  result = result1;
            }
         }
      }
      else
      {
         if (_stricmp(redirect, spfp->spf_domain) == 0
            || _stricmp(redirect, domain) == 0) // loop
            return SPF_PermError;
         spfp->spf_level += 0x100;
         result = check_host(spfp, redirect);
         if (result == SPF_None)
            result = SPF_PermError;
      }
   }
   if (redirect != NULL)
   {
      spffree((void*)redirect);
      redirect = NULL;
   }

   if (datap != NULL)
   {
      spffree((void*)datap);
      datap = NULL;
   }

   if (explain != NULL)
   {
      if (result == SPF_Fail)
      {
         if (spfp->spf_expdom != NULL)
         {
            spffree((void*)spfp->spf_expdom);
            spfp->spf_expdom = NULL;
         }

         spfp->spf_expdom = explain;
      }
      else
      {
         spffree((void*)explain);
         explain = NULL;
      }
   }

   if (result == SPF_NoMatch)
      return -1;
   return result;
 }


// get explanation
static const char*
getexplanation(spfrec* spfp)
{
   unsigned int i;
   int len;
   int datalen;
   int result;
   char* explain;
   DNS_RECORD* dnsresp;
   DNS_RECORD* dnsp;
   dnsrec* datap;
   char data[256];
#ifdef SPFCACHE
   spfbool fromcache;
   unsigned keylen;
   char key[SPFKEYSIZE];
#endif //SPFCACHE

   datap = (dnsrec*)data;

#ifdef SPFCACHE
   // first look up in cache
   fromcache=false;
   if (!withcache)
      datalen=0;
   else
   {
      keylen=makekey(key,spfp->spf_expdom,SPF_Type_EXP);
      datalen=readcache(key,keylen,&datap,sizeof(data),spfp->spf_time);
   }

   if (datalen!=0) // was in cache
      fromcache=true;
   else
#endif //SPFCACHE
   {
      // get the TXT records for the domain
      result = dnsquery(spfp->spf_expdom, DNS_TYPE_TEXT, &dnsresp);
      if (result > 0)
         datalen = -result;
      else
      {
         // calculate total string length
         datalen = SPFEXPSIZE;
         for (dnsp = dnsresp; dnsp != NULL; dnsp = dnsp->pNext)
         {
            if (dnsp->wType != DNS_TYPE_TEXT)
               continue;
            if (dnsp->Flags.S.Section != DNSREC_ANSWER)
               continue;
            if (datalen > SPFEXPSIZE) // more than one TXT record
            {
               datalen = SPFEXPSIZE;
               break;
            }
            for (i = 0; i < dnsp->Data.TXT.dwStringCount; i++)
               datalen += (int)strlen(dnsp->Data.TXT.pStringArray[i]);
         }

         // concatenate the strings
         if (datalen == SPFEXPSIZE)
            datalen = -SPF_None;
         else
         {
            datap = (dnsrec*)spfmalloc(datalen);
            if (datap == NULL)
            {
               pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);
               return NULL;
            }
            datalen = SPFEXPSIZE;
            for (dnsp = dnsresp; dnsp != NULL; dnsp = dnsp->pNext)
            {
               if (dnsp->wType != DNS_TYPE_TEXT)
                  continue;
               if (dnsp->Flags.S.Section != DNSREC_ANSWER)
                  continue;
#ifdef SPFCACHE
               datap->dns_expire = dnsp->dwTtl;
#endif //SPFCACHE
               for (i = 0; i < dnsp->Data.TXT.dwStringCount; i++)
               {
                  len = (int)strlen(dnsp->Data.TXT.pStringArray[i]);
                  memcpy((char*)datap + datalen, dnsp->Data.TXT.pStringArray[i], len);
                  datalen += len;
               }
            }
         }

         pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);
      }
   }

   if (datalen < 0)
   {
#ifdef SPFCACHE
      if (!fromcache)
         writecacheerror(key, keylen, -datalen, spfp->spf_time);
#endif //SPFCACHE
      return NULL;
}

   explain = NULL;
   expand(spfp, datap->dns_data, (char*)datap + datalen, spfp->spf_domain, &explain,
      false);

#ifdef SPFCACHE
   if (withcache && !fromcache)
      writecache(key, keylen, datap, datalen, spfp->spf_time);
#endif //SPFCACHE

   if (datap != (dnsrec*)data)
      spffree((void*)datap);
   return explain;
 }


// check if the host is designated to send mail from the 'sender' (SPF)
int
SPFQuery(int family, const void* ipaddr, const char* sender,
const char* policy, const char* helo, const char* client,
const char** explain)
{
   int i;
   int result;
   spfbool ipv6;
   const char* cp;
   const char* cp1;
   spfrec spfdata;
   uchar addr[16];
   char domain[256];

   if (explain != NULL)
      *explain = NULL;

   if (ipaddr == NULL)
      return SPF_PermError;

   switch (family)
   {
   case AF_INET6:
      if (addrequal(ipaddr, localhost6, 128))
         return SPF_Pass;
      // check if it is an IPv4-mapped IPv6 address
      if (!addrequal(ipaddr, ipv4mapped, 96))
      {
         spfdata.spf_ipv6 = true;
         break;
      }
      *(const uchar**)&ipaddr += 12;
      // fall thru
   case AF_INET:
      if (addrequal(ipaddr, localhost4, 8))
         return SPF_Pass;
      spfdata.spf_ipv6 = false;
      break;
   default:
      return SPF_PermError;
   }

   if (initialized <= 0 && !initspf(1))
      return SPF_PermError;

   // if no sender is specified, use the helo string instead
   if (sender == NULL || *sender == 0)
   {
      if (helo == NULL || *helo == 0)
         return SPF_PermError;
      sender = helo;
      spfdata.spf_testhelo = true;
   }
   else
      spfdata.spf_testhelo = false;

   spfdata.spf_time = time(NULL);
   spfdata.spf_policy = policy;
   spfdata.spf_ipaddr = (const uchar*)ipaddr;
   spfdata.spf_helo = helo;
   spfdata.spf_client = client;
   spfdata.spf_explain = explain;
   spfdata.spf_expdom = NULL;
   spfdata.spf_level = 0;
   spfdata.spf_lookups = 0;

   // if there is a source routing, first try the leftmost domain
   if (*sender == '@' && (cp1 = strchr(sender, ':')) != NULL)
   {
      spfdata.spf_domain = domain;
      spfdata.spf_sender = NULL;
      for (i = 0, cp = sender + 1; i < sizeof(domain) - 1 && *cp != ':' && *cp != ',';
         cp++, i++)
      {
         domain[i] = *cp;
      }
      domain[i] = 0;
      result = check_host(&spfdata, spfdata.spf_domain);
      if (result != SPF_Neutral && result != SPF_None)
      {
         if (spfdata.spf_expdom != NULL) // this is never true if explain==NULL
         {
            *explain = getexplanation(&spfdata);
            spffree((void*)spfdata.spf_expdom);
         }
         return result;
      }
      if (spfdata.spf_expdom != NULL)
         spffree((void*)spfdata.spf_expdom);
      sender = cp1 + 1; // strip source routing
      spfdata.spf_expdom = NULL;
      spfdata.spf_level = 0;
      spfdata.spf_lookups = 0;
   }

   spfdata.spf_domain = strchr(sender, '@');
   if (spfdata.spf_domain != NULL)
   {
      spfdata.spf_domain++;
      spfdata.spf_sender = sender;
   }
   else
   {
      spfdata.spf_domain = sender;
      spfdata.spf_sender = NULL;
   }

   // if it is a domain literal, get the domain name by reverse DNS lookup
   if (*spfdata.spf_domain == '['
      && ((cp = aton4(spfdata.spf_domain + 1, addr, ']')) != NULL && (ipv6 = false, true)
      || _memicmp(spfdata.spf_domain + 1, "IPv6:", 5) == 0
      && (cp = aton6(spfdata.spf_domain + 6, addr, ']')) != NULL && (ipv6 = true, true))
      && *cp == ']' && cp[1] == 0)
   {
      result = gethostname(ipv6, addr, domain, sizeof(domain), NULL, 1 | 8,
         spfdata.spf_time);
      if (result == 0) // PTR found and A/AAAA matches IP address
      {
         spfdata.spf_domain = domain;
         result = check_host(&spfdata, spfdata.spf_domain);
      }
      else if (result != SPF_TempError)
         result = SPF_None | SPF_Literal;
   }
   else
      result = check_host(&spfdata, spfdata.spf_domain);

   if (spfdata.spf_expdom != NULL) // this is never true if explain==NULL
   {
      *explain = getexplanation(&spfdata);
      spffree((void*)spfdata.spf_expdom);
   }

   return result;
}


#ifdef SPFFILECACHE
// remove the expired records from the cache.
static void
cleanupcache(time_t timeout, int type)
{
   int what;
   long pause;
   unsigned keylen;
   unsigned datlen;
   char key[SPFKEYSIZE];
   dnsrec dat;

   if (multithread)
      EnterCriticalSection(&cachemutex);
   pIsamLock(hCache);
   pause=GetTickCount()+100;

   keylen=0;
   what=ISAM_First;
   while (pIsamGet(hCache,what,key,keylen,key,&keylen,sizeof(key),
      &dat,&datlen,sizeof(dat))==ISAM_Ok)
   {
      what=ISAM_Next;
      if (datlen<=SPFEXPSIZE || dat.dns_expire<=timeout
         && (type<0 || type==(key[0]<=1?SPF_Type_PTR:key[keylen-1])))
         pIsamDelete(hCache,key,keylen);
      if ((long)(GetTickCount()-pause)>=0)
      {
         // pause to let others access the cache
         pIsamUnlock(hCache);
         if (multithread)
            LeaveCriticalSection(&cachemutex);
         Sleep(100);
         if (multithread)
            EnterCriticalSection(&cachemutex);
         pIsamLock(hCache);
         pause=GetTickCount()+100;
      }
   }

   pIsamUnlock(hCache);
   if (multithread)
      LeaveCriticalSection(&cachemutex);
 }
#endif //SPFFILECACHE


// remove the expired records from the cache.
void
SPFCleanupCache(long ttl)
{
   time_t timeout;
   time_t currtime;
   int i;
   lstrec* lstp;
#ifdef SPFCACHE
   dnsrec* datp;
#endif //SPFCACHE

   if (initialized <= 0)
      return;

   if (ttl == LONG_MAX)
      timeout = TIME_MAX;
   else
   {
      currtime = time(NULL);
      timeout = currtime + ttl;
      if (ttl > 0 && timeout < currtime) // overflow
         timeout = TIME_MAX;
      else if (ttl<0 && timeout>currtime) // underflow
         timeout = TIME_MIN;
   }

#ifdef SPFFILECACHE
   if (hCache!=NULL)
   {
      cleanupcache(timeout,-1);
      return;
}
#endif //SPFFILECACHE

   if (multithread)
      EnterCriticalSection(&listmutex);
   for (i = listn - 1; i >= 0; i--)
   {
      lstp=listRec[i];
#ifdef SPFCACHE
      datp=(dnsrec*)(lstp->lst_data+lstp->lst_keylen);
      if (datp->dns_expire <= timeout)
#else //SPFCACHE
      if (lstp->lst_expire <= timeout)
#endif //SPFCACHE
      {
         spffree(lstp);
         listn--;
         memmove(listRec + i, listRec + i + 1, (listn - i)*sizeof(lstrec*));
      }
   }
   if (multithread)
      LeaveCriticalSection(&listmutex);
 }


// look up A record of the domain
unsigned long
SPFGetAddress(const char* domain)
{
   int datalen;
   unsigned long ipaddr;
   dnsrec* datap;
   char data[SPFARECSIZE];

   if (domain == NULL || initialized <= 0 && !initspf(1))
      return 0xffffffffU;

   datap = (dnsrec*)data;
   datalen = lookup_addr(domain, false, &datap, sizeof(data), NULL, time(NULL));
   if (datalen < 0)
   {
      if (datalen == -SPF_TempError)
         return 0xffffffffU;
      return 0;
   }
   ipaddr = *(uint32*)(datap->dns_data);
   if (datap != (dnsrec*)data)
      spffree(datap);
   return ipaddr;
}


// set the fall-back policy for the specified domain
int
SPFSetFallBack(const char* domain, const char* policy, long ttl)
{
   int i;
   int ok;
   int keylen;
   int datalen;
   dnsrec* datap;
   char key[SPFKEYSIZE];

   if (initialized <= 0 && !initspf(1))
      return SPF_PermError;

   if (domain == NULL) // remove all fall-back policies
   {
#ifdef SPFFILECACHE
      if (hCache == NULL)
#endif //SPFFILECACHE
      {
         if (multithread)
            EnterCriticalSection(&listmutex);
         for (i = 0; i < listn; i++)
            spffree(listRec[i]);
         listn = 0;
         if (multithread)
            LeaveCriticalSection(&listmutex);
      }
#ifdef SPFFILECACHE
      else
         cleanupcache(TIME_MAX, SPF_Type_FallBack);
#endif //SPFFILECACHE
      return 0;
      }

   keylen = makekey(key, domain, SPF_Type_FallBack);

   if (policy == NULL || *policy == 0) // remove fall-back policy
   {
      if (keylen != 0)
      {
#ifdef SPFFILECACHE
         if (hCache == NULL)
#endif //SPFFILECACHE
            deletelist(key,keylen);
#ifdef SPFFILECACHE
         else
         {
            if (multithread)
               EnterCriticalSection(&cachemutex);
            pIsamDelete(hCache,key,keylen);
            if (multithread)
               LeaveCriticalSection(&cachemutex);
   }
#endif //SPFFILECACHE
}
      return 0;
   }

   if (!checkversion(policy))
      return SPF_PermError;

   datalen = (int)strlen(policy) + SPFEXPSIZE + 1;
   datap = (dnsrec*)spfmalloc(datalen);
   if (datap == NULL)
      return SPF_TempError;
   if (ttl == 0)
      ttl=LONG_MAX;
#ifdef SPFCACHE
   datap->dns_expire = ttl;
#endif //SPFCACHE
   memcpy(datap->dns_data,policy,datalen-SPFEXPSIZE);
#ifdef SPFFILECACHE
   if (hCache!=NULL)
      ok=writecache(key,keylen,datap,datalen,time(NULL));
   else
#endif //SPFFILECACHE
      ok = writelist(key, keylen, datap, datalen, time(NULL), ttl);
   spffree(datap);
   return ok ? 0 : SPF_TempError;
 }


// check if the domain has a valid MX or A record
int
SPFHasMX(const char* address)
{
   int i;
   int k;
   int result;
   int result1;
   int datalen;
   time_t currtime;
   const char* domain;
   DNS_RECORD* dnsresp;
   const char* cp;
   const char* cp1;
   dnsrec* datap;
   uchar addr[16];
   char data[SPFNRECSIZE];

   if (address == NULL || *address == 0)
      return SPF_Neutral;

   domain = strchr(address, '@');
   if (domain == NULL)
      domain = address;
   else
   {
      domain++;
      if (*domain == 0)
         return SPF_PermError;
   }

   if (initialized <= 0 && !initspf(1))
      return SPF_PermError;

   result = SPF_Fail;
   currtime = time(NULL);
   datap = (dnsrec*)data;
   datalen=lookup_mx(domain,&datap,sizeof(data),&dnsresp,currtime);
#ifdef SPFCACHE
   if (datalen==-SPF_NoMemory)
      datalen = -SPF_TempError;
#endif //SPFCACHE

   if (datalen > 0)
   {
      // check A/AAAA records of MX hosts
      cp1 = (char*)datap + datalen;
      for (cp = datap->dns_data; cp < cp1; cp = strchr(cp, 0) + 1)
      {
         // check if it's an IP4 address
         if (aton4(cp, addr, 0) != NULL)
         {
            if (result == SPF_Fail && validaddr4(addr))
               result = SPF_SoftFail;
         }
         else
         {
            // A records
            result1 = findaddr(dnsresp, cp, false, NULL, 0, currtime);
            if (result1 == 0)
            {
               result = SPF_Pass;
               break;
            }
            if (result1 == SPF_TempError)
               result = SPF_TempError;
         }
      }
      if (result != SPF_Pass && result != SPF_SoftFail)
      {
         for (cp = datap->dns_data; cp < cp1; cp = strchr(cp, 0) + 1)
         {
            // AAAA records
            if (aton4(cp, addr, 0) == NULL)
            {
               result1 = findaddr(dnsresp, cp, true, NULL, 0, currtime);
               if (result1 == 0)
               {
                  result = SPF_Pass;
                  break;
               }
               if (result1 == SPF_TempError)
                  result = SPF_TempError;
            }
         }
      }
   }
   else if (datalen == -SPF_None)
   {
      // no MX record, look for A records
      datalen = lookup_addr(domain, false, &datap, sizeof(data), NULL, currtime);
      if (datalen > 0)
      {
         for (i = SPFEXPSIZE, k = i + 4; k <= datalen; i = k, k += 4)
         {
            if (validaddr4((uchar*)datap + i))
            {
               result = SPF_None;
               break;
            }
         }
      }
      else if (datalen != -SPF_None)
         result = -datalen; // SPF_TempError or SPF_PermError
   }
   else
      result = -datalen; // SPF_TempError or SPF_PermError

   if (dnsresp != NULL)
      pDnsRecordListFree(dnsresp, DnsFreeRecordListDeep);
   if (datap != (dnsrec*)data)
      spffree(datap);
   return result;
}


// get result as a string
const char*
SPFResultString(int result)
{
   result &= SPF_ResultMask;
   if ((unsigned)result >= sizeof(SPFResults) / sizeof(SPFResults[0]))
      return unknownstr;
   return SPFResults[result];
}


#ifdef SPFDLL

// DLL entry point
BOOL __stdcall
LibMain(HANDLE hDLL, DWORD dwReason, void* lpReserved)
{
   hDLL=hDLL;
   dwReason=dwReason;
   lpReserved=lpReserved;
   return TRUE;
}


// DLL entry point
BOOL __stdcall
DllMain(HANDLE hDLL, DWORD dwReason, void* lpReserved)
{
   hDLL=hDLL;
   dwReason=dwReason;
   lpReserved=lpReserved;
   return TRUE;
}

#endif //SPFDLL
