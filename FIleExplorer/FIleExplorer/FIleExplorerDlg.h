
// FIleExplorerDlg.h : ͷ�ļ�
//

#pragma once


#include "afxcmn.h"
#include <vector>
#include <string>
#include <shlobj.h>
#include "afxwin.h"
#include <windows.h>
#include <tchar.h>
#include <regex>
#include <shlwapi.h>
#include <knownfolders.h>  // ȷ��������ͷ�ļ�
#include <comdef.h>
#include <Wbemidl.h> // ��Ҫ���� wbemuuid.lib
#include <wininet.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
// ��Ҫ���� Shell32.lib �� Shlwapi.lib
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "wbemuuid.lib")

/*
// ��� SHGetKnownFolderIDListFromIDList δ���壬���ֶ�����
#if (_WIN32_WINNT < 0x0600) || !defined(SHGetKnownFolderIDListFromIDList)
typedef HRESULT(WINAPI* PFNSHGetKnownFolderIDListFromIDList)(PCIDLIST_ABSOLUTE pidl, KNOWNFOLDERID* pkfid);

HRESULT SHGetKnownFolderIDListFromIDList_Compat(PCIDLIST_ABSOLUTE pidl, KNOWNFOLDERID* pkfid)
{
	HMODULE hShell32 = LoadLibrary(_T("shell32.dll"));
	if (!hShell32) return E_FAIL;

	PFNSHGetKnownFolderIDListFromIDList pfn = (PFNSHGetKnownFolderIDListFromIDList)
		GetProcAddress(hShell32, "SHGetKnownFolderIDListFromIDList");

	HRESULT hr = E_NOTIMPL;
	if (pfn) hr = pfn(pidl, pkfid);

	FreeLibrary(hShell32);
	return hr;
}

#define SHGetKnownFolderIDListFromIDList SHGetKnownFolderIDListFromIDList_Compat
#endif

*/

#ifndef FOLDERID_QuickAccess
#define FOLDERID_QuickAccess {0x679f85cb-0220-4080-b29b-5540cc05aab6}
#endif

// CFIleExplorerDlg �Ի���
class CFIleExplorerDlg : public CDialogEx
{
// ����
public:
	CFIleExplorerDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// ����Ҫ��ʾ�������ļ���
	struct SpecialFolder {
		KNOWNFOLDERID folderId;
		CString displayName;
	};

	const SpecialFolder* m_specialFolders;

	  // �ؼ�����
    CTreeCtrl m_treeCtrl;
    CListCtrl m_listCtrl;
	CString m_strCurrentPath;

	void LoadSettings();
    // ϵͳͼ���б�
    CImageList m_systemImageList;
	CImageList m_imageList;
	// ��ǰĿ¼
    CString m_strCurrentDir;
	void LoadDirectory(LPCTSTR lpszPath);
	void deleteChild(HTREEITEM hItem);
	void RepositionControls();
	 // ��¼�ؼ���ʼλ�úʹ�С
    CRect m_recttree;
    CRect m_recttreeFast;
    CRect m_rectList;
    CRect m_rectListFast;
	HTREEITEM m_hRoot;
	CString m_strUserName;

    // �����ļ��нṹ
    void LoadFolderTree(HTREEITEM hParent, const CString& strPath);
	int GetIconIndex(const CString& strPath,const CString& cFileName, BOOL bIsFolder);
    // ��ʾ�ļ��б�
	void LoadSubFolders(HTREEITEM hParent, const CString& strPath);
    void ShowFileList(const CString& strPath);
	CString GetFullPath(HTREEITEM hItem);
	void LoadDrivesToTreeCtrl();
	void PopulateTree(HTREEITEM hParent, const CString& strPath);
	CString GetItemPath(HTREEITEM hItem);
	bool IsValidFilePath(LPCTSTR lpszFilePath);
	bool ExpandToPath1(CTreeCtrl& tree, LPCTSTR lpszPath);
	void SplitPath(LPCTSTR lpszPath, CStringArray& parts);
	bool FindAndExpandNode(CTreeCtrl& tree, HTREEITEM hCurrent, const CStringArray& pathParts, int nIndex);
	void CleanupTreeData(CTreeCtrl& tree, HTREEITEM hItem);
	HTREEITEM AddTreeItem(CTreeCtrl& tree, LPCTSTR lpszName, LPCTSTR lpszFullPath,
		_In_ int nImage, _In_ int nSelectedImage, _In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter = TVI_LAST);
	bool FindItem(CTreeCtrl& tree, HTREEITEM hCurrent, const CStringArray& pathParts, int nIndex);
	BOOL IsPathFormatValid(LPCTSTR lpszPath);
	void ShowDesktopMenuSafely(CWnd* pParentWnd, CPoint point);
	void ShowShellContextMenu(HWND hWnd, const CString& strFilePath, CPoint point);
	void ShowShellContextMenuForMultipleFiles(HWND hWnd, const CStringArray& arrFiles, CPoint point);
// �Ի�������
	enum { IDD = IDD_FILEEXPLORER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	void CenterToPrimaryMonitor();
	void SelectFirstChildItem();
	void OnEdit1EnterPressed();
	bool ValidateEdit1Content();
	void LoadAndDisplayDrives();
	void GetDriveDetails(const CString& strDrive, CListCtrl* pListCtrl, int nItem);
	std::vector<std::wstring> GetLogicalDrives();
	CString GetCurrentUserNameWMI();
	void CleanupTreeData(CTreeCtrl* pTree, HTREEITEM hItem);
	void CleanupListData(CListCtrl* pList);
	BOOL DestroyWindow();
	void UpdateStatusBarNetwork();
	BOOL IsInternetAvailable();
	void GetLocalIPAddressesEx(CStringArray& ipArray);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	BOOL initToolbar();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	void UpdateStatusBar(LPCTSTR lpszMessage, int nPaneIndex);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEditEnterPressed();  // �Զ���س���������
	afx_msg void OnNMCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDblclkList1();
	afx_msg void OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_curpath;
	void ShowDesktopContextMenu(CPoint point); // ��ʾ�����Ҽ��˵�

private:
	CStatusBar m_wndStatusBar; // ״̬������
	CToolBar m_wndToolBar; // ����������
	CStatic m_static;
	CButton m_btn;
	CComboBox m_combo;
	CReBar m_reBar;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChangeEdit1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton1();
	void InitializeSpecialFoldersTree();
	HTREEITEM AddShellItemToTree(CTreeCtrl* pTree, HTREEITEM hParent, REFKNOWNFOLDERID folderId, LPCTSTR lpszDisplayName);
	CListCtrl m_pListCtrl;
	CTreeCtrl m_treeCtrlFast;
	afx_msg void OnSelchangedTreeFast(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_listCtrlFast;
};
