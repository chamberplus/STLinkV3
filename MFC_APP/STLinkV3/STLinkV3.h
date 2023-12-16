
// STLinkV3.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSTLinkV3App:
// See STLinkV3.cpp for the implementation of this class
//

class CSTLinkV3App : public CWinAppEx
{
public:
	CSTLinkV3App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSTLinkV3App theApp;