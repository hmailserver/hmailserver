// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../common/persistence/PersistentMessage.h"
#include "../common/BO/messageData.h"

namespace HM 
{ 
   class Message; 
   class Messages; 
}

#include "COMCollection.h"

class ATL_NO_VTABLE InterfaceMessage : 
   public COMCollectionItem<HM::Message, HM::Messages>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceMessage, &CLSID_Message>,
	public IDispatchImpl<IInterfaceMessage, &IID_IInterfaceMessage, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:
   InterfaceMessage();


DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEMESSAGE)


BEGIN_COM_MAP(InterfaceMessage)
	COM_INTERFACE_ENTRY(IInterfaceMessage)
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

public:
   STDMETHOD(get_State)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_DeliveryAttempt)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_FromAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_FromAddress)(/*[in]*/ BSTR newVal);
   
   STDMETHOD(AddRecipient)(BSTR Name, BSTR sAddress);
   STDMETHOD(ClearRecipients)();
   
   STDMETHOD(get_To)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_CC)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(Save)();
   STDMETHOD(RefreshContent)();
   STDMETHOD(Copy)(long lDestinationFolderID);
   STDMETHOD(get_Attachments)(/*[out, retval]*/ IInterfaceAttachments* *pVal);
   STDMETHOD(get_HTMLBody)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_HTMLBody)(/*[out, retval]*/ BSTR newVal);
   STDMETHOD(get_Body)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Body)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Date)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Date)(/*[out, retval]*/ BSTR newVal);
   STDMETHOD(get_From)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_From)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Subject)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Subject)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Charset)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Charset)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_Filename)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_ID)(/*[out, retval]*/ hyper *pVal);
   STDMETHOD(get_UID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_Size)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_Recipients)(/*[out, retval]*/ IInterfaceRecipients* *pVal);
   STDMETHOD(get_HeaderValue)(BSTR FieldName, BSTR *pVal);
   STDMETHOD(put_HeaderValue)(BSTR FieldName, BSTR FieldValue);
   STDMETHOD(HasBodyType)(/*[in]*/ BSTR BodyType, /*[out, retval]*/ VARIANT_BOOL *pVal);

   STDMETHOD(get_EncodeFields)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_EncodeFields)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_Flag)(eMessageFlag iFlag, VARIANT_BOOL *pVal);
   STDMETHOD(put_Flag)(eMessageFlag iFlag, VARIANT_BOOL newVal);

   STDMETHOD(get_InternalDate)(/*[out, retval]*/ VARIANT *pVal);
   STDMETHOD(get_Headers)(/*[out, retval]*/ IInterfaceMessageHeaders* *pVal);
   

 private:

   STDMETHODIMP SaveNewMessageToIMAPFolder_();
   
   
   std::shared_ptr<HM::MessageData> GetMessageData_();

   std::shared_ptr<HM::MessageData> msg_data_;
};

OBJECT_ENTRY_AUTO(__uuidof(Message), InterfaceMessage)
