#pragma once
#include "resource.h"
#include "IGLibrary.h"

// this class handles : 
//		- frame zoom management
//		- frame position management
//		- frame render scheduling

#define IGFRAME_BORDERWIDTH				2
#define	IGFRAME_TIMEREVENT_MOVETOLEFT	1
#define	IGFRAME_TIMEREVENT_MOVETORIGHT	2
#define	IGFRAME_TIMEREVENT_MOVETOTOP	4
#define	IGFRAME_TIMEREVENT_MOVETOBOTTOM	8
#define	IGFRAME_TIMEREVENT_RENDER		100
#define IGFRAME_TIMERRENDER_INTERVAL	200	
#define IGFRAME_NBTICKSBEFORESTART		10
#define IGFRAME_MINFRAMESIZE			40	// under this frame size, no frame moving
#define IGFRAME_MOVINGBORDER_MINWIDTH	10	// minimum width of the area for frame moving
#define IGFRAME_MOVINGBORDER_WIDTHPERCENT	0.05f	// width ratio of the area for frame moving
#define IGFRAME_ZOOMINCREASE			0.1f // increase zoom by 10% by mousewheel
#define IGFRAME_MAXZOOM					32.0f // max zoom is 3200%
#define IGFRAME_MOVINGFRAME_OFFSET		3
#define IGFRAME_MINSIZE					20

