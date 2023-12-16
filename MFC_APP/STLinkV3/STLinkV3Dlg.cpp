
// STLinkV3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "STLinkV3.h"
#include "STLinkV3Dlg.h"
#include "bridge.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSTLinkV3Dlg dialog




CSTLinkV3Dlg::CSTLinkV3Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSTLinkV3Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSTLinkV3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USB_VID, m_DeviceVID);
	DDX_Control(pDX, IDC_USB_PID, m_DevicePID);
	DDX_Control(pDX, IDC_USB_SN, m_DeviceSN);
	DDX_Control(pDX, IDC_USB_FVER, m_DeviceFVER);
}

BEGIN_MESSAGE_MAP(CSTLinkV3Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GPIOTest, &CSTLinkV3Dlg::OnBnClickedButton_GPIO_Test_BTNEvent)
END_MESSAGE_MAP()


// CSTLinkV3Dlg message handlers

BOOL CSTLinkV3Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	///----
	m_DeviceVID.SetTextColor(GRAY);
	m_DeviceVID.SetWindowText("0x0000");
	m_DevicePID.SetTextColor(GRAY);
	m_DevicePID.SetWindowText("0x0000");
	//	m_FWVersion.SetTextColor(GRAY);
	//	m_FWVersion.SetWindowText("00.00");
	m_DeviceSN.SetTextColor(GRAY);
	m_DeviceSN.SetWindowText("-----");
	m_DeviceFVER.SetTextColor(GRAY);
	m_DeviceFVER.SetWindowText("-----");
	//m_FWVersion.SetTextColor(GRAY);
	//m_FWVersion.SetWindowText("00.00");

//	Brg_StatusT brgStat = BRG_NO_ERR;
	STLinkIf_StatusT ifStat = STLINKIF_NO_ERR;
	char path[MAX_PATH];
#ifdef WIN32 //Defined for applications for Win32 and Win64.
	char *pEndOfPrefix;
#endif
	uint32_t i, numDevices;
	int firstDevNotInUse = -1;
	STLink_DeviceInfo2T devInfo2;
	//Brg* m_pBrg = NULL;
	//STLinkInterface *m_pStlinkIf = NULL;
	char m_serialNumber[SERIAL_NUM_STR_MAX_LEN];

	// USB interface initialization and device detection done using STLinkInterface

	// Create USB BRIDGE interface
	m_pStlinkIf = new STLinkInterface(STLINK_BRIDGE);
#ifdef USING_ERRORLOG
	m_pStlinkIf->BindErrLog(&g_ErrLog);
#endif

#ifdef WIN32 //Defined for applications for Win32 and Win64.
	GetModuleFileNameA(NULL, path, MAX_PATH);
	// Remove process file name from the path
	pEndOfPrefix = strrchr(path, '\\');

	if (pEndOfPrefix != NULL)
	{
		*(pEndOfPrefix + 1) = '\0';
	}
#else
	strcpy(path, "");
