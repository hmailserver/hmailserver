// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


// InterfaceCache

class ATL_NO_VTABLE InterfaceCache : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceCache, &CLSID_Cache>,
	public IDispatchImpl<IInterfaceCache, &IID_IInterfaceCache, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
   InterfaceCache();
	
   bool LoadSettings();
DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACECACHE)


BEGIN_COM_MAP(InterfaceCache)
	COM_INTERFACE_ENTRY(IInterfaceCache)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:
   STDMETHOD(get_Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Enabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_DomainCacheTTL)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_DomainCacheTTL)(/*[in]*/ long newVal);
   STDMETHOD(get_DomainHitRate)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_DomainCacheSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_DomainCacheMaxSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_DomainCacheMaxSizeKb)(/*[out, retval]*/ long pVal);

   STDMETHOD(get_AccountCacheTTL)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_AccountCacheTTL)(/*[in]*/ long newVal);
   STDMETHOD(get_AccountHitRate)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_AccountCacheSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_AccountCacheMaxSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_AccountCacheMaxSizeKb)(/*[out, retval]*/ long pVal);

   STDMETHOD(get_AliasCacheTTL)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_AliasCacheTTL)(/*[in]*/ long newVal);
   STDMETHOD(get_AliasHitRate)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_AliasCacheSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_AliasCacheMaxSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_AliasCacheMaxSizeKb)(/*[out, retval]*/ long pVal);

   STDMETHOD(get_DistributionListCacheTTL)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_DistributionListCacheTTL)(/*[in]*/ long newVal);
   STDMETHOD(get_DistributionListHitRate)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_DistributionListCacheSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_DistributionListCacheMaxSizeKb)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_DistributionListCacheMaxSizeKb)(/*[out, retval]*/ long pVal);

   STDMETHOD(Clear)();

private:

   HM::Configuration *config_;
   std::shared_ptr<HM::CacheConfiguration> cache_config_;
};

OBJECT_ENTRY_AUTO(__uuidof(Cache), InterfaceCache)
