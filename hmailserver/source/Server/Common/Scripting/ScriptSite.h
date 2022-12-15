/////////////////////////////////////////////////////////////////////////////
//
// ATL Active Script Host Wrapper
// (C) Copyright 2001 VisionTech Limited. All rights reserved.
// http://www.visiontech.ltd.uk/
// bateman@acm.org
//
// VisionTech Limited makes no warranties, either express or implied,
// with respect to this source code and any accompanying materials.
//
// In no event shall VisionTech Limited or its suppliers be liable for
// any damages whatsoever (including, without limitation, damages for
// loss of business profits, business interruption, loss of business
// information, or other percuniary loss) arising out of the use or
// inability to use this software.
//
// This source code may be used for any purpose, including commercial
// applications, and may be modified or redistributed subject to the
// following conditions:
//
// a) This notice may not be removed or changed in any source distribution.
//
// b) Altered source versions must be include a note to that effect,
//    and must not be misrepresented as the original.
//
// c) The origin of this software may not be misrepresented - you may
//    not claim to have written the original version. If you use this
//    source in a product, an acknowledgement in the documentation
//    would be appreciated, but is not required.
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <activscp.h>
#include "ScriptObjectContainer.h"



// If you want a different application title, declare SCRIPTSITE_APPNAME in
// you code about the reference to ScriptSite.h
#ifndef SCRIPTSITE_APPNAME
#define SCRIPTSITE_APPNAME  _T("ScriptSite")
#endif

#ifndef HR
#define HR(_ex) { HRESULT _hr = _ex; if(FAILED(_hr)) return _hr; }
#endif

/////////////////////////////////////////////////////////////////////////////
// IActiveScriptSite

class ATL_NO_VTABLE IActiveScriptSiteImpl : public IActiveScriptSite
{
private:

public:
   STDMETHOD(GetLCID)(LCID *plcid)
   {
      *plcid = LOCALE_SYSTEM_DEFAULT;
      return NOERROR;
   };

   STDMETHOD(GetItemInfo)(LPCOLESTR pstrName,DWORD dwMask,LPUNKNOWN* ppunkItem,LPTYPEINFO* ppTypeInfo)
   {
      CComPtr<IUnknown> spUnk;
      HR(LookupNamedItem(pstrName,&spUnk));

      if(dwMask & SCRIPTINFO_ITYPEINFO) {
         CComPtr<ITypeInfo> spTI;
         CComQIPtr<IProvideClassInfo> spPCI = spUnk;
         if(!!spPCI) {
            // Got IProvideClassInfo interface so use it
            HR(spPCI->GetClassInfo(&spTI));
            spPCI.Release();
         } else {
            // Try for IDispatch::GetTypeInfo
            CComQIPtr<IDispatch> spDisp = spUnk;
            if(!!spDisp) {
               HR(spDisp->GetTypeInfo(0,LOCALE_SYSTEM_DEFAULT,&spTI));
               spDisp.Release();
            }
         }
         *ppTypeInfo = spTI.Detach();

      }

      if(dwMask & SCRIPTINFO_IUNKNOWN) {
         *ppunkItem = spUnk.Detach();
      }

      return NOERROR;
   }

   STDMETHOD(GetDocVersionString)(BSTR *pbstrversionString)
   {
      if(pbstrversionString==NULL)
         return E_POINTER;
      USES_CONVERSION;
      *pbstrversionString = ::SysAllocString(T2OLE(SCRIPTSITE_APPNAME));
      return NOERROR;
   };

   // This method is called when the script engine terminates
   STDMETHOD(OnScriptTerminate)(const VARIANT* pvarResult,const EXCEPINFO* /*pexcepinfo*/)
   {
      return NOERROR;
   };

   // This method is called when the script engine's state is changed
   STDMETHOD(OnStateChange)(SCRIPTSTATE /*ssScriptState*/)
   {
      return NOERROR;
   };

   // This method is called when the script engine wants to report an error to the user
   STDMETHOD(OnScriptError)(IActiveScriptError* pase)
   {
      if(pase==NULL)
         return E_POINTER;
      EXCEPINFO ei;
      HR(pase->GetExceptionInfo(&ei));
      if(ei.pfnDeferredFillIn!=NULL) {
         HR((*ei.pfnDeferredFillIn)(&ei));
      }
      DWORD dwContext;
      ULONG ulLine;
      LONG lPos;
      HR(pase->GetSourcePosition(&dwContext,&ulLine,&lPos));
      CComBSTR src;
      pase->GetSourceLineText(&src);
      return HandleScriptError(&ei,ulLine,lPos,src);
   }