#endif
	// Load STLinkUSBDriver library 
	// In this example STLinkUSBdriver (dll on windows) must be copied near test executable
	ifStat = m_pStlinkIf->LoadStlinkLibrary(path);
	if (ifStat != STLINKIF_NO_ERR) {
		TRACE0("STLinkUSBDriver library (dll) issue \n");
		AfxMessageBox(_T("STLinkUSBDriver library (dll) is not found !!"), MB_OK | MB_ICONERROR);
		return TRUE;
	}

	if (ifStat == STLINKIF_NO_ERR) {
		ifStat = m_pStlinkIf->EnumDevices(&numDevices, false);
		// or brgStat = Brg::ConvSTLinkIfToBrgStatus(m_pStlinkIf->EnumDevices(&numDevices, false));
	}

	// Choose the first STLink Bridge available
	if ((ifStat == STLINKIF_NO_ERR) || (ifStat == STLINKIF_PERMISSION_ERR)) {
		TRACE1("%d BRIDGE device found\n", (int)numDevices);

		for (i = 0; i < numDevices; i++) {
			ifStat = m_pStlinkIf->GetDeviceInfo2(i, &devInfo2, sizeof(devInfo2));
			TRACE3("Bridge %d PID: 0X%04hx SN:%s\n", (int)i, (unsigned short)devInfo2.ProductId, devInfo2.EnumUniqueId);

			if ((firstDevNotInUse == -1) && (devInfo2.DeviceUsed == false)) {
				firstDevNotInUse = i;
				memcpy(m_serialNumber, &devInfo2.EnumUniqueId, SERIAL_NUM_STR_MAX_LEN);
				//printf(" SELECTED BRIDGE Stlink SN:%s\n", m_serialNumber);
				TRACE1(" SELECTED BRIDGE Stlink SN:%s\n", m_serialNumber);
			}
		}
	}
	else if (ifStat == STLINKIF_CONNECT_ERR) {	///USB Connection error
		TRACE0("No STLink BRIDGE device detected\n");
		AfxMessageBox(_T("No STLink BRIDGE device detected !!"), MB_OK | MB_ICONERROR);
		return TRUE;  // return TRUE  unless you set the focus to a control
	}
	else if (ifStat == STLINKIF_NO_STLINK) {	///STLink device not opened error
		TRACE0("STLink BRIDGE device open fail\n");
		AfxMessageBox(_T("STLink BRIDGE device open fail (No Connectted) !!"), MB_OK | MB_ICONERROR);
		return TRUE;  // return TRUE  unless you set the focus to a control
	}
	else {
		TRACE0("enum error\n");
		AfxMessageBox(_T("STLink BRIDGE device Enum Error (No Connectted) !!"), MB_OK | MB_ICONERROR);
		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	brgStat = Brg::ConvSTLinkIfToBrgStatus(ifStat);

	// USB Connection to a given device done with Brg
	if (brgStat == BRG_NO_ERR) {
		m_pBrg = new Brg(*m_pStlinkIf);
#ifdef USING_ERRORLOG
		m_pBrg->BindErrLog(&g_ErrLog);
#endif
	}

	// The firmware may not be the very last one, but it may be OK like that (just inform)
	bool bOldFirmwareWarning = false;
	//----
	//--- USB Device Already --
	TRACE2("USB Device VID:0x%04X, PID:0x%04X\n", (unsigned short)devInfo2.VendorId, (unsigned short)devInfo2.ProductId);
	TRACE1("USB Device SN:%s\n", devInfo2.EnumUniqueId);
	//--
	m_DeviceVID.SetTextColor(LIGHTBLUE);
	s_DeviceVID.Format(_T("%x"), (unsigned short)devInfo2.VendorId);
	if (s_DeviceVID.GetLength() == 3)
		s_DeviceVID.Format(_T("0x0%X"), (unsigned short)devInfo2.VendorId);
	else
		s_DeviceVID.Format(_T("0x%X"), (unsigned short)devInfo2.VendorId);

	m_DeviceVID.SetWindowText(s_DeviceVID);
	//--
	m_DevicePID.SetTextColor(LIGHTBLUE);
	s_DeviceVID.Format(_T("%x"), (unsigned short)devInfo2.ProductId);
	if (s_DevicePID.GetLength() == 3)
		s_DevicePID.Format(_T("0x0%X"), (unsigned short)devInfo2.ProductId);
	else
		s_DevicePID.Format(_T("0x%X"), (unsigned short)devInfo2.ProductId);

	m_DevicePID.SetWindowText(s_DevicePID);
	//---
	//---	char * pEnumUniqueId = devInfo2.EnumUniqueId;
	m_DeviceSN.SetTextColor(LIGHTBLUE);
	CStringA sUSBSN;
	memcpy(sUSBSN.GetBufferSetLength(sizeof(devInfo2.EnumUniqueId)), devInfo2.EnumUniqueId, sizeof(devInfo2.EnumUniqueId));
	s_DeviceSN = sUSBSN;
	sUSBSN.ReleaseBuffer(sizeof(devInfo2.EnumUniqueId));
	m_DeviceSN.SetWindowText(s_DeviceSN);

	// Open the STLink connection
	if (brgStat == BRG_NO_ERR) {
		m_pBrg->SetOpenModeExclusive(true);

		brgStat = m_pBrg->OpenStlink(firstDevNotInUse);

		if (brgStat == BRG_NOT_SUPPORTED) {
			brgStat = Brg::ConvSTLinkIfToBrgStatus(m_pStlinkIf->GetDeviceInfo2(0, &devInfo2, sizeof(devInfo2)));
			TRACE2("BRIDGE not supported PID: 0X%04hx SN:%s\n", (unsigned short)devInfo2.ProductId, devInfo2.EnumUniqueId);
		}

		if (brgStat == BRG_OLD_FIRMWARE_WARNING) {
			// Status to restore at the end if all is OK
			bOldFirmwareWarning = true;
			brgStat = BRG_NO_ERR;
		}
	}
	// Test Voltage command
	if (brgStat == BRG_NO_ERR) {
		float voltage = 0;
		// T_VCC pin must be connected to target voltage on debug connector
		brgStat = m_pBrg->GetTargetVoltage(&voltage);
		if ((brgStat != BRG_NO_ERR) || (voltage == 0)) {
			TRACE0("BRIDGE get voltage error (check if T_VCC pin is connected to target voltage on debug connector)\n");
		}
		else {
			TRACE1("BRIDGE get voltage: %.2f V \n", (double)voltage);
		}
	}

	if ((brgStat == BRG_NO_ERR) && (bOldFirmwareWarning == true)) {
		// brgStat = BRG_OLD_FIRMWARE_WARNING;
		TRACE2("BRG_OLD_FIRMWARE_WARNING: v%d B%d \n", (int)m_pBrg->m_Version.Major_Ver, (int)m_pBrg->m_Version.Bridge_Ver);
	}
	//----
	TRACE2("USB Device MV:V%d, JV:J%d\n", (int)m_pBrg->m_Version.Major_Ver, (int)m_pBrg->m_Version.Jtag_Ver);
	TRACE3("USB Device MV:M%d, BV:B%d, SV:S%d\n", (int)m_pBrg->m_Version.Msc_Ver, (int)m_pBrg->m_Version.Bridge_Ver, (int)m_pBrg->m_Version.Swim_Ver);
	//CString s_DeviceFVER;
	m_DeviceFVER.SetTextColor(LIGHTBLUE);
	s_DeviceFVER.Format(_T("V%d.J%d.M%d.B%d.S%d"), (int)m_pBrg->m_Version.Major_Ver, (int)m_pBrg->m_Version.Jtag_Ver, (int)m_pBrg->m_Version.Msc_Ver, (int)m_pBrg->m_Version.Bridge_Ver, (int)m_pBrg->m_Version.Swim_Ver);

	m_DeviceFVER.SetWindowText(s_DeviceFVER);

	// Test GET CLOCK command
	if (brgStat == BRG_NO_ERR) {
		uint32_t StlHClkKHz, comInputClkKHz;
		// Get the current bridge input Clk for all com:
		brgStat = m_pBrg->GetClk(COM_SPI, (uint32_t*)&comInputClkKHz, (uint32_t*)&StlHClkKHz);
		TRACE2("SPI input CLK: %d KHz, ST-Link HCLK: %d KHz \n", (int)comInputClkKHz, (int)StlHClkKHz);
		if (brgStat == BRG_NO_ERR) {
			brgStat = m_pBrg->GetClk(COM_I2C, (uint32_t*)&comInputClkKHz, (uint32_t*)&StlHClkKHz);
			TRACE2("I2C input CLK: %d KHz, ST-Link HCLK: %d KHz \n", (int)comInputClkKHz, (int)StlHClkKHz);
		}
		if (brgStat == BRG_NO_ERR) {
			brgStat = m_pBrg->GetClk(COM_CAN, (uint32_t*)&comInputClkKHz, (uint32_t*)&StlHClkKHz);
			TRACE2("CAN input CLK: %d KHz, ST-Link HCLK: %d KHz \n", (int)comInputClkKHz, (int)StlHClkKHz);
		}
		if (brgStat == BRG_NO_ERR) {
			brgStat = m_pBrg->GetClk(COM_GPIO, (uint32_t*)&comInputClkKHz, (uint32_t*)&StlHClkKHz);
			TRACE2("GPIO input CLK: %d KHz, ST-Link HCLK: %d KHz \n", (int)comInputClkKHz, (int)StlHClkKHz);
			//----
		}
		if (brgStat != BRG_NO_ERR) {
			TRACE0("Error in GetClk()\n");
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSTLinkV3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSTLinkV3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSTLinkV3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------

void CSTLinkV3Dlg::OnBnClickedButton_GPIO_Test_BTNEvent()
{
	// TODO: Add your control notification handler code here
	if (brgStat == BRG_NO_ERR) {
		//----
		Brg_StatusT BrgStatus = BRG_NO_ERR;
		Brg_GpioInitT gpioParams;
		Brg_GpioConfT gpioConf[BRG_GPIO_MAX_NB];
		//Brg_GpioValT gpioReadVal[BRG_GPIO_MAX_NB];
		Brg_GpioValT gpioWriteVal[BRG_GPIO_MAX_NB];
		uint8_t gpioMsk = 0, gpioErrMsk;
		//----initial GPIO
		int i;
		gpioMsk = BRG_GPIO_ALL;
		gpioParams.GpioMask = gpioMsk; // BRG_GPIO_0 1 2 3
		gpioParams.ConfigNb = BRG_GPIO_MAX_NB; //must be BRG_GPIO_MAX_NB or 1 (if 1 pGpioConf[0] used for all gpios)
		gpioParams.pGpioConf = &gpioConf[0];
		for (i = 0; i < BRG_GPIO_MAX_NB; i++) {
			//gpioConf[i].Mode = GPIO_MODE_INPUT; // GPIO_MODE_INPUT GPIO_MODE_OUTPUT GPIO_MODE_ANALOG
			gpioConf[i].Mode = GPIO_MODE_OUTPUT; // GPIO_MODE_INPUT GPIO_MODE_OUTPUT GPIO_MODE_ANALOG
			gpioConf[i].Speed = GPIO_SPEED_MEDIUM; // GPIO_SPEED_LOW GPIO_SPEED_MEDIUM GPIO_SPEED_HIGH GPIO_SPEED_VERY_HIGH
			gpioConf[i].Pull = GPIO_PULL_UP; // GPIO_NO_PULL GPIO_PULL_UP GPIO_PULL_DOWN
			gpioConf[i].OutputType = GPIO_OUTPUT_PUSHPULL; // GPIO_OUTPUT_PUSHPULL GPIO_OUTPUT_OPENDRAIN
		}
		BrgStatus = m_pBrg->InitGPIO(&gpioParams);
		if (BrgStatus != BRG_NO_ERR) {
			TRACE3("Bridge Gpio init failed (mask=%d, gpio0: mode= %d, pull = %d, ...)\n", (int)gpioParams.GpioMask, (int)gpioConf[0].Mode, (int)gpioConf[0].Pull);
			AfxMessageBox(_T("BRIDGE GPIO Init failed... !!"), MB_OK | MB_ICONERROR);
			return;  // return TRUE  unless you set the focus to a control
		}
		BOOL bSetReset = false;

		for (int j = 0; j < 10; j++) {
			//---
			for (i = 0; i < BRG_GPIO_MAX_NB; i++) {
				if (bSetReset) {
					gpioWriteVal[i] = GPIO_SET;
				}
				else {
					gpioWriteVal[i] = GPIO_RESET;
				}
			}
			bSetReset = ~bSetReset;
			//BrgStatus = m_pBrg->ReadGPIO(gpioMsk, &gpioReadVal[0], &gpioErrMsk);
			BrgStatus = m_pBrg->SetResetGPIO(gpioMsk, &gpioWriteVal[0], &gpioErrMsk);
			if ((BrgStatus != BRG_NO_ERR) || (gpioErrMsk != 0)) {
				TRACE0(" Bridge Set/Reset error\n");
				break;
			}
			else {
				// GPIO OK ...
			}
			Sleep(1000);
		}
		if (BrgStatus == BRG_NO_ERR) {
			TRACE0("GPIO Test OK \n");
			brgStat = m_pBrg->CloseBridge(COM_GPIO);
			AfxMessageBox(_T("STLink-V3 Bridge GPIO Test OK !!"), MB_OK | MB_ICONINFORMATION);
		}
	}
}


void CSTLinkV3Dlg::OnDestroy()
{

	// STLink-V3 Bridge Disconnect
	if (m_pBrg != NULL) {
		m_pBrg->CloseBridge(COM_UNDEF_ALL);
		m_pBrg->CloseStlink();
		delete m_pBrg;
		m_pBrg = NULL;
	}
	// unload STLinkUSBdriver library
	if (m_pStlinkIf != NULL) {
		// always delete STLinkInterface after Brg (because Brg uses STLinkInterface)
		delete m_pStlinkIf;
		m_pStlinkIf = NULL;
	}

	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}
