// Program to test SPF routines for Windows
//
// Author: Roger Moser (roger.moser@pamho.net)
// Version 1.10
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

    int family;
  int result = 0;
  char ipaddr[16];
  char* cp;
  const char* sender;
  const char* policy;
  const char* explain;

  if (SPFInit(NULL,0, SPF_Multithread)!=0)
    return -1;

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

  sender=argv[2];
  policy=argc>3?argv[3]:NULL;

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
