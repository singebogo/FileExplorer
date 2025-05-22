
// FIleExplorerDlg.h : 头文件
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
#include <knownfolders.h>  // 确保包含此头文件
#include <comdef.h>
#include <Wbemidl.h> // 需要链接 wbemuuid.lib
#include <wininet.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
// 需要链接 Shell32.lib 和 Shlwapi.lib
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "wbemuuid.lib")

/*
// 如果 SHGetKnownFolderIDListFromIDList 未定义，则手动声明
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

// CFIleExplorerDlg 对话框
class CFIleExplorerDlg : public CDialogEx
{
// 构造
public:
	CFIleExplorerDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 定义要显示的特殊文件夹
	struct SpecialFolder {
		KNOWNFOLDERID folderId;
		CString displayName;
	};

	const SpecialFolder* m_specialFolders;

	  // 控件变量
    CTreeCtrl m_treeCtrl;
    CListCtrl m_listCtrl;
	CString m_strCurrentPath;

	void LoadSettings();
    // 系统图像列表
    CImageList m_systemImageList;
	CImageList m_imageList;
	// 当前目录
    CString m_strCurrentDir;
	void LoadDirectory(LPCTSTR lpszPath);
	void deleteChild(HTREEITEM hItem);
	void RepositionControls();
	 // 记录控件初始位置和大小
    CRect m_recttree;
    CRect m_recttreeFast;
    CRect m_rectList;
    CRect m_rectListFast;
	HTREEITEM m_hRoot;
	CString m_strUserName;

    // 加载文件夹结构
    void LoadFolderTree(HTREEITEM hParent, const CString& strPath);
	int GetIconIndex(const CString& strPath,const CString& cFileName, BOOL bIsFolder);
    // 显示文件列表
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
// 对话框数据
	enum { IDD = IDD_FILEEXPLORER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
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
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	BOOL initToolbar();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	void UpdateStatusBar(LPCTSTR lpszMessage, int nPaneIndex);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEditEnterPressed();  // 自定义回车键处理函数
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
	void ShowDesktopContextMenu(CPoint point); // 显示桌面右键菜单

private:
	CStatusBar m_wndStatusBar; // 状态栏变量
	CToolBar m_wndToolBar; // 工具栏变量
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