   // This method is called when (before) the script engine starts executing the script/event handler
   STDMETHOD(OnEnterScript)(void)
   {
      return NOERROR;
   };

   // This method is called when (after) the script engine finishes executing the script/event handler
   STDMETHOD(OnLeaveScript)(void)
   {
      return NOERROR;
   };

   // This is an implementation method.
   // Override this method in your implementation and return the desired object
   // or TYPE_E_ELEMENTNOTFOUND if the name doesn't match one of yours
   // (You must call CScriptSiteImpl::AddObject in the first place to tell
   // the script engine that your objects exist).
   STDMETHOD(LookupNamedItem)(LPCOLESTR pstrName,LPUNKNOWN* ppunkItem)
   {
      object_container_->GetObjectByName(pstrName, ppunkItem);

      if (ppunkItem == 0)
         return TYPE_E_ELEMENTNOTFOUND;
      else
         return S_OK;
   }

   // This is an implementation method.
   // Override this method in your implementation to handle error messages
   STDMETHOD(HandleScriptError)(EXCEPINFO* pei,ULONG ulLine,LONG lPos,BSTR src)
   {
      HM::String sMsg;
      sMsg.Format(_T("Script Error: Source: %ws - Error: %08X - Description: %ws - Line: %d Column: %d - Code: %ws"),
                  pei->bstrSource,pei->scode,pei->bstrDescription,ulLine+1,lPos,src);
      
      HM::Logger::Instance()->LogError(sMsg);

      last_error_message_ = sMsg;

      return NOERROR;
   }

   HM::String GetLastError()
   {
      return last_error_message_;
   }

   protected:
      std::shared_ptr<HM::ScriptObjectContainer> object_container_;

      HM::String last_error_message_;
};

/////////////////////////////////////////////////////////////////////////////
// IActiveScriptSiteWindow

class ATL_NO_VTABLE IActiveScriptSiteWindowImpl : public IActiveScriptSiteWindow
{
public:
   // The script engine uses the window which this method returns as a
   // parent window when the engine needs to show a window (e.g. MsgBox)
   STDMETHOD(GetWindow)(HWND *phWnd)
   {
      if(phWnd==NULL)
         return E_POINTER;
      *phWnd = ::GetDesktopWindow();
      return NOERROR;
   };

   STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
   {
      return NOERROR;
   };
};

/////////////////////////////////////////////////////////////////////////////
// CScriptSiteImpl

class ATL_NO_VTABLE CScriptSiteImpl : public IActiveScriptSiteImpl, public IActiveScriptSiteWindowImpl
{
public:
   CScriptSiteImpl()
   {
      wnd_ = NULL;
      init_ = false;
   }

   STDMETHOD(Initiate)(LPCTSTR pszLanguage,HWND hWnd)
   {
      if(!!engine_)
         HR(Terminate());
      wnd_ = hWnd;

      // Create new script engine
      USES_CONVERSION;
      HR(engine_.CoCreateInstance(T2COLE(pszLanguage)));

      //if (lstrcmp(pszLanguage, _T("JScript")) == 0)
      //{
      //   // Set the JScript version to use - see https://docs.microsoft.com/en-us/scripting/winscript/reference/iactivescriptproperty-setproperty
      //   CComPtr<IActiveScriptProperty> pScriptProperty;
      //   HR(engine_->QueryInterface(IID_IActiveScriptProperty, reinterpret_cast<void**>(&pScriptProperty)));
      //   VARIANT engineVersion;
      //   engineVersion.vt = VT_I4;
      //   engineVersion.llVal = SCRIPTLANGUAGEVERSION_5_8;
      //   HR(pScriptProperty->SetProperty(SCRIPTPROP_INVOKEVERSIONING, 0, &engineVersion));
      //}

      // Attach to site
      HR(engine_->SetScriptSite(static_cast<IActiveScriptSite*>(this)));

      CComQIPtr<IActiveScriptParse> spParse = engine_;
      if(!spParse) return E_NOINTERFACE;
      HR(spParse->InitNew());

      init_ = true;
      return NOERROR;
   }

   STDMETHOD(Run)()
   {
      if(!init_) return E_FAIL;
      HR(engine_->SetScriptState(SCRIPTSTATE_STARTED));
      // connect - this makes the script engine handle incoming events
      HR(engine_->SetScriptState(SCRIPTSTATE_CONNECTED));
      return NOERROR;
   }

