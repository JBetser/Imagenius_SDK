// IGSocket.cpp : Implementation of CIGSocket

#include "stdafx.h"
#include "IGSocket.h"
#include <IGUserMessages.h>
#include <IGFrameAction.h>
#include <IGAnswer.h>
#include <IGLog.h>

#define IGSOCKET_ERROR				L"IGSocket error"
#define	IGSOCKET_TIMER_EVENT		256

using namespace std;
using namespace IGLibrary;

// CIGSocket
CIGSocket::CIGSocket()	: m_socket (NULL)
						, m_hWndApplication (NULL)
						, m_nInputPortId (-1)
						, m_bIsConnected (false)
						, m_nTimeOutTicks (0)
						, m_nHearthbeatTicks (0)
{
	ZeroMemory (&m_socketInfo, sizeof (SOCKADDR_IN));
}

CIGSocket::~CIGSocket()
{
}

STDMETHODIMP CIGSocket::CreateConnection (OLE_HANDLE hWndApp, SHORT nInputPortId, BSTR bstrServerIp)
{
	m_hWndApplication = (HWND)hWndApp;
	m_spbstrServerIp = bstrServerIp;
	m_nInputPortId = nInputPortId;
	::SetWindowSubclass (m_hWndApplication, StaticHookAppWndProc, 0, (DWORD_PTR)this);
	if (!listenOnPort (m_nInputPortId))
	{
		DestroyConnection();
		return E_FAIL;
	}
	::SetTimer (m_hWndApplication, IGSOCKET_TIMER_EVENT, IGSOCKET_TIMER_INTERVAL, NULL);
	return S_OK;
}

