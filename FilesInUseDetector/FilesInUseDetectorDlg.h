
// FilesInUseDetectorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CFilesInUseDetectorDlg dialog
class CFilesInUseDetectorDlg : public CDialogEx
{
// Construction
public:
	CFilesInUseDetectorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FILESINUSEDETECTOR_DIALOG };

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
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonBrowse();
    void ShowOccupiers( CString fileInUse );

    //Controls
    CEdit fileEdit;
    CButton browseButton;
    CListCtrl occupierList;
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
