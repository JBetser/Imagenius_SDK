

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Jul 13 20:09:27 2013
 */
/* Compiler settings for IGPictureEditor.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

#ifndef __IGPictureEditor_i_h__
#define __IGPictureEditor_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IIGFrame_FWD_DEFINED__
#define __IIGFrame_FWD_DEFINED__
typedef interface IIGFrame IIGFrame;
#endif 	/* __IIGFrame_FWD_DEFINED__ */


#ifndef __IIGMultiFrame_FWD_DEFINED__
#define __IIGMultiFrame_FWD_DEFINED__
typedef interface IIGMultiFrame IIGMultiFrame;
#endif 	/* __IIGMultiFrame_FWD_DEFINED__ */


#ifndef __IIGMenuBar_FWD_DEFINED__
#define __IIGMenuBar_FWD_DEFINED__
typedef interface IIGMenuBar IIGMenuBar;
#endif 	/* __IIGMenuBar_FWD_DEFINED__ */


#ifndef __IIGMenuBarDropButton_FWD_DEFINED__
#define __IIGMenuBarDropButton_FWD_DEFINED__
typedef interface IIGMenuBarDropButton IIGMenuBarDropButton;
#endif 	/* __IIGMenuBarDropButton_FWD_DEFINED__ */


#ifndef __IIGLibraryMenuButton_FWD_DEFINED__
#define __IIGLibraryMenuButton_FWD_DEFINED__
typedef interface IIGLibraryMenuButton IIGLibraryMenuButton;
#endif 	/* __IIGLibraryMenuButton_FWD_DEFINED__ */


#ifndef __IIGImageLibrary_FWD_DEFINED__
#define __IIGImageLibrary_FWD_DEFINED__
typedef interface IIGImageLibrary IIGImageLibrary;
#endif 	/* __IIGImageLibrary_FWD_DEFINED__ */


#ifndef __IIGDockPanel_FWD_DEFINED__
#define __IIGDockPanel_FWD_DEFINED__
typedef interface IIGDockPanel IIGDockPanel;
#endif 	/* __IIGDockPanel_FWD_DEFINED__ */


#ifndef __IIGWorkspace_FWD_DEFINED__
#define __IIGWorkspace_FWD_DEFINED__
typedef interface IIGWorkspace IIGWorkspace;
#endif 	/* __IIGWorkspace_FWD_DEFINED__ */


#ifndef __IIGLayerManager_FWD_DEFINED__
#define __IIGLayerManager_FWD_DEFINED__
typedef interface IIGLayerManager IIGLayerManager;
#endif 	/* __IIGLayerManager_FWD_DEFINED__ */


#ifndef __IIGSimpleButton_FWD_DEFINED__
#define __IIGSimpleButton_FWD_DEFINED__
typedef interface IIGSimpleButton IIGSimpleButton;
#endif 	/* __IIGSimpleButton_FWD_DEFINED__ */


#ifndef __IIGScrollbar_FWD_DEFINED__
#define __IIGScrollbar_FWD_DEFINED__
typedef interface IIGScrollbar IIGScrollbar;
#endif 	/* __IIGScrollbar_FWD_DEFINED__ */


#ifndef __IIGHistoryManager_FWD_DEFINED__
#define __IIGHistoryManager_FWD_DEFINED__
typedef interface IIGHistoryManager IIGHistoryManager;
#endif 	/* __IIGHistoryManager_FWD_DEFINED__ */


#ifndef __IIGToolBox_FWD_DEFINED__
#define __IIGToolBox_FWD_DEFINED__
typedef interface IIGToolBox IIGToolBox;
#endif 	/* __IIGToolBox_FWD_DEFINED__ */


#ifndef __IIGMainWindow_FWD_DEFINED__
#define __IIGMainWindow_FWD_DEFINED__
typedef interface IIGMainWindow IIGMainWindow;
#endif 	/* __IIGMainWindow_FWD_DEFINED__ */


#ifndef __IIGPropertyManager_FWD_DEFINED__
#define __IIGPropertyManager_FWD_DEFINED__
typedef interface IIGPropertyManager IIGPropertyManager;
#endif 	/* __IIGPropertyManager_FWD_DEFINED__ */


#ifndef __IIGSocket_FWD_DEFINED__
#define __IIGSocket_FWD_DEFINED__
typedef interface IIGSocket IIGSocket;
#endif 	/* __IIGSocket_FWD_DEFINED__ */


#ifndef ___IMainWindowEvents_FWD_DEFINED__
#define ___IMainWindowEvents_FWD_DEFINED__
typedef interface _IMainWindowEvents _IMainWindowEvents;
#endif 	/* ___IMainWindowEvents_FWD_DEFINED__ */


#ifndef __MainWindow_FWD_DEFINED__
#define __MainWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class MainWindow MainWindow;
#else
typedef struct MainWindow MainWindow;
#endif /* __cplusplus */

#endif 	/* __MainWindow_FWD_DEFINED__ */


#ifndef ___IIGFrameEvents_FWD_DEFINED__
#define ___IIGFrameEvents_FWD_DEFINED__
typedef interface _IIGFrameEvents _IIGFrameEvents;
#endif 	/* ___IIGFrameEvents_FWD_DEFINED__ */


#ifndef __IGFrameControl_FWD_DEFINED__
#define __IGFrameControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGFrameControl IGFrameControl;
#else
typedef struct IGFrameControl IGFrameControl;
#endif /* __cplusplus */

#endif 	/* __IGFrameControl_FWD_DEFINED__ */


#ifndef ___IIGMultiFrameEvents_FWD_DEFINED__
#define ___IIGMultiFrameEvents_FWD_DEFINED__
typedef interface _IIGMultiFrameEvents _IIGMultiFrameEvents;
#endif 	/* ___IIGMultiFrameEvents_FWD_DEFINED__ */


#ifndef __IGMultiFrame_FWD_DEFINED__
#define __IGMultiFrame_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGMultiFrame IGMultiFrame;
#else
typedef struct IGMultiFrame IGMultiFrame;
#endif /* __cplusplus */

#endif 	/* __IGMultiFrame_FWD_DEFINED__ */


#ifndef ___IIGMenuBarEvents_FWD_DEFINED__
#define ___IIGMenuBarEvents_FWD_DEFINED__
typedef interface _IIGMenuBarEvents _IIGMenuBarEvents;
#endif 	/* ___IIGMenuBarEvents_FWD_DEFINED__ */


#ifndef __IGMenuBar_FWD_DEFINED__
#define __IGMenuBar_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGMenuBar IGMenuBar;
#else
typedef struct IGMenuBar IGMenuBar;
#endif /* __cplusplus */

#endif 	/* __IGMenuBar_FWD_DEFINED__ */


#ifndef ___IIGMenuBarDropButtonEvents_FWD_DEFINED__
#define ___IIGMenuBarDropButtonEvents_FWD_DEFINED__
typedef interface _IIGMenuBarDropButtonEvents _IIGMenuBarDropButtonEvents;
#endif 	/* ___IIGMenuBarDropButtonEvents_FWD_DEFINED__ */


#ifndef __IGMenuBarDropButton_FWD_DEFINED__
#define __IGMenuBarDropButton_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGMenuBarDropButton IGMenuBarDropButton;
#else
typedef struct IGMenuBarDropButton IGMenuBarDropButton;
#endif /* __cplusplus */

#endif 	/* __IGMenuBarDropButton_FWD_DEFINED__ */


#ifndef __IGLibraryMenuButton_FWD_DEFINED__
#define __IGLibraryMenuButton_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGLibraryMenuButton IGLibraryMenuButton;
#else
typedef struct IGLibraryMenuButton IGLibraryMenuButton;
#endif /* __cplusplus */

#endif 	/* __IGLibraryMenuButton_FWD_DEFINED__ */


#ifndef ___IIGImageLibraryEvents_FWD_DEFINED__
#define ___IIGImageLibraryEvents_FWD_DEFINED__
typedef interface _IIGImageLibraryEvents _IIGImageLibraryEvents;
#endif 	/* ___IIGImageLibraryEvents_FWD_DEFINED__ */


#ifndef __IGImageLibrary_FWD_DEFINED__
#define __IGImageLibrary_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGImageLibrary IGImageLibrary;
#else
typedef struct IGImageLibrary IGImageLibrary;
#endif /* __cplusplus */

#endif 	/* __IGImageLibrary_FWD_DEFINED__ */


#ifndef ___IIGDockPanelEvents_FWD_DEFINED__
#define ___IIGDockPanelEvents_FWD_DEFINED__
typedef interface _IIGDockPanelEvents _IIGDockPanelEvents;
#endif 	/* ___IIGDockPanelEvents_FWD_DEFINED__ */


#ifndef __IGDockPanel_FWD_DEFINED__
#define __IGDockPanel_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGDockPanel IGDockPanel;
#else
typedef struct IGDockPanel IGDockPanel;
#endif /* __cplusplus */

#endif 	/* __IGDockPanel_FWD_DEFINED__ */


#ifndef ___IIGWorkspaceEvents_FWD_DEFINED__
#define ___IIGWorkspaceEvents_FWD_DEFINED__
typedef interface _IIGWorkspaceEvents _IIGWorkspaceEvents;
#endif 	/* ___IIGWorkspaceEvents_FWD_DEFINED__ */


#ifndef __IGWorkspace_FWD_DEFINED__
#define __IGWorkspace_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGWorkspace IGWorkspace;
#else
typedef struct IGWorkspace IGWorkspace;
#endif /* __cplusplus */

#endif 	/* __IGWorkspace_FWD_DEFINED__ */


#ifndef ___IIGLayerManagerEvents_FWD_DEFINED__
#define ___IIGLayerManagerEvents_FWD_DEFINED__
typedef interface _IIGLayerManagerEvents _IIGLayerManagerEvents;
#endif 	/* ___IIGLayerManagerEvents_FWD_DEFINED__ */


#ifndef __IGLayerManager_FWD_DEFINED__
#define __IGLayerManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGLayerManager IGLayerManager;
#else
typedef struct IGLayerManager IGLayerManager;
#endif /* __cplusplus */