STDMETHODIMP CIGSocket::DestroyConnection()
{
	::KillTimer (m_hWndApplication, IGSOCKET_TIMER_EVENT);
	::RemoveWindowSubclass (m_hWndApplication, StaticHookAppWndProc, 0);
	if (m_socket)
	{
		closesocket (m_socket); //Shut down socket
		WSACleanup(); //Clean up Winsock
	}
	return S_OK;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGSocket::StaticHookAppWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
														UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGSocket* pThis = (CIGSocket*)dwRefData;
	if (pThis != NULL)
		return pThis->HookAppWndProc (hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGSocket::HookAppWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_TIMER:
		if (wParam == IGSOCKET_TIMER_EVENT){
			if (!processRequests()){
				IGLog log;
				log.Add (L"IGSocket Error - Failed to process a request.");
				harakiri();
				return false;
			}
			if (!processAnswers()){
				IGLog log;
				log.Add (L"IGSocket Error - Failed to send an answer.");
				harakiri();
				return false;
			}
		}
		break;
	// Image processing result message
	case UM_IMAGEPROCESSING:
		{
			int nRes = IGRequest::OnProcessEnded (wParam, lParam);
			if (nRes == IGREQUEST_FAILED){
				IGLog log;
				log.Add (L"IGSocket Error - Error finalizing a request process.");
			}
			return nRes;
		}
	//Winsock related message...
    case UM_WINSOCK:
        switch (lParam)
        {
			case FD_CONNECT: //Connection OK
				{
					IGLog log;
					log.Add (L"IGSocket Info - Connection Established.");
				}
			break;
	        
			case FD_CLOSE: //Lost connection
				//Clean up
				{
					if (m_socket) closesocket(m_socket);
					WSACleanup();

					IGLog log;
					log.Add (L"IGSocket Warning - Connection to Remote Host Lost.");

					// reset connection
					listenOnPort (m_nInputPortId);
				}
			break;
	        
			case FD_READ: //Incoming data to receive
				char cBuffer [IGSOCKET_MAXREQUESTLEN];
				ZeroMemory (cBuffer, sizeof (cBuffer)); //Clear the buffer

				//Get the data
				if (recv (m_socket, cBuffer, sizeof (cBuffer) - 1, 0) == SOCKET_ERROR)
				{
					wchar_t wErrorMessage[256];
					wsprintf (wErrorMessage, L"IGSocket Error - Failed to receive a message. WSA Error code: %d.",
								WSAGetLastError()); 
					IGLog log;
					log.Add (wErrorMessage);
				}
				else
				{
					if (m_lsRequests.size() >= IGREQUESTQUEUE_MAXNUMBER){
						IGLog log;
						log.Add (L"IGSocket Error - Maximum number of requests reached.");
					}
					string sRequest (cBuffer);
					size_t szOffset = 0;
					size_t szNextOffset = 0;
					while (szOffset != string::npos && sRequest.size() > szOffset)
					{
						szNextOffset = sRequest.find ("/Request>", szOffset + 1);
                        if (szNextOffset == -1)
                        {
                            m_sCurRequest += sRequest.substr (szOffset);
                            break;
                        }
                        else
                        {
                            m_sCurRequest += sRequest.substr (szOffset,  szNextOffset + 9 - szOffset);
                            m_lsRequests.push_back (m_sCurRequest);
                            m_sCurRequest = "";
                            szOffset = szNextOffset + 9;
                        }  
					}
				}
			break;

			case FD_ACCEPT: //Connection request
			{
				MessageBeep(MB_OK);
				int nSocketInfoLen = sizeof (m_socketInfo);
				SOCKET TempSock = accept (m_socket, (struct sockaddr*)&m_socketInfo, &nSocketInfoLen);
				m_socket = TempSock; //Switch our old socket to the new one
				char szAcceptAddr[100];
				wsprintfA (szAcceptAddr, "IGSocket Info - Connection from [%s] accepted.",
							inet_ntoa (m_socketInfo.sin_addr)); 
				wchar_t wAcceptAddr[100];
				::mbstowcs (wAcceptAddr, szAcceptAddr, 100);
				IGLog log;
				log.Add (wAcceptAddr);	
				m_bIsConnected = true;
			}
			break;

			case FD_WRITE:
			{
				IGSOCKET_WRITE *pRequest = (IGSOCKET_WRITE*)wParam;
				_ASSERTE (pRequest && L"IGSocket Error - NULL request");
				if (pRequest)
				{
					if ((pRequest->nSize != 0) && (pRequest->nSize < IGSOCKET_MAXREQUESTLEN))
					{
						if (m_socket)
						{
							// Send the packet
							int nReqLen = strlen (&pRequest->cXml[0]);
							int nRes = send (m_socket, &pRequest->cXml[0], nReqLen, 0);
							if (nRes != nReqLen){
								IGLog log;
								log.Add (L"IGSocket Error - Error sending a request");
								harakiri();
							}
						}
						else
						{
							//We aren't connected!!
							IGLog log;
							log.Add (L"IGSocket Critical Error - No connection established");
							harakiri();
						}						
					}
					else
					{
						IGLog log;
						if (pRequest->nSize == 0)
							log.Add (L"IGSocket Critical Error - Null request");		
						else
							log.Add (L"IGSocket Critical Error - Request over maximum size");		
						harakiri();
					}
					delete pRequest;
				}
			}
			break;
        }
    break;
	}
	return ::DefSubclassProc (hWnd, msg, wParam, lParam);
}

