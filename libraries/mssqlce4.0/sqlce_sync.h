
//=============================================================================
// Microsoft SQL Server Compact (Version 4.0)
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft
// premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license
// agreement, you are not authorized to use this source code.
// For the terms of the license, please see the license agreement
// signed by you and Microsoft.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
// Component: Connectivity (Merge & RDA)
// 
// File:  sqlce_sync.h
//
// File Comments: Public Interfaces for
//   a) Merge Replication
//   b) Remote Data Access
//   c) Database Maintenance (Engine)
//
//=============================================================================


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0365 */
//@@MIDL_FILE_HEADING(  )


#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SQLCE_SYNC_H__
#define __SQLCE_SYNC_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISSCEParam_FWD_DEFINED__
#define __ISSCEParam_FWD_DEFINED__
typedef interface ISSCEParam ISSCEParam;
#endif 	/* __ISSCEParam_FWD_DEFINED__ */


#ifndef __ISSCEParams_FWD_DEFINED__
#define __ISSCEParams_FWD_DEFINED__
typedef interface ISSCEParams ISSCEParams;
#endif 	/* __ISSCEParams_FWD_DEFINED__ */


#ifndef __ISSCEError_FWD_DEFINED__
#define __ISSCEError_FWD_DEFINED__
typedef interface ISSCEError ISSCEError;
#endif 	/* __ISSCEError_FWD_DEFINED__ */


#ifndef __ISSCEStatusReporting_FWD_DEFINED__
#define __ISSCEStatusReporting_FWD_DEFINED__
typedef interface ISSCEStatusReporting ISSCEStatusReporting;
#endif 	/* __ISSCEStatusReporting_FWD_DEFINED__ */


#ifndef __ISSCEErrors_FWD_DEFINED__
#define __ISSCEErrors_FWD_DEFINED__
typedef interface ISSCEErrors ISSCEErrors;
#endif 	/* __ISSCEErrors_FWD_DEFINED__ */


#ifndef __ISSCEMerge_FWD_DEFINED__
#define __ISSCEMerge_FWD_DEFINED__
typedef interface ISSCEMerge ISSCEMerge;
#endif 	/* __ISSCEMerge_FWD_DEFINED__ */


#ifndef __ISSCERDA_FWD_DEFINED__
#define __ISSCERDA_FWD_DEFINED__
typedef interface ISSCERDA ISSCERDA;
#endif 	/* __ISSCERDA_FWD_DEFINED__ */


#ifndef __ISSCEEngine_FWD_DEFINED__
#define __ISSCEEngine_FWD_DEFINED__
typedef interface ISSCEEngine ISSCEEngine;
#endif 	/* __ISSCEEngine_FWD_DEFINED__ */


#ifndef __Replication_FWD_DEFINED__
#define __Replication_FWD_DEFINED__

#ifdef __cplusplus
typedef class Replication Replication;
#else
typedef struct Replication Replication;
#endif /* __cplusplus */

#endif 	/* __Replication_FWD_DEFINED__ */


#ifndef __RemoteDataAccess_FWD_DEFINED__
#define __RemoteDataAccess_FWD_DEFINED__

#ifdef __cplusplus
typedef class RemoteDataAccess RemoteDataAccess;
#else
typedef struct RemoteDataAccess RemoteDataAccess;
#endif /* __cplusplus */

#endif 	/* __RemoteDataAccess_FWD_DEFINED__ */


#ifndef __Engine_FWD_DEFINED__
#define __Engine_FWD_DEFINED__

#ifdef __cplusplus
typedef class Engine Engine;
#else
typedef struct Engine Engine;
#endif /* __cplusplus */

#endif 	/* __Engine_FWD_DEFINED__ */


#ifndef __SSCEErrors_FWD_DEFINED__
#define __SSCEErrors_FWD_DEFINED__

#ifdef __cplusplus
typedef class SSCEErrors SSCEErrors;
#else
typedef struct SSCEErrors SSCEErrors;
#endif /* __cplusplus */

#endif 	/* __SSCEErrors_FWD_DEFINED__ */


#ifndef __SSCEError_FWD_DEFINED__
#define __SSCEError_FWD_DEFINED__

#ifdef __cplusplus
typedef class SSCEError SSCEError;
#else
typedef struct SSCEError SSCEError;
#endif /* __cplusplus */

#endif 	/* __SSCEError_FWD_DEFINED__ */


#ifndef __SSCEParams_FWD_DEFINED__
#define __SSCEParams_FWD_DEFINED__

#ifdef __cplusplus
typedef class SSCEParams SSCEParams;
#else
typedef struct SSCEParams SSCEParams;
#endif /* __cplusplus */

#endif 	/* __SSCEParams_FWD_DEFINED__ */


#ifndef __SSCEParam_FWD_DEFINED__
#define __SSCEParam_FWD_DEFINED__

#ifdef __cplusplus
typedef class SSCEParam SSCEParam;
#else
typedef struct SSCEParam SSCEParam;
#endif /* __cplusplus */

#endif 	/* __SSCEParam_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_ca_mergex_0000 */
/* [local] */ 

typedef /* [uuid] */  DECLSPEC_UUID("539ed304-a322-4294-9083-b6bd560ebd26") 
enum REPAIROPTION
    {	DELETECORRUPTED	= 0,
	RECOVERCORRUPTED = 1,
	RECOVERALLPOSSIBLEROWS = 1,
        RECOVERALLORFAIL = 2
    } 	REPAIROPTION;

typedef /* [uuid] */  DECLSPEC_UUID("ba00674b-ae9b-4c47-89dd-f3c9e3426a37") 
enum REPL_VALIDATE_TYPE
    {	NO_VALIDATION	= 0,
	ROWCOUNT_ONLY	= 1
    } 	REPL_VALIDATE_TYPE;

typedef /* [uuid] */  DECLSPEC_UUID("12c8b3f5-6942-4e7f-a1d4-704954ec2753") 
enum REPL_EXCHANGE_TYPE
    {	UPLOAD	= 1,
	BIDIRECTIONAL	= 3
    } 	REPL_EXCHANGE_TYPE;

typedef /* [uuid] */  DECLSPEC_UUID("2d11f21c-a6dc-42aa-a618-65dc7ab3567b") 
enum RDA_TRACKOPTION
    {	TRACKINGOFF	= 0,
	TRACKINGON	= 1,
	TRACKINGON_INDEXES	= 2,
	TRACKINGOFF_INDEXES	= 3
    } 	RDA_TRACKOPTION;

typedef /* [uuid] */  DECLSPEC_UUID("50c36238-4906-4b1c-9a01-7b33887d0eeb") 
enum RDA_BATCHOPTION
    {	BATCHINGOFF	= 0,
	BATCHINGON	= 1
    } 	RDA_BATCHOPTION;

typedef /* [uuid] */  DECLSPEC_UUID("b614bde6-0ce7-4b12-8aeb-e1dafa978783") 
enum SSCE_ENCRYPTOPTION
    {	ENCRYPTCOPY	= 0,
	ENCRYPTOFF	= 1,
	ENCRYPTON	= 2
    } 	SSCE_ENCRYPTOPTION;

typedef /* [uuid] */  DECLSPEC_UUID("f6298625-c8aa-416c-a27a-ad37470d3304") 
enum SNAPSHOT_TRANSFER_TYPE
    {	SNAPSHOT_TRANSFER_UNC	= 0,
	SNAPSHOT_TRANSFER_FTP	= 1
    } 	SNAPSHOT_TRANSFER_TYPE;

typedef /* [uuid] */  DECLSPEC_UUID("5501a84c-f065-49d3-b6eb-63cdf9081639") 
enum REPL_SAVEPROFILEOPTION
    {	EXISTING_PROFILE	= 0,
	CREATE_PROFILE	= 1
    } 	REPL_SAVEPROFILEOPTION;



///////////////////////////////////////////////////////////////////////////
// definitions from ReplCommon.h
//
#ifndef REPL_SECURITY_TYPE
typedef
enum SECURITY_TYPE
    { DB_AUTHENTICATION = 0,
  NT_AUTHENTICATION = 1
    }   REPL_SECURITY_TYPE;

#endif //REPL_SECURITY_TYPE

#ifndef REPL_NETWORK_TYPE
typedef
enum NETWORK_TYPE
    { DEFAULT_NETWORK = 0,
  TCPIP_SOCKETS = 1,
  MULTI_PROTOCOL  = 2
    }   REPL_NETWORK_TYPE;
#endif // REPL_NETWORK_TYPE


#ifndef REPL_DBADDOPTION
typedef
enum DBADDOPTION
    { EXISTING_DATABASE = 0,
  CREATE_DATABASE = 1,
  ATTACH_DATABASE = 2,
  ATTACH_SUBSCRIPTION = 3,
  REGISTER_SUBSCRIPTION = 4
    }   REPL_DBADDOPTION;
#endif  // REPL_DBADDOPTION

#ifndef REPL_DBDROPOPTION
typedef
enum DBDROPOPTION
    { LEAVE_DATABASE  = 0,
  DROP_DATABASE = 1,
  UNREGISTER_SUBSCRIPTION = 2
    }   REPL_DBDROPOPTION;

#endif //REPL_DBDROPOPTION



typedef /* [uuid] */  DECLSPEC_UUID("6a120382-da3d-4c7f-9673-02f227e646b5") struct INTERNET_TRANSPORT_OPTIONS
    {
    SHORT CompressionLevel;
    SHORT ConnectionRetryTimeout;
    LONG ConnectTimeout;
    LONG SendTimeout;
    LONG ReceiveTimeout;
    LONG DataSendTimeout;
    LONG DataReceiveTimeout;
    LONG ControlSendTimeout;
    LONG ControlReceiveTimeout;
    } 	INTERNET_TRANSPORT_OPTIONS;