#endif 	/* __IGLayerManager_FWD_DEFINED__ */


#ifndef __IGPropertyManager_FWD_DEFINED__
#define __IGPropertyManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGPropertyManager IGPropertyManager;
#else
typedef struct IGPropertyManager IGPropertyManager;
#endif /* __cplusplus */

#endif 	/* __IGPropertyManager_FWD_DEFINED__ */


#ifndef __IGFramePropertyManager_FWD_DEFINED__
#define __IGFramePropertyManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGFramePropertyManager IGFramePropertyManager;
#else
typedef struct IGFramePropertyManager IGFramePropertyManager;
#endif /* __cplusplus */

#endif 	/* __IGFramePropertyManager_FWD_DEFINED__ */


#ifndef __IGSimpleButton_FWD_DEFINED__
#define __IGSimpleButton_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGSimpleButton IGSimpleButton;
#else
typedef struct IGSimpleButton IGSimpleButton;
#endif /* __cplusplus */

#endif 	/* __IGSimpleButton_FWD_DEFINED__ */


#ifndef __IGScrollbar_FWD_DEFINED__
#define __IGScrollbar_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGScrollbar IGScrollbar;
#else
typedef struct IGScrollbar IGScrollbar;
#endif /* __cplusplus */

#endif 	/* __IGScrollbar_FWD_DEFINED__ */


#ifndef __IGHistoryManager_FWD_DEFINED__
#define __IGHistoryManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGHistoryManager IGHistoryManager;
#else
typedef struct IGHistoryManager IGHistoryManager;
#endif /* __cplusplus */

#endif 	/* __IGHistoryManager_FWD_DEFINED__ */


#ifndef __IGToolBox_FWD_DEFINED__
#define __IGToolBox_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGToolBox IGToolBox;
#else
typedef struct IGToolBox IGToolBox;
#endif /* __cplusplus */

#endif 	/* __IGToolBox_FWD_DEFINED__ */


#ifndef __IGMainWindow_FWD_DEFINED__
#define __IGMainWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGMainWindow IGMainWindow;
#else
typedef struct IGMainWindow IGMainWindow;
#endif /* __cplusplus */

#endif 	/* __IGMainWindow_FWD_DEFINED__ */


#ifndef __IGSocket_FWD_DEFINED__
#define __IGSocket_FWD_DEFINED__

#ifdef __cplusplus
typedef class IGSocket IGSocket;
#else
typedef struct IGSocket IGSocket;
#endif /* __cplusplus */

#endif 	/* __IGSocket_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IIGFrame_INTERFACE_DEFINED__
#define __IIGFrame_INTERFACE_DEFINED__

/* interface IIGFrame */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGFrame;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DF9A05EC-8289-4AA5-9A5F-BD24A33C563C")
    IIGFrame : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrameFromHandle( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT prc,
            /* [in] */ OLE_HANDLE hMem,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager = 0,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT prc,
            /* [in] */ BSTR bstrPath,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager = 0,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL bAutoRotate = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateNewFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT prc,
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [in] */ LONG nColorMode,
            /* [in] */ LONG nBackgroundMode,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager = 0,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdatePosition( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLayer( 
            /* [defaultvalue][in] */ LONG nPos = -1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveLayer( 
            /* [in] */ LONG nPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveLayer( 
            /* [in] */ LONG nPosFrom,
            /* [in] */ LONG nPosDest) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetToolBox( 
            /* [in] */ IDispatch *pDispToolbox) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ManageSelection( 
            /* [in] */ LONG nSelectionActionId,
            /* [in] */ VARIANT *safeArrayParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConvertCoordFrameToPixel( 
            /* [out][in] */ LONG *p_nPosX,
            /* [out][in] */ LONG *p_nPosY) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessImage( 
            /* [in] */ LONG nImageProcessingId,
            /* [in] */ OLE_HANDLE hParams) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_isMinimized( 
            /* [retval][out] */ VARIANT_BOOL *p_bMinimized) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_isMinimized( 
            /* [in] */ VARIANT_BOOL bMinimized) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_isMaximized( 
            /* [retval][out] */ VARIANT_BOOL *p_bMaximized) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_isMaximized( 
            /* [in] */ VARIANT_BOOL bMaximized) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_isVisible( 
            /* [in] */ VARIANT_BOOL bVisible) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGFrameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGFrame * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGFrame * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGFrame * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGFrame * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGFrame * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGFrame * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGFrame * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrameFromHandle )( 
            IIGFrame * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT prc,
            /* [in] */ OLE_HANDLE hMem,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGFrame * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT prc,
            /* [in] */ BSTR bstrPath,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager,
            /* [defaultvalue][in] */ VARIANT_BOOL bAutoRotate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateNewFrame )( 
            IIGFrame * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT prc,
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [in] */ LONG nColorMode,
            /* [in] */ LONG nBackgroundMode,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGFrame * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdatePosition )( 
            IIGFrame * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddLayer )( 
            IIGFrame * This,
            /* [defaultvalue][in] */ LONG nPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveLayer )( 
            IIGFrame * This,
            /* [in] */ LONG nPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveLayer )( 
            IIGFrame * This,
            /* [in] */ LONG nPosFrom,
            /* [in] */ LONG nPosDest);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetToolBox )( 
            IIGFrame * This,
            /* [in] */ IDispatch *pDispToolbox);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ManageSelection )( 
            IIGFrame * This,
            /* [in] */ LONG nSelectionActionId,
            /* [in] */ VARIANT *safeArrayParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertCoordFrameToPixel )( 
            IIGFrame * This,
            /* [out][in] */ LONG *p_nPosX,
            /* [out][in] */ LONG *p_nPosY);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessImage )( 
            IIGFrame * This,
            /* [in] */ LONG nImageProcessingId,
            /* [in] */ OLE_HANDLE hParams);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_isMinimized )( 
            IIGFrame * This,
            /* [retval][out] */ VARIANT_BOOL *p_bMinimized);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_isMinimized )( 
            IIGFrame * This,
            /* [in] */ VARIANT_BOOL bMinimized);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_isMaximized )( 
            IIGFrame * This,
            /* [retval][out] */ VARIANT_BOOL *p_bMaximized);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_isMaximized )( 
            IIGFrame * This,
            /* [in] */ VARIANT_BOOL bMaximized);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_isVisible )( 
            IIGFrame * This,
            /* [in] */ VARIANT_BOOL bVisible);
        
        END_INTERFACE
    } IIGFrameVtbl;

    interface IIGFrame
    {
        CONST_VTBL struct IIGFrameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGFrame_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGFrame_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGFrame_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGFrame_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGFrame_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGFrame_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGFrame_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGFrame_CreateFrameFromHandle(This,hWnd,prc,hMem,pDispLayerManager,pDispPropertyManager)	\
    ( (This)->lpVtbl -> CreateFrameFromHandle(This,hWnd,prc,hMem,pDispLayerManager,pDispPropertyManager) ) 

#define IIGFrame_CreateFrame(This,hWnd,prc,bstrPath,pDispLayerManager,pDispPropertyManager,bAutoRotate)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,prc,bstrPath,pDispLayerManager,pDispPropertyManager,bAutoRotate) ) 

#define IIGFrame_CreateNewFrame(This,hWnd,prc,nWidth,nHeight,nColorMode,nBackgroundMode,pDispLayerManager,pDispPropertyManager)	\
    ( (This)->lpVtbl -> CreateNewFrame(This,hWnd,prc,nWidth,nHeight,nColorMode,nBackgroundMode,pDispLayerManager,pDispPropertyManager) ) 

#define IIGFrame_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGFrame_UpdatePosition(This)	\
    ( (This)->lpVtbl -> UpdatePosition(This) ) 

#define IIGFrame_AddLayer(This,nPos)	\
    ( (This)->lpVtbl -> AddLayer(This,nPos) ) 

#define IIGFrame_RemoveLayer(This,nPos)	\
    ( (This)->lpVtbl -> RemoveLayer(This,nPos) ) 

#define IIGFrame_MoveLayer(This,nPosFrom,nPosDest)	\
    ( (This)->lpVtbl -> MoveLayer(This,nPosFrom,nPosDest) ) 

#define IIGFrame_SetToolBox(This,pDispToolbox)	\
    ( (This)->lpVtbl -> SetToolBox(This,pDispToolbox) ) 

#define IIGFrame_ManageSelection(This,nSelectionActionId,safeArrayParams)	\
    ( (This)->lpVtbl -> ManageSelection(This,nSelectionActionId,safeArrayParams) ) 

#define IIGFrame_ConvertCoordFrameToPixel(This,p_nPosX,p_nPosY)	\
    ( (This)->lpVtbl -> ConvertCoordFrameToPixel(This,p_nPosX,p_nPosY) ) 

#define IIGFrame_ProcessImage(This,nImageProcessingId,hParams)	\
    ( (This)->lpVtbl -> ProcessImage(This,nImageProcessingId,hParams) ) 

#define IIGFrame_get_isMinimized(This,p_bMinimized)	\
    ( (This)->lpVtbl -> get_isMinimized(This,p_bMinimized) ) 

#define IIGFrame_put_isMinimized(This,bMinimized)	\
    ( (This)->lpVtbl -> put_isMinimized(This,bMinimized) ) 

#define IIGFrame_get_isMaximized(This,p_bMaximized)	\
    ( (This)->lpVtbl -> get_isMaximized(This,p_bMaximized) ) 

#define IIGFrame_put_isMaximized(This,bMaximized)	\
    ( (This)->lpVtbl -> put_isMaximized(This,bMaximized) ) 

#define IIGFrame_put_isVisible(This,bVisible)	\
    ( (This)->lpVtbl -> put_isVisible(This,bVisible) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGFrame_INTERFACE_DEFINED__ */


#ifndef __IIGMultiFrame_INTERFACE_DEFINED__
#define __IIGMultiFrame_INTERFACE_DEFINED__

/* interface IIGMultiFrame */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGMultiFrame;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("252A54F9-3639-4F24-8E06-D66546ACE93F")
    IIGMultiFrame : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager = 0,
            /* [defaultvalue][in] */ IDispatch *pDispWorkspace = 0,
            /* [defaultvalue][in] */ IDispatch *pDispToolbox = 0,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddImage( 
            /* [in] */ BSTR bstrImagePath,
            /* [defaultvalue][in] */ VARIANT_BOOL bAutoRotate = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddImageFromHandle( 
            /* [in] */ OLE_HANDLE hMem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveFrame( 
            /* [in] */ LONG nFrameId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DrawFrame( 
            /* [in] */ LONG IdxBitmap,
            /* [in] */ OLE_HANDLE hDC,
            /* [in] */ UINT_PTR pRect) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDropTarget( 
            IDropTarget **ppDropTarget) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddNewImage( 
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [in] */ LONG nColorMode,
            /* [in] */ LONG nBackgroundMode) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_isMinimized( 
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [retval][out] */ VARIANT_BOOL *p_bMinimized) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_isMinimized( 
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [in] */ VARIANT_BOOL bMinimized) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_isMaximized( 
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [retval][out] */ VARIANT_BOOL *p_bMaximized) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_isMaximized( 
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [in] */ VARIANT_BOOL bMaximized) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_frameId( 
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [retval][out] */ LONG *p_nFrameId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGMultiFrameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGMultiFrame * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGMultiFrame * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGMultiFrame * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGMultiFrame * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGMultiFrame * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGMultiFrame * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGMultiFrame * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc,
            /* [defaultvalue][in] */ IDispatch *pDispLayerManager,
            /* [defaultvalue][in] */ IDispatch *pDispWorkspace,
            /* [defaultvalue][in] */ IDispatch *pDispToolbox,
            /* [defaultvalue][in] */ IDispatch *pDispPropertyManager);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGMultiFrame * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddImage )( 
            IIGMultiFrame * This,
            /* [in] */ BSTR bstrImagePath,
            /* [defaultvalue][in] */ VARIANT_BOOL bAutoRotate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddImageFromHandle )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hMem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IIGMultiFrame * This,
            /* [in] */ LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveFrame )( 
            IIGMultiFrame * This,
            /* [in] */ LONG nFrameId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DrawFrame )( 
            IIGMultiFrame * This,
            /* [in] */ LONG IdxBitmap,
            /* [in] */ OLE_HANDLE hDC,
            /* [in] */ UINT_PTR pRect);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDropTarget )( 
            IIGMultiFrame * This,
            IDropTarget **ppDropTarget);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddNewImage )( 
            IIGMultiFrame * This,
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [in] */ LONG nColorMode,
            /* [in] */ LONG nBackgroundMode);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_isMinimized )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [retval][out] */ VARIANT_BOOL *p_bMinimized);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_isMinimized )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [in] */ VARIANT_BOOL bMinimized);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_isMaximized )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [retval][out] */ VARIANT_BOOL *p_bMaximized);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_isMaximized )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [in] */ VARIANT_BOOL bMaximized);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_frameId )( 
            IIGMultiFrame * This,
            /* [in] */ OLE_HANDLE hPtrFrame,
            /* [retval][out] */ LONG *p_nFrameId);
        
        END_INTERFACE
    } IIGMultiFrameVtbl;

    interface IIGMultiFrame
    {
        CONST_VTBL struct IIGMultiFrameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGMultiFrame_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGMultiFrame_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGMultiFrame_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGMultiFrame_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGMultiFrame_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGMultiFrame_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGMultiFrame_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGMultiFrame_CreateFrame(This,hWnd,rc,pDispLayerManager,pDispWorkspace,pDispToolbox,pDispPropertyManager)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,rc,pDispLayerManager,pDispWorkspace,pDispToolbox,pDispPropertyManager) ) 