bool CIGSocket::listenOnPort (SHORT nPortId)
{
	IGLog log;
	m_bIsConnected = false;

	if (m_socket)
	{
		closesocket (m_socket); //Shut down socket
		WSACleanup(); //Clean up Winsock
	}

    WSADATA w;    
    int error = WSAStartup (0x0202, &w);   // Fill in WSA info
     
    if (error)
    { // there was an error
		log.Add (L"Could not initialize Winsock.");
		return false;
    }
    if (w.wVersion != 0x0202)
    { // wrong WinSock version!
		WSACleanup(); // unload ws2_32.dll
		log.Add (L"Wrong Winsock version.");
		return false;
    }
    
    SOCKADDR_IN addr; // the address structure for a TCP socket
   
    addr.sin_family = AF_INET;      // Address family Internet
    addr.sin_port = htons (nPortId);   // Assign port to this socket
    addr.sin_addr.s_addr = htonl (INADDR_ANY);   // No destination
    
    m_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create socket
    
    if (m_socket == INVALID_SOCKET)
    {
		log.Add (L"Could not create socket.");
		return false;
    }
    
    if (::bind (m_socket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) //Try binding
    { // error
		log.Add (L"Could not bind to IP.");
		return false;
    }
    
    listen (m_socket, 10); //Start listening
    WSAAsyncSelect (m_socket, m_hWndApplication, 1045, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT); //Switch to Non-Blocking mode
    
	wchar_t szTemp[300];
    ::wsprintf (szTemp, L"IGServer - Port %d", nPortId);
	::SetWindowText (m_hWndApplication, szTemp);
    
    return true;
}

bool CIGSocket::processAnswers()
{
	if (m_socket)
	{
		list <string>& lAnswerBuffer = IGAnswer::g_safelAnswerBuffer.Lock();
		if (lAnswerBuffer.size() > 0)
		{
			int nIdxAnswer = 0;
			string sAnswerBuffer;
			list <string>::iterator itAnswer = lAnswerBuffer.begin();
			do{
				sAnswerBuffer += *itAnswer;
			} while ((++nIdxAnswer < IGSOCKET_MAXNBANSWERS) && (++itAnswer != lAnswerBuffer.end()));
			int nNbAnswersToRemove = min (nIdxAnswer, IGSOCKET_MAXNBANSWERS);
			while (nNbAnswersToRemove--)
				lAnswerBuffer.pop_front();
			if (sAnswerBuffer.size() >= IGSOCKET_MAXANSWERLEN)
			{
				IGLog log;
				log.Add (L"IGAnswer Critical Error - Answer over maximum size");
				IGAnswer::g_safelAnswerBuffer.UnLock();
				return false;
			}
			// Send the packet
			try{
				int nRes;
				do{
					nRes = send (m_socket, sAnswerBuffer.c_str(), sAnswerBuffer.size(), 0);
					if (nRes == SOCKET_ERROR){
						wchar_t codeError[32];
						::_itow_s(::WSAGetLastError(), codeError, 10);
						IGLog log;
						log.Add (L"IGSocket Error - Error sending the answer: ");
						log.Add (sAnswerBuffer.c_str());						
						log.Add (L". Error Code: ");
						log.Add (codeError);
						IGAnswer::g_safelAnswerBuffer.UnLock();
						return false;
					}
					if (nRes < (int)sAnswerBuffer.size())
						sAnswerBuffer = sAnswerBuffer.substr (nRes,  sAnswerBuffer.size() - nRes);
					else
						sAnswerBuffer.clear();
				}
				while (!sAnswerBuffer.empty());
			}
			catch(...)
			{
				IGLog log;
				log.Add (L"IGSocket Error - Error sending the answer: ");
				log.Add (sAnswerBuffer.c_str());
				IGAnswer::g_safelAnswerBuffer.UnLock();
				return false;
			}
		}
		IGAnswer::g_safelAnswerBuffer.UnLock();
	}
	return true;
}

bool CIGSocket::processRequests()
{
	if (m_bIsConnected)
	{
		if (!IGRequest::ProcessRequests (m_lsRequests)){
			IGLog log;
			log.Add (L"IGSocket Error - A corrupted request has been received");
			return false;
		}
		if (m_nHearthbeatTicks++ == IGSOCKET_HEARTHBEAT_NBTIMERTICKS)
		{
			m_nHearthbeatTicks = 0;
			IGAnswerHearthbeat hearthbeat;
			hearthbeat.Write();
		}
	}
	else
	{
		if (m_nTimeOutTicks++ * IGSOCKET_TIMER_INTERVAL > IGSOCKET_TIMER_CONNECTION_TIMEOUT){
			IGLog log;
			log.Add (L"IGSocket Error - Connection timeout");
			return false;
		}
	}
	return true;
}

void CIGSocket::harakiri()
{
	try{
		IGLog log;
		log.Add (L"IGSocket Critical Error - leave the process.");
	}
	catch(...){}
#ifndef DEBUG
	exit(0);
#endif
}