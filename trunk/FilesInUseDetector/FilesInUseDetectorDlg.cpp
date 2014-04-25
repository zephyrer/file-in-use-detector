
// FilesInUseDetectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FilesInUseDetector.h"
#include "FilesInUseDetectorDlg.h"
#include "afxdialogex.h"
#include "RestartManager.h"

#pragma comment(lib,"Rstrtmgr.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFilesInUseDetectorDlg dialog




CFilesInUseDetectorDlg::CFilesInUseDetectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFilesInUseDetectorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilesInUseDetectorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_FILE, fileEdit);
    DDX_Control(pDX, IDC_BUTTON_BROWSE, browseButton);
    DDX_Control(pDX, IDC_LIST_OCCUPIER, occupierList);
}

BEGIN_MESSAGE_MAP(CFilesInUseDetectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFilesInUseDetectorDlg::OnBnClickedButtonBrowse)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CFilesInUseDetectorDlg message handlers

BOOL CFilesInUseDetectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
    occupierList.InsertColumn(0,L"ProcessName",0,150);
    occupierList.InsertColumn(1,L"Pid",0,50);
    occupierList.InsertColumn(2,L"Type",0,80);
    occupierList.InsertColumn(3,L"ImageName",0,400);
    occupierList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFilesInUseDetectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFilesInUseDetectorDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFilesInUseDetectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFilesInUseDetectorDlg::OnBnClickedButtonBrowse()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog dlgFile(TRUE,0,0,0,L"All Files (*.*)|*.*|");
    if(dlgFile.DoModal() == IDOK)
    {
        CString fileInUse = dlgFile.GetPathName();
        fileEdit.SetWindowTextW(fileInUse);
        
        ShowOccupiers(fileInUse);
    }
}

void CFilesInUseDetectorDlg::ShowOccupiers( CString fileInUse )
{
    //Detail is described in the below link.
    //http://blogs.msdn.com/b/oldnewthing/archive/2012/02/17/10268840.aspx
    occupierList.DeleteAllItems();

    DWORD dwSession;
    WCHAR szSessionKey[CCH_RM_SESSION_KEY+1] = { 0 };
    DWORD dwError = RmStartSession(&dwSession, 0, szSessionKey);
    wprintf(L"RmStartSession returned %d\n", dwError);
    if (dwError == ERROR_SUCCESS) 
    {
        PCWSTR pszFile = fileInUse;
        dwError = RmRegisterResources(dwSession, 1, &pszFile,
            0, NULL, 0, NULL);
        wprintf(L"RmRegisterResources(%ls) returned %d\n",
            pszFile, dwError);
        if (dwError == ERROR_SUCCESS) {
            DWORD dwReason;
            UINT i;
            UINT nProcInfoNeeded;
            UINT nProcInfo = 1000;
            RM_PROCESS_INFO rgpi[1000];
            dwError = RmGetList(dwSession, &nProcInfoNeeded,
                &nProcInfo, rgpi, &dwReason);
            wprintf(L"RmGetList returned %d\n", dwError);
            if (dwError == ERROR_SUCCESS) 
            {
                wprintf(L"RmGetList returned %d infos (%d needed)\n",
                    nProcInfo, nProcInfoNeeded);
                for (i = 0; i < nProcInfo; i++) 
                {
                    occupierList.InsertItem(i,rgpi[i].strAppName);
                    wchar_t pidBuf[100] = L"";
                    _itow_s(rgpi[i].Process.dwProcessId,pidBuf,10);
                    occupierList.SetItemText(i,1,pidBuf);
                    CString type;
                    switch(rgpi[i].ApplicationType)
                    {
                    case RmUnknownApp:
                        {
                            type = L"Unknown";
                            break;
                        }
                    case RmMainWindow:
                        {
                            type = L"MainWindow";
                            break;
                        }
                    case RmService:
                        {
                            type = L"Service";
                            break;
                        }
                    case RmExplorer:
                        {
                            type = L"Explorer";
                            break;
                        }
                    case RmConsole:
                        {
                            type = L"Console";
                            break;
                        }
                    case RmCritical:
                        {
                            type = L"Critical";
                            break;
                        }
                    default:
                        {
                            type = L"UnspecifiedType";
                            break;
                        }
                    }
                    occupierList.SetItemText(i,2,type);

                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                        FALSE, rgpi[i].Process.dwProcessId);
                    if (hProcess) 
                    {
                        FILETIME ftCreate, ftExit, ftKernel, ftUser;
                        if (GetProcessTimes(hProcess, &ftCreate, &ftExit,
                            &ftKernel, &ftUser) &&
                            CompareFileTime(&rgpi[i].Process.ProcessStartTime,
                            &ftCreate) == 0) 
                        {
                            WCHAR sz[MAX_PATH];
                            DWORD cch = MAX_PATH;
                            if (QueryFullProcessImageNameW(hProcess, 0, sz, &cch) &&
                                cch <= MAX_PATH) 
                            {
                                occupierList.SetItemText(i,3,sz);
                            }
                        }
                        CloseHandle(hProcess);
                    }

                    //wprintf(L"%d.ApplicationType = %d\n", i,
                    //    rgpi[i].ApplicationType);
                    //wprintf(L"%d.strAppName = %ls\n", i,
                    //    rgpi[i].strAppName);
                    //wprintf(L"%d.Process.dwProcessId = %d\n", i,
                    //    rgpi[i].Process.dwProcessId);
                    //HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                    //    FALSE, rgpi[i].Process.dwProcessId);
                    //if (hProcess) 
                    //{
                    //    FILETIME ftCreate, ftExit, ftKernel, ftUser;
                    //    if (GetProcessTimes(hProcess, &ftCreate, &ftExit,
                    //        &ftKernel, &ftUser) &&
                    //        CompareFileTime(&rgpi[i].Process.ProcessStartTime,
                    //        &ftCreate) == 0) 
                    //    {
                    //            WCHAR sz[MAX_PATH];
                    //            DWORD cch = MAX_PATH;
                    //            if (QueryFullProcessImageNameW(hProcess, 0, sz, &cch) &&
                    //                cch <= MAX_PATH) 
                    //            {
                    //                    wprintf(L"  = %ls\n", sz);
                    //            }
                    //    }
                    //    CloseHandle(hProcess);
                    //}
                }
            }
        }
        RmEndSession(dwSession);
    }
}


void CFilesInUseDetectorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
    CDialogEx::OnLButtonDown(nFlags, point);
}