#define IIGMultiFrame_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGMultiFrame_AddImage(This,bstrImagePath,bAutoRotate)	\
    ( (This)->lpVtbl -> AddImage(This,bstrImagePath,bAutoRotate) ) 

#define IIGMultiFrame_AddImageFromHandle(This,hMem)	\
    ( (This)->lpVtbl -> AddImageFromHandle(This,hMem) ) 

#define IIGMultiFrame_Move(This,p_rc)	\
    ( (This)->lpVtbl -> Move(This,p_rc) ) 

#define IIGMultiFrame_RemoveFrame(This,nFrameId)	\
    ( (This)->lpVtbl -> RemoveFrame(This,nFrameId) ) 

#define IIGMultiFrame_DrawFrame(This,IdxBitmap,hDC,pRect)	\
    ( (This)->lpVtbl -> DrawFrame(This,IdxBitmap,hDC,pRect) ) 

#define IIGMultiFrame_GetDropTarget(This,ppDropTarget)	\
    ( (This)->lpVtbl -> GetDropTarget(This,ppDropTarget) ) 

#define IIGMultiFrame_AddNewImage(This,nWidth,nHeight,nColorMode,nBackgroundMode)	\
    ( (This)->lpVtbl -> AddNewImage(This,nWidth,nHeight,nColorMode,nBackgroundMode) ) 

#define IIGMultiFrame_get_isMinimized(This,hPtrFrame,p_bMinimized)	\
    ( (This)->lpVtbl -> get_isMinimized(This,hPtrFrame,p_bMinimized) ) 

#define IIGMultiFrame_put_isMinimized(This,hPtrFrame,bMinimized)	\
    ( (This)->lpVtbl -> put_isMinimized(This,hPtrFrame,bMinimized) ) 

#define IIGMultiFrame_get_isMaximized(This,hPtrFrame,p_bMaximized)	\
    ( (This)->lpVtbl -> get_isMaximized(This,hPtrFrame,p_bMaximized) ) 

#define IIGMultiFrame_put_isMaximized(This,hPtrFrame,bMaximized)	\
    ( (This)->lpVtbl -> put_isMaximized(This,hPtrFrame,bMaximized) ) 

#define IIGMultiFrame_get_frameId(This,hPtrFrame,p_nFrameId)	\
    ( (This)->lpVtbl -> get_frameId(This,hPtrFrame,p_nFrameId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGMultiFrame_INTERFACE_DEFINED__ */


#ifndef __IIGMenuBar_INTERFACE_DEFINED__
#define __IIGMenuBar_INTERFACE_DEFINED__

/* interface IIGMenuBar */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGMenuBar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("21AF46D9-60E7-4F44-B609-27D363DA7C7D")
    IIGMenuBar : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc,
            /* [in] */ BSTR xmlConfig,
            /* [defaultvalue][in] */ LONG idxButton = -1,
            /* [defaultvalue][in] */ LONG nRefPosX = 0,
            /* [defaultvalue][in] */ LONG nRefPosY = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL bVertical = 0,
            /* [defaultvalue][in] */ LONG idxSubMenu = -1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowPopup( 
            /* [in] */ LONG nNewRefPosX,
            /* [in] */ LONG nNewRefPosY) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [in] */ LONG idxButtonClick) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnSelect( 
            /* [in] */ LONG idxButtonClick) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableButton( 
            /* [in] */ LONG idxButtonClick) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisableButton( 
            /* [in] */ LONG idxButtonClick) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddDropTarget( 
            IDropTarget *pDropTarget) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_IdxSubMenu( 
            /* [retval][out] */ LONG *p_idxSubMenu) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_IdxButton( 
            /* [retval][out] */ LONG *p_idxButton) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Font( 
            /* [in] */ OLE_HANDLE hFont) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGMenuBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGMenuBar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGMenuBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGMenuBar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGMenuBar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGMenuBar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGMenuBar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGMenuBar * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGMenuBar * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc,
            /* [in] */ BSTR xmlConfig,
            /* [defaultvalue][in] */ LONG idxButton,
            /* [defaultvalue][in] */ LONG nRefPosX,
            /* [defaultvalue][in] */ LONG nRefPosY,
            /* [defaultvalue][in] */ VARIANT_BOOL bVertical,
            /* [defaultvalue][in] */ LONG idxSubMenu);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGMenuBar * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowPopup )( 
            IIGMenuBar * This,
            /* [in] */ LONG nNewRefPosX,
            /* [in] */ LONG nNewRefPosY);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IIGMenuBar * This,
            /* [in] */ LONG idxButtonClick);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnSelect )( 
            IIGMenuBar * This,
            /* [in] */ LONG idxButtonClick);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableButton )( 
            IIGMenuBar * This,
            /* [in] */ LONG idxButtonClick);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisableButton )( 
            IIGMenuBar * This,
            /* [in] */ LONG idxButtonClick);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IIGMenuBar * This,
            /* [in] */ LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddDropTarget )( 
            IIGMenuBar * This,
            IDropTarget *pDropTarget);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IdxSubMenu )( 
            IIGMenuBar * This,
            /* [retval][out] */ LONG *p_idxSubMenu);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IdxButton )( 
            IIGMenuBar * This,
            /* [retval][out] */ LONG *p_idxButton);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Font )( 
            IIGMenuBar * This,
            /* [in] */ OLE_HANDLE hFont);
        
        END_INTERFACE
    } IIGMenuBarVtbl;

    interface IIGMenuBar
    {
        CONST_VTBL struct IIGMenuBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGMenuBar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGMenuBar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGMenuBar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGMenuBar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGMenuBar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGMenuBar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGMenuBar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGMenuBar_CreateFrame(This,hWnd,rc,xmlConfig,idxButton,nRefPosX,nRefPosY,bVertical,idxSubMenu)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,rc,xmlConfig,idxButton,nRefPosX,nRefPosY,bVertical,idxSubMenu) ) 

#define IIGMenuBar_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGMenuBar_ShowPopup(This,nNewRefPosX,nNewRefPosY)	\
    ( (This)->lpVtbl -> ShowPopup(This,nNewRefPosX,nNewRefPosY) ) 

#define IIGMenuBar_Select(This,idxButtonClick)	\
    ( (This)->lpVtbl -> Select(This,idxButtonClick) ) 

#define IIGMenuBar_UnSelect(This,idxButtonClick)	\
    ( (This)->lpVtbl -> UnSelect(This,idxButtonClick) ) 

#define IIGMenuBar_EnableButton(This,idxButtonClick)	\
    ( (This)->lpVtbl -> EnableButton(This,idxButtonClick) ) 

