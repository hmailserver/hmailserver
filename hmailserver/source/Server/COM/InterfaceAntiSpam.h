// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "..\hMailServer\resource.h"       // main symbols

#include "..\hMailServer\hMailServer.h"


// InterfaceAntiSpam

class ATL_NO_VTABLE InterfaceAntiSpam :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceAntiSpam, &CLSID_AntiSpam>,
	public IDispatchImpl<IInterfaceAntiSpam, &IID_IInterfaceAntiSpam, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
   InterfaceAntiSpam();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEANTISPAM)


BEGIN_COM_MAP(InterfaceAntiSpam)
	COM_INTERFACE_ENTRY(IInterfaceAntiSpam)
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

   bool LoadSettings();

   STDMETHOD(get_SURBLServers)(IInterfaceSURBLServers **pVal);

   STDMETHOD(get_GreyListingEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_GreyListingEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_GreyListingInitialDelay)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_GreyListingInitialDelay)(/*[in]*/ long newVal);

   STDMETHOD(get_GreyListingInitialDelete)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_GreyListingInitialDelete)(/*[in]*/ long newVal);

   STDMETHOD(get_GreyListingFinalDelete)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_GreyListingFinalDelete)(/*[in]*/ long newVal);

   STDMETHOD(get_CheckHostInHelo)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_CheckHostInHelo)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_CheckHostInHeloScore)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_CheckHostInHeloScore)(/*[in]*/ long newVal);	

   STDMETHOD(get_CheckPTR)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_CheckPTR)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_CheckPTRScore)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_CheckPTRScore)(/*[in]*/ long newVal);

   STDMETHOD(get_SpamMarkThreshold)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SpamMarkThreshold)(/*[in]*/ long newVal);	

   STDMETHOD(get_SpamDeleteThreshold)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SpamDeleteThreshold)(/*[in]*/ long newVal);	


   STDMETHOD(get_AddHeaderSpam)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AddHeaderSpam)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_AddHeaderReason)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AddHeaderReason)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_PrependSubject)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_PrependSubject)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_PrependSubjectText)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_PrependSubjectText)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_GreyListingWhiteAddresses)(/*[out, retval]*/ IInterfaceGreyListingWhiteAddresses **pVal);
   STDMETHOD(get_WhiteListAddresses)(/*[out, retval]*/ IInterfaceWhiteListAddresses **pVal);

   STDMETHOD(get_UseSPF)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseSPF)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_UseSPFScore)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_UseSPFScore)(/*[in]*/ long newVal);	

   STDMETHOD(get_UseMXChecks)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseMXChecks)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_UseMXChecksScore)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_UseMXChecksScore)(/*[in]*/ long newVal);	

   STDMETHOD(get_DNSBlackLists)(IInterfaceDNSBlackLists **pVal);

   STDMETHOD(get_TarpitDelay)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_TarpitDelay)(/*[in]*/ long newVal);
   STDMETHOD(get_TarpitCount)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_TarpitCount)(/*[in]*/ long newVal);

   STDMETHOD(get_SpamAssassinEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SpamAssassinEnabled)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_SpamAssassinScore)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SpamAssassinScore)(/*[in]*/ long newVal);	

   STDMETHOD(get_SpamAssassinMergeScore)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SpamAssassinMergeScore)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_SpamAssassinHost)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SpamAssassinHost)(/*[in]*/ BSTR newVal);	

   STDMETHOD(get_SpamAssassinPort)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SpamAssassinPort)(/*[in]*/ long newVal);	

   STDMETHOD(get_MaximumMessageSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaximumMessageSize)(/*[in]*/ long newVal);	

   STDMETHOD(ClearGreyListingTriplets)();

   STDMETHOD(DKIMVerify)(BSTR File, eDKIMResult *pVal);
   STDMETHOD(TestSpamAssassinConnection)(BSTR hostname, long port, BSTR *messageText, VARIANT_BOOL *pResult);

   STDMETHOD(get_DKIMVerificationEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_DKIMVerificationEnabled)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_DKIMVerificationFailureScore)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_DKIMVerificationFailureScore)(/*[in]*/ long newVal);	

   STDMETHOD(get_BypassGreylistingOnSPFSuccess)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_BypassGreylistingOnSPFSuccess)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_BypassGreylistingOnMailFromMX)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_BypassGreylistingOnMailFromMX)(/*[in]*/ VARIANT_BOOL newVal);	

private:

   HM::Configuration* config_;
};

OBJECT_ENTRY_AUTO(__uuidof(AntiSpam), InterfaceAntiSpam)