//
// Time units
//
#define SECONDS                                  1000 /* milli seconds */
//
// INETOPT_MIN Values
//
#define INETOPT_MIN_COMPRESSIONLEVEL             (SHORT) (0)
#define INETOPT_MIN_CONNECTIONRETRYTIMEOUT       (SHORT) (0) /* seconds */
#define INETOPT_MIN_CONNECTTIMEOUT               0*SECONDS
#define INETOPT_MIN_SENDTIMEOUT                  0*SECONDS
#define INETOPT_MIN_RECEIVETIMEOUT               0*SECONDS
#define INETOPT_MIN_DATARECEIVETIMEOUT           0*SECONDS 
#define INETOPT_MIN_DATASENDTIMEOUT              0*SECONDS
#define INETOPT_MIN_CONTROLSENDTIMEOUT           0*SECONDS
#define INETOPT_MIN_CONTROLRECEIVETIMEOUT        0*SECONDS
//
// INETOPT_MAX Values
//
#define INETOPT_MAX_COMPRESSIONLEVEL             (SHORT) (6)
#define INETOPT_MAX_CONNECTIONRETRYTIMEOUT       (SHORT) (900) /* seconds */
#define INETOPT_MAX_CONNECTTIMEOUT               360*SECONDS
#define INETOPT_MAX_SENDTIMEOUT                  360*SECONDS
#define INETOPT_MAX_RECEIVETIMEOUT               360*SECONDS
#define INETOPT_MAX_DATARECEIVETIMEOUT           360*SECONDS 
#define INETOPT_MAX_DATASENDTIMEOUT              360*SECONDS
#define INETOPT_MAX_CONTROLSENDTIMEOUT           360*SECONDS
#define INETOPT_MAX_CONTROLRECEIVETIMEOUT        360*SECONDS
//
// INETOPT_DEFAULT Values
//
#define INETOPT_DEFAULT_COMPRESSIONLEVEL         (SHORT) (1)
#define INETOPT_DEFAULT_CONNECTIONRETRYTIMEOUT   (SHORT) (120) /* seconds */
#define INETOPT_DEFAULT_CONNECTTIMEOUT           60*SECONDS
#define INETOPT_DEFAULT_SENDTIMEOUT              60*SECONDS
#define INETOPT_DEFAULT_RECEIVETIMEOUT           60*SECONDS
#define INETOPT_DEFAULT_DATARECEIVETIMEOUT       60*SECONDS 
#define INETOPT_DEFAULT_DATASENDTIMEOUT          60*SECONDS
#define INETOPT_DEFAULT_CONTROLSENDTIMEOUT       60*SECONDS
#define INETOPT_DEFAULT_CONTROLRECEIVETIMEOUT    60*SECONDS




extern RPC_IF_HANDLE __MIDL_itf_ca_mergex_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ca_mergex_0000_v0_0_s_ifspec;

#ifndef __ISSCEParam_INTERFACE_DEFINED__
#define __ISSCEParam_INTERFACE_DEFINED__

/* interface ISSCEParam */
/* [unique][helpstring][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a9876c60-2667-44e5-89db-e9a46ed392c0")
    ISSCEParam : public IUnknown
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Param( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Param( 
            /* [in] */ VARIANT *newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEParamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEParam * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEParam * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEParam * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Param )( 
            ISSCEParam * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Param )( 
            ISSCEParam * This,
            /* [in] */ VARIANT *newVal);
        
        END_INTERFACE
    } ISSCEParamVtbl;

    interface ISSCEParam
    {
        CONST_VTBL struct ISSCEParamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEParam_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEParam_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEParam_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEParam_get_Param(This,pVal)	\
    (This)->lpVtbl -> get_Param(This,pVal)

#define ISSCEParam_put_Param(This,newVal)	\
    (This)->lpVtbl -> put_Param(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEParam_get_Param_Proxy( 
    ISSCEParam * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB ISSCEParam_get_Param_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEParam_put_Param_Proxy( 
    ISSCEParam * This,
    /* [in] */ VARIANT *newVal);


void __RPC_STUB ISSCEParam_put_Param_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCEParam_INTERFACE_DEFINED__ */


#ifndef __ISSCEParams_INTERFACE_DEFINED__
#define __ISSCEParams_INTERFACE_DEFINED__

/* interface ISSCEParams */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a78aff90-049c-41ec-b1d8-665968aac4a6")
    ISSCEParams : public IUnknown
    {
    public:
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get_NewEnum( 
            /* [retval][out] */ LPUNKNOWN *ppvObject) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *cMembers) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE ClearParams( void) = 0;
        
        virtual /* [hidden][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ISSCEParam **ppobj) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE AddParam( 
            /* [in] */ VARIANT *pvarParam) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISSCEParam *pISSCEParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEParamsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEParams * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEParams * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEParams * This);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NewEnum )( 
            ISSCEParams * This,
            /* [retval][out] */ LPUNKNOWN *ppvObject);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISSCEParams * This,
            /* [retval][out] */ long *cMembers);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *ClearParams )( 
            ISSCEParams * This);
        
        /* [hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISSCEParams * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ISSCEParam **ppobj);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *AddParam )( 
            ISSCEParams * This,
            /* [in] */ VARIANT *pvarParam);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISSCEParams * This,
            /* [in] */ ISSCEParam *pISSCEParam);
        
        END_INTERFACE
    } ISSCEParamsVtbl;

    interface ISSCEParams
    {
        CONST_VTBL struct ISSCEParamsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEParams_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEParams_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEParams_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEParams_get_NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> get_NewEnum(This,ppvObject)

#define ISSCEParams_get_Count(This,cMembers)	\
    (This)->lpVtbl -> get_Count(This,cMembers)

#define ISSCEParams_ClearParams(This)	\
    (This)->lpVtbl -> ClearParams(This)

#define ISSCEParams_get_Item(This,Index,ppobj)	\
    (This)->lpVtbl -> get_Item(This,Index,ppobj)

#define ISSCEParams_AddParam(This,pvarParam)	\
    (This)->lpVtbl -> AddParam(This,pvarParam)

#define ISSCEParams_Add(This,pISSCEParam)	\
    (This)->lpVtbl -> Add(This,pISSCEParam)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEParams_get_NewEnum_Proxy( 
    ISSCEParams * This,
    /* [retval][out] */ LPUNKNOWN *ppvObject);


void __RPC_STUB ISSCEParams_get_NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEParams_get_Count_Proxy( 
    ISSCEParams * This,
    /* [retval][out] */ long *cMembers);


void __RPC_STUB ISSCEParams_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id] */ HRESULT STDMETHODCALLTYPE ISSCEParams_ClearParams_Proxy( 
    ISSCEParams * This);


void __RPC_STUB ISSCEParams_ClearParams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEParams_get_Item_Proxy( 
    ISSCEParams * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ ISSCEParam **ppobj);


void __RPC_STUB ISSCEParams_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEParams_AddParam_Proxy( 
    ISSCEParams * This,
    /* [in] */ VARIANT *pvarParam);


void __RPC_STUB ISSCEParams_AddParam_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEParams_Add_Proxy( 
    ISSCEParams * This,
    /* [in] */ ISSCEParam *pISSCEParam);


void __RPC_STUB ISSCEParams_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCEParams_INTERFACE_DEFINED__ */


#ifndef __ISSCEError_INTERFACE_DEFINED__
#define __ISSCEError_INTERFACE_DEFINED__

