// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


// InterfaceUtilities

class ATL_NO_VTABLE InterfaceUtilities : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceUtilities, &CLSID_Utilities>,
	public IDispatchImpl<IInterfaceUtilities, &IID_IInterfaceUtilities, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo

{
public:
	InterfaceUtilities()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEUTILITIES)


BEGIN_COM_MAP(InterfaceUtilities)
	COM_INTERFACE_ENTRY(IInterfaceUtilities)
	COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
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
   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(IsValidDomainName)(BSTR sDomainName, VARIANT_BOOL *bIsValid);
   STDMETHOD(GetMailServer)(/*[in]*/ BSTR EMailAddress, /*[out, retval]*/ BSTR *MailServer);
   STDMETHOD(MD5)(BSTR Input, BSTR *Output);
   STDMETHOD(SHA256)(BSTR Input, BSTR *Output);
   STDMETHOD(IsValidEmailAddress)(/*[in]*/ BSTR EMailAddress, /*[out, retval]*/ VARIANT_BOOL *bIsValid);
   STDMETHOD(BlowfishEncrypt)(BSTR Input, BSTR *Output);
   STDMETHOD(BlowfishDecrypt)(BSTR Input, BSTR *Output);
   STDMETHOD(GenerateGUID)(BSTR *Output);
   STDMETHOD(MakeDependent)(BSTR sOtherService);
   STDMETHOD(ImportMessageFromFile)(/*[in]*/ BSTR sFilename, long lAccountID, /*[out, retval]*/VARIANT_BOOL *bIsSuccessful);
   STDMETHOD(RetrieveMessageID)(/*[in]*/ BSTR sFilename, /*[out, retval]*/hyper *messageID);
   STDMETHOD(EmailAllAccounts)(BSTR sRecipientWildcard, BSTR sFromAddress, BSTR sFromName, BSTR sSubject, BSTR sBody, VARIANT_BOOL *bIsSuccessful);
   STDMETHOD(RunTestSuite)(BSTR sTestPassword);
   STDMETHOD(IsLocalHost)(BSTR sDomainName, VARIANT_BOOL *bIsValid);
   STDMETHOD(ImportMessageFromFileToIMAPFolder)(/*[in]*/ BSTR sFilename, long lAccountID, BSTR sIMAPFolder, /*[out, retval]*/VARIANT_BOOL *bIsSuccessful);
   STDMETHOD(IsStrongPassword)(BSTR Username, BSTR Password, VARIANT_BOOL *bIsValid);
   STDMETHOD(CriteriaMatch)(BSTR MatchValue, eRuleMatchType, BSTR TestValue,  /*[out, retval]*/ VARIANT_BOOL *bIsValid);
   STDMETHOD(IsValidIPAddress)(/*[in]*/ BSTR IPAddress, /*[out, retval]*/ VARIANT_BOOL *bIsValid);
   STDMETHOD(PerformMaintenance)(/*[in]*/ eMaintenanceOperation operation);
};

OBJECT_ENTRY_AUTO(__uuidof(Utilities), InterfaceUtilities)