   STDMETHOD(Terminate)()
   {
      if(init_) 
      {
         // Disconnect the host application from the engine. This will prevent
         // the further firing of events. Event sinks that are in progress will
         // be completed before the state changes.
         engine_->SetScriptState(SCRIPTSTATE_DISCONNECTED);

         // Call to InterruptScriptThread to abandon any running scripts and
         // force cleanup of all script elements.
         engine_->InterruptScriptThread(SCRIPTTHREADID_ALL,NULL,0);

         init_ = false;
      }

      if(!!engine_) 
      {
         // Always call prior to release
         engine_->Close();
         engine_.Release();
      }

      return NOERROR;
   }

   bool 
   ProcedureExists(HM::String sName)
   {
      // Determines wether a procedure exists in
      // the script. 

      IDispatch *ScriptDispatch = NULL;

      engine_->GetScriptDispatch(NULL, &ScriptDispatch);

      DISPID dispid;
      BSTR names[1];
      names[0] = sName.AllocSysString();
      HRESULT hr = ScriptDispatch->GetIDsOfNames( IID_NULL, names, 1, 0, &dispid );

      SysFreeString(names[0]);

      if ( SUCCEEDED( hr ) )
         return true;
      else
         return false;
   }

   STDMETHOD(AddScript)(LPCTSTR pszScript,LPCTSTR pszContext=NULL)
   {
      if(!init_) return E_FAIL;

      CComQIPtr<IActiveScriptParse> spParse = engine_;
      if(!spParse) return E_NOINTERFACE;

      USES_CONVERSION;
      const DWORD dwFlags = SCRIPTTEXT_ISVISIBLE;
      EXCEPINFO einfo;
      return spParse->ParseScriptText(T2COLE(pszScript),pszContext!=NULL ? T2COLE(pszContext) : OLESTR(""),NULL,NULL,0,0,dwFlags,NULL,&einfo);

      
   }

   STDMETHOD(SetObjectContainer)(std::shared_ptr<HM::ScriptObjectContainer> pObject)
   {
      object_container_ = pObject;

      // Add the objects to namespace
      std::vector<HM::String> vecNames = object_container_->GetObjectNames();
      
      for(HM::String name : vecNames)
      {
         AddObject(name, TRUE);
      }

      return S_OK;
   }
      
   STDMETHOD(AddObject)(LPCTSTR pszName,BOOL bGlobalCollection=FALSE)
   {
      if(!init_) return E_FAIL;

      DWORD dwFlags = SCRIPTITEM_ISVISIBLE;
      if(bGlobalCollection)
         dwFlags |= SCRIPTITEM_GLOBALMEMBERS;

      USES_CONVERSION;
      return engine_->AddNamedItem(T2COLE(pszName),dwFlags);
   }

   STDMETHOD(GetWindow)(HWND *phWnd)
   {
      if(phWnd==NULL)
         return E_POINTER;
      *phWnd = wnd_;
      return NOERROR;
   };

protected:
   ~CScriptSiteImpl()
   {
      if(!!engine_)
         Terminate();
   }

protected:
   HWND                    wnd_;
   bool                    init_;
   CComPtr<IActiveScript>  engine_;
};

/////////////////////////////////////////////////////////////////////////////
// CScriptSiteBasic
//
// This is the minimum code needed to run a script engine. It has no support
// for your own object model, and displays errors as a messagebox. To
// overcome this, you'll need to write your own version and override
// LookupNamedItem and/or HandleScriptError.
//
// Use it like this:
//
//   LPCTSTR strScriptCode; // this points to the script code we want to run
//   ...
//   CComObject<CScriptSiteBasic>* pBasic;
//   CComQIPtr<IActiveScriptSite> spUnk;
//   HR(CComObject<CScriptSiteBasic>::CreateInstance(&pBasic));
//   spUnk = pBasic; // let CComQIPtr tidy up for us
//   HR(pBasic->Initiate(_T("jscript"),GetDesktopWindow()));
//   HR(pBasic->AddScript(strScriptCode));
//   HR(pBasic->Run());
//   HR(pBasic->Terminate());

class ATL_NO_VTABLE CScriptSiteBasic :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CScriptSiteImpl
{
public:
   DECLARE_PROTECT_FINAL_CONSTRUCT()
   BEGIN_COM_MAP(CScriptSiteBasic)
      COM_INTERFACE_ENTRY(IActiveScriptSite)
      COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
   END_COM_MAP()
};



