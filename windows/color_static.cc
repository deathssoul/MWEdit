/*===========================================================================
 *
 * File:    Colorstatic1.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  February 16, 2003
 *
 * Description
 *
 *=========================================================================*/
#include "windows/color_static.h"

#include <atltypes.h>
#include <windef.h>  // TODO: Required for winuser.h
#include <winuser.h>
#include <afxwin.h>  // TODO: Needs to go after winuser.h for UpdateLayeredWindow()

#include "common/dl_base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _DEBUG

DEFINE_FILE("ColorStatic.cpp");
/*===========================================================================
 *
 * Begin CColorStatic Message Map
 *
 *=========================================================================*/
BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*===========================================================================
 *
 * Class CColorStatic Constructor
 *
 *=========================================================================*/
CColorStatic::CColorStatic() {
	m_Color = 0;
	m_Red = 0;
	m_Green = 0;
	m_Blue = 0;
	m_Modified = false;
}


/*===========================================================================
 *
 * Class CColorStatic Destructor
 *
 *=========================================================================*/
CColorStatic::~CColorStatic() {
}


/*===========================================================================
 *
 * Class CColorStatic Event - void OnPaint ();
 *
 *=========================================================================*/
void CColorStatic::OnPaint() {
	CPaintDC DC(this);
	CRect ClientRect;
	GetClientRect(&ClientRect);
	DC.FillSolidRect(ClientRect, m_Color);
}