#define IIGMenuBar_DisableButton(This,idxButtonClick)	\
    ( (This)->lpVtbl -> DisableButton(This,idxButtonClick) ) 

#define IIGMenuBar_Move(This,p_rc)	\
    ( (This)->lpVtbl -> Move(This,p_rc) ) 

#define IIGMenuBar_AddDropTarget(This,pDropTarget)	\
    ( (This)->lpVtbl -> AddDropTarget(This,pDropTarget) ) 

#define IIGMenuBar_get_IdxSubMenu(This,p_idxSubMenu)	\
    ( (This)->lpVtbl -> get_IdxSubMenu(This,p_idxSubMenu) ) 

#define IIGMenuBar_get_IdxButton(This,p_idxButton)	\
    ( (This)->lpVtbl -> get_IdxButton(This,p_idxButton) ) 

#define IIGMenuBar_put_Font(This,hFont)	\
    ( (This)->lpVtbl -> put_Font(This,hFont) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGMenuBar_INTERFACE_DEFINED__ */


#ifndef __IIGMenuBarDropButton_INTERFACE_DEFINED__
#define __IIGMenuBarDropButton_INTERFACE_DEFINED__

/* interface IIGMenuBarDropButton */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGMenuBarDropButton;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6B1D4D23-D8BD-4095-B344-FA1373949B1A")
    IIGMenuBarDropButton : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc,
            /* [in] */ BSTR xmlConfig,
            /* [in] */ VARIANT_BOOL bIsMainMenu,
            /* [in] */ LONG idxButton,
            /* [defaultvalue][in] */ LONG idxSubMenu = -1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnSelect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Enable( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Disable( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_IdxSubMenu( 
            /* [retval][out] */ LONG *p_idxSubMenu) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_IdxButtonClick( 
            /* [retval][out] */ LONG *p_idxButtonClick) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Font( 
            /* [in] */ OLE_HANDLE hFont) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGMenuBarDropButtonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGMenuBarDropButton * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGMenuBarDropButton * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGMenuBarDropButton * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGMenuBarDropButton * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGMenuBarDropButton * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGMenuBarDropButton * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGMenuBarDropButton * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGMenuBarDropButton * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc,
            /* [in] */ BSTR xmlConfig,
            /* [in] */ VARIANT_BOOL bIsMainMenu,
            /* [in] */ LONG idxButton,
            /* [defaultvalue][in] */ LONG idxSubMenu);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGMenuBarDropButton * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IIGMenuBarDropButton * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnSelect )( 
            IIGMenuBarDropButton * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Enable )( 
            IIGMenuBarDropButton * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Disable )( 
            IIGMenuBarDropButton * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IdxSubMenu )( 
            IIGMenuBarDropButton * This,
            /* [retval][out] */ LONG *p_idxSubMenu);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IdxButtonClick )( 
            IIGMenuBarDropButton * This,
            /* [retval][out] */ LONG *p_idxButtonClick);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Font )( 
            IIGMenuBarDropButton * This,
            /* [in] */ OLE_HANDLE hFont);
        
        END_INTERFACE
    } IIGMenuBarDropButtonVtbl;

    interface IIGMenuBarDropButton
    {
        CONST_VTBL struct IIGMenuBarDropButtonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGMenuBarDropButton_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGMenuBarDropButton_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGMenuBarDropButton_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGMenuBarDropButton_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGMenuBarDropButton_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGMenuBarDropButton_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGMenuBarDropButton_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGMenuBarDropButton_CreateFrame(This,hWnd,rc,xmlConfig,bIsMainMenu,idxButton,idxSubMenu)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,rc,xmlConfig,bIsMainMenu,idxButton,idxSubMenu) ) 

#define IIGMenuBarDropButton_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGMenuBarDropButton_Select(This)	\
    ( (This)->lpVtbl -> Select(This) ) 

#define IIGMenuBarDropButton_UnSelect(This)	\
    ( (This)->lpVtbl -> UnSelect(This) ) 

#define IIGMenuBarDropButton_Enable(This)	\
    ( (This)->lpVtbl -> Enable(This) ) 

#define IIGMenuBarDropButton_Disable(This)	\
    ( (This)->lpVtbl -> Disable(This) ) 

#define IIGMenuBarDropButton_get_IdxSubMenu(This,p_idxSubMenu)	\
    ( (This)->lpVtbl -> get_IdxSubMenu(This,p_idxSubMenu) ) 

#define IIGMenuBarDropButton_get_IdxButtonClick(This,p_idxButtonClick)	\
    ( (This)->lpVtbl -> get_IdxButtonClick(This,p_idxButtonClick) ) 

#define IIGMenuBarDropButton_put_Font(This,hFont)	\
    ( (This)->lpVtbl -> put_Font(This,hFont) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGMenuBarDropButton_INTERFACE_DEFINED__ */


#ifndef __IIGLibraryMenuButton_INTERFACE_DEFINED__
#define __IIGLibraryMenuButton_INTERFACE_DEFINED__

/* interface IIGLibraryMenuButton */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGLibraryMenuButton;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("46CC152B-E803-46B6-99A0-FE1771AC31D3")
    IIGLibraryMenuButton : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGLibraryMenuButtonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGLibraryMenuButton * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGLibraryMenuButton * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGLibraryMenuButton * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGLibraryMenuButton * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGLibraryMenuButton * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGLibraryMenuButton * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGLibraryMenuButton * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGLibraryMenuButton * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGLibraryMenuButton * This);
        
        END_INTERFACE
    } IIGLibraryMenuButtonVtbl;

    interface IIGLibraryMenuButton
    {
        CONST_VTBL struct IIGLibraryMenuButtonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGLibraryMenuButton_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGLibraryMenuButton_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGLibraryMenuButton_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGLibraryMenuButton_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGLibraryMenuButton_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGLibraryMenuButton_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGLibraryMenuButton_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGLibraryMenuButton_CreateFrame(This,hWnd,rc)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,rc) ) 

#define IIGLibraryMenuButton_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGLibraryMenuButton_INTERFACE_DEFINED__ */


#ifndef __IIGImageLibrary_INTERFACE_DEFINED__
#define __IIGImageLibrary_INTERFACE_DEFINED__

/* interface IIGImageLibrary */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGImageLibrary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FFEC1F6B-7F30-4B86-BB65-0EFF6AA43AB3")
    IIGImageLibrary : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowFrame( 
            /* [in] */ LPRECT prc,
            /* [defaultvalue][in] */ LONG nId = -1) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Font( 
            /* [in] */ OLE_HANDLE hFont) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGImageLibraryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGImageLibrary * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGImageLibrary * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGImageLibrary * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGImageLibrary * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGImageLibrary * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGImageLibrary * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGImageLibrary * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGImageLibrary * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGImageLibrary * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowFrame )( 
            IIGImageLibrary * This,
            /* [in] */ LPRECT prc,
            /* [defaultvalue][in] */ LONG nId);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Font )( 
            IIGImageLibrary * This,
            /* [in] */ OLE_HANDLE hFont);
        
        END_INTERFACE
    } IIGImageLibraryVtbl;

    interface IIGImageLibrary
    {
        CONST_VTBL struct IIGImageLibraryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGImageLibrary_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGImageLibrary_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGImageLibrary_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGImageLibrary_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGImageLibrary_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGImageLibrary_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGImageLibrary_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGImageLibrary_CreateFrame(This,hWnd,p_rc)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,p_rc) ) 

#define IIGImageLibrary_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGImageLibrary_ShowFrame(This,prc,nId)	\
    ( (This)->lpVtbl -> ShowFrame(This,prc,nId) ) 

#define IIGImageLibrary_put_Font(This,hFont)	\
    ( (This)->lpVtbl -> put_Font(This,hFont) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGImageLibrary_INTERFACE_DEFINED__ */


#ifndef __IIGDockPanel_INTERFACE_DEFINED__
#define __IIGDockPanel_INTERFACE_DEFINED__

/* interface IIGDockPanel */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGDockPanel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("052C51EB-7B92-43CF-9223-303B783F5E2C")
    IIGDockPanel : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT p_rc,
            /* [in] */ LONG nSide,
            /* [defaultvalue][in] */ LONG nDockSize = 100,
            /* [defaultvalue][in] */ LONG nStyle = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetString( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHWND( 
            /* [out] */ OLE_HANDLE *p_hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClientRECT( 
            /* [out] */ LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterControls( 
            /* [in] */ OLE_HANDLE hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnRegisterControls( 
            /* [in] */ OLE_HANDLE hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RedrawControls( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddDropTarget( 
            IDropTarget *pDropTarget) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGDockPanelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGDockPanel * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGDockPanel * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGDockPanel * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGDockPanel * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGDockPanel * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGDockPanel * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGDockPanel * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGDockPanel * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [in] */ LPRECT p_rc,
            /* [in] */ LONG nSide,
            /* [defaultvalue][in] */ LONG nDockSize,
            /* [defaultvalue][in] */ LONG nStyle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGDockPanel * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IIGDockPanel * This,
            /* [in] */ LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetString )( 
            IIGDockPanel * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHWND )( 
            IIGDockPanel * This,
            /* [out] */ OLE_HANDLE *p_hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClientRECT )( 
            IIGDockPanel * This,
            /* [out] */ LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterControls )( 
            IIGDockPanel * This,
            /* [in] */ OLE_HANDLE hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnRegisterControls )( 
            IIGDockPanel * This,
            /* [in] */ OLE_HANDLE hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RedrawControls )( 
            IIGDockPanel * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddDropTarget )( 
            IIGDockPanel * This,
            IDropTarget *pDropTarget);
        
        END_INTERFACE
    } IIGDockPanelVtbl;

    interface IIGDockPanel
    {
        CONST_VTBL struct IIGDockPanelVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGDockPanel_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGDockPanel_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGDockPanel_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGDockPanel_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGDockPanel_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGDockPanel_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGDockPanel_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGDockPanel_CreateFrame(This,hWnd,p_rc,nSide,nDockSize,nStyle)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,p_rc,nSide,nDockSize,nStyle) ) 

#define IIGDockPanel_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGDockPanel_Move(This,p_rc)	\
    ( (This)->lpVtbl -> Move(This,p_rc) ) 

#define IIGDockPanel_SetString(This,bstrName)	\
    ( (This)->lpVtbl -> SetString(This,bstrName) ) 

#define IIGDockPanel_GetHWND(This,p_hWnd)	\
    ( (This)->lpVtbl -> GetHWND(This,p_hWnd) ) 

#define IIGDockPanel_GetClientRECT(This,p_rc)	\
    ( (This)->lpVtbl -> GetClientRECT(This,p_rc) ) 

#define IIGDockPanel_RegisterControls(This,hWnd)	\
    ( (This)->lpVtbl -> RegisterControls(This,hWnd) ) 

#define IIGDockPanel_UnRegisterControls(This,hWnd)	\
    ( (This)->lpVtbl -> UnRegisterControls(This,hWnd) ) 

#define IIGDockPanel_RedrawControls(This)	\
    ( (This)->lpVtbl -> RedrawControls(This) ) 

#define IIGDockPanel_AddDropTarget(This,pDropTarget)	\
    ( (This)->lpVtbl -> AddDropTarget(This,pDropTarget) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGDockPanel_INTERFACE_DEFINED__ */


#ifndef __IIGWorkspace_INTERFACE_DEFINED__
#define __IIGWorkspace_INTERFACE_DEFINED__

/* interface IIGWorkspace */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGWorkspace;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E1A1F103-92BB-4C44-802F-BBF9312DAF31")
    IIGWorkspace : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ IDispatch *pDispWindowOwner,
            /* [in] */ IDispatch *pDispMultiFrame) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddFrame( 
            /* [defaultvalue][in] */ BSTR bstrImagePath = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL bAbsolutePath = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL bAutoRotate = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddFrameFromHandle( 
            /* [in] */ OLE_HANDLE hMem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveFrame( 
            /* [in] */ LONG nFrameId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveFrame( 
            /* [in] */ LONG nFrameId,
            /* [defaultvalue][in] */ VARIANT_BOOL bSilent = 0,
            /* [defaultvalue][in] */ BSTR bstrImagePath = 0,
            /* [defaultvalue][in] */ VARIANT_BOOL bCloseAfterSave = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetActiveFrame( 
            /* [in] */ OLE_HANDLE *p_hFrame) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateNewFrame( 
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [in] */ LONG nColorMode,
            /* [in] */ LONG nBackgroundMode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConnectUser( 
            /* [in] */ BSTR bstrUserLogin,
            /* [defaultvalue][in] */ BSTR bstrGuid = 0,
            /* [defaultvalue][out] */ LONG *p_nNbFrames = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisconnectUser( 
            /* [defaultvalue][in] */ VARIANT_BOOL bSaveAllToTemp = -1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameProperty( 
            /* [in] */ LONG nFrameId,
            /* [in] */ BSTR bstrPropId,
            /* [out][in] */ VARIANT *pPropVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFrameProperty( 
            /* [in] */ LONG nFrameId,
            /* [in] */ BSTR bstrPropId,
            /* [in] */ VARIANT *pPropVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateFrame( 
            /* [in] */ LONG nFrameId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ BSTR bstrPropId,
            /* [out][in] */ VARIANT *pPropVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProperty( 
            /* [in] */ BSTR bstrPropId,
            /* [in] */ VARIANT *pPropVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPropertiesAndValues( 
            /* [out] */ BSTR *p_bstrPropId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ BSTR bstrImageGuid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Cut( 
            /* [in] */ BSTR bstrImageGuid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Paste( 
            /* [in] */ BSTR bstrImageGuid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameIds( 
            /* [out][in] */ BSTR *p_bstrIds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetActiveFrameId( 
            /* [out][in] */ BSTR *p_bstrIds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameNames( 
            /* [out][in] */ BSTR *p_bstrNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameNbLayers( 
            /* [out][in] */ BSTR *p_bstrNbLayers) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameLayerVisibility( 
            /* [out][in] */ BSTR *p_bstrLayerVisibility) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameProperties( 
            /* [out][in] */ BSTR *p_bstrFrameProperties) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFrameStepIds( 
            /* [out][in] */ BSTR *p_bstrFrameStepIds) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGWorkspaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGWorkspace * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGWorkspace * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGWorkspace * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGWorkspace * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGWorkspace * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGWorkspace * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGWorkspace * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGWorkspace * This,
            /* [in] */ IDispatch *pDispWindowOwner,
            /* [in] */ IDispatch *pDispMultiFrame);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGWorkspace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IIGWorkspace * This,
            LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddFrame )( 
            IIGWorkspace * This,
            /* [defaultvalue][in] */ BSTR bstrImagePath,
            /* [defaultvalue][in] */ VARIANT_BOOL bAbsolutePath,
            /* [defaultvalue][in] */ VARIANT_BOOL bAutoRotate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddFrameFromHandle )( 
            IIGWorkspace * This,
            /* [in] */ OLE_HANDLE hMem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveFrame )( 
            IIGWorkspace * This,
            /* [in] */ LONG nFrameId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveFrame )( 
            IIGWorkspace * This,
            /* [in] */ LONG nFrameId,
            /* [defaultvalue][in] */ VARIANT_BOOL bSilent,
            /* [defaultvalue][in] */ BSTR bstrImagePath,
            /* [defaultvalue][in] */ VARIANT_BOOL bCloseAfterSave);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetActiveFrame )( 
            IIGWorkspace * This,
            /* [in] */ OLE_HANDLE *p_hFrame);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateNewFrame )( 
            IIGWorkspace * This,
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [in] */ LONG nColorMode,
            /* [in] */ LONG nBackgroundMode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConnectUser )( 
            IIGWorkspace * This,
            /* [in] */ BSTR bstrUserLogin,
            /* [defaultvalue][in] */ BSTR bstrGuid,
            /* [defaultvalue][out] */ LONG *p_nNbFrames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisconnectUser )( 
            IIGWorkspace * This,
            /* [defaultvalue][in] */ VARIANT_BOOL bSaveAllToTemp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameProperty )( 
            IIGWorkspace * This,
            /* [in] */ LONG nFrameId,
            /* [in] */ BSTR bstrPropId,
            /* [out][in] */ VARIANT *pPropVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFrameProperty )( 
            IIGWorkspace * This,
            /* [in] */ LONG nFrameId,
            /* [in] */ BSTR bstrPropId,
            /* [in] */ VARIANT *pPropVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateFrame )( 
            IIGWorkspace * This,
            /* [in] */ LONG nFrameId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IIGWorkspace * This,
            /* [in] */ BSTR bstrPropId,
            /* [out][in] */ VARIANT *pPropVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IIGWorkspace * This,
            /* [in] */ BSTR bstrPropId,
            /* [in] */ VARIANT *pPropVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPropertiesAndValues )( 
            IIGWorkspace * This,
            /* [out] */ BSTR *p_bstrPropId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Update )( 
            IIGWorkspace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Copy )( 
            IIGWorkspace * This,
            /* [in] */ BSTR bstrImageGuid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Cut )( 
            IIGWorkspace * This,
            /* [in] */ BSTR bstrImageGuid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Paste )( 
            IIGWorkspace * This,
            /* [in] */ BSTR bstrImageGuid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameIds )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrIds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetActiveFrameId )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrIds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameNames )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameNbLayers )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrNbLayers);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameLayerVisibility )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrLayerVisibility);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameProperties )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrFrameProperties);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFrameStepIds )( 
            IIGWorkspace * This,
            /* [out][in] */ BSTR *p_bstrFrameStepIds);
        
        END_INTERFACE
    } IIGWorkspaceVtbl;

    interface IIGWorkspace
    {
        CONST_VTBL struct IIGWorkspaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGWorkspace_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGWorkspace_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGWorkspace_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGWorkspace_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGWorkspace_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGWorkspace_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGWorkspace_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGWorkspace_CreateFrame(This,pDispWindowOwner,pDispMultiFrame)	\
    ( (This)->lpVtbl -> CreateFrame(This,pDispWindowOwner,pDispMultiFrame) ) 

#define IIGWorkspace_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGWorkspace_Move(This,p_rc)	\
    ( (This)->lpVtbl -> Move(This,p_rc) ) 

#define IIGWorkspace_AddFrame(This,bstrImagePath,bAbsolutePath,bAutoRotate)	\
    ( (This)->lpVtbl -> AddFrame(This,bstrImagePath,bAbsolutePath,bAutoRotate) ) 

#define IIGWorkspace_AddFrameFromHandle(This,hMem)	\
    ( (This)->lpVtbl -> AddFrameFromHandle(This,hMem) ) 

#define IIGWorkspace_RemoveFrame(This,nFrameId)	\
    ( (This)->lpVtbl -> RemoveFrame(This,nFrameId) ) 

#define IIGWorkspace_SaveFrame(This,nFrameId,bSilent,bstrImagePath,bCloseAfterSave)	\
    ( (This)->lpVtbl -> SaveFrame(This,nFrameId,bSilent,bstrImagePath,bCloseAfterSave) ) 

#define IIGWorkspace_GetActiveFrame(This,p_hFrame)	\
    ( (This)->lpVtbl -> GetActiveFrame(This,p_hFrame) ) 

#define IIGWorkspace_CreateNewFrame(This,nWidth,nHeight,nColorMode,nBackgroundMode)	\
    ( (This)->lpVtbl -> CreateNewFrame(This,nWidth,nHeight,nColorMode,nBackgroundMode) ) 

#define IIGWorkspace_ConnectUser(This,bstrUserLogin,bstrGuid,p_nNbFrames)	\
    ( (This)->lpVtbl -> ConnectUser(This,bstrUserLogin,bstrGuid,p_nNbFrames) ) 

#define IIGWorkspace_DisconnectUser(This,bSaveAllToTemp)	\
    ( (This)->lpVtbl -> DisconnectUser(This,bSaveAllToTemp) ) 

#define IIGWorkspace_GetFrameProperty(This,nFrameId,bstrPropId,pPropVal)	\
    ( (This)->lpVtbl -> GetFrameProperty(This,nFrameId,bstrPropId,pPropVal) ) 

#define IIGWorkspace_SetFrameProperty(This,nFrameId,bstrPropId,pPropVal)	\
    ( (This)->lpVtbl -> SetFrameProperty(This,nFrameId,bstrPropId,pPropVal) ) 

#define IIGWorkspace_UpdateFrame(This,nFrameId)	\
    ( (This)->lpVtbl -> UpdateFrame(This,nFrameId) ) 

#define IIGWorkspace_GetProperty(This,bstrPropId,pPropVal)	\
    ( (This)->lpVtbl -> GetProperty(This,bstrPropId,pPropVal) ) 

#define IIGWorkspace_SetProperty(This,bstrPropId,pPropVal)	\
    ( (This)->lpVtbl -> SetProperty(This,bstrPropId,pPropVal) ) 

#define IIGWorkspace_GetPropertiesAndValues(This,p_bstrPropId)	\
    ( (This)->lpVtbl -> GetPropertiesAndValues(This,p_bstrPropId) ) 

#define IIGWorkspace_Update(This)	\
    ( (This)->lpVtbl -> Update(This) ) 

#define IIGWorkspace_Copy(This,bstrImageGuid)	\
    ( (This)->lpVtbl -> Copy(This,bstrImageGuid) ) 

#define IIGWorkspace_Cut(This,bstrImageGuid)	\
    ( (This)->lpVtbl -> Cut(This,bstrImageGuid) ) 

#define IIGWorkspace_Paste(This,bstrImageGuid)	\
    ( (This)->lpVtbl -> Paste(This,bstrImageGuid) ) 

#define IIGWorkspace_GetFrameIds(This,p_bstrIds)	\
    ( (This)->lpVtbl -> GetFrameIds(This,p_bstrIds) ) 

#define IIGWorkspace_GetActiveFrameId(This,p_bstrIds)	\
    ( (This)->lpVtbl -> GetActiveFrameId(This,p_bstrIds) ) 

#define IIGWorkspace_GetFrameNames(This,p_bstrNames)	\
    ( (This)->lpVtbl -> GetFrameNames(This,p_bstrNames) ) 

#define IIGWorkspace_GetFrameNbLayers(This,p_bstrNbLayers)	\
    ( (This)->lpVtbl -> GetFrameNbLayers(This,p_bstrNbLayers) ) 

#define IIGWorkspace_GetFrameLayerVisibility(This,p_bstrLayerVisibility)	\
    ( (This)->lpVtbl -> GetFrameLayerVisibility(This,p_bstrLayerVisibility) ) 

#define IIGWorkspace_GetFrameProperties(This,p_bstrFrameProperties)	\
    ( (This)->lpVtbl -> GetFrameProperties(This,p_bstrFrameProperties) ) 

#define IIGWorkspace_GetFrameStepIds(This,p_bstrFrameStepIds)	\
    ( (This)->lpVtbl -> GetFrameStepIds(This,p_bstrFrameStepIds) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGWorkspace_INTERFACE_DEFINED__ */


#ifndef __IIGLayerManager_INTERFACE_DEFINED__
#define __IIGLayerManager_INTERFACE_DEFINED__

/* interface IIGLayerManager */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGLayerManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2EDC0F94-0772-41B3-97B5-363000DBF494")
    IIGLayerManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ IDispatch *pDispWindowOwner) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWorking( 
            /* [in] */ LONG nId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PopulateListBox( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGLayerManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGLayerManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGLayerManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGLayerManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGLayerManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGLayerManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGLayerManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGLayerManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGLayerManager * This,
            /* [in] */ IDispatch *pDispWindowOwner);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGLayerManager * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IIGLayerManager * This,
            /* [in] */ LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWorking )( 
            IIGLayerManager * This,
            /* [in] */ LONG nId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PopulateListBox )( 
            IIGLayerManager * This);
        
        END_INTERFACE
    } IIGLayerManagerVtbl;

    interface IIGLayerManager
    {
        CONST_VTBL struct IIGLayerManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGLayerManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGLayerManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGLayerManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGLayerManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGLayerManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGLayerManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGLayerManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGLayerManager_CreateFrame(This,pDispWindowOwner)	\
    ( (This)->lpVtbl -> CreateFrame(This,pDispWindowOwner) ) 

#define IIGLayerManager_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGLayerManager_Move(This,p_rc)	\
    ( (This)->lpVtbl -> Move(This,p_rc) ) 

#define IIGLayerManager_SetWorking(This,nId)	\
    ( (This)->lpVtbl -> SetWorking(This,nId) ) 

#define IIGLayerManager_PopulateListBox(This)	\
    ( (This)->lpVtbl -> PopulateListBox(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGLayerManager_INTERFACE_DEFINED__ */


#ifndef __IIGSimpleButton_INTERFACE_DEFINED__
#define __IIGSimpleButton_INTERFACE_DEFINED__

/* interface IIGSimpleButton */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGSimpleButton;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6866D3DC-9E1A-4C28-A7BA-06FF52B6DA7F")
    IIGSimpleButton : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hParent,
            /* [in] */ SHORT nButtonId,
            /* [in] */ LONG nCode,
            /* [in] */ LPRECT p_rc,
            /* [in] */ BSTR bstrResIconNormal,
            /* [in] */ BSTR bstrResIconPushed,
            /* [in] */ VARIANT_BOOL bDoubleState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Redraw( 
            /* [defaultvalue][in] */ VARIANT_BOOL bForce = -1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBackground( 
            LONG cBackground) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHWND( 
            /* [out] */ OLE_HANDLE *p_hWnd) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Pushed( 
            /* [in] */ VARIANT_BOOL bPushed) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Pushed( 
            /* [retval][out] */ VARIANT_BOOL *p_bPushed) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGSimpleButtonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGSimpleButton * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGSimpleButton * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGSimpleButton * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGSimpleButton * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGSimpleButton * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGSimpleButton * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGSimpleButton * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGSimpleButton * This,
            /* [in] */ OLE_HANDLE hParent,
            /* [in] */ SHORT nButtonId,
            /* [in] */ LONG nCode,
            /* [in] */ LPRECT p_rc,
            /* [in] */ BSTR bstrResIconNormal,
            /* [in] */ BSTR bstrResIconPushed,
            /* [in] */ VARIANT_BOOL bDoubleState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGSimpleButton * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IIGSimpleButton * This,
            LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Redraw )( 
            IIGSimpleButton * This,
            /* [defaultvalue][in] */ VARIANT_BOOL bForce);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBackground )( 
            IIGSimpleButton * This,
            LONG cBackground);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHWND )( 
            IIGSimpleButton * This,
            /* [out] */ OLE_HANDLE *p_hWnd);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Pushed )( 
            IIGSimpleButton * This,
            /* [in] */ VARIANT_BOOL bPushed);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Pushed )( 
            IIGSimpleButton * This,
            /* [retval][out] */ VARIANT_BOOL *p_bPushed);
        
        END_INTERFACE
    } IIGSimpleButtonVtbl;

    interface IIGSimpleButton
    {
        CONST_VTBL struct IIGSimpleButtonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGSimpleButton_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGSimpleButton_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGSimpleButton_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGSimpleButton_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGSimpleButton_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGSimpleButton_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGSimpleButton_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGSimpleButton_CreateFrame(This,hParent,nButtonId,nCode,p_rc,bstrResIconNormal,bstrResIconPushed,bDoubleState)	\
    ( (This)->lpVtbl -> CreateFrame(This,hParent,nButtonId,nCode,p_rc,bstrResIconNormal,bstrResIconPushed,bDoubleState) ) 

#define IIGSimpleButton_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGSimpleButton_Move(This,p_rc)	\
    ( (This)->lpVtbl -> Move(This,p_rc) ) 

#define IIGSimpleButton_Redraw(This,bForce)	\
    ( (This)->lpVtbl -> Redraw(This,bForce) ) 

#define IIGSimpleButton_SetBackground(This,cBackground)	\
    ( (This)->lpVtbl -> SetBackground(This,cBackground) ) 

#define IIGSimpleButton_GetHWND(This,p_hWnd)	\
    ( (This)->lpVtbl -> GetHWND(This,p_hWnd) ) 

#define IIGSimpleButton_put_Pushed(This,bPushed)	\
    ( (This)->lpVtbl -> put_Pushed(This,bPushed) ) 

#define IIGSimpleButton_get_Pushed(This,p_bPushed)	\
    ( (This)->lpVtbl -> get_Pushed(This,p_bPushed) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGSimpleButton_INTERFACE_DEFINED__ */


#ifndef __IIGScrollbar_INTERFACE_DEFINED__
#define __IIGScrollbar_INTERFACE_DEFINED__

/* interface IIGScrollbar */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGScrollbar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B822CA32-D89B-47FA-ACF2-988389F8AF9E")
    IIGScrollbar : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [defaultvalue][in] */ VARIANT_BOOL bHoriz = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGScrollbarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGScrollbar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGScrollbar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGScrollbar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGScrollbar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGScrollbar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGScrollbar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGScrollbar * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGScrollbar * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [defaultvalue][in] */ VARIANT_BOOL bHoriz);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGScrollbar * This);
        
        END_INTERFACE
    } IIGScrollbarVtbl;

    interface IIGScrollbar
    {
        CONST_VTBL struct IIGScrollbarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGScrollbar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGScrollbar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGScrollbar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGScrollbar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGScrollbar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGScrollbar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGScrollbar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGScrollbar_CreateFrame(This,hWnd,bHoriz)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,bHoriz) ) 

#define IIGScrollbar_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGScrollbar_INTERFACE_DEFINED__ */


#ifndef __IIGHistoryManager_INTERFACE_DEFINED__
#define __IIGHistoryManager_INTERFACE_DEFINED__

/* interface IIGHistoryManager */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGHistoryManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F70E29C1-72F8-4D25-810D-C5EFD7818BBE")
    IIGHistoryManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hParent,
            /* [defaultvalue][in] */ IDispatch *pDispDockPanel = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGHistoryManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGHistoryManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGHistoryManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGHistoryManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGHistoryManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGHistoryManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGHistoryManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGHistoryManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGHistoryManager * This,
            /* [in] */ OLE_HANDLE hParent,
            /* [defaultvalue][in] */ IDispatch *pDispDockPanel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGHistoryManager * This);
        
        END_INTERFACE
    } IIGHistoryManagerVtbl;

    interface IIGHistoryManager
    {
        CONST_VTBL struct IIGHistoryManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGHistoryManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGHistoryManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGHistoryManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGHistoryManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGHistoryManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGHistoryManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGHistoryManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGHistoryManager_CreateFrame(This,hParent,pDispDockPanel)	\
    ( (This)->lpVtbl -> CreateFrame(This,hParent,pDispDockPanel) ) 

#define IIGHistoryManager_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGHistoryManager_INTERFACE_DEFINED__ */


#ifndef __IIGToolBox_INTERFACE_DEFINED__
#define __IIGToolBox_INTERFACE_DEFINED__

/* interface IIGToolBox */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGToolBox;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E061C834-3DEB-462B-902F-24593A88799C")
    IIGToolBox : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            OLE_HANDLE hOwner,
            LPRECT p_rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Destroy( void) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_SelectedToolId( 
            /* [in] */ LONG nToolId) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SelectedToolId( 
            /* [retval][out] */ LONG *p_nToolId) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Frame( 
            /* [in] */ IDispatch *pDispFrame) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Frame( 
            /* [retval][out] */ IDispatch **pDispFrame) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGToolBoxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGToolBox * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGToolBox * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGToolBox * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGToolBox * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGToolBox * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGToolBox * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGToolBox * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Create )( 
            IIGToolBox * This,
            OLE_HANDLE hOwner,
            LPRECT p_rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Destroy )( 
            IIGToolBox * This);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_SelectedToolId )( 
            IIGToolBox * This,
            /* [in] */ LONG nToolId);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SelectedToolId )( 
            IIGToolBox * This,
            /* [retval][out] */ LONG *p_nToolId);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Frame )( 
            IIGToolBox * This,
            /* [in] */ IDispatch *pDispFrame);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Frame )( 
            IIGToolBox * This,
            /* [retval][out] */ IDispatch **pDispFrame);
        
        END_INTERFACE
    } IIGToolBoxVtbl;

    interface IIGToolBox
    {
        CONST_VTBL struct IIGToolBoxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGToolBox_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGToolBox_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGToolBox_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGToolBox_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGToolBox_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGToolBox_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGToolBox_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGToolBox_Create(This,hOwner,p_rc)	\
    ( (This)->lpVtbl -> Create(This,hOwner,p_rc) ) 

#define IIGToolBox_Destroy(This)	\
    ( (This)->lpVtbl -> Destroy(This) ) 

#define IIGToolBox_put_SelectedToolId(This,nToolId)	\
    ( (This)->lpVtbl -> put_SelectedToolId(This,nToolId) ) 

#define IIGToolBox_get_SelectedToolId(This,p_nToolId)	\
    ( (This)->lpVtbl -> get_SelectedToolId(This,p_nToolId) ) 

#define IIGToolBox_put_Frame(This,pDispFrame)	\
    ( (This)->lpVtbl -> put_Frame(This,pDispFrame) ) 

#define IIGToolBox_get_Frame(This,pDispFrame)	\
    ( (This)->lpVtbl -> get_Frame(This,pDispFrame) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGToolBox_INTERFACE_DEFINED__ */


#ifndef __IIGMainWindow_INTERFACE_DEFINED__
#define __IIGMainWindow_INTERFACE_DEFINED__

/* interface IIGMainWindow */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGMainWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A24EB55A-84ED-4D68-9638-17CD58ED8ED1")
    IIGMainWindow : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [defaultvalue][in] */ VARIANT_BOOL bIsServer = 0,
            /* [defaultvalue][in] */ BSTR bstrServerIP = 0,
            /* [defaultvalue][in] */ BSTR bstrOutputPath = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ SHORT nInputPortId,
            /* [in] */ BSTR bstrServerIp,
            /* [defaultvalue][in] */ OLE_HANDLE hWndStatus = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowOwner( 
            /* [in] */ BOOL bVisible) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIGInterface( 
            /* [in] */ REFIID iid,
            /* [out] */ OLE_HANDLE *p_hObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGMainWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGMainWindow * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGMainWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGMainWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGMainWindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGMainWindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGMainWindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGMainWindow * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGMainWindow * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [defaultvalue][in] */ VARIANT_BOOL bIsServer,
            /* [defaultvalue][in] */ BSTR bstrServerIP,
            /* [defaultvalue][in] */ BSTR bstrOutputPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGMainWindow * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IIGMainWindow * This,
            /* [in] */ SHORT nInputPortId,
            /* [in] */ BSTR bstrServerIp,
            /* [defaultvalue][in] */ OLE_HANDLE hWndStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowOwner )( 
            IIGMainWindow * This,
            /* [in] */ BOOL bVisible);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetIGInterface )( 
            IIGMainWindow * This,
            /* [in] */ REFIID iid,
            /* [out] */ OLE_HANDLE *p_hObject);
        
        END_INTERFACE
    } IIGMainWindowVtbl;

    interface IIGMainWindow
    {
        CONST_VTBL struct IIGMainWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGMainWindow_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGMainWindow_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGMainWindow_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGMainWindow_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGMainWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGMainWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGMainWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGMainWindow_CreateFrame(This,hWnd,bIsServer,bstrServerIP,bstrOutputPath)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,bIsServer,bstrServerIP,bstrOutputPath) ) 