namespace IGLibrary
{
	template <class T>
	class IGView
	{
	public:
		IGView(HWND hOwner) : m_hOwner (hOwner)
							, m_fZoomBestFit (0.0f)
							, m_fZoomToFrame (0.0f)
							, m_fCoordFromCenterX (0.0f)
							, m_fCoordFromCenterY (0.0f)
							, m_uiCurrentTimerEvent (0)
							, m_nNbTicks (0)
		{
			::SetWindowSubclass (m_hOwner, StaticHookFrameProc, 0, (DWORD_PTR)this);
			if (g_nNbRefs++ == 0)
			{
				g_hCursorUp = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWUP), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorDown = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWDOWN), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorLeft = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWLEFT), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorRight = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWRIGHT), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorUpLeft = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWUPLEFT), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorDownLeft = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWDOWNLEFT), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorUpRight = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWUPRIGHT), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
				g_hCursorDownRight = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_ARROWDOWNRIGHT), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
			}
		}

		virtual ~IGView(void)
		{
			if (--g_nNbRefs == 0)
			{
				::DestroyCursor (g_hCursorUp);
				::DestroyCursor (g_hCursorDown);
				::DestroyCursor (g_hCursorLeft);
				::DestroyCursor (g_hCursorRight);
				::DestroyCursor (g_hCursorUpLeft);
				::DestroyCursor (g_hCursorDownLeft);
				::DestroyCursor (g_hCursorUpRight);
				::DestroyCursor (g_hCursorDownRight);
			}
			::RemoveWindowSubclass (m_hOwner, StaticHookFrameProc, 0);
		}		

		void GetCurrentCoordFromCenter (float &fCenterX, float &fCenterY) const
		{
			fCenterX = m_fCoordFromCenterX;
			fCenterY = m_fCoordFromCenterY;
		}		

		// Zoom in direction to a specific coordinates by nDelta number of mouse wheel ticks
		void ZoomToPoint(int nPosX, int nPosY, int nDelta)
		{
			float fCurrentZoom = ((T*)this)->GetCurrentZoom();
			if (fCurrentZoom * (1.0f + (float)nDelta * IGFRAME_ZOOMINCREASE) > IGFRAME_MAXZOOM)
				return;
			int nFrameWidth = ((T*)this)->GetFrameWidth();
			int nFrameHeight = ((T*)this)->GetFrameHeight();
			int nImageWidth = ((T*)this)->GetWidth();
			int nImageHeight = ((T*)this)->GetHeight();
			float fImageDisplayedWidth = (float)nImageWidth * fCurrentZoom;
			float fImageDisplayedHeight = (float)nImageHeight * fCurrentZoom;
			float fCoordFromCenterX = ((float)nPosX - (float)nFrameWidth / 2.0f) - m_fCoordFromCenterX;
			float fCoordFromCenterY = ((float)nPosY - (float)nFrameHeight / 2.0f) - m_fCoordFromCenterY;
			if (((fCoordFromCenterX > -1.0f * fImageDisplayedWidth / 2.0f) && (fCoordFromCenterX < fImageDisplayedWidth / 2.0f)) 
				&& ((fCoordFromCenterY > -1.0f * fImageDisplayedHeight / 2.0f) && (fCoordFromCenterY < fImageDisplayedHeight / 2.0f)))
			{
				if (fImageDisplayedWidth < (float)nFrameWidth)
					m_fCoordFromCenterX = 0.0f;
				else
					m_fCoordFromCenterX -= fCoordFromCenterX * (float)nDelta * IGFRAME_ZOOMINCREASE;
				
				if (fImageDisplayedHeight < (float)nFrameHeight)
					m_fCoordFromCenterY = 0.0f;
				else
					m_fCoordFromCenterY -= fCoordFromCenterY * (float)nDelta * IGFRAME_ZOOMINCREASE;
			}
			else
			{
				m_fCoordFromCenterX = 0.0f;
				m_fCoordFromCenterY = 0.0f;
			}
			ZoomIncrease ((float)nDelta * IGFRAME_ZOOMINCREASE);
		}

		// Compute to highest zoom with image still inside the frame
		// Zoom cannot exceed 100% 
		void ZoomBestFit()
		{
			ZoomToFrame();
			((T*)this)->SetCurrentZoom (m_fZoomBestFit);
		}

		// Compute to highest zoom with image still inside the frame
		// Zoom can exceed 100% 
		void ZoomToFrame()
		{
			if (((T*)this)->GetNbLayers() > 0)
			{
				int nFrameWidth = ((T*)this)->GetFrameWidth();
				int nFrameHeight = ((T*)this)->GetFrameHeight();
				int nImageWidth = ((T*)this)->GetWidth();
				int nImageHeight = ((T*)this)->GetHeight();	
				if ((nImageWidth > 0) && (nImageHeight > 0))
				{
					m_fZoomToFrame = ((float)nFrameWidth / (float)nFrameHeight) > ((float)nImageWidth / (float)nImageHeight) ?
						((float)nFrameHeight / (float)nImageHeight) : ((float)nFrameWidth / (float)nImageWidth);
					m_fZoomBestFit = m_fZoomToFrame > 1.0f ? 1.0f : m_fZoomToFrame;
					((T*)this)->SetCurrentZoom (m_fZoomToFrame);
					m_fCoordFromCenterX = 0.0f;
					m_fCoordFromCenterY = 0.0f;
				}
			}
		}

		void ZoomIncrease (float fIncrease)
		{
			float fCurrentZoom = ((T*)this)->GetCurrentZoom();
			if ((fCurrentZoom * (1.0f + fIncrease)) > IGFRAME_MAXZOOM)
				return;
			// check that frame size is higher than the minimum
			if (((int)(fCurrentZoom * (1.0f + fIncrease) * (float)((T*)this)->GetWidth()) < IGFRAME_MINSIZE) 
				&& (fCurrentZoom < 1.0f) && (fIncrease < 0.0f))
				return;
			if (((int)(fCurrentZoom * (1.0f + fIncrease) * (float)((T*)this)->GetHeight()) < IGFRAME_MINSIZE) 
				&& (fCurrentZoom < 1.0f) && (fIncrease < 0.0f))
				return;
			((T*)this)->SetCurrentZoom ((1.0f + fIncrease) * fCurrentZoom);
		}

		// get the current frame corrdinates
		void GetFramePos(int &nZoomPosX, int &nZoomPosY, int &nZoomWidth, int &nZoomHeight)
		{
			if (((T*)this)->GetNbLayers() > 0)
			{
				int nFrameWidth = ((T*)this)->GetFrameWidth();
				int nFrameHeight = ((T*)this)->GetFrameHeight();
				int nImageWidth = ((T*)this)->GetWidth();
				int nImageHeight = ((T*)this)->GetHeight();
				float fCurrentZoom = ((T*)this)->GetCurrentZoom();
				int nFrameLeft = (int)((float)nFrameWidth / 2.0f + m_fCoordFromCenterX - (float)nImageWidth * fCurrentZoom / 2.0f);
				int nFrameTop = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY - (float)nImageHeight * fCurrentZoom / 2.0f);
				nZoomPosX = nFrameLeft;
				nZoomPosY = nFrameTop;
				nZoomWidth = (int)(fCurrentZoom * (float)nImageWidth);
				nZoomHeight = (int)(fCurrentZoom * (float)nImageHeight);
			}
		}

		// get the current layer pixel corrdinates
		void ConvertCoordFrameToPixel (int *p_nFrameToPixX, int *p_nFrameToPixY)
		{
			*p_nFrameToPixY += 6; // ?? strange offset...
			int nZoomPosX, nZoomPosY, nZoomPosWidth, nZoomPosHeight;
			GetFramePos (nZoomPosX, nZoomPosY, nZoomPosWidth, nZoomPosHeight);
			CxImage *pLayer = ((T*)this)->GetWorkingLayer();
			*p_nFrameToPixX -= nZoomPosX;
			*p_nFrameToPixY -= nZoomPosY;
			float fCurrentZoom = ((T*)this)->GetCurrentZoom();
			*p_nFrameToPixX = (int)((float)*p_nFrameToPixX / fCurrentZoom);
			*p_nFrameToPixY = (int)((float)*p_nFrameToPixY / fCurrentZoom);
			int nLayerOriginX = 0;
			int nLayerOriginY = 0;
			pLayer->GetOriginCoordinates (nLayerOriginX, nLayerOriginY);
			*p_nFrameToPixX -= nLayerOriginX;
			*p_nFrameToPixY -= nLayerOriginY;
		}

		void ThrowDelayedRendering ()
		{
			if (!m_hOwner)
				return;
			::SetTimer (m_hOwner, IGFRAME_TIMEREVENT_RENDER, IGFRAME_TIMERRENDER_INTERVAL, NULL);
		}

	private:
		// owner window subclass
		static LRESULT APIENTRY StaticHookFrameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
		{
			IGView <T>* pThis = (IGView <T>*)dwRefData;
			if( pThis != NULL )
				return pThis->HookFrameProc(hwnd, msg, wParam, lParam);

			return ::DefSubclassProc(hwnd, msg, wParam, lParam);;
		}

		// owner window subclass
		LRESULT HookFrameProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch(msg)
			{
				// if cursor moves to frame border, toggle timer with appropriate event on
				case WM_MOUSEMOVE:
				{
					if (!((T*)this)->RequestAccess())
						break;
					if (((T*)this)->GetSelected())
					{
						int xPos = LOWORD(lParam); 
						int yPos = HIWORD(lParam); 
						POINT pt = {xPos, yPos};
						int nFrameWidth = ((T*)this)->GetFrameWidth();
						int nFrameHeight = ((T*)this)->GetFrameHeight();	
						int nImageWidth = ((T*)this)->GetWidth();
						int nImageHeight = ((T*)this)->GetHeight();	
						float fCurrentZoom = ((T*)this)->GetCurrentZoom();
						RECT rc;
						::GetWindowRect (m_hOwner, &rc);
						rc.bottom -= rc.top;
						rc.right -= rc.left;
						rc.left = rc.top = 0;
						bool bKillTimer = true;
						if (::PtInRect (&rc, pt))
						{							
							if (rc.right - rc.left > IGFRAME_MINFRAMESIZE)
							{
								int nFrameLeft = (int)((float)nFrameWidth / 2.0f + m_fCoordFromCenterX - (float)nImageWidth * fCurrentZoom / 2.0f);
								int nFrameRight = (int)((float)nFrameWidth / 2.0f + m_fCoordFromCenterX + (float)nImageWidth * fCurrentZoom / 2.0f);
								if (((m_fCoordFromCenterX < 0.0f) || (nFrameLeft < IGFRAME_BORDERWIDTH))
									&& (xPos - rc.left < (IGFRAME_MOVINGBORDER_MINWIDTH + (int)((float)(rc.right - rc.left) * IGFRAME_MOVINGBORDER_WIDTHPERCENT))))
								{
									if (!(m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETORIGHT))
										changeTimer (IGFRAME_TIMEREVENT_MOVETORIGHT, IGFRAME_TIMEREVENT_MOVETOLEFT);
									bKillTimer = false;
								}
								else
								{
									if (m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETORIGHT)
										changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETORIGHT, IGFRAME_TIMEREVENT_MOVETORIGHT);
								}
								if (((m_fCoordFromCenterX > 0.0f) || (nFrameRight > nFrameWidth - IGFRAME_BORDERWIDTH))
									&& (rc.right - xPos < (IGFRAME_MOVINGBORDER_MINWIDTH + (int)((float)(rc.right - rc.left) * IGFRAME_MOVINGBORDER_WIDTHPERCENT))))
								{
									if (!(m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETOLEFT))
										changeTimer (IGFRAME_TIMEREVENT_MOVETOLEFT, IGFRAME_TIMEREVENT_MOVETORIGHT);
									bKillTimer = false;
								}
								else
								{
									if (m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETOLEFT)
										changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETOLEFT, IGFRAME_TIMEREVENT_MOVETOLEFT);
								}
							}
							if (rc.bottom - rc.top > IGFRAME_MINFRAMESIZE)
							{
								int nFrameTop = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY - (float)nImageHeight * fCurrentZoom / 2.0f);
								int nFrameBottom = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY + (float)nImageHeight * fCurrentZoom / 2.0f);
								if (((m_fCoordFromCenterY < 0.0f) || (nFrameTop < IGFRAME_BORDERWIDTH))
									&& (yPos - rc.top < (IGFRAME_MOVINGBORDER_MINWIDTH + (int)((float)(rc.bottom - rc.top) * IGFRAME_MOVINGBORDER_WIDTHPERCENT))))
								{
									if (!(m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETOBOTTOM))
										changeTimer (IGFRAME_TIMEREVENT_MOVETOBOTTOM, IGFRAME_TIMEREVENT_MOVETOTOP);
									bKillTimer = false;
								}
								else
								{
									if (m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETOBOTTOM)
										changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETOBOTTOM, IGFRAME_TIMEREVENT_MOVETOBOTTOM);
								}
								if (((m_fCoordFromCenterY > 0.0f) || (nFrameBottom > nFrameHeight - IGFRAME_BORDERWIDTH))
									&& (rc.bottom - yPos < (IGFRAME_MOVINGBORDER_MINWIDTH + (int)((float)(rc.bottom - rc.top) * IGFRAME_MOVINGBORDER_WIDTHPERCENT))))
								{
									if (!(m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETOTOP))
										changeTimer (IGFRAME_TIMEREVENT_MOVETOTOP, IGFRAME_TIMEREVENT_MOVETOBOTTOM);
									bKillTimer = false;
								}
								else
								{
									if (m_uiCurrentTimerEvent & IGFRAME_TIMEREVENT_MOVETOTOP)
										changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETOTOP, IGFRAME_TIMEREVENT_MOVETOTOP);
								}
							}							
						}
						if (bKillTimer)
							resetTimer ();
					}
				}
				break;

				// increase or decrease the current zoom
				case WM_MOUSEWHEEL:
				{
					if (!((T*)this)->RequestAccess())
						break;
					if (((T*)this)->GetSelected())
					{
						int xPos = LOWORD(lParam); 
						int yPos = HIWORD(lParam); 
						POINT pt = {xPos, yPos};
						RECT rc;
						::GetWindowRect (m_hOwner, &rc);
						int nDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;						
						if (!::PtInRect (&rc, pt))
						{
							m_fCoordFromCenterX = 0.0f;
							m_fCoordFromCenterY = 0.0f;							
						}
						ZoomToPoint (xPos - rc.left, yPos - rc.top, nDelta);
						((T*)this)->Redraw();
					}
				}
				break;

				// At each tick check that the timer is still needed according to new 
				// frame position and zoom 
				case WM_TIMER:
				{
					if (!((T*)this)->RequestAccess())
						break;
					if (wParam == IGFRAME_TIMEREVENT_RENDER)
					{
						// frame rendering timer event
						handleDelayedRendering ();
					}
					else
					{
						// frame moving timer event
						// wait for a while before starting moving frame
						if (m_nNbTicks++ < IGFRAME_NBTICKSBEFORESTART)
							break;
						int nFrameWidth = ((T*)this)->GetFrameWidth();
						int nFrameHeight = ((T*)this)->GetFrameHeight();	
						int nImageWidth = ((T*)this)->GetWidth();
						int nImageHeight = ((T*)this)->GetHeight();	
						float fCurrentZoom = ((T*)this)->GetCurrentZoom();
						int nFrameTop = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY - (float)nImageHeight * fCurrentZoom / 2.0f);
						int nFrameBottom = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY + (float)nImageHeight * fCurrentZoom / 2.0f);
						if (wParam & IGFRAME_TIMEREVENT_MOVETOLEFT)
						{
							int nFrameRight = (int)((float)nFrameWidth / 2.0f + m_fCoordFromCenterX + (float)nImageWidth * fCurrentZoom / 2.0f);
							m_fCoordFromCenterX -= IGFRAME_MOVINGFRAME_OFFSET;
							if ((m_fCoordFromCenterX < 0.0f) && (nFrameRight < nFrameWidth - IGFRAME_BORDERWIDTH))
								changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETOLEFT, IGFRAME_TIMEREVENT_MOVETOLEFT);
						}
						else if (wParam & IGFRAME_TIMEREVENT_MOVETORIGHT)
						{
							int nFrameLeft = (int)((float)nFrameWidth / 2.0f + m_fCoordFromCenterX - (float)nImageWidth * fCurrentZoom / 2.0f);
							m_fCoordFromCenterX += IGFRAME_MOVINGFRAME_OFFSET;
							if ((m_fCoordFromCenterX > 0.0f) && (nFrameLeft > IGFRAME_BORDERWIDTH))
								changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETORIGHT, IGFRAME_TIMEREVENT_MOVETORIGHT);
						}
						if (wParam & IGFRAME_TIMEREVENT_MOVETOTOP)
						{
							int nFrameBottom = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY + (float)nImageHeight * fCurrentZoom / 2.0f);
							m_fCoordFromCenterY -= IGFRAME_MOVINGFRAME_OFFSET;
							if ((m_fCoordFromCenterY < 0.0f) && (nFrameBottom < nFrameHeight - IGFRAME_BORDERWIDTH))
								changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETOTOP, IGFRAME_TIMEREVENT_MOVETOTOP);
						}
						else if (wParam & IGFRAME_TIMEREVENT_MOVETOBOTTOM)
						{
							int nFrameTop = (int)((float)nFrameHeight / 2.0f + m_fCoordFromCenterY - (float)nImageHeight * fCurrentZoom / 2.0f);
							m_fCoordFromCenterY += IGFRAME_MOVINGFRAME_OFFSET;
							if ((m_fCoordFromCenterY > 0.0f) && (nFrameTop > IGFRAME_BORDERWIDTH))
								changeTimer (m_uiCurrentTimerEvent & ~IGFRAME_TIMEREVENT_MOVETOBOTTOM, IGFRAME_TIMEREVENT_MOVETOBOTTOM);
						}
						((T*)this)->Redraw();
					}
				}
				break;

				// when mouse is leaving the window, stop moving frame
				case WM_MOUSELEAVE:
					resetTimer ();
					break;
			}

			// Default behavior using the old window proc			
			return ::DefSubclassProc(hWnd, msg, wParam, lParam);
		}
		
		// change the current timer event by adding uiNewTimerEvent and removing uiKillEvent
		void changeTimer (UINT_PTR uiNewTimerEvent, UINT_PTR uiKillEvent)
		{
			if (m_uiCurrentTimerEvent)
				::KillTimer (m_hOwner, m_uiCurrentTimerEvent);
			m_uiCurrentTimerEvent &= ~uiKillEvent;
			m_uiCurrentTimerEvent |= uiNewTimerEvent;
			if (m_uiCurrentTimerEvent)
			{
				::SetTimer (m_hOwner, m_uiCurrentTimerEvent, 20, NULL);
				// track mouse leaving the window
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = m_hOwner;
				::TrackMouseEvent (&trackMouseEvent);
				if (m_uiCurrentTimerEvent == IGFRAME_TIMEREVENT_MOVETOLEFT)
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorRight);
				else if (m_uiCurrentTimerEvent == IGFRAME_TIMEREVENT_MOVETORIGHT)
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorLeft);
				else if (m_uiCurrentTimerEvent == IGFRAME_TIMEREVENT_MOVETOBOTTOM)
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorUp);
				else if (m_uiCurrentTimerEvent == IGFRAME_TIMEREVENT_MOVETOTOP)
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorDown);
				else if (m_uiCurrentTimerEvent == (IGFRAME_TIMEREVENT_MOVETOLEFT | IGFRAME_TIMEREVENT_MOVETOTOP))
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorDownRight);
				else if (m_uiCurrentTimerEvent == (IGFRAME_TIMEREVENT_MOVETORIGHT | IGFRAME_TIMEREVENT_MOVETOTOP))
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorDownLeft);
				else if (m_uiCurrentTimerEvent == (IGFRAME_TIMEREVENT_MOVETOLEFT | IGFRAME_TIMEREVENT_MOVETOBOTTOM))
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorUpRight);
				else if (m_uiCurrentTimerEvent == (IGFRAME_TIMEREVENT_MOVETORIGHT | IGFRAME_TIMEREVENT_MOVETOBOTTOM))
					::SetClassLong (m_hOwner, GCL_HCURSOR, (LONG) g_hCursorUpLeft);
			}
			else
			{
				m_nNbTicks = 0;
				// current tool cursor
				((T*)this)->SetCurrentCursor();
			}
		}

		// kill the current timer
		void resetTimer ()
		{
			if (m_uiCurrentTimerEvent)
			{
				::KillTimer (m_hOwner, m_uiCurrentTimerEvent);
				// current tool cursor
				((T*)this)->SetCurrentCursor();
				m_uiCurrentTimerEvent = 0;
				m_nNbTicks = 0;
			}
		}

		void handleDelayedRendering ()
		{
			::KillTimer (m_hOwner, IGFRAME_TIMEREVENT_RENDER);
			((T*)this)->Redraw (true);
		}

		static int			g_nNbRefs;
		static HCURSOR		g_hCursorUp;
		static HCURSOR		g_hCursorDown;
		static HCURSOR		g_hCursorLeft;
		static HCURSOR		g_hCursorRight;
		static HCURSOR		g_hCursorUpLeft;
		static HCURSOR		g_hCursorDownLeft;
		static HCURSOR		g_hCursorUpRight;
		static HCURSOR		g_hCursorDownRight;

		HWND		m_hOwner;		
		float		m_fZoomBestFit;
		float		m_fZoomToFrame;
		bool		m_bSelected;
		float		m_fCoordFromCenterX;
		float		m_fCoordFromCenterY;
		UINT_PTR	m_uiCurrentTimerEvent;
		int			m_nNbTicks;
	};

	template<class T> int IGView<T>::g_nNbRefs = 0;
	template<class T> HCURSOR IGView<T>::g_hCursorUp = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorDown = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorLeft = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorRight = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorUpLeft = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorDownLeft = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorUpRight = NULL;
	template<class T> HCURSOR IGView<T>::g_hCursorDownRight = NULL;
}
