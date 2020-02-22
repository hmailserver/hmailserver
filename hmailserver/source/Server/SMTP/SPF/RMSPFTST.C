// Program to test SPF routines for Windows
//
// Author: Roger Moser (roger.moser@pamho.net)
// Version 1.12
//
// License: You can do with this source whatever you want.


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rmspf.h"

// from winsock.h
#ifndef AF_INET
#define AF_INET 2
#endif //AF_INET
#ifndef AF_INET6
#define AF_INET6 23
#endif //AF_INET6


int main(int argc, char** argv)
 {
  int i;
  int family;
  int result;
  int expresult;
  long ttl;
  char ipaddr[16];
  char* cp;
  const char* sender;
  const char* policy;
  const char* explain;
  const char* testno;
  FILE* file;
  char s[512];
  static const char sep[]=" \t";

  if (argc<2 || argc>4)
   {
    printf(
      "Usage:  RMSPFTST ipaddress sender [\"policy\"]\n"
      "or:     RMSPFTST testfile\n"
      "or:     RMSPFTST ttl       (to clean-up cache)\n");
    return -1;
   }

  if (SPFInit("SPFCACHE.DAT",0,0)!=0)
    return -1;

  if (SPFSetOurDomain("example.com")!=0)
    return -1;
#if 0
  if (SPFSetBestGuess("v=spf1 a/24 mx/24 ptr ?all","v=spf1 a -all")!=0)
    return -1;
#endif //0
  if (SPFSetWhiteList("v=spf1 include:spf.trusted-forwarder.org")!=0)
    return -1;
  if (SPFSetFallBack("microsoft.com","v=spf1 mx "
    "ip4:213.199.128.160 ip4:213.199.128.145 ip4:207.46.71.29 "
    "ip4:194.121.59.20 ip4:157.60.216.10 ip4:131.107.3.116 "
    "ip4:131.107.3.117 ip4:131.107.3.100 ?all",0)!=0)
    return -1;

  if (argc==2)
   {
    // clean-up cache if argument is a number
    ttl=strtol(argv[1],&cp,10);
    if (*cp==0)
     {
      SPFCleanupCache(ttl);
      return 0;
     }

    // process test file
    file=fopen(argv[1],"rt");
    if (file==NULL)
     {
      printf("Cannot open file %s\n",argv[1]);
      return 1;
     }

    while (fgets(s,sizeof(s),file)!=NULL)
     {
      for (i=0; s[i]!=0 && s[i]!='#' && s[i]!='\n'; i++)
	;
      while (i>0 && s[i-1]<=' ')
	i--;
      if (i==0)
	continue;
      s[i]=0;

      testno=strtok(s,sep);
      sender=strtok(NULL,sep);
      if (strchr(sender,':')!=NULL)
       {
	printf("%s: test with local policy override skipped\n",testno);
	continue;
       }

      cp=strtok(NULL,sep);
      if (strchr(cp,':')==NULL)
	family=AF_INET;
      else
	family=AF_INET6;

      if (SPFStringToAddr(cp,family,ipaddr)==NULL)
       {
	printf("%s: Invalid IP address: %s\n",testno,cp);
	continue;
       }

      cp=strtok(NULL,sep);
      if (cp==NULL)
       {
        printf("%s: Expected result missing\n",testno);
        continue;
       }
      for (expresult=SPF_Pass; expresult<=SPF_PermError; expresult++)
	if (stricmp(cp,SPFResultString(expresult))==0)
	  break;
      if (expresult>SPF_PermError)
       {
	printf("%s: Invalid 'expected result': %s\n",testno,cp);
	continue;
       }

      result=SPFQuery(family,ipaddr,sender,NULL,NULL,NULL,&explain);

      printf("%s: %s",testno,SPFResultString(result));
      if (result!=expresult)
	printf(" [%s]",SPFResultString(expresult));
      if (explain!=NULL)
       {
	printf(" (%s)",explain);
	SPFFree(explain);
       }
      printf("\n");
     }
    fclose(file);
    return 0;
   }

  cp=argv[1];
  if (strchr(cp,':')==NULL)
    family=AF_INET;
  else
    family=AF_INET6;

  if (SPFStringToAddr(cp,family,ipaddr)==NULL)
   {
    printf("Invalid IP address\n");
    return 1;
   }

  result=SPFGetHostName(family,ipaddr,s,sizeof(s),NULL);
  printf("PTR: %s (%s)\n",s,SPFResultString(result));

  sender=argv[2];
  policy=argc>3?argv[3]:NULL;

  result=SPFHasMX(sender);
  printf("MX:  %s\n",SPFResultString(result));

  result=SPFQuery(family,ipaddr,sender,policy,NULL,NULL,&explain);

  printf("SPF: %s",SPFResultString(result));
  if (explain!=NULL)
   {
    printf(" (%s)",explain);
    SPFFree(explain);
   }
  printf("\n");

  return 0;
 }