/* interface ISSCEError */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("31155a3b-871d-407f-9f73-debfac7efbe3")
    ISSCEError : public IUnknown
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Number( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_NativeError( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Params( 
            /* [retval][out] */ ISSCEParams **pVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE SetDescription( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE SetSource( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE SetNumber( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE SetNativeError( 
            /* [in] */ long newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEError * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEError * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEError * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            ISSCEError * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            ISSCEError * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Number )( 
            ISSCEError * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NativeError )( 
            ISSCEError * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Params )( 
            ISSCEError * This,
            /* [retval][out] */ ISSCEParams **pVal);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *SetDescription )( 
            ISSCEError * This,
            /* [in] */ BSTR newVal);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *SetSource )( 
            ISSCEError * This,
            /* [in] */ BSTR newVal);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *SetNumber )( 
            ISSCEError * This,
            /* [in] */ long newVal);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *SetNativeError )( 
            ISSCEError * This,
            /* [in] */ long newVal);
        
        END_INTERFACE
    } ISSCEErrorVtbl;

    interface ISSCEError
    {
        CONST_VTBL struct ISSCEErrorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEError_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define ISSCEError_get_Source(This,pVal)	\
    (This)->lpVtbl -> get_Source(This,pVal)

#define ISSCEError_get_Number(This,pVal)	\
    (This)->lpVtbl -> get_Number(This,pVal)

#define ISSCEError_get_NativeError(This,pVal)	\
    (This)->lpVtbl -> get_NativeError(This,pVal)

#define ISSCEError_get_Params(This,pVal)	\
    (This)->lpVtbl -> get_Params(This,pVal)

#define ISSCEError_SetDescription(This,newVal)	\
    (This)->lpVtbl -> SetDescription(This,newVal)

#define ISSCEError_SetSource(This,newVal)	\
    (This)->lpVtbl -> SetSource(This,newVal)

#define ISSCEError_SetNumber(This,newVal)	\
    (This)->lpVtbl -> SetNumber(This,newVal)

#define ISSCEError_SetNativeError(This,newVal)	\
    (This)->lpVtbl -> SetNativeError(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEError_get_Description_Proxy( 
    ISSCEError * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEError_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEError_get_Source_Proxy( 
    ISSCEError * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEError_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEError_get_Number_Proxy( 
    ISSCEError * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISSCEError_get_Number_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEError_get_NativeError_Proxy( 
    ISSCEError * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISSCEError_get_NativeError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEError_get_Params_Proxy( 
    ISSCEError * This,
    /* [retval][out] */ ISSCEParams **pVal);


void __RPC_STUB ISSCEError_get_Params_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEError_SetDescription_Proxy( 
    ISSCEError * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEError_SetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEError_SetSource_Proxy( 
    ISSCEError * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEError_SetSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEError_SetNumber_Proxy( 
    ISSCEError * This,
    /* [in] */ long newVal);


void __RPC_STUB ISSCEError_SetNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEError_SetNativeError_Proxy( 
    ISSCEError * This,
    /* [in] */ long newVal);


void __RPC_STUB ISSCEError_SetNativeError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCEError_INTERFACE_DEFINED__ */


#ifndef __ISSCEStatusReporting_INTERFACE_DEFINED__
#define __ISSCEStatusReporting_INTERFACE_DEFINED__

/* interface ISSCEStatusReporting */
/* [unique][helpstring][nonextensible][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9be851c8-d1ab-4091-a5e2-c200c02f0e8d")
    ISSCEStatusReporting : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStartTableUpload( 
            const WCHAR *wszTableName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStartTableDownload( 
            const WCHAR *wszTableName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSynchronization( 
            DWORD dwPrecentCompleted) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEStatusReportingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEStatusReporting * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEStatusReporting * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEStatusReporting * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnStartTableUpload )( 
            ISSCEStatusReporting * This,
            const WCHAR *wszTableName);
        
        HRESULT ( STDMETHODCALLTYPE *OnStartTableDownload )( 
            ISSCEStatusReporting * This,
            const WCHAR *wszTableName);
        
        HRESULT ( STDMETHODCALLTYPE *OnSynchronization )( 
            ISSCEStatusReporting * This,
            DWORD dwPrecentCompleted);
        
        END_INTERFACE
    } ISSCEStatusReportingVtbl;

    interface ISSCEStatusReporting
    {
        CONST_VTBL struct ISSCEStatusReportingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEStatusReporting_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEStatusReporting_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEStatusReporting_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEStatusReporting_OnStartTableUpload(This,wszTableName)	\
    (This)->lpVtbl -> OnStartTableUpload(This,wszTableName)

#define ISSCEStatusReporting_OnStartTableDownload(This,wszTableName)	\
    (This)->lpVtbl -> OnStartTableDownload(This,wszTableName)

#define ISSCEStatusReporting_OnSynchronization(This,dwPrecentCompleted)	\
    (This)->lpVtbl -> OnSynchronization(This,dwPrecentCompleted)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISSCEStatusReporting_OnStartTableUpload_Proxy( 
    ISSCEStatusReporting * This,
    const WCHAR *wszTableName);


void __RPC_STUB ISSCEStatusReporting_OnStartTableUpload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISSCEStatusReporting_OnStartTableDownload_Proxy( 
    ISSCEStatusReporting * This,
    const WCHAR *wszTableName);


void __RPC_STUB ISSCEStatusReporting_OnStartTableDownload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISSCEStatusReporting_OnSynchronization_Proxy( 
    ISSCEStatusReporting * This,
    DWORD dwPrecentCompleted);


void __RPC_STUB ISSCEStatusReporting_OnSynchronization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCEStatusReporting_INTERFACE_DEFINED__ */


#ifndef __ISSCEErrors_INTERFACE_DEFINED__
#define __ISSCEErrors_INTERFACE_DEFINED__

/* interface ISSCEErrors */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c40143ca-e9f9-4ff4-b8b4-cc02c064fc1b")
    ISSCEErrors : public IUnknown
    {
    public:
        virtual /* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get_NewEnum( 
            /* [retval][out] */ LPUNKNOWN *ppvObject) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *cMembers) = 0;
        
        virtual /* [restricted][id] */ HRESULT STDMETHODCALLTYPE ClearErrors( void) = 0;
        
        virtual /* [hidden][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ISSCEError **ppobj) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE AddError( 
            /* [in] */ BSTR bstrDescription,
            /* [in] */ BSTR bstrSource,
            /* [in] */ long lNumber,
            /* [in] */ long lNativeError,
            /* [in] */ long lParams,
            /* [in] */ VARIANT *pvarParam) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISSCEError *pISSCEError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEErrorsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEErrors * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEErrors * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEErrors * This);
        
        /* [restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NewEnum )( 
            ISSCEErrors * This,
            /* [retval][out] */ LPUNKNOWN *ppvObject);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISSCEErrors * This,
            /* [retval][out] */ long *cMembers);
        
        /* [restricted][id] */ HRESULT ( STDMETHODCALLTYPE *ClearErrors )( 
            ISSCEErrors * This);
        
        /* [hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISSCEErrors * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ISSCEError **ppobj);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *AddError )( 
            ISSCEErrors * This,
            /* [in] */ BSTR bstrDescription,
            /* [in] */ BSTR bstrSource,
            /* [in] */ long lNumber,
            /* [in] */ long lNativeError,
            /* [in] */ long lParams,
            /* [in] */ VARIANT *pvarParam);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISSCEErrors * This,
            /* [in] */ ISSCEError *pISSCEError);
        
        END_INTERFACE
    } ISSCEErrorsVtbl;

    interface ISSCEErrors
    {
        CONST_VTBL struct ISSCEErrorsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEErrors_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEErrors_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEErrors_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEErrors_get_NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> get_NewEnum(This,ppvObject)

#define ISSCEErrors_get_Count(This,cMembers)	\
    (This)->lpVtbl -> get_Count(This,cMembers)

#define ISSCEErrors_ClearErrors(This)	\
    (This)->lpVtbl -> ClearErrors(This)

#define ISSCEErrors_get_Item(This,Index,ppobj)	\
    (This)->lpVtbl -> get_Item(This,Index,ppobj)

#define ISSCEErrors_AddError(This,bstrDescription,bstrSource,lNumber,lNativeError,lParams,pvarParam)	\
    (This)->lpVtbl -> AddError(This,bstrDescription,bstrSource,lNumber,lNativeError,lParams,pvarParam)

#define ISSCEErrors_Add(This,pISSCEError)	\
    (This)->lpVtbl -> Add(This,pISSCEError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [restricted][id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEErrors_get_NewEnum_Proxy( 
    ISSCEErrors * This,
    /* [retval][out] */ LPUNKNOWN *ppvObject);


void __RPC_STUB ISSCEErrors_get_NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEErrors_get_Count_Proxy( 
    ISSCEErrors * This,
    /* [retval][out] */ long *cMembers);


void __RPC_STUB ISSCEErrors_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id] */ HRESULT STDMETHODCALLTYPE ISSCEErrors_ClearErrors_Proxy( 
    ISSCEErrors * This);


void __RPC_STUB ISSCEErrors_ClearErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEErrors_get_Item_Proxy( 
    ISSCEErrors * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ ISSCEError **ppobj);


void __RPC_STUB ISSCEErrors_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEErrors_AddError_Proxy( 
    ISSCEErrors * This,
    /* [in] */ BSTR bstrDescription,
    /* [in] */ BSTR bstrSource,
    /* [in] */ long lNumber,
    /* [in] */ long lNativeError,
    /* [in] */ long lParams,
    /* [in] */ VARIANT *pvarParam);


void __RPC_STUB ISSCEErrors_AddError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEErrors_Add_Proxy( 
    ISSCEErrors * This,
    /* [in] */ ISSCEError *pISSCEError);


void __RPC_STUB ISSCEErrors_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCEErrors_INTERFACE_DEFINED__ */


#ifndef __ISSCEMerge_INTERFACE_DEFINED__
#define __ISSCEMerge_INTERFACE_DEFINED__

/* interface ISSCEMerge */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c6eb397f-d585-428d-a4f4-454a1842cb47")
    ISSCEMerge : public IUnknown
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Distributor( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Distributor( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DistributorAddress( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributorAddress( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DistributorNetwork( 
            /* [retval][out] */ REPL_NETWORK_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributorNetwork( 
            /* [in] */ REPL_NETWORK_TYPE newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DistributorSecurityMode( 
            /* [retval][out] */ REPL_SECURITY_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributorSecurityMode( 
            /* [in] */ REPL_SECURITY_TYPE newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DistributorLogin( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributorLogin( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DistributorPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributorPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorRecords( 
            /* [retval][out] */ ISSCEErrors **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ExchangeType( 
            /* [retval][out] */ REPL_EXCHANGE_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ExchangeType( 
            /* [in] */ REPL_EXCHANGE_TYPE newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetURL( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetURL( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetLogin( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetLogin( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_LoginTimeout( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_LoginTimeout( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ProfileName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ProfileName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Publisher( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Publisher( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherAddress( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherAddress( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherNetwork( 
            /* [retval][out] */ REPL_NETWORK_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherNetwork( 
            /* [in] */ REPL_NETWORK_TYPE newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherDatabase( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherDatabase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherSecurityMode( 
            /* [retval][out] */ REPL_SECURITY_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherSecurityMode( 
            /* [in] */ REPL_SECURITY_TYPE newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherLogin( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherLogin( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Publication( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Publication( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_QueryTimeout( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_QueryTimeout( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriberConnectionString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SubscriberConnectionString( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherChanges( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherConflicts( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriberChanges( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriberConflicts( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Validate( 
            /* [retval][out] */ REPL_VALIDATE_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Validate( 
            /* [in] */ REPL_VALIDATE_TYPE newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_HostName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_HostName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Subscriber( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Subscriber( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetProxyServer( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetProxyServer( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetProxyLogin( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetProxyLogin( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetProxyPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetProxyPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionRetryTimeout( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionRetryTimeout( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_CompressionLevel( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_CompressionLevel( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionManager( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionManager( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_StatusReportingHandler( 
            /* [retval][out] */ ISSCEStatusReporting **ppVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_StatusReportingHandler( 
            /* [in] */ ISSCEStatusReporting *newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SnapshotTransferType( 
            /* [retval][out] */ SNAPSHOT_TRANSFER_TYPE *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SnapshotTransferType( 
            /* [in] */ SNAPSHOT_TRANSFER_TYPE newVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Initialize( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddSubscription( 
            /* [defaultvalue][in] */ REPL_DBADDOPTION DBAddOption = CREATE_DATABASE) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DropSubscription( 
            /* [defaultvalue][in] */ REPL_DBDROPOPTION DBDropOption = DROP_DATABASE) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Terminate( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ReInitializeSubscription( 
            /* [defaultvalue][in] */ VARIANT_BOOL UploadBeforeReinit = FALSE) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SendTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SendTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ReceiveTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ReceiveTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DataSendTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DataSendTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DataReceiveTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DataReceiveTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ControlSendTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ControlSendTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ControlReceiveTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ControlReceiveTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetTransportOptions( 
            /* [retval][out] */ INTERNET_TRANSPORT_OPTIONS *pGetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetTransportOptions( 
            /* [in] */ INTERNET_TRANSPORT_OPTIONS *pPutVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyLocalFile( 
            /* [in] */ const WCHAR *pwszFileURL) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PostSyncCleanup( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PostSyncCleanup( 
            /* [in] */ SHORT newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEMergeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEMerge * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEMerge * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEMerge * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Distributor )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Distributor )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DistributorAddress )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributorAddress )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DistributorNetwork )( 
            ISSCEMerge * This,
            /* [retval][out] */ REPL_NETWORK_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributorNetwork )( 
            ISSCEMerge * This,
            /* [in] */ REPL_NETWORK_TYPE newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DistributorSecurityMode )( 
            ISSCEMerge * This,
            /* [retval][out] */ REPL_SECURITY_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributorSecurityMode )( 
            ISSCEMerge * This,
            /* [in] */ REPL_SECURITY_TYPE newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DistributorLogin )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributorLogin )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DistributorPassword )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributorPassword )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ErrorRecords )( 
            ISSCEMerge * This,
            /* [retval][out] */ ISSCEErrors **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExchangeType )( 
            ISSCEMerge * This,
            /* [retval][out] */ REPL_EXCHANGE_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ExchangeType )( 
            ISSCEMerge * This,
            /* [in] */ REPL_EXCHANGE_TYPE newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetURL )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetURL )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetLogin )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetLogin )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetPassword )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetPassword )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LoginTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LoginTimeout )( 
            ISSCEMerge * This,
            /* [in] */ short newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProfileName )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ProfileName )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Publisher )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Publisher )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherAddress )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PublisherAddress )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherNetwork )( 
            ISSCEMerge * This,
            /* [retval][out] */ REPL_NETWORK_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PublisherNetwork )( 
            ISSCEMerge * This,
            /* [in] */ REPL_NETWORK_TYPE newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherDatabase )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PublisherDatabase )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherSecurityMode )( 
            ISSCEMerge * This,
            /* [retval][out] */ REPL_SECURITY_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PublisherSecurityMode )( 
            ISSCEMerge * This,
            /* [in] */ REPL_SECURITY_TYPE newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherLogin )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PublisherLogin )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherPassword )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PublisherPassword )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Publication )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Publication )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_QueryTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_QueryTimeout )( 
            ISSCEMerge * This,
            /* [in] */ short newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubscriberConnectionString )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SubscriberConnectionString )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherChanges )( 
            ISSCEMerge * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PublisherConflicts )( 
            ISSCEMerge * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubscriberChanges )( 
            ISSCEMerge * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubscriberConflicts )( 
            ISSCEMerge * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Validate )( 
            ISSCEMerge * This,
            /* [retval][out] */ REPL_VALIDATE_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Validate )( 
            ISSCEMerge * This,
            /* [in] */ REPL_VALIDATE_TYPE newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HostName )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_HostName )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Subscriber )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Subscriber )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetProxyServer )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetProxyServer )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetProxyLogin )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetProxyLogin )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetProxyPassword )( 
            ISSCEMerge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetProxyPassword )( 
            ISSCEMerge * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectionRetryTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConnectionRetryTimeout )( 
            ISSCEMerge * This,
            /* [in] */ short newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CompressionLevel )( 
            ISSCEMerge * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CompressionLevel )( 
            ISSCEMerge * This,
            /* [in] */ short newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectionManager )( 
            ISSCEMerge * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConnectionManager )( 
            ISSCEMerge * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StatusReportingHandler )( 
            ISSCEMerge * This,
            /* [retval][out] */ ISSCEStatusReporting **ppVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StatusReportingHandler )( 
            ISSCEMerge * This,
            /* [in] */ ISSCEStatusReporting *newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SnapshotTransferType )( 
            ISSCEMerge * This,
            /* [retval][out] */ SNAPSHOT_TRANSFER_TYPE *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SnapshotTransferType )( 
            ISSCEMerge * This,
            /* [in] */ SNAPSHOT_TRANSFER_TYPE newVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ISSCEMerge * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddSubscription )( 
            ISSCEMerge * This,
            /* [defaultvalue][in] */ REPL_DBADDOPTION DBAddOption);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DropSubscription )( 
            ISSCEMerge * This,
            /* [defaultvalue][in] */ REPL_DBDROPOPTION DBDropOption);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Run )( 
            ISSCEMerge * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            ISSCEMerge * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ReInitializeSubscription )( 
            ISSCEMerge * This,
            /* [defaultvalue][in] */ VARIANT_BOOL UploadBeforeReinit);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Cancel )( 
            ISSCEMerge * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConnectTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SendTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SendTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReceiveTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ReceiveTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataSendTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DataSendTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataReceiveTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DataReceiveTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ControlSendTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ControlSendTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ControlReceiveTimeout )( 
            ISSCEMerge * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ControlReceiveTimeout )( 
            ISSCEMerge * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetTransportOptions )( 
            ISSCEMerge * This,
            /* [retval][out] */ INTERNET_TRANSPORT_OPTIONS *pGetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetTransportOptions )( 
            ISSCEMerge * This,
            /* [in] */ INTERNET_TRANSPORT_OPTIONS *pPutVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyLocalFile )( 
            ISSCEMerge * This,
            /* [in] */ const WCHAR *pwszFileURL);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PostSyncCleanup )( 
            ISSCEMerge * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PostSyncCleanup )( 
            ISSCEMerge * This,
            /* [in] */ SHORT newVal);
        
        END_INTERFACE
    } ISSCEMergeVtbl;

    interface ISSCEMerge
    {
        CONST_VTBL struct ISSCEMergeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEMerge_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEMerge_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEMerge_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEMerge_get_Distributor(This,pVal)	\
    (This)->lpVtbl -> get_Distributor(This,pVal)

#define ISSCEMerge_put_Distributor(This,newVal)	\
    (This)->lpVtbl -> put_Distributor(This,newVal)

#define ISSCEMerge_get_DistributorAddress(This,pVal)	\
    (This)->lpVtbl -> get_DistributorAddress(This,pVal)

#define ISSCEMerge_put_DistributorAddress(This,newVal)	\
    (This)->lpVtbl -> put_DistributorAddress(This,newVal)

#define ISSCEMerge_get_DistributorNetwork(This,pVal)	\
    (This)->lpVtbl -> get_DistributorNetwork(This,pVal)

#define ISSCEMerge_put_DistributorNetwork(This,newVal)	\
    (This)->lpVtbl -> put_DistributorNetwork(This,newVal)

#define ISSCEMerge_get_DistributorSecurityMode(This,pVal)	\
    (This)->lpVtbl -> get_DistributorSecurityMode(This,pVal)

#define ISSCEMerge_put_DistributorSecurityMode(This,newVal)	\
    (This)->lpVtbl -> put_DistributorSecurityMode(This,newVal)

#define ISSCEMerge_get_DistributorLogin(This,pVal)	\
    (This)->lpVtbl -> get_DistributorLogin(This,pVal)

#define ISSCEMerge_put_DistributorLogin(This,newVal)	\
    (This)->lpVtbl -> put_DistributorLogin(This,newVal)

#define ISSCEMerge_get_DistributorPassword(This,pVal)	\
    (This)->lpVtbl -> get_DistributorPassword(This,pVal)

#define ISSCEMerge_put_DistributorPassword(This,newVal)	\
    (This)->lpVtbl -> put_DistributorPassword(This,newVal)

#define ISSCEMerge_get_ErrorRecords(This,pVal)	\
    (This)->lpVtbl -> get_ErrorRecords(This,pVal)

#define ISSCEMerge_get_ExchangeType(This,pVal)	\
    (This)->lpVtbl -> get_ExchangeType(This,pVal)

#define ISSCEMerge_put_ExchangeType(This,newVal)	\
    (This)->lpVtbl -> put_ExchangeType(This,newVal)

#define ISSCEMerge_get_InternetURL(This,pVal)	\
    (This)->lpVtbl -> get_InternetURL(This,pVal)

#define ISSCEMerge_put_InternetURL(This,newVal)	\
    (This)->lpVtbl -> put_InternetURL(This,newVal)

#define ISSCEMerge_get_InternetLogin(This,pVal)	\
    (This)->lpVtbl -> get_InternetLogin(This,pVal)

#define ISSCEMerge_put_InternetLogin(This,newVal)	\
    (This)->lpVtbl -> put_InternetLogin(This,newVal)

#define ISSCEMerge_get_InternetPassword(This,pVal)	\
    (This)->lpVtbl -> get_InternetPassword(This,pVal)

#define ISSCEMerge_put_InternetPassword(This,newVal)	\
    (This)->lpVtbl -> put_InternetPassword(This,newVal)

#define ISSCEMerge_get_LoginTimeout(This,pVal)	\
    (This)->lpVtbl -> get_LoginTimeout(This,pVal)

#define ISSCEMerge_put_LoginTimeout(This,newVal)	\
    (This)->lpVtbl -> put_LoginTimeout(This,newVal)

#define ISSCEMerge_get_ProfileName(This,pVal)	\
    (This)->lpVtbl -> get_ProfileName(This,pVal)

#define ISSCEMerge_put_ProfileName(This,newVal)	\
    (This)->lpVtbl -> put_ProfileName(This,newVal)

#define ISSCEMerge_get_Publisher(This,pVal)	\
    (This)->lpVtbl -> get_Publisher(This,pVal)

#define ISSCEMerge_put_Publisher(This,newVal)	\
    (This)->lpVtbl -> put_Publisher(This,newVal)

#define ISSCEMerge_get_PublisherAddress(This,pVal)	\
    (This)->lpVtbl -> get_PublisherAddress(This,pVal)

#define ISSCEMerge_put_PublisherAddress(This,newVal)	\
    (This)->lpVtbl -> put_PublisherAddress(This,newVal)

#define ISSCEMerge_get_PublisherNetwork(This,pVal)	\
    (This)->lpVtbl -> get_PublisherNetwork(This,pVal)

#define ISSCEMerge_put_PublisherNetwork(This,newVal)	\
    (This)->lpVtbl -> put_PublisherNetwork(This,newVal)

#define ISSCEMerge_get_PublisherDatabase(This,pVal)	\
    (This)->lpVtbl -> get_PublisherDatabase(This,pVal)

#define ISSCEMerge_put_PublisherDatabase(This,newVal)	\
    (This)->lpVtbl -> put_PublisherDatabase(This,newVal)

#define ISSCEMerge_get_PublisherSecurityMode(This,pVal)	\
    (This)->lpVtbl -> get_PublisherSecurityMode(This,pVal)

#define ISSCEMerge_put_PublisherSecurityMode(This,newVal)	\
    (This)->lpVtbl -> put_PublisherSecurityMode(This,newVal)

#define ISSCEMerge_get_PublisherLogin(This,pVal)	\
    (This)->lpVtbl -> get_PublisherLogin(This,pVal)

#define ISSCEMerge_put_PublisherLogin(This,newVal)	\
    (This)->lpVtbl -> put_PublisherLogin(This,newVal)

#define ISSCEMerge_get_PublisherPassword(This,pVal)	\
    (This)->lpVtbl -> get_PublisherPassword(This,pVal)

#define ISSCEMerge_put_PublisherPassword(This,newVal)	\
    (This)->lpVtbl -> put_PublisherPassword(This,newVal)

#define ISSCEMerge_get_Publication(This,pVal)	\
    (This)->lpVtbl -> get_Publication(This,pVal)

#define ISSCEMerge_put_Publication(This,newVal)	\
    (This)->lpVtbl -> put_Publication(This,newVal)

#define ISSCEMerge_get_QueryTimeout(This,pVal)	\
    (This)->lpVtbl -> get_QueryTimeout(This,pVal)

#define ISSCEMerge_put_QueryTimeout(This,newVal)	\
    (This)->lpVtbl -> put_QueryTimeout(This,newVal)

#define ISSCEMerge_get_SubscriberConnectionString(This,pVal)	\
    (This)->lpVtbl -> get_SubscriberConnectionString(This,pVal)

#define ISSCEMerge_put_SubscriberConnectionString(This,newVal)	\
    (This)->lpVtbl -> put_SubscriberConnectionString(This,newVal)

#define ISSCEMerge_get_PublisherChanges(This,pVal)	\
    (This)->lpVtbl -> get_PublisherChanges(This,pVal)

#define ISSCEMerge_get_PublisherConflicts(This,pVal)	\
    (This)->lpVtbl -> get_PublisherConflicts(This,pVal)

#define ISSCEMerge_get_SubscriberChanges(This,pVal)	\
    (This)->lpVtbl -> get_SubscriberChanges(This,pVal)

#define ISSCEMerge_get_SubscriberConflicts(This,pVal)	\
    (This)->lpVtbl -> get_SubscriberConflicts(This,pVal)

#define ISSCEMerge_get_Validate(This,pVal)	\
    (This)->lpVtbl -> get_Validate(This,pVal)

#define ISSCEMerge_put_Validate(This,newVal)	\
    (This)->lpVtbl -> put_Validate(This,newVal)

#define ISSCEMerge_get_HostName(This,pVal)	\
    (This)->lpVtbl -> get_HostName(This,pVal)

#define ISSCEMerge_put_HostName(This,newVal)	\
    (This)->lpVtbl -> put_HostName(This,newVal)

#define ISSCEMerge_get_Subscriber(This,pVal)	\
    (This)->lpVtbl -> get_Subscriber(This,pVal)

#define ISSCEMerge_put_Subscriber(This,newVal)	\
    (This)->lpVtbl -> put_Subscriber(This,newVal)

#define ISSCEMerge_get_InternetProxyServer(This,pVal)	\
    (This)->lpVtbl -> get_InternetProxyServer(This,pVal)

#define ISSCEMerge_put_InternetProxyServer(This,newVal)	\
    (This)->lpVtbl -> put_InternetProxyServer(This,newVal)

#define ISSCEMerge_get_InternetProxyLogin(This,pVal)	\
    (This)->lpVtbl -> get_InternetProxyLogin(This,pVal)

#define ISSCEMerge_put_InternetProxyLogin(This,newVal)	\
    (This)->lpVtbl -> put_InternetProxyLogin(This,newVal)

#define ISSCEMerge_get_InternetProxyPassword(This,pVal)	\
    (This)->lpVtbl -> get_InternetProxyPassword(This,pVal)

#define ISSCEMerge_put_InternetProxyPassword(This,newVal)	\
    (This)->lpVtbl -> put_InternetProxyPassword(This,newVal)

#define ISSCEMerge_get_ConnectionRetryTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ConnectionRetryTimeout(This,pVal)

#define ISSCEMerge_put_ConnectionRetryTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ConnectionRetryTimeout(This,newVal)

#define ISSCEMerge_get_CompressionLevel(This,pVal)	\
    (This)->lpVtbl -> get_CompressionLevel(This,pVal)

#define ISSCEMerge_put_CompressionLevel(This,newVal)	\
    (This)->lpVtbl -> put_CompressionLevel(This,newVal)

#define ISSCEMerge_get_ConnectionManager(This,pVal)	\
    (This)->lpVtbl -> get_ConnectionManager(This,pVal)

#define ISSCEMerge_put_ConnectionManager(This,newVal)	\
    (This)->lpVtbl -> put_ConnectionManager(This,newVal)

#define ISSCEMerge_get_StatusReportingHandler(This,ppVal)	\
    (This)->lpVtbl -> get_StatusReportingHandler(This,ppVal)

#define ISSCEMerge_put_StatusReportingHandler(This,newVal)	\
    (This)->lpVtbl -> put_StatusReportingHandler(This,newVal)

#define ISSCEMerge_get_SnapshotTransferType(This,pVal)	\
    (This)->lpVtbl -> get_SnapshotTransferType(This,pVal)

#define ISSCEMerge_put_SnapshotTransferType(This,newVal)	\
    (This)->lpVtbl -> put_SnapshotTransferType(This,newVal)

#define ISSCEMerge_Initialize(This)	\
    (This)->lpVtbl -> Initialize(This)

#define ISSCEMerge_AddSubscription(This,DBAddOption)	\
    (This)->lpVtbl -> AddSubscription(This,DBAddOption)

#define ISSCEMerge_DropSubscription(This,DBDropOption)	\
    (This)->lpVtbl -> DropSubscription(This,DBDropOption)

#define ISSCEMerge_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define ISSCEMerge_Terminate(This)	\
    (This)->lpVtbl -> Terminate(This)

#define ISSCEMerge_ReInitializeSubscription(This,UploadBeforeReinit)	\
    (This)->lpVtbl -> ReInitializeSubscription(This,UploadBeforeReinit)

#define ISSCEMerge_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define ISSCEMerge_get_ConnectTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ConnectTimeout(This,pVal)

#define ISSCEMerge_put_ConnectTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ConnectTimeout(This,newVal)

#define ISSCEMerge_get_SendTimeout(This,pVal)	\
    (This)->lpVtbl -> get_SendTimeout(This,pVal)

#define ISSCEMerge_put_SendTimeout(This,newVal)	\
    (This)->lpVtbl -> put_SendTimeout(This,newVal)

#define ISSCEMerge_get_ReceiveTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ReceiveTimeout(This,pVal)

#define ISSCEMerge_put_ReceiveTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ReceiveTimeout(This,newVal)

#define ISSCEMerge_get_DataSendTimeout(This,pVal)	\
    (This)->lpVtbl -> get_DataSendTimeout(This,pVal)

#define ISSCEMerge_put_DataSendTimeout(This,newVal)	\
    (This)->lpVtbl -> put_DataSendTimeout(This,newVal)

#define ISSCEMerge_get_DataReceiveTimeout(This,pVal)	\
    (This)->lpVtbl -> get_DataReceiveTimeout(This,pVal)

#define ISSCEMerge_put_DataReceiveTimeout(This,newVal)	\
    (This)->lpVtbl -> put_DataReceiveTimeout(This,newVal)

#define ISSCEMerge_get_ControlSendTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ControlSendTimeout(This,pVal)

#define ISSCEMerge_put_ControlSendTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ControlSendTimeout(This,newVal)

#define ISSCEMerge_get_ControlReceiveTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ControlReceiveTimeout(This,pVal)

#define ISSCEMerge_put_ControlReceiveTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ControlReceiveTimeout(This,newVal)

#define ISSCEMerge_get_InternetTransportOptions(This,pGetVal)	\
    (This)->lpVtbl -> get_InternetTransportOptions(This,pGetVal)

#define ISSCEMerge_put_InternetTransportOptions(This,pPutVal)	\
    (This)->lpVtbl -> put_InternetTransportOptions(This,pPutVal)

#define ISSCEMerge_ApplyLocalFile(This,pwszFileURL)	\
    (This)->lpVtbl -> ApplyLocalFile(This,pwszFileURL)

#define ISSCEMerge_get_PostSyncCleanup(This,pVal)	\
    (This)->lpVtbl -> get_PostSyncCleanup(This,pVal)

#define ISSCEMerge_put_PostSyncCleanup(This,newVal)	\
    (This)->lpVtbl -> put_PostSyncCleanup(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_Distributor_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_Distributor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_Distributor_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_Distributor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DistributorAddress_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_DistributorAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DistributorAddress_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_DistributorAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DistributorNetwork_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ REPL_NETWORK_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_DistributorNetwork_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DistributorNetwork_Proxy( 
    ISSCEMerge * This,
    /* [in] */ REPL_NETWORK_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_DistributorNetwork_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DistributorSecurityMode_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ REPL_SECURITY_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_DistributorSecurityMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DistributorSecurityMode_Proxy( 
    ISSCEMerge * This,
    /* [in] */ REPL_SECURITY_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_DistributorSecurityMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DistributorLogin_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_DistributorLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DistributorLogin_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_DistributorLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DistributorPassword_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_DistributorPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DistributorPassword_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_DistributorPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ErrorRecords_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ ISSCEErrors **pVal);


void __RPC_STUB ISSCEMerge_get_ErrorRecords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ExchangeType_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ REPL_EXCHANGE_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_ExchangeType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ExchangeType_Proxy( 
    ISSCEMerge * This,
    /* [in] */ REPL_EXCHANGE_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_ExchangeType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetURL_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_InternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetURL_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_InternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetLogin_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_InternetLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetLogin_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_InternetLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetPassword_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_InternetPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetPassword_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_InternetPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_LoginTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB ISSCEMerge_get_LoginTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_LoginTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ short newVal);


void __RPC_STUB ISSCEMerge_put_LoginTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ProfileName_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_ProfileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ProfileName_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_ProfileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_Publisher_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_Publisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_Publisher_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_Publisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherAddress_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PublisherAddress_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_PublisherAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherNetwork_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ REPL_NETWORK_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherNetwork_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PublisherNetwork_Proxy( 
    ISSCEMerge * This,
    /* [in] */ REPL_NETWORK_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_PublisherNetwork_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherDatabase_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PublisherDatabase_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_PublisherDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherSecurityMode_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ REPL_SECURITY_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherSecurityMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PublisherSecurityMode_Proxy( 
    ISSCEMerge * This,
    /* [in] */ REPL_SECURITY_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_PublisherSecurityMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherLogin_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PublisherLogin_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_PublisherLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherPassword_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PublisherPassword_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_PublisherPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_Publication_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_Publication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_Publication_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_Publication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_QueryTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB ISSCEMerge_get_QueryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_QueryTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ short newVal);


void __RPC_STUB ISSCEMerge_put_QueryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_SubscriberConnectionString_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_SubscriberConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_SubscriberConnectionString_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_SubscriberConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherChanges_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PublisherConflicts_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISSCEMerge_get_PublisherConflicts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_SubscriberChanges_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISSCEMerge_get_SubscriberChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_SubscriberConflicts_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISSCEMerge_get_SubscriberConflicts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_Validate_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ REPL_VALIDATE_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_Validate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_Validate_Proxy( 
    ISSCEMerge * This,
    /* [in] */ REPL_VALIDATE_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_Validate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_HostName_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_HostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_HostName_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_HostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_Subscriber_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_Subscriber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_Subscriber_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_Subscriber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetProxyServer_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_InternetProxyServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetProxyServer_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_InternetProxyServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetProxyLogin_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_InternetProxyLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetProxyLogin_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_InternetProxyLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetProxyPassword_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCEMerge_get_InternetProxyPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetProxyPassword_Proxy( 
    ISSCEMerge * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCEMerge_put_InternetProxyPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ConnectionRetryTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB ISSCEMerge_get_ConnectionRetryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ConnectionRetryTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ short newVal);


void __RPC_STUB ISSCEMerge_put_ConnectionRetryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_CompressionLevel_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB ISSCEMerge_get_CompressionLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_CompressionLevel_Proxy( 
    ISSCEMerge * This,
    /* [in] */ short newVal);


void __RPC_STUB ISSCEMerge_put_CompressionLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ConnectionManager_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISSCEMerge_get_ConnectionManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ConnectionManager_Proxy( 
    ISSCEMerge * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISSCEMerge_put_ConnectionManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_StatusReportingHandler_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ ISSCEStatusReporting **ppVal);


void __RPC_STUB ISSCEMerge_get_StatusReportingHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_StatusReportingHandler_Proxy( 
    ISSCEMerge * This,
    /* [in] */ ISSCEStatusReporting *newVal);


void __RPC_STUB ISSCEMerge_put_StatusReportingHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_SnapshotTransferType_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ SNAPSHOT_TRANSFER_TYPE *pVal);


void __RPC_STUB ISSCEMerge_get_SnapshotTransferType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_SnapshotTransferType_Proxy( 
    ISSCEMerge * This,
    /* [in] */ SNAPSHOT_TRANSFER_TYPE newVal);


void __RPC_STUB ISSCEMerge_put_SnapshotTransferType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_Initialize_Proxy( 
    ISSCEMerge * This);


void __RPC_STUB ISSCEMerge_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_AddSubscription_Proxy( 
    ISSCEMerge * This,
    /* [defaultvalue][in] */ REPL_DBADDOPTION DBAddOption);


void __RPC_STUB ISSCEMerge_AddSubscription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_DropSubscription_Proxy( 
    ISSCEMerge * This,
    /* [defaultvalue][in] */ REPL_DBDROPOPTION DBDropOption);


void __RPC_STUB ISSCEMerge_DropSubscription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_Run_Proxy( 
    ISSCEMerge * This);


void __RPC_STUB ISSCEMerge_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_Terminate_Proxy( 
    ISSCEMerge * This);


void __RPC_STUB ISSCEMerge_Terminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_ReInitializeSubscription_Proxy( 
    ISSCEMerge * This,
    /* [defaultvalue][in] */ VARIANT_BOOL UploadBeforeReinit);


void __RPC_STUB ISSCEMerge_ReInitializeSubscription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_Cancel_Proxy( 
    ISSCEMerge * This);


void __RPC_STUB ISSCEMerge_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ConnectTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_ConnectTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ConnectTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_ConnectTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_SendTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_SendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_SendTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_SendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ReceiveTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_ReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ReceiveTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_ReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DataSendTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_DataSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DataSendTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_DataSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_DataReceiveTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_DataReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_DataReceiveTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_DataReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ControlSendTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_ControlSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ControlSendTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_ControlSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_ControlReceiveTimeout_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCEMerge_get_ControlReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_ControlReceiveTimeout_Proxy( 
    ISSCEMerge * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCEMerge_put_ControlReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_InternetTransportOptions_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ INTERNET_TRANSPORT_OPTIONS *pGetVal);


void __RPC_STUB ISSCEMerge_get_InternetTransportOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_InternetTransportOptions_Proxy( 
    ISSCEMerge * This,
    /* [in] */ INTERNET_TRANSPORT_OPTIONS *pPutVal);


void __RPC_STUB ISSCEMerge_put_InternetTransportOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_ApplyLocalFile_Proxy( 
    ISSCEMerge * This,
    /* [in] */ const WCHAR *pwszFileURL);


void __RPC_STUB ISSCEMerge_ApplyLocalFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_get_PostSyncCleanup_Proxy( 
    ISSCEMerge * This,
    /* [retval][out] */ SHORT *pVal);


void __RPC_STUB ISSCEMerge_get_PostSyncCleanup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCEMerge_put_PostSyncCleanup_Proxy( 
    ISSCEMerge * This,
    /* [in] */ SHORT newVal);


void __RPC_STUB ISSCEMerge_put_PostSyncCleanup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCEMerge_INTERFACE_DEFINED__ */


#ifndef __ISSCERDA_INTERFACE_DEFINED__
#define __ISSCERDA_INTERFACE_DEFINED__

/* interface ISSCERDA */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4f04f79d-1ff1-4dcd-802b-3d51b9356c14")
    ISSCERDA : public IUnknown
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_LocalConnectionString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_LocalConnectionString( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetURL( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetURL( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetLogin( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetLogin( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorRecords( 
            /* [retval][out] */ ISSCEErrors **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetProxyServer( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetProxyServer( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetProxyLogin( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetProxyLogin( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetProxyPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetProxyPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionRetryTimeout( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionRetryTimeout( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_CompressionLevel( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_CompressionLevel( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionManager( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionManager( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Pull( 
            /* [in] */ BSTR LocalTableName,
            /* [in] */ BSTR SQLSelectString,
            /* [in] */ BSTR OLEDBConnectionString,
            /* [defaultvalue][in] */ RDA_TRACKOPTION TrackOption = TRACKINGOFF,
            /* [defaultvalue][in] */ BSTR ErrorTableName = L"") = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Push( 
            /* [in] */ BSTR LocalTableName,
            /* [in] */ BSTR OLEDBConnectionString,
            /* [defaultvalue][in] */ RDA_BATCHOPTION BatchOption = BATCHINGOFF) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SubmitSQL( 
            /* [in] */ BSTR SQLString,
            /* [in] */ BSTR OLEDBConnectionString) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SendTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SendTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ReceiveTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ReceiveTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DataSendTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DataSendTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DataReceiveTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DataReceiveTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ControlSendTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ControlSendTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ControlReceiveTimeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ControlReceiveTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_InternetTransportOptions( 
            /* [retval][out] */ INTERNET_TRANSPORT_OPTIONS *pGetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_InternetTransportOptions( 
            /* [in] */ INTERNET_TRANSPORT_OPTIONS *pPutVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCERDAVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCERDA * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCERDA * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCERDA * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LocalConnectionString )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LocalConnectionString )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetURL )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetURL )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetLogin )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetLogin )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetPassword )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetPassword )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ErrorRecords )( 
            ISSCERDA * This,
            /* [retval][out] */ ISSCEErrors **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetProxyServer )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetProxyServer )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetProxyLogin )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetProxyLogin )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetProxyPassword )( 
            ISSCERDA * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetProxyPassword )( 
            ISSCERDA * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectionRetryTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConnectionRetryTimeout )( 
            ISSCERDA * This,
            /* [in] */ short newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CompressionLevel )( 
            ISSCERDA * This,
            /* [retval][out] */ short *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CompressionLevel )( 
            ISSCERDA * This,
            /* [in] */ short newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectionManager )( 
            ISSCERDA * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConnectionManager )( 
            ISSCERDA * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Pull )( 
            ISSCERDA * This,
            /* [in] */ BSTR LocalTableName,
            /* [in] */ BSTR SQLSelectString,
            /* [in] */ BSTR OLEDBConnectionString,
            /* [defaultvalue][in] */ RDA_TRACKOPTION TrackOption,
            /* [defaultvalue][in] */ BSTR ErrorTableName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Push )( 
            ISSCERDA * This,
            /* [in] */ BSTR LocalTableName,
            /* [in] */ BSTR OLEDBConnectionString,
            /* [defaultvalue][in] */ RDA_BATCHOPTION BatchOption);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SubmitSQL )( 
            ISSCERDA * This,
            /* [in] */ BSTR SQLString,
            /* [in] */ BSTR OLEDBConnectionString);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConnectTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SendTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SendTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReceiveTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ReceiveTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataSendTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DataSendTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataReceiveTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DataReceiveTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ControlSendTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ControlSendTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ControlReceiveTimeout )( 
            ISSCERDA * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ControlReceiveTimeout )( 
            ISSCERDA * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InternetTransportOptions )( 
            ISSCERDA * This,
            /* [retval][out] */ INTERNET_TRANSPORT_OPTIONS *pGetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InternetTransportOptions )( 
            ISSCERDA * This,
            /* [in] */ INTERNET_TRANSPORT_OPTIONS *pPutVal);
        
        END_INTERFACE
    } ISSCERDAVtbl;

    interface ISSCERDA
    {
        CONST_VTBL struct ISSCERDAVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCERDA_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCERDA_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCERDA_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCERDA_get_LocalConnectionString(This,pVal)	\
    (This)->lpVtbl -> get_LocalConnectionString(This,pVal)

#define ISSCERDA_put_LocalConnectionString(This,newVal)	\
    (This)->lpVtbl -> put_LocalConnectionString(This,newVal)

#define ISSCERDA_get_InternetURL(This,pVal)	\
    (This)->lpVtbl -> get_InternetURL(This,pVal)

#define ISSCERDA_put_InternetURL(This,newVal)	\
    (This)->lpVtbl -> put_InternetURL(This,newVal)

#define ISSCERDA_get_InternetLogin(This,pVal)	\
    (This)->lpVtbl -> get_InternetLogin(This,pVal)

#define ISSCERDA_put_InternetLogin(This,newVal)	\
    (This)->lpVtbl -> put_InternetLogin(This,newVal)

#define ISSCERDA_get_InternetPassword(This,pVal)	\
    (This)->lpVtbl -> get_InternetPassword(This,pVal)

#define ISSCERDA_put_InternetPassword(This,newVal)	\
    (This)->lpVtbl -> put_InternetPassword(This,newVal)

#define ISSCERDA_get_ErrorRecords(This,pVal)	\
    (This)->lpVtbl -> get_ErrorRecords(This,pVal)

#define ISSCERDA_get_InternetProxyServer(This,pVal)	\
    (This)->lpVtbl -> get_InternetProxyServer(This,pVal)

#define ISSCERDA_put_InternetProxyServer(This,newVal)	\
    (This)->lpVtbl -> put_InternetProxyServer(This,newVal)

#define ISSCERDA_get_InternetProxyLogin(This,pVal)	\
    (This)->lpVtbl -> get_InternetProxyLogin(This,pVal)

#define ISSCERDA_put_InternetProxyLogin(This,newVal)	\
    (This)->lpVtbl -> put_InternetProxyLogin(This,newVal)

#define ISSCERDA_get_InternetProxyPassword(This,pVal)	\
    (This)->lpVtbl -> get_InternetProxyPassword(This,pVal)

#define ISSCERDA_put_InternetProxyPassword(This,newVal)	\
    (This)->lpVtbl -> put_InternetProxyPassword(This,newVal)

#define ISSCERDA_get_ConnectionRetryTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ConnectionRetryTimeout(This,pVal)

#define ISSCERDA_put_ConnectionRetryTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ConnectionRetryTimeout(This,newVal)

#define ISSCERDA_get_CompressionLevel(This,pVal)	\
    (This)->lpVtbl -> get_CompressionLevel(This,pVal)

#define ISSCERDA_put_CompressionLevel(This,newVal)	\
    (This)->lpVtbl -> put_CompressionLevel(This,newVal)

#define ISSCERDA_get_ConnectionManager(This,pVal)	\
    (This)->lpVtbl -> get_ConnectionManager(This,pVal)

#define ISSCERDA_put_ConnectionManager(This,newVal)	\
    (This)->lpVtbl -> put_ConnectionManager(This,newVal)

#define ISSCERDA_Pull(This,LocalTableName,SQLSelectString,OLEDBConnectionString,TrackOption,ErrorTableName)	\
    (This)->lpVtbl -> Pull(This,LocalTableName,SQLSelectString,OLEDBConnectionString,TrackOption,ErrorTableName)

#define ISSCERDA_Push(This,LocalTableName,OLEDBConnectionString,BatchOption)	\
    (This)->lpVtbl -> Push(This,LocalTableName,OLEDBConnectionString,BatchOption)

#define ISSCERDA_SubmitSQL(This,SQLString,OLEDBConnectionString)	\
    (This)->lpVtbl -> SubmitSQL(This,SQLString,OLEDBConnectionString)

#define ISSCERDA_get_ConnectTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ConnectTimeout(This,pVal)

#define ISSCERDA_put_ConnectTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ConnectTimeout(This,newVal)

#define ISSCERDA_get_SendTimeout(This,pVal)	\
    (This)->lpVtbl -> get_SendTimeout(This,pVal)

#define ISSCERDA_put_SendTimeout(This,newVal)	\
    (This)->lpVtbl -> put_SendTimeout(This,newVal)

#define ISSCERDA_get_ReceiveTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ReceiveTimeout(This,pVal)

#define ISSCERDA_put_ReceiveTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ReceiveTimeout(This,newVal)

#define ISSCERDA_get_DataSendTimeout(This,pVal)	\
    (This)->lpVtbl -> get_DataSendTimeout(This,pVal)

#define ISSCERDA_put_DataSendTimeout(This,newVal)	\
    (This)->lpVtbl -> put_DataSendTimeout(This,newVal)

#define ISSCERDA_get_DataReceiveTimeout(This,pVal)	\
    (This)->lpVtbl -> get_DataReceiveTimeout(This,pVal)

#define ISSCERDA_put_DataReceiveTimeout(This,newVal)	\
    (This)->lpVtbl -> put_DataReceiveTimeout(This,newVal)

#define ISSCERDA_get_ControlSendTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ControlSendTimeout(This,pVal)

#define ISSCERDA_put_ControlSendTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ControlSendTimeout(This,newVal)

#define ISSCERDA_get_ControlReceiveTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ControlReceiveTimeout(This,pVal)

#define ISSCERDA_put_ControlReceiveTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ControlReceiveTimeout(This,newVal)

#define ISSCERDA_get_InternetTransportOptions(This,pGetVal)	\
    (This)->lpVtbl -> get_InternetTransportOptions(This,pGetVal)

#define ISSCERDA_put_InternetTransportOptions(This,pPutVal)	\
    (This)->lpVtbl -> put_InternetTransportOptions(This,pPutVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_LocalConnectionString_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_LocalConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_LocalConnectionString_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_LocalConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetURL_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_InternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetURL_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_InternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetLogin_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_InternetLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetLogin_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_InternetLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetPassword_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_InternetPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetPassword_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_InternetPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ErrorRecords_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ ISSCEErrors **pVal);


void __RPC_STUB ISSCERDA_get_ErrorRecords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetProxyServer_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_InternetProxyServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetProxyServer_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_InternetProxyServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetProxyLogin_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_InternetProxyLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetProxyLogin_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_InternetProxyLogin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetProxyPassword_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISSCERDA_get_InternetProxyPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetProxyPassword_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISSCERDA_put_InternetProxyPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ConnectionRetryTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB ISSCERDA_get_ConnectionRetryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_ConnectionRetryTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ short newVal);


void __RPC_STUB ISSCERDA_put_ConnectionRetryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_CompressionLevel_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB ISSCERDA_get_CompressionLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_CompressionLevel_Proxy( 
    ISSCERDA * This,
    /* [in] */ short newVal);


void __RPC_STUB ISSCERDA_put_CompressionLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ConnectionManager_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISSCERDA_get_ConnectionManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_ConnectionManager_Proxy( 
    ISSCERDA * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISSCERDA_put_ConnectionManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCERDA_Pull_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR LocalTableName,
    /* [in] */ BSTR SQLSelectString,
    /* [in] */ BSTR OLEDBConnectionString,
    /* [defaultvalue][in] */ RDA_TRACKOPTION TrackOption,
    /* [defaultvalue][in] */ BSTR ErrorTableName);


void __RPC_STUB ISSCERDA_Pull_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCERDA_Push_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR LocalTableName,
    /* [in] */ BSTR OLEDBConnectionString,
    /* [defaultvalue][in] */ RDA_BATCHOPTION BatchOption);


void __RPC_STUB ISSCERDA_Push_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCERDA_SubmitSQL_Proxy( 
    ISSCERDA * This,
    /* [in] */ BSTR SQLString,
    /* [in] */ BSTR OLEDBConnectionString);


void __RPC_STUB ISSCERDA_SubmitSQL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ConnectTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_ConnectTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_ConnectTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_ConnectTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_SendTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_SendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_SendTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_SendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ReceiveTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_ReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_ReceiveTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_ReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_DataSendTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_DataSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_DataSendTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_DataSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_DataReceiveTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_DataReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_DataReceiveTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_DataReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ControlSendTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_ControlSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_ControlSendTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_ControlSendTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_ControlReceiveTimeout_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISSCERDA_get_ControlReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_ControlReceiveTimeout_Proxy( 
    ISSCERDA * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISSCERDA_put_ControlReceiveTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCERDA_get_InternetTransportOptions_Proxy( 
    ISSCERDA * This,
    /* [retval][out] */ INTERNET_TRANSPORT_OPTIONS *pGetVal);


void __RPC_STUB ISSCERDA_get_InternetTransportOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISSCERDA_put_InternetTransportOptions_Proxy( 
    ISSCERDA * This,
    /* [in] */ INTERNET_TRANSPORT_OPTIONS *pPutVal);


void __RPC_STUB ISSCERDA_put_InternetTransportOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISSCERDA_INTERFACE_DEFINED__ */


#ifndef __ISSCEEngine_INTERFACE_DEFINED__
#define __ISSCEEngine_INTERFACE_DEFINED__

/* interface ISSCEEngine */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 




#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("10ec3e45-0870-4d7b-9a2d-f4f81b6b7fa2")
    ISSCEEngine : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CompactDatabase( 
            /* [in] */ BSTR SourceConnection,
            /* [in] */ BSTR DestinationConnection) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorRecords( 
            /* [retval][out] */ ISSCEErrors **pVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE CreateDatabase( 
            /* [in] */ BSTR LocalConnection) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Repair( 
            /* [in] */ BSTR SourceConnection,
            /* [in] */ BSTR DestinationConnection,
            /* [in] */ REPAIROPTION RepairOption) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpgradeDatabase( 
            /* [in] */ BSTR SourceConnection,
            /* [in] */ BSTR DestinationConnection) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Verify( 
            /* [in] */ BSTR SourceConnection) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE VerifyEx( 
            /* [in] */ BSTR SourceConnection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISSCEEngineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISSCEEngine * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISSCEEngine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISSCEEngine * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CompactDatabase )( 
            ISSCEEngine * This,
            /* [in] */ BSTR SourceConnection,
            /* [in] */ BSTR DestinationConnection);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ErrorRecords )( 
            ISSCEEngine * This,
            /* [retval][out] */ ISSCEErrors **pVal);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *CreateDatabase )( 
            ISSCEEngine * This,
            /* [in] */ BSTR LocalConnection);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Repair )( 
            ISSCEEngine * This,
            /* [in] */ BSTR SourceConnection,
            /* [in] */ BSTR DestinationConnection,
            /* [in] */ REPAIROPTION RepairOption);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpgradeDatabase )( 
            ISSCEEngine * This,
            /* [in] */ BSTR SourceConnection,
            /* [in] */ BSTR DestinationConnection);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Verify )( 
            ISSCEEngine * This,
            /* [in] */ BSTR SourceConnection);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *VerifyEx )( 
            ISSCEEngine * This,
            /* [in] */ BSTR SourceConnection);
        
        END_INTERFACE
    } ISSCEEngineVtbl;

    interface ISSCEEngine
    {
        CONST_VTBL struct ISSCEEngineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISSCEEngine_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISSCEEngine_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISSCEEngine_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISSCEEngine_CompactDatabase(This,SourceConnection,DestinationConnection)	\
    (This)->lpVtbl -> CompactDatabase(This,SourceConnection,DestinationConnection)

#define ISSCEEngine_get_ErrorRecords(This,pVal)	\
    (This)->lpVtbl -> get_ErrorRecords(This,pVal)

#define ISSCEEngine_CreateDatabase(This,LocalConnection)	\
    (This)->lpVtbl -> CreateDatabase(This,LocalConnection)

#define ISSCEEngine_Repair(This,SourceConnection,DestinationConnection,RepairOption)	\
    (This)->lpVtbl -> Repair(This,SourceConnection,DestinationConnection,RepairOption)

#define ISSCEEngine_UpgradeDatabase(This,SourceConnection,DestinationConnection)	\
    (This)->lpVtbl -> UpgradeDatabase(This,SourceConnection,DestinationConnection)

#define ISSCEEngine_Verify(This,SourceConnection)	\
    (This)->lpVtbl -> Verify(This,SourceConnection)

#define ISSCEEngine_VerifyEx(This,SourceConnection)	\
    (This)->lpVtbl -> VerifyEx(This,SourceConnection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_CompactDatabase_Proxy( 
    ISSCEEngine * This,
    /* [in] */ BSTR SourceConnection,
    /* [in] */ BSTR DestinationConnection);


void __RPC_STUB ISSCEEngine_CompactDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_get_ErrorRecords_Proxy( 
    ISSCEEngine * This,
    /* [retval][out] */ ISSCEErrors **pVal);


void __RPC_STUB ISSCEEngine_get_ErrorRecords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_CreateDatabase_Proxy( 
    ISSCEEngine * This,
    /* [in] */ BSTR LocalConnection);


void __RPC_STUB ISSCEEngine_CreateDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_Repair_Proxy( 
    ISSCEEngine * This,
    /* [in] */ BSTR SourceConnection,
    /* [in] */ BSTR DestinationConnection,
    /* [in] */ REPAIROPTION RepairOption);


void __RPC_STUB ISSCEEngine_Repair_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_UpgradeDatabase_Proxy( 
    ISSCEEngine * This,
    /* [in] */ BSTR SourceConnection,
    /* [in] */ BSTR DestinationConnection);

void __RPC_STUB ISSCEEngine_UpgradeDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_Verify_Proxy( 
    ISSCEEngine * This,
    /* [in] */ BSTR SourceConnection);

void __RPC_STUB ISSCEEngine_Verify_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISSCEEngine_VerifyEx_Proxy( 
    ISSCEEngine * This,
    /* [in] */ BSTR SourceConnection);


void __RPC_STUB ISSCEEngine_VerifyEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

#endif 	/* __ISSCEEngine_INTERFACE_DEFINED__ */



#ifndef __SSCE_LIBRARY_DEFINED__
#define __SSCE_LIBRARY_DEFINED__

/* library SSCE */
/* [helpstring][version][uuid] */ 






#ifdef __cplusplus

class DECLSPEC_UUID("11D5B2D4-26A4-44F5-A48B-0FAC3A919ED8")
Replication;
#endif



#ifdef __cplusplus

class DECLSPEC_UUID("58BC9AD6-BF11-40B3-9AB1-E3F2ED784C08")
RemoteDataAccess;
#endif



#ifdef __cplusplus

class DECLSPEC_UUID("68D45319-3702-4837-9F8E-DA6845D82482")
Engine;
#endif



#ifdef __cplusplus

class DECLSPEC_UUID("92C2E038-C009-4338-9A4D-0011A5801654")
SSCEErrors;
#endif



#ifdef __cplusplus

class DECLSPEC_UUID("36228F21-B5C7-4054-8DC2-47D3E236E8B5")
SSCEError;
#endif



#ifdef __cplusplus

class DECLSPEC_UUID("CAFC260B-B433-47CE-89A2-259BB0805652")
SSCEParams;
#endif



#ifdef __cplusplus

class DECLSPEC_UUID("0B3A7B75-A9B0-4580-9AA5-1A7DA47AD1CB")
SSCEParam;
#endif
#endif /* __SSCE_LIBRARY_DEFINED__ */

#ifndef SSCECA_DEFINE_GUID

#define SSCECA_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // of !SSCECA_DEFINE_GUID

SSCECA_DEFINE_GUID(IID, IID_ISSCEParam,0xa9876c60,0x2667,0x44e5,0x89,0xdb,0xe9,0xa4,0x6e,0xd3,0x92,0xc0);


SSCECA_DEFINE_GUID(IID, IID_ISSCEParams,0xa78aff90,0x049c,0x41ec,0xb1,0xd8,0x66,0x59,0x68,0xaa,0xc4,0xa6);


SSCECA_DEFINE_GUID(IID, IID_ISSCEError,0x31155a3b,0x871d,0x407f,0x9f,0x73,0xde,0xbf,0xac,0x7e,0xfb,0xe3);


SSCECA_DEFINE_GUID(IID, IID_ISSCEStatusReporting,0x9be851c8,0xd1ab,0x4091,0xa5,0xe2,0xc2,0x00,0xc0,0x2f,0x0e,0x8d);


SSCECA_DEFINE_GUID(IID, IID_ISSCEErrors,0xc40143ca,0xe9f9,0x4ff4,0xb8,0xb4,0xcc,0x02,0xc0,0x64,0xfc,0x1b);


SSCECA_DEFINE_GUID(IID, IID_ISSCEMerge,0xc6eb397f,0xd585,0x428d,0xa4,0xf4,0x45,0x4a,0x18,0x42,0xcb,0x47);


SSCECA_DEFINE_GUID(IID, IID_ISSCERDA,0x4f04f79d,0x1ff1,0x4dcd,0x80,0x2b,0x3d,0x51,0xb9,0x35,0x6c,0x14);


SSCECA_DEFINE_GUID(IID, IID_ISSCEEngine,0x10ec3e45,0x0870,0x4d7b,0x9a,0x2d,0xf4,0xf8,0x1b,0x6b,0x7f,0xa2);


SSCECA_DEFINE_GUID(IID, LIBID_SSCE,0xce4aacfa,0x3cfd,0x4028,0xb2,0xd9,0xf2,0x72,0x31,0x4f,0x07,0xc8);


SSCECA_DEFINE_GUID(CLSID, CLSID_Replication,0x11D5B2D4,0x26A4,0x44F5,0xA4,0x8B,0x0F,0xAC,0x3A,0x91,0x9E,0xD8);


SSCECA_DEFINE_GUID(CLSID, CLSID_RemoteDataAccess,0x58BC9AD6,0xBF11,0x40B3,0x9A,0xB1,0xE3,0xF2,0xED,0x78,0x4C,0x08);


SSCECA_DEFINE_GUID(CLSID, CLSID_Engine,0x68D45319,0x3702,0x4837,0x9F,0x8E,0xDA,0x68,0x45,0xD8,0x24,0x82);


SSCECA_DEFINE_GUID(CLSID, CLSID_SSCEErrors,0x92C2E038,0xC009,0x4338,0x9A,0x4D,0x00,0x11,0xA5,0x80,0x16,0x54);


SSCECA_DEFINE_GUID(CLSID, CLSID_SSCEError,0x36228F21,0xB5C7,0x4054,0x8D,0xC2,0x47,0xD3,0xE2,0x36,0xE8,0xB5);


SSCECA_DEFINE_GUID(CLSID, CLSID_SSCEParams,0xCAFC260B,0xB433,0x47CE,0x89,0xA2,0x25,0x9B,0xB0,0x80,0x56,0x52);


SSCECA_DEFINE_GUID(CLSID, CLSID_SSCEParam,0x0B3A7B75,0xA9B0,0x4580,0x9A,0xA5,0x1A,0x7D,0xA4,0x7A,0xD1,0xCB);


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* of !__SQLCE_SYNC_H__ */






