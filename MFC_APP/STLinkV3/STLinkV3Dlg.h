
// STLinkV3Dlg.h : header file
//
#include "ColorStatic.h"
#include "bridge.h"

#pragma once

// CSTLinkV3Dlg dialog
class CSTLinkV3Dlg : public CDialog
{
// Construction
public:
	CSTLinkV3Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STLINKV3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	//--- STlink Libraray definition --
	Brg* m_pBrg = NULL;
	STLinkInterface *m_pStlinkIf = NULL;
	Brg_StatusT brgStat = BRG_NO_ERR;


	CColorStatic m_DeviceVID;
	CColorStatic m_DevicePID;
	CColorStatic m_DeviceSN;
	CColorStatic m_DeviceFVER;
	//--
	CString s_DeviceVID;
	CString s_DevicePID;
	CString s_DeviceSN;
	CString s_DeviceFVER;
	afx_msg void OnBnClickedButton_GPIO_Test_BTNEvent();

};
