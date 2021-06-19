// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"



namespace HM 
{
   class Domain; 
   class Domains;
   class COMAuthentication;
}

class ATL_NO_VTABLE InterfaceDomain : 
   public COMCollectionItem<HM::Domain, HM::Domains>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDomain, &CLSID_Domain>,
	public IDispatchImpl<IInterfaceDomain, &IID_IInterfaceDomain, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:
   
   InterfaceDomain()
   {
#ifdef _DEBUG
      InterlockedIncrement(&counter);
#endif
   }

   ~InterfaceDomain()
   {
#ifdef _DEBUG
      InterlockedDecrement(&counter);
#endif
   }

   void SetAuthentication(std::shared_ptr<HM::COMAuthentication> pAuthentication);

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDOMAIN)


BEGIN_COM_MAP(InterfaceDomain)
	COM_INTERFACE_ENTRY(IInterfaceDomain)
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

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_Aliases)(/*[out, retval]*/ IInterfaceAliases * *pVal);
   STDMETHOD(get_DomainAliases)(/*[out, retval]*/ IInterfaceDomainAliases * *pVal);
   

   STDMETHOD(Delete)();
   STDMETHOD(SynchronizeDirectory)();
   STDMETHOD(get_Accounts)(/*[out, retval]*/ IInterfaceAccounts **pVal);
   STDMETHOD(get_Active)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Active)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(Save)();
   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Postmaster)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Postmaster)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_ADDomainName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ADDomainName)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_MaxMessageSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxMessageSize)(/*[in]*/ long newVal);

   STDMETHOD(get_PlusAddressingEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_PlusAddressingEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_PlusAddressingCharacter)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_PlusAddressingCharacter)(/*[in]*/ BSTR newVal);


   STDMETHOD(get_DistributionLists)(/*[out, retval]*/ IInterfaceDistributionLists **pVal);

   STDMETHOD(get_AntiSpamEnableGreylisting)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AntiSpamEnableGreylisting)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MaxSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxSize)(/*[in]*/ long newVal);
   STDMETHOD(get_MaxAccountSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxAccountSize)(/*[in]*/ long newVal);

   STDMETHOD(get_Size)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_AllocatedSize)(/*[out, retval]*/ hyper  *pVal);

   STDMETHOD(get_SignatureEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SignatureEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_SignatureMethod)(/*[out, retval]*/ eDomainSignatureMethod *pVal);
   STDMETHOD(put_SignatureMethod)(/*[in]*/ eDomainSignatureMethod newVal);
   STDMETHOD(get_SignaturePlainText)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SignaturePlainText)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_SignatureHTML)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SignatureHTML)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_AddSignaturesToReplies)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AddSignaturesToReplies)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_AddSignaturesToLocalMail)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AddSignaturesToLocalMail)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MaxNumberOfAccounts)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxNumberOfAccounts)(/*[in]*/ long newVal);
   STDMETHOD(get_MaxNumberOfAliases)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxNumberOfAliases)(/*[in]*/ long newVal);
   STDMETHOD(get_MaxNumberOfDistributionLists)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxNumberOfDistributionLists)(/*[in]*/ long newVal);

   STDMETHOD(get_MaxNumberOfAccountsEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_MaxNumberOfAccountsEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_MaxNumberOfAliasesEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_MaxNumberOfAliasesEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_MaxNumberOfDistributionListsEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_MaxNumberOfDistributionListsEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   // dkim begin
   STDMETHOD(get_DKIMSignEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_DKIMSignEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_DKIMSelector)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DKIMSelector)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_DKIMPrivateKeyFile)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DKIMPrivateKeyFile)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_DKIMHeaderCanonicalizationMethod)(/*[out, retval]*/ eDKIMCanonicalizationMethod *pVal);
   STDMETHOD(put_DKIMHeaderCanonicalizationMethod)(/*[in]*/ eDKIMCanonicalizationMethod newVal);
   STDMETHOD(get_DKIMBodyCanonicalizationMethod)(/*[out, retval]*/ eDKIMCanonicalizationMethod *pVal);
   STDMETHOD(put_DKIMBodyCanonicalizationMethod)(/*[in]*/ eDKIMCanonicalizationMethod newVal);
   STDMETHOD(get_DKIMSigningAlgorithm)(/*[out, retval]*/ eDKIMAlgorithm *pVal);
   STDMETHOD(put_DKIMSigningAlgorithm)(/*[in]*/ eDKIMAlgorithm newVal);
   STDMETHOD(get_DKIMSignAliasesEnabled)(/*[out, retval]*/ VARIANT_BOOL* pVal);
   STDMETHOD(put_DKIMSignAliasesEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   // dkim end.
private:
   
   std::shared_ptr<HM::COMAuthentication> authentication_;

#ifdef _DEBUG
   static long counter;
#endif

};

OBJECT_ENTRY_AUTO(__uuidof(Domain), InterfaceDomain)