#define IIGMainWindow_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#define IIGMainWindow_Connect(This,nInputPortId,bstrServerIp,hWndStatus)	\
    ( (This)->lpVtbl -> Connect(This,nInputPortId,bstrServerIp,hWndStatus) ) 

#define IIGMainWindow_ShowOwner(This,bVisible)	\
    ( (This)->lpVtbl -> ShowOwner(This,bVisible) ) 

#define IIGMainWindow_GetIGInterface(This,iid,p_hObject)	\
    ( (This)->lpVtbl -> GetIGInterface(This,iid,p_hObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGMainWindow_INTERFACE_DEFINED__ */


#ifndef __IIGPropertyManager_INTERFACE_DEFINED__
#define __IIGPropertyManager_INTERFACE_DEFINED__

/* interface IIGPropertyManager */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGPropertyManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8DD8ADB6-DD9F-405E-B82A-B691740A54FA")
    IIGPropertyManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFrame( 
            /* [in] */ OLE_HANDLE hWnd,
            /* [defaultvalue][in] */ IDispatch *pDispDockPanel = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyFrame( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGPropertyManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGPropertyManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGPropertyManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGPropertyManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGPropertyManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGPropertyManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGPropertyManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGPropertyManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFrame )( 
            IIGPropertyManager * This,
            /* [in] */ OLE_HANDLE hWnd,
            /* [defaultvalue][in] */ IDispatch *pDispDockPanel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyFrame )( 
            IIGPropertyManager * This);
        
        END_INTERFACE
    } IIGPropertyManagerVtbl;

    interface IIGPropertyManager
    {
        CONST_VTBL struct IIGPropertyManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGPropertyManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGPropertyManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGPropertyManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGPropertyManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGPropertyManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGPropertyManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGPropertyManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGPropertyManager_CreateFrame(This,hWnd,pDispDockPanel)	\
    ( (This)->lpVtbl -> CreateFrame(This,hWnd,pDispDockPanel) ) 

#define IIGPropertyManager_DestroyFrame(This)	\
    ( (This)->lpVtbl -> DestroyFrame(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGPropertyManager_INTERFACE_DEFINED__ */


#ifndef __IIGSocket_INTERFACE_DEFINED__
#define __IIGSocket_INTERFACE_DEFINED__

/* interface IIGSocket */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIGSocket;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("08F352B0-0D73-40F0-B848-C6A056CD3BB2")
    IIGSocket : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateConnection( 
            /* [in] */ OLE_HANDLE hWndApp,
            /* [in] */ SHORT nInputPortId,
            /* [in] */ BSTR bstrServerIp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyConnection( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIGSocketVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIGSocket * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIGSocket * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIGSocket * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIGSocket * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIGSocket * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIGSocket * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIGSocket * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateConnection )( 
            IIGSocket * This,
            /* [in] */ OLE_HANDLE hWndApp,
            /* [in] */ SHORT nInputPortId,
            /* [in] */ BSTR bstrServerIp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyConnection )( 
            IIGSocket * This);
        
        END_INTERFACE
    } IIGSocketVtbl;

    interface IIGSocket
    {
        CONST_VTBL struct IIGSocketVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIGSocket_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIGSocket_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIGSocket_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIGSocket_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIGSocket_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIGSocket_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIGSocket_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIGSocket_CreateConnection(This,hWndApp,nInputPortId,bstrServerIp)	\
    ( (This)->lpVtbl -> CreateConnection(This,hWndApp,nInputPortId,bstrServerIp) ) 

#define IIGSocket_DestroyConnection(This)	\
    ( (This)->lpVtbl -> DestroyConnection(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIGSocket_INTERFACE_DEFINED__ */



#ifndef __IGPictureEditorLib_LIBRARY_DEFINED__
#define __IGPictureEditorLib_LIBRARY_DEFINED__

/* library IGPictureEditorLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IGPictureEditorLib;

#ifndef ___IMainWindowEvents_DISPINTERFACE_DEFINED__
#define ___IMainWindowEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IMainWindowEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IMainWindowEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1D7AD6A3-482B-47FB-89C3-B5AAF14B428E")
    _IMainWindowEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IMainWindowEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IMainWindowEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IMainWindowEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IMainWindowEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IMainWindowEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IMainWindowEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IMainWindowEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IMainWindowEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IMainWindowEventsVtbl;

    interface _IMainWindowEvents
    {
        CONST_VTBL struct _IMainWindowEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IMainWindowEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IMainWindowEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IMainWindowEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IMainWindowEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IMainWindowEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IMainWindowEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IMainWindowEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IMainWindowEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MainWindow;

#ifdef __cplusplus

class DECLSPEC_UUID("91E7B5FC-0BF6-4F35-8186-EF0104A479E1")
MainWindow;
#endif

#ifndef ___IIGFrameEvents_DISPINTERFACE_DEFINED__
#define ___IIGFrameEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGFrameEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGFrameEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B126F24F-C92E-4F99-A4D9-7E224F4E9E7B")
    _IIGFrameEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGFrameEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGFrameEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGFrameEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGFrameEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGFrameEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGFrameEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGFrameEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGFrameEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGFrameEventsVtbl;

    interface _IIGFrameEvents
    {
        CONST_VTBL struct _IIGFrameEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGFrameEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGFrameEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGFrameEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGFrameEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGFrameEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGFrameEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGFrameEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGFrameEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGFrameControl;

#ifdef __cplusplus

class DECLSPEC_UUID("73090B4B-ABB2-4656-BC83-7D9F714E99C8")
IGFrameControl;
#endif

#ifndef ___IIGMultiFrameEvents_DISPINTERFACE_DEFINED__
#define ___IIGMultiFrameEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGMultiFrameEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGMultiFrameEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("2F3202D4-EE48-44BE-B9AC-D0A070826479")
    _IIGMultiFrameEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGMultiFrameEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGMultiFrameEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGMultiFrameEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGMultiFrameEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGMultiFrameEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGMultiFrameEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGMultiFrameEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGMultiFrameEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGMultiFrameEventsVtbl;

    interface _IIGMultiFrameEvents
    {
        CONST_VTBL struct _IIGMultiFrameEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGMultiFrameEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGMultiFrameEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGMultiFrameEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGMultiFrameEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGMultiFrameEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGMultiFrameEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGMultiFrameEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGMultiFrameEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGMultiFrame;

#ifdef __cplusplus

class DECLSPEC_UUID("C06A446A-A4C0-4D97-8A0A-A128CE852CC3")
IGMultiFrame;
#endif

#ifndef ___IIGMenuBarEvents_DISPINTERFACE_DEFINED__
#define ___IIGMenuBarEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGMenuBarEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGMenuBarEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("03C242A1-B601-40D4-9AAD-139771524F54")
    _IIGMenuBarEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGMenuBarEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGMenuBarEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGMenuBarEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGMenuBarEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGMenuBarEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGMenuBarEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGMenuBarEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGMenuBarEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGMenuBarEventsVtbl;

    interface _IIGMenuBarEvents
    {
        CONST_VTBL struct _IIGMenuBarEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGMenuBarEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGMenuBarEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGMenuBarEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGMenuBarEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGMenuBarEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGMenuBarEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGMenuBarEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGMenuBarEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGMenuBar;

#ifdef __cplusplus

class DECLSPEC_UUID("E53E76E2-ABF8-4748-8B12-5890166EBC3E")
IGMenuBar;
#endif

#ifndef ___IIGMenuBarDropButtonEvents_DISPINTERFACE_DEFINED__
#define ___IIGMenuBarDropButtonEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGMenuBarDropButtonEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGMenuBarDropButtonEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BBE74F2F-D803-473E-8D69-EC0C2CE00CE5")
    _IIGMenuBarDropButtonEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGMenuBarDropButtonEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGMenuBarDropButtonEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGMenuBarDropButtonEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGMenuBarDropButtonEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGMenuBarDropButtonEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGMenuBarDropButtonEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGMenuBarDropButtonEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGMenuBarDropButtonEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGMenuBarDropButtonEventsVtbl;

    interface _IIGMenuBarDropButtonEvents
    {
        CONST_VTBL struct _IIGMenuBarDropButtonEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGMenuBarDropButtonEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGMenuBarDropButtonEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGMenuBarDropButtonEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGMenuBarDropButtonEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGMenuBarDropButtonEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGMenuBarDropButtonEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGMenuBarDropButtonEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGMenuBarDropButtonEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGMenuBarDropButton;

#ifdef __cplusplus

class DECLSPEC_UUID("BAE17D3F-8CB4-4B0C-A2C6-650B534F5367")
IGMenuBarDropButton;
#endif

EXTERN_C const CLSID CLSID_IGLibraryMenuButton;

#ifdef __cplusplus

class DECLSPEC_UUID("42CFF718-C1D7-4162-8950-BADC1AEEF87B")
IGLibraryMenuButton;
#endif

#ifndef ___IIGImageLibraryEvents_DISPINTERFACE_DEFINED__
#define ___IIGImageLibraryEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGImageLibraryEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGImageLibraryEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6771CB5B-CFF8-42DA-836E-BA94E7FE2F2C")
    _IIGImageLibraryEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGImageLibraryEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGImageLibraryEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGImageLibraryEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGImageLibraryEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGImageLibraryEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGImageLibraryEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGImageLibraryEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGImageLibraryEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGImageLibraryEventsVtbl;

    interface _IIGImageLibraryEvents
    {
        CONST_VTBL struct _IIGImageLibraryEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGImageLibraryEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGImageLibraryEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGImageLibraryEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGImageLibraryEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGImageLibraryEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGImageLibraryEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGImageLibraryEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGImageLibraryEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGImageLibrary;

#ifdef __cplusplus

class DECLSPEC_UUID("01A323CA-7FCC-4AF1-86A9-38F17A92BCA5")
IGImageLibrary;
#endif

#ifndef ___IIGDockPanelEvents_DISPINTERFACE_DEFINED__
#define ___IIGDockPanelEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGDockPanelEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGDockPanelEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("931738C4-FB98-4c62-8E5A-D0924099654F")
    _IIGDockPanelEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGDockPanelEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGDockPanelEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGDockPanelEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGDockPanelEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGDockPanelEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGDockPanelEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGDockPanelEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGDockPanelEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGDockPanelEventsVtbl;

    interface _IIGDockPanelEvents
    {
        CONST_VTBL struct _IIGDockPanelEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGDockPanelEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGDockPanelEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGDockPanelEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGDockPanelEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGDockPanelEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGDockPanelEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGDockPanelEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGDockPanelEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGDockPanel;

#ifdef __cplusplus

class DECLSPEC_UUID("F1B854B3-C4D6-4FAC-8114-1B6789B54758")
IGDockPanel;
#endif

#ifndef ___IIGWorkspaceEvents_DISPINTERFACE_DEFINED__
#define ___IIGWorkspaceEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGWorkspaceEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGWorkspaceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BBD00962-6F98-45C4-8BB1-3AB742F2F3A5")
    _IIGWorkspaceEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGWorkspaceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGWorkspaceEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGWorkspaceEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGWorkspaceEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGWorkspaceEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGWorkspaceEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGWorkspaceEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGWorkspaceEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGWorkspaceEventsVtbl;

    interface _IIGWorkspaceEvents
    {
        CONST_VTBL struct _IIGWorkspaceEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGWorkspaceEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGWorkspaceEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGWorkspaceEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGWorkspaceEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGWorkspaceEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGWorkspaceEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGWorkspaceEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGWorkspaceEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGWorkspace;

#ifdef __cplusplus

class DECLSPEC_UUID("008C3FE6-C81C-4D05-9F9D-0F3E0ADB672C")
IGWorkspace;
#endif

#ifndef ___IIGLayerManagerEvents_DISPINTERFACE_DEFINED__
#define ___IIGLayerManagerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IIGLayerManagerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IIGLayerManagerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D42A7974-A9FA-4400-A267-ED450997166D")
    _IIGLayerManagerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IIGLayerManagerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IIGLayerManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IIGLayerManagerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IIGLayerManagerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IIGLayerManagerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IIGLayerManagerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IIGLayerManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IIGLayerManagerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IIGLayerManagerEventsVtbl;

    interface _IIGLayerManagerEvents
    {
        CONST_VTBL struct _IIGLayerManagerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IIGLayerManagerEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IIGLayerManagerEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IIGLayerManagerEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IIGLayerManagerEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IIGLayerManagerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IIGLayerManagerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IIGLayerManagerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IIGLayerManagerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_IGLayerManager;

#ifdef __cplusplus

class DECLSPEC_UUID("8E8BAC98-FAB9-4434-B40B-5B0CD1162C0C")
IGLayerManager;
#endif

EXTERN_C const CLSID CLSID_IGPropertyManager;

#ifdef __cplusplus

class DECLSPEC_UUID("2C692EC6-97F7-45B2-8F7D-DA597552223F")
IGPropertyManager;
#endif

EXTERN_C const CLSID CLSID_IGFramePropertyManager;

#ifdef __cplusplus

class DECLSPEC_UUID("020BBAD4-52E8-44A2-9231-B4CD7858CE7B")
IGFramePropertyManager;
#endif

EXTERN_C const CLSID CLSID_IGSimpleButton;

#ifdef __cplusplus

class DECLSPEC_UUID("F9E65B70-C1E5-4058-B587-9BA98BC59C83")
IGSimpleButton;
#endif

EXTERN_C const CLSID CLSID_IGScrollbar;

#ifdef __cplusplus

class DECLSPEC_UUID("D252C9E8-0B65-41FC-9689-CC61CE096853")
IGScrollbar;
#endif

EXTERN_C const CLSID CLSID_IGHistoryManager;

#ifdef __cplusplus

class DECLSPEC_UUID("9B8BD9F2-10F5-40E2-B33F-3AC8F5E3AD9B")
IGHistoryManager;
#endif

EXTERN_C const CLSID CLSID_IGToolBox;

#ifdef __cplusplus

class DECLSPEC_UUID("44C6FEE5-826A-419C-AE60-1A7FC7F08E0C")
IGToolBox;
#endif

EXTERN_C const CLSID CLSID_IGMainWindow;

#ifdef __cplusplus

class DECLSPEC_UUID("DF95F65E-E9E9-4B63-893D-D484C77CE09E")
IGMainWindow;
#endif

EXTERN_C const CLSID CLSID_IGSocket;

#ifdef __cplusplus

class DECLSPEC_UUID("18B387CD-5517-4173-A28E-12CE04FC71CD")
IGSocket;
#endif
#endif /* __IGPictureEditorLib_LIBRARY_DEFINED__ */

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

#endif


