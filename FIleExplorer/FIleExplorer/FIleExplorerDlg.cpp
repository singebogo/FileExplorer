
// FIleExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
//#include "pch.h"
#include "FIleExplorer.h"
#include "FIleExplorerDlg.h"
#include "afxdialogex.h"
#include "CShellMenuHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define  ID_INDICATOR_PROGRESS  10001  // 进度显示
#define  ID_INDICATOR_TIME  10003      // 时间显示
#define  ID_INDICATOR_IP  10004      // 时间显示

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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


// CFIleExplorerDlg 对话框




CFIleExplorerDlg::CFIleExplorerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFIleExplorerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME1);
}

void CFIleExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_curpath);
	DDX_Control(pDX, IDC_LIST_DRIVES, m_pListCtrl);
	DDX_Control(pDX, IDC_TREE_FAST, m_treeCtrlFast);
	DDX_Control(pDX, IDC_LIST2, m_listCtrlFast);
}

BEGIN_MESSAGE_MAP(CFIleExplorerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CFIleExplorerDlg::OnDblclkTree1)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, &CFIleExplorerDlg::OnItemexpandingTree1)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_LBN_DBLCLK(IDC_LIST1, &CFIleExplorerDlg::OnDblclkList1)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CFIleExplorerDlg::OnNMDblclkListFiles)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFIleExplorerDlg::OnDblclkList1)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CFIleExplorerDlg::OnRclickList1)
	ON_WM_CONTEXTMENU()
	ON_EN_CHANGE(IDC_EDIT1, &CFIleExplorerDlg::OnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CFIleExplorerDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DRIVES, OnNMCustomdrawList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FAST, &CFIleExplorerDlg::OnSelchangedTreeFast)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFIleExplorerDlg::OnSelchangedTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CFIleExplorerDlg::OnDblclkList2)
END_MESSAGE_MAP()


// CFIleExplorerDlg 消息处理程序

BOOL CFIleExplorerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	SetWindowText(L"文件管理器");

	 //initToolbar();
	
	 // 创建状态栏
    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return FALSE;
    }
	// 设置状态栏分区
    static UINT indicators[] =
    {
        ID_SEPARATOR,           // 状态栏第一个分区（默认提示信息）
        ID_INDICATOR_PROGRESS,  // 进度显示
        ID_INDICATOR_TIME,      // 时间显示
		ID_INDICATOR_IP,		// 网络管理
    };
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	 // 设置分区宽度
    CRect rect;
    GetClientRect(&rect);
    int nWidth = rect.Width() / 4; // 将状态栏分为 3 个等宽分区
    m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, nWidth);
    m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, nWidth);
    m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TIME, SBPS_NORMAL, nWidth);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_IP, SBPS_NORMAL, nWidth);
    // 调整状态栏位置
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);	

	ShowWindow(SW_NORMAL);

	CenterToPrimaryMonitor();

	m_treeCtrl.GetWindowRect(&m_recttree);
	ScreenToClient(&m_recttree);

	m_treeCtrlFast.GetWindowRect(&m_recttreeFast);
	ScreenToClient(&m_recttreeFast);

	m_listCtrl.GetWindowRect(&m_rectList);
	ScreenToClient(&m_rectList);

	m_listCtrlFast.GetWindowRect(&m_rectListFast);
	ScreenToClient(&m_rectListFast);

	// 设置列表控件样式
	//m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES|LVS_REPORT);
	 // 初始化 ListCtrl 为报表模式
    m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_pListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_listCtrlFast.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	// 添加列
    m_listCtrl.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 200);
    m_listCtrl.InsertColumn(1, _T("类型"), LVCFMT_LEFT, 120);
    m_listCtrl.InsertColumn(2, _T("大小"), LVCFMT_RIGHT, 100);
    m_listCtrl.InsertColumn(3, _T("修改日期"), LVCFMT_LEFT, 150);
	// 设置字体
	m_treeCtrl.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	m_treeCtrl.ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

	

	// 创建图像列表
	m_systemImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 4);
	// 确保 CImageList 对象是空的
	m_imageList.Create(16, 16, ILC_COLOR32, 4, 4);

	if (m_systemImageList.GetSafeHandle() != NULL)
	{
		m_systemImageList.Detach();
	}

	// 获取系统图像列表
	SHFILEINFO sfi = {0};
	HIMAGELIST hSystemImageList = (HIMAGELIST)SHGetFileInfo(
		_T("C:\\"), 0, &sfi, sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON
		);

	if (hSystemImageList == NULL)
	{
		AfxMessageBox(_T("Failed to get system image list!"));
		return FALSE;
	}

	// 将系统图像列表附加到CImageList
	m_systemImageList.Attach(hSystemImageList);

	// 获取系统图像列表
	SHFILEINFO listsfi = { 0 };
    HIMAGELIST hListSystemImageList = (HIMAGELIST)SHGetFileInfo(
        _T("C:\\"), 0, &listsfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	if (hListSystemImageList == NULL)
	{
		AfxMessageBox(_T("Failed to get system image list!"));
		return FALSE;
	}
	if (m_imageList.GetSafeHandle() != NULL)
	{
		m_imageList.Detach();
	}
	//m_imageList.Attach(hListSystemImageList);
	// 将图像列表关联到CTreeCtrl
	m_treeCtrl.SetImageList(&m_systemImageList, TVSIL_NORMAL);
	m_listCtrl.SetImageList(CImageList::FromHandle(hListSystemImageList), LVSIL_SMALL);
	m_listCtrlFast.SetImageList(CImageList::FromHandle(hListSystemImageList), LVSIL_SMALL);
	// 添加根节点
	// HTREEITEM m_hRoot = m_treeCtrl.InsertItem(_T("My Computer"), TVI_ROOT);
	HTREEITEM m_hRoot = AddTreeItem(m_treeCtrl, _T("My Computer"), L"", listsfi.iIcon, listsfi.iIcon, TVI_ROOT);

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2); // 列表控件ID
	m_listCtrl.ShowWindow(SW_NORMAL);
	pList->ShowWindow(SW_HIDE);

	m_strUserName = GetCurrentUserNameWMI();

	InitializeSpecialFoldersTree();

	m_strUserName = GetCurrentUserNameWMI();
	LoadAndDisplayDrives();

	// 加载盘符到树形控件
	LoadDrivesToTreeCtrl();

	// 读取配置
	LoadSettings();

	// 更新状态栏
	UpdateStatusBar(m_strUserName, 1);

	// 网络连接 ip地址
	UpdateStatusBarNetwork();
	

	// 设置定时器（每秒更新一次）
    SetTimer(1, 1000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFIleExplorerDlg::UpdateStatusBarNetwork()
{
	if (IsInternetAvailable())
	{
		CStringArray ips;
		GetLocalIPAddressesEx(ips);
		ips.InsertAt(0, CString("已联网 "));
		CString strCombined;
		for (int i = 0; i < ips.GetSize(); i++)
		{
			if (i > 0) strCombined += _T(", ");
			strCombined += ips.GetAt(i);
		}

		UpdateStatusBar(strCombined, 3);
	}
	else
	{
		UpdateStatusBar(_T("未检测到网络连接"), 3);
	}
}

 void CFIleExplorerDlg::GetLocalIPAddressesEx(CStringArray& ipArray)
{
	ULONG ulOutBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;

	// 第一次调用获取缓冲区大小
	dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &ulOutBufLen);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
		pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(ulOutBufLen);
		if (pAddresses == NULL) {
			return ;
		}
	}

	// 第二次调用获取实际数据
	if ((dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &ulOutBufLen)) == NO_ERROR) {
		PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
		while (pCurrAddresses) {
			PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
			while (pUnicast) {
				if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
					// IPv4地址
					sockaddr_in* sa_in = (sockaddr_in*)pUnicast->Address.lpSockaddr;
					char ipStr[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &(sa_in->sin_addr), ipStr, INET_ADDRSTRLEN);
					CString strIP(ipStr);
					if (strIP != "127.0.0.1") {
						ipArray.Add(strIP);
					}
				}
				pUnicast = pUnicast->Next;
			}
			pCurrAddresses = pCurrAddresses->Next;
		}
	}

	if (pAddresses)
		free(pAddresses);

	return;
}

BOOL CFIleExplorerDlg::IsInternetAvailable()
{
	DWORD dwFlags = 0;
	return InternetGetConnectedState(&dwFlags, 0) != FALSE;
}

BOOL CFIleExplorerDlg::initToolbar()
{
	if (!m_reBar.Create(this))
		return FALSE;

	if (!m_wndToolBar.CreateEx(this))
		return FALSE;


	//TBSTYLE_TRANSPARENT是使CToolBar透明，可以显示CReBar的背景。
	//TBSTYLE_LIST用于设置按钮文字时，文字在按钮的右边，默认情况下是文字在按钮的下部
	m_wndToolBar.ModifyStyle(0, TBSTYLE_TRANSPARENT | TBSTYLE_LIST);

	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);//设置下拉箭头样式

	m_wndToolBar.SetButtons(NULL, 5);//设置ToolBar 按钮个数
	m_reBar.AddBar(&m_wndToolBar);

	//2、添加按钮：

		CRect rect;
	int nIndex = -1;

	//添加文本
	m_wndToolBar.SetButtonInfo(++nIndex, 0, TBSTYLE_BUTTON | BTNS_AUTOSIZE | TBSTYLE_AUTOSIZE | TBBS_DISABLED, -1);
	//此处是为了增加按钮的宽度，可以更加自己的需要适当的调整，由于是不可见字符，因此是透明的
	m_wndToolBar.SetButtonText(nIndex, _T(" "));
	m_wndToolBar.GetItemRect(nIndex, &rect);
	rect.top += 3;//此处是为了让文本垂直居中
	m_static.Create(_T("工具栏"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SIMPLE, rect, &m_wndToolBar);
	m_static.SetFont(m_wndToolBar.GetFont());

	//添加组合框
	m_wndToolBar.SetButtonInfo(++nIndex, 0, TBSTYLE_SEP, 80);
	m_wndToolBar.GetItemRect(nIndex, &rect);
	rect.bottom += 100;
	m_combo.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, rect, &m_wndToolBar, 0);m_combo.SetFont(m_wndToolBar.GetFont());

	//添加
	m_wndToolBar.SetButtonInfo(++nIndex, 0,TBSTYLE_BUTTON | TBBS_DISABLED | TBSTYLE_AUTOSIZE, -1);
	m_wndToolBar.SetButtonText(nIndex, _T(" "));
	m_wndToolBar.GetItemRect(nIndex, &rect);
	m_btn.Create(_T("你好"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, &m_wndToolBar, 0);
	m_btn.SetFont(m_wndToolBar.GetFont());
	//添加下拉按钮，这个-2表示不显示按钮位图，但是显示了文字，这个我通过跟踪IE8的CToolBar按钮得到的，微软完全没有说明。
	//至于这个-2是怎么来的，我会在另一个文章中说明。
	m_wndToolBar.SetButtonInfo(++nIndex, 0, TBSTYLE_AUTOSIZE | BTNS_WHOLEDROPDOWN, -2);
	m_wndToolBar.SetButtonText(nIndex, _T("下拉列表"));

	//显示ReBar和工具栏
	CRect rcWnd;
	GetClientRect(&rcWnd);
	m_reBar.SetWindowPos(NULL, 0, 0, rcWnd.Width(), 30, SWP_SHOWWINDOW);//显示CReBar
	return TRUE;
}

void CFIleExplorerDlg::RepositionControls()
{
    if (m_wndToolBar.GetSafeHwnd())
    {
        CRect rectClient;
        GetClientRect(rectClient);

        // 设置工具栏位置
        m_wndToolBar.SetWindowPos(NULL, 0, 0, rectClient.Width(), 30, SWP_NOZORDER);

        // 调整其他控件的位置（例如，将其他控件下移工具栏的高度）
        CRect rectToolBar;
        m_wndToolBar.GetWindowRect(rectToolBar);
        int toolbarHeight = rectToolBar.Height();

        // 假设有一个控件需要调整位置
		/*
        CWnd* pControl = GetDlgItem(IDC_EDIT1);
        if (pControl)
        {
            CRect rectControl;
            pControl->GetWindowRect(rectControl);
            ScreenToClient(rectControl);
            pControl->SetWindowPos(NULL, rectControl.left, rectControl.top + toolbarHeight, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
		*/
    }
}

void CFIleExplorerDlg::CenterToPrimaryMonitor()
{
	// 获取主显示器的信息
	HMONITOR hMonitor = MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo = { 0 };
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &monitorInfo);

	// 获取显示器的矩形区域
	CRect rectMonitor = monitorInfo.rcWork;

	// 获取窗口的宽度和高度
	CRect rectWindow;
	GetWindowRect(&rectWindow);
	int cxWindow = rectWindow.Width();
	int cyWindow = rectWindow.Height();

	// 计算居中位置
	int x = rectMonitor.left + (rectMonitor.Width() - cxWindow) / 2;
	int y = rectMonitor.top + (rectMonitor.Height() - cyWindow) / 2;

	// 设置窗口位置
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CFIleExplorerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFIleExplorerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFIleExplorerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFIleExplorerDlg::LoadDrivesToTreeCtrl()
{
	// 获取所有盘符
	std::vector<std::wstring> drives = GetLogicalDrives();

	// 清空树形控件
	m_treeCtrl.DeleteAllItems();

	// 添加盘符到树形控件
	for(size_t i = 0; i < drives.size(); ++i)
	{	
		// 获取系统图像列表
		SHFILEINFO sfi;
		SHGetFileInfo(
			drives[i].c_str(), 0, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
			);

		// 添加节点并设置图标
		HTREEITEM hDrive = AddTreeItem(m_treeCtrl, drives[i].c_str(), drives[i].c_str(), sfi.iIcon, sfi.iIcon, TVI_ROOT);
		// 添加一个空的子节点（用于动态加载子文件夹）
		//m_treeCtrl.InsertItem(drives[i].c_str(), hDrive);
		
		AddTreeItem(m_treeCtrl, drives[i].c_str(), GetFullPath(hDrive), sfi.iIcon, sfi.iIcon, hDrive, NULL);
	}
	//SelectFirstChildItem();
	
}

void CFIleExplorerDlg::SelectFirstChildItem()
{
    HTREEITEM hRootItem = m_treeCtrl.GetRootItem(); // 获取根节点

    if (hRootItem != NULL)
    {
        HTREEITEM hFirstChild = m_treeCtrl.GetChildItem(hRootItem); // 获取第一个子节点
        if (hFirstChild != NULL)
        {
            m_treeCtrl.SelectItem(hFirstChild); // 选择第一个子节点
            m_treeCtrl.EnsureVisible(hFirstChild); // 确保子节点可见
			CString strPath = GetFullPath(hRootItem);
			m_strCurrentPath = strPath;	
			m_curpath.SetWindowTextW(m_strCurrentPath);
			ShowFileList(m_strCurrentPath);
        }
    }
}

std::vector<std::wstring> CFIleExplorerDlg::GetLogicalDrives()
{
	std::vector<std::wstring> drives;

	// 获取逻辑驱动器字符串
	TCHAR szDrives[MAX_PATH];
	DWORD dwResult = GetLogicalDriveStrings(MAX_PATH, szDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		TCHAR* pDrive = szDrives;
		while (*pDrive)
		{
			drives.push_back(pDrive); // 将盘符添加到 vector
			pDrive += _tcslen(pDrive) + 1;
		}
	}

	return drives;
}


// 显示文件列表
void CFIleExplorerDlg::ShowFileList(const CString& strPath)
{

	// 设置列表控件为报表模式
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2); // 列表控件ID
	pList->ShowWindow(SW_HIDE);
	m_listCtrl.ShowWindow(SW_NORMAL);
	m_listCtrl.SetView(LV_VIEW_DETAILS);
	CHeaderCtrl* pHeaderCtrl = m_listCtrl.GetHeaderCtrl();
	if (pHeaderCtrl != NULL)
	{
		int  nColumnCount = pHeaderCtrl->GetItemCount();
		for (int i = 0; i < nColumnCount; i++)
		{
			m_listCtrl.DeleteColumn(0);
		}
	}

	// 添加列
	m_listCtrl.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(1, _T("类型"), LVCFMT_LEFT, 120);
	m_listCtrl.InsertColumn(2, _T("大小"), LVCFMT_RIGHT, 100);
	m_listCtrl.InsertColumn(3, _T("修改日期"), LVCFMT_LEFT, 150);

	m_listCtrl.DeleteAllItems();
	m_strCurrentPath = strPath;	
	m_curpath.SetWindowTextW(m_strCurrentPath);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + _T("\\*.*"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// 跳过特殊目录（. 和 ..）
		if (finder.IsDots())
		{
			// 添加 "返回上级" 项（如果不是根目录）
			CString name = L"";
			if (strPath[3] != _T('\0')) { // 非根目录（如 C:\Test）
				SHFILEINFO sfi = { 0 };
				if(_T("..") == finder.GetFileName())
				{
					name = _T("上级目录");
				}
				if(_T(".") == finder.GetFileName())
				{
					name = _T("当前目录");
				}
				
				SHGetFileInfo(finder.GetFileName(), 0, &sfi, sizeof(SHFILEINFO), 
					SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);
				int nIndex = m_listCtrl.InsertItem(0, finder.GetFileName(), sfi.iIcon);
				m_listCtrl.SetItemText(nIndex, 1, name);
			}
			continue;
		}

		// 获取文件或文件夹名称
		CString strName = finder.GetFileName();

		// 判断是文件还是文件夹
		BOOL bIsFolder = finder.IsDirectory();

		// 获取图标索引
		int nIconIndex = GetIconIndex(finder.GetFilePath(),strName, bIsFolder);

		int nItem = m_listCtrl.InsertItem(0, strName, nIconIndex);
		m_listCtrl.SetItemText(nItem, 1, bIsFolder ? _T("文件夹") : _T("文件"));

		// 如果是文件，显示大小
		if (!bIsFolder)
		{
			CString strSize;
			strSize.Format(_T("%lld KB"), finder.GetLength() / 1024);
			m_listCtrl.SetItemText(nItem, 2, strSize);
		}
		 // 修改时间
        CTime timeModified;
		finder.GetLastWriteTime(timeModified);
        m_listCtrl.SetItemText(nItem, 3, timeModified.Format(_T("%Y-%m-%d %H:%M")));
	}
}

void CFIleExplorerDlg::LoadDirectory(LPCTSTR lpszPath)
{
    m_listCtrl.DeleteAllItems(); // 清空列表

    // 添加 "返回上级" 项（如果不是根目录）
    if (lpszPath[3] != _T('\0')) { // 非根目录（如 C:\Test）
        SHFILEINFO sfi = { 0 };
        SHGetFileInfo(_T(".."), 0, &sfi, sizeof(SHFILEINFO), 
            SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);
        
        int nIndex = m_listCtrl.InsertItem(0, _T(".."), sfi.iIcon);
        m_listCtrl.SetItemText(nIndex, 1, _T("上级目录"));
    }

    // 遍历目录
    CString strSearchPath = lpszPath;
    strSearchPath += _T("*.*");

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(strSearchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        //AfxMessageBox(_T("目录无法访问！"), _T("错误"), MB_ICONERROR);
        return;
    }

    int nItemIndex = 0;
    do {
        if (wcscmp(findData.cFileName, _T(".")) == 0 || 
            wcscmp(findData.cFileName, _T("..")) == 0) 
            continue;

		 // 获取文件信息
        SHFILEINFO sfi = { 0 };
		int nIndex = 0;
		CString cFileName = findData.cFileName;
		 // 设置其他列数据
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			 // 插入列表项
			int nImageIndex = GetIconIndex(lpszPath+cFileName, cFileName, TRUE);
			nIndex = m_listCtrl.InsertItem(nItemIndex, findData.cFileName, nImageIndex);
		}
		else{
			int nImageIndex = GetIconIndex(lpszPath+cFileName, cFileName, FALSE);
			nIndex = m_listCtrl.InsertItem(nItemIndex, findData.cFileName, nImageIndex);
		}              

        // 设置其他列数据
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            m_listCtrl.SetItemText(nIndex, 1, _T("文件夹"));
            m_listCtrl.SetItemText(nIndex, 2, _T(""));
        } else {
            // 文件类型
            m_listCtrl.SetItemText(nIndex, 1, sfi.szTypeName);

            // 文件大小
            CString strSize;
            strSize.Format(_T("%.2f KB"), (double)findData.nFileSizeLow / 1024);
            m_listCtrl.SetItemText(nIndex, 2, strSize);
        }

        // 修改时间
        CTime timeModified(findData.ftLastWriteTime);
        m_listCtrl.SetItemText(nIndex, 3, timeModified.Format(_T("%Y-%m-%d %H:%M")));

        nItemIndex++;
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
}

// 获取文件或文件夹的图标索引
int CFIleExplorerDlg::GetIconIndex(const CString& strPath, const CString& cFileName, BOOL bIsFolder)
{
	SHFILEINFO sfi = {0};
	DWORD dwFlags = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;

	if (bIsFolder)
	{
		// 如果是文件夹，直接获取文件夹图标
		SHGetFileInfo(
			strPath, 0, &sfi, sizeof(SHFILEINFO),
			dwFlags | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
			);
	}
	else
	{
		// 如果是文件，根据扩展名获取图标
		SHGetFileInfo(
			strPath, 0, &sfi, sizeof(SHFILEINFO),
			dwFlags | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME
			);
	}

	return sfi.iIcon;
}

// 获取树节点的完整路径
CString CFIleExplorerDlg::GetFullPath(HTREEITEM hItem)
{
	CString strPath;
	while (hItem)
	{
		strPath = m_treeCtrl.GetItemText(hItem) + _T("\\") + strPath;
		hItem = m_treeCtrl.GetParentItem(hItem);
	}
	return strPath;
}

void CFIleExplorerDlg::OnDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
	if (hItem)
	{
		CString strPath = GetFullPath(hItem);
		
		//LoadDirectory(strPath);
		ShowFileList(strPath);
	}
	*pResult = 0;

}

void CFIleExplorerDlg::deleteChild(HTREEITEM hItem)
{
	// 清理对应分支
	m_treeCtrl.SetRedraw(FALSE); // 禁用重绘
	HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		// 释放关联的自定义数据
		DWORD_PTR pData = m_treeCtrl.GetItemData(hChildItem);
		if (pData != NULL)
		{
			delete reinterpret_cast<DWORD_PTR*>(pData); // 替换为实际数据类型
		}

		// 删除当前子项
		HTREEITEM hNextItem = m_treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
		m_treeCtrl.DeleteItem(hChildItem);
		hChildItem = hNextItem;
	}

	m_treeCtrl.SetRedraw(TRUE); // 启用重绘
	m_treeCtrl.Invalidate(); // 强制重绘
}


void CFIleExplorerDlg::OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// 如果节点正在展开
	if (pNMTreeView->action == TVE_EXPAND)
	{
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;

		// 删除空的子节点
		deleteChild(hItem);

		// 获取节点路径
		CString strPath = GetItemPath(hItem);

		// 遍历文件夹并添加到树形控件
		PopulateTree(hItem, strPath);

	}
}
void CFIleExplorerDlg::PopulateTree(HTREEITEM hParent, const CString& strPath)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + _T("\\*.*"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			// 插入子节点
			SHFILEINFO sfiFolder;
			SHGetFileInfo(
				finder.GetFilePath(), 0, &sfiFolder, sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
				);

			HTREEITEM hDrive = AddTreeItem(m_treeCtrl, finder.GetFileName(), GetFullPath(hParent)+"\\"+ finder.GetFileName(), sfiFolder.iIcon, sfiFolder.iIcon, hParent, NULL);
			// 添加一个空的子节点（用于动态加载子文件夹）
			//m_treeCtrl.InsertItem(_T(""), hDrive);  
			AddTreeItem(m_treeCtrl, finder.GetFileName(), GetFullPath(hParent) + "\\" + finder.GetFileName(), sfiFolder.iIcon, sfiFolder.iIcon, hDrive, NULL);
		}
		else
		{
			CString strName = finder.GetFileName();
			// 插入子节点
			SHFILEINFO filesfi;
			SHGetFileInfo(
				finder.GetFilePath(), 0, &filesfi, sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME
				);
			//m_treeCtrl.InsertItem(strName, filesfi.iIcon, filesfi.iIcon, hParent, NULL);
			AddTreeItem(m_treeCtrl, strName, GetFullPath(hParent) + "\\" + strName, filesfi.iIcon, filesfi.iIcon, hParent, NULL);
		}
	}

	finder.Close();
}

CString CFIleExplorerDlg::GetItemPath(HTREEITEM hItem)
{
	CString strPath;

	// 从当前节点开始构建路径
	while (hItem != NULL)
	{
		CString strName = m_treeCtrl.GetItemText(hItem);
		strPath = strName + _T("\\") + strPath;
		hItem = m_treeCtrl.GetParentItem(hItem);
	}

	return strPath;
}


void CFIleExplorerDlg::LoadSubFolders(HTREEITEM hParent, const CString& strPath)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + _T("\\*.*"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDirectory() && !finder.IsDots())
		{
			// 插入子节点
			SHFILEINFO sfi;
			SHGetFileInfo(
				finder.GetFilePath(), 0, &sfi, sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
				);

			//m_treeCtrl.InsertItem(finder.GetFileName(), sfi.iIcon, sfi.iIcon, hParent);
			AddTreeItem(m_treeCtrl, finder.GetFileName(), finder.GetFileName(), sfi.iIcon, sfi.iIcon, hParent, NULL);
		}
	}
}

void CFIleExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType != SIZE_MINIMIZED) // 如果窗口不是最小化
	{
		if (m_wndStatusBar.GetSafeHwnd())
		{
			// 调整状态栏位置
			RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

			// 调整分区宽度
			CRect rect;
			GetClientRect(&rect);
			int nWidth = rect.Width() / 3;
			m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, nWidth);
			m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, nWidth);
			m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TIME, SBPS_NORMAL, rect.Width() / 5);
		}

		if (m_wndToolBar.GetSafeHwnd())
		{
			// 调整工具栏位置
			RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		}

		if(m_recttree && m_listCtrl)
		{
			// 计算控件的新位置和大小
			CRect rc;
			m_wndStatusBar.GetWindowRect(&rc);
			int dx = cx - m_recttree.right  - 2;  // 水平变化量
			int dy = cy - m_recttree.bottom  - rc.Height(); // 垂直变化量
			
			// 调整编辑框大小
			m_treeCtrl.MoveWindow(
				m_recttree.left, m_recttree.top,
				m_recttree.Width(), m_recttree.Height()  + dy
				);

			m_treeCtrlFast.MoveWindow(
				m_recttreeFast.left, m_recttreeFast.top,
				m_recttreeFast.Width(), m_recttreeFast.Height() + dy
			);
			// 调整列表控件大小
			m_listCtrl.MoveWindow(
				m_rectList.left, m_rectList.top,
				m_rectList.Width() + dx, m_rectList.Height() + dy
				);
			// 调整列表控件大小
			m_listCtrlFast.MoveWindow(
				m_rectListFast.left, m_rectListFast.top,
				m_rectListFast.Width() + dx, m_rectListFast.Height() + dy
			);
		}        
	}
}


void CFIleExplorerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);


	if (m_wndStatusBar.GetSafeHwnd())
    {
        // 调整状态栏位置
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

        int nWidth = (pRect->right - pRect->left) / 3;
        m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, nWidth);
        m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, nWidth);
        m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TIME, SBPS_NORMAL, nWidth);
    }

	if(m_recttree && m_listCtrl)
		{
			// 计算控件的新位置和大小
			int dx = pRect->right - pRect->left - m_recttree.right - 2;  // 水平变化量
			CRect rc;
			m_wndStatusBar.GetWindowRect(&rc);
			int dy = pRect->bottom - pRect->top - m_recttree.bottom  - rc.Height(); // 垂直变化	

			// 控件大小
			m_treeCtrl.MoveWindow(
				m_recttree.left, m_recttree.top,
				m_recttree.Width(), m_recttree.Height()  + dy
				);

			m_treeCtrlFast.MoveWindow(
				m_recttreeFast.left, m_recttreeFast.top,
				m_recttreeFast.Width(), m_recttreeFast.Height() + dy
			);

			// 调整列表控件大小
			m_listCtrl.MoveWindow(
				m_rectList.left, m_rectList.top,
				m_rectList.Width() + dx, m_rectList.Height() + dy
				);
			// 调整列表控件大小
			m_listCtrlFast.MoveWindow(
				m_rectListFast.left, m_rectListFast.top,
				m_rectListFast.Width() + dx, m_rectListFast.Height() + dy
			);
		}     	
}


void CFIleExplorerDlg::OnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码


}


void CFIleExplorerDlg::OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	*pResult = 0;
}


void CFIleExplorerDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nItem = pNMItemActivate->iItem;
    if (nItem == -1) return;

    CString strName = m_listCtrl.GetItemText(nItem, 0);
    CString strCurrentPath = m_strCurrentPath; // 成员变量保存当前路径
	m_curpath.SetWindowTextW(m_strCurrentPath);

	if (strName == _T(".")) {
		// 当前目录
	}
    else if (strName == _T("..")) {
        // 返回上级目录
		 if (strCurrentPath.Right(1) == _T("\\"))
			 strCurrentPath = strCurrentPath.Left(strCurrentPath.GetLength()  - 1);
        int nPos = strCurrentPath.ReverseFind(_T('\\'));
        if (nPos != -1) {
            strCurrentPath = strCurrentPath.Left(nPos);
            if (strCurrentPath.Right(1) != _T("\\")) 
                strCurrentPath += _T("\\");
        }
    } else {
        // 检查是否为文件夹
        CString strFullPath = strCurrentPath + strName;
        DWORD dwAttrib = GetFileAttributes(strFullPath);
        if (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
            strCurrentPath = strFullPath + _T("\\");
        } else {
            // 处理文件打开操作（例如调用 ShellExecute）
            ShellExecute(NULL, _T("open"), strFullPath, NULL, NULL, SW_SHOWNORMAL);
            return;
        }
    }

    // 重新加载目录
    ShowFileList(strCurrentPath);

	*pResult = 0;
}


void CFIleExplorerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
    {
        // 更新时间显示
        UpdateStatusBar(CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")), 2);
    }

	CDialogEx::OnTimer(nIDEvent);
}

void CFIleExplorerDlg::UpdateStatusBar(LPCTSTR lpszMessage, int nPaneIndex)
{
    m_wndStatusBar.SetPaneText(nPaneIndex, lpszMessage);
    m_wndStatusBar.UpdateWindow();
}

void CFIleExplorerDlg::OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CFIleExplorerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	// 确保右键点击的是 ListCtrl
     // 显示桌面右键菜单
	 if (pWnd == &m_listCtrl)
    {
		// ShowDesktopContextMenu(point);
		 // 获取当前选择的项索引（-1表示没有选择）
		 

		 // 获取当前选中项数量
		 int nSelectedCount = m_listCtrl.GetSelectedCount();
		 CString strPath = m_strCurrentPath;
		 strPath.Replace(_T("\\\\"), _T("\\"));

		 if (nSelectedCount > 1)
		 {
			 CStringArray arr;
			 // 获取所有选择的项
			 POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
			 if (pos == NULL)
			 {
				 TRACE("No items are selected.\n");
			 }
			 else
			 {
				 while (pos)
				 {
					 int nItem = m_listCtrl.GetNextSelectedItem(pos);
					 CString strText = m_listCtrl.GetItemText(nItem, 0);
					 CString path = strPath + strText;
					 arr.Add(path);
				 }
			 }
			 ShowShellContextMenuForMultipleFiles(m_listCtrl.m_hWnd, arr, point);
		 }
		 else if (nSelectedCount == 1)
		 {
			 int nSelectedItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
			 // 获取该项的文本
			 CString strText = m_listCtrl.GetItemText(nSelectedItem, 0); // 0表示第一列
			 CString path = strPath + strText;
			 ShowShellContextMenu(m_listCtrl.m_hWnd, path, point);
		 }
		 else
		 {
			 ShowDesktopMenuSafely(this->FromHandle(m_listCtrl.m_hWnd), point);
		 }		 
	 }
}
void CFIleExplorerDlg::ShowDesktopMenuSafely(CWnd* pParentWnd, CPoint point)
{
	CShellMenuHelper helper;
	if (!helper.GetDesktopMenu())
	{
		AfxMessageBox(_T("无法获取桌面菜单"));
		return;
	}

	HMENU hMenu = helper.CreateMenu();
	if (!hMenu)
	{
		AfxMessageBox(_T("无法创建菜单"));
		return;
	}

	int nCmd = TrackPopupMenu(hMenu,
		TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
		point.x, point.y,
		0, pParentWnd->GetSafeHwnd(), NULL);

	if (nCmd > 0)
	{
		CMINVOKECOMMANDINFO cmdInfo = { 0 };
		cmdInfo.cbSize = sizeof(CMINVOKECOMMANDINFO);
		cmdInfo.hwnd = pParentWnd->GetSafeHwnd();
		cmdInfo.lpVerb = (LPCSTR)MAKEINTRESOURCE(nCmd - 1);
		cmdInfo.nShow = SW_SHOWNORMAL;

		helper.GetContextMenu()->InvokeCommand(&cmdInfo);
	}

	DestroyMenu(hMenu);
}

void CFIleExplorerDlg::ShowShellContextMenu(HWND hWnd, const CString& strFilePath, CPoint point)
{
	// 1. 初始化 COM
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
		return;

	// 2. 获取桌面文件夹接口
	IShellFolder* pDesktopFolder = NULL;
	hr = SHGetDesktopFolder(&pDesktopFolder);
	if (FAILED(hr))
	{
		CoUninitialize();
		return;
	}

	// 3. 将文件路径转换为宽字符
	LPITEMIDLIST pidl = NULL;
	//STRRET strret;
	WCHAR wszPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, CW2A(strFilePath), -1, wszPath, MAX_PATH);
	ULONG chEaten = 0;
	ULONG dwAttributes = 0;
	// 4. 解析路径获取 PIDL
	//hr = pDesktopFolder->ParseDisplayName(NULL, NULL, wszPath, NULL, &pidl, NULL);
	hr = pDesktopFolder->ParseDisplayName(
		NULL,                   // HWND hwndOwner
		NULL,                   // IBindCtx* pbc
		wszPath,                // LPWSTR pszDisplayName
		&chEaten,               // ULONG* pchEaten
		&pidl,                  // LPITEMIDLIST* ppidl
		&dwAttributes           // ULONG* pdwAttributes
	);

	if (FAILED(hr))
	{
		pDesktopFolder->Release();
		CoUninitialize();
		return;
	}

	// 5. 获取上下文菜单接口
	IContextMenu* pContextMenu = NULL;
	hr = pDesktopFolder->GetUIObjectOf(hWnd, 1, (LPCITEMIDLIST*)&pidl,
		IID_IContextMenu, NULL, (void**)&pContextMenu);
	if (FAILED(hr))
	{
		CoTaskMemFree(pidl);
		pDesktopFolder->Release();
		CoUninitialize();
		return;
	}

	// 6. 创建弹出菜单
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		// 获取默认菜单
		hr = pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL);
		if (SUCCEEDED(hr))
		{
			// 获取光标位置
			//POINT pt;
			//GetCursorPos(&pt);

			// 显示菜单
			int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, hWnd, NULL);

			// 处理菜单选择
			if (iCmd > 0)
			{
				CMINVOKECOMMANDINFO cmi = { 0 };
				cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
				cmi.hwnd = hWnd;
				cmi.lpVerb = (LPCSTR)MAKEINTRESOURCE(iCmd - 1);
				cmi.nShow = SW_SHOWNORMAL;

				pContextMenu->InvokeCommand(&cmi);
			}
		}

		DestroyMenu(hMenu);
	}

	// 7. 释放资源
	pContextMenu->Release();
	CoTaskMemFree(pidl);
	pDesktopFolder->Release();
	CoUninitialize();
}

void CFIleExplorerDlg::ShowShellContextMenuForMultipleFiles(HWND hWnd, const CStringArray& arrFiles, CPoint point)
{
	// 初始化 COM
	CoInitialize(NULL);

	// 获取桌面文件夹
	IShellFolder* pDesktopFolder = NULL;
	SHGetDesktopFolder(&pDesktopFolder);

	// 分配 PIDL 数组
	LPITEMIDLIST* pidlArray = new LPITEMIDLIST[arrFiles.GetCount()];
	ZeroMemory(pidlArray, sizeof(LPITEMIDLIST) * arrFiles.GetCount());

	// 转换每个文件路径为 PIDL
	for (int i = 0; i < arrFiles.GetCount(); i++)
	{
		WCHAR wszPath[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, CW2A(arrFiles[i]), -1, wszPath, MAX_PATH);
		pDesktopFolder->ParseDisplayName(NULL, NULL, wszPath, NULL, &pidlArray[i], NULL);
	}

	// 获取上下文菜单
	IContextMenu* pContextMenu = NULL;
	pDesktopFolder->GetUIObjectOf(hWnd, arrFiles.GetCount(), (LPCITEMIDLIST*)pidlArray,
		IID_IContextMenu, NULL, (void**)&pContextMenu);

	// 创建并显示菜单
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL);

		//POINT pt;
		//GetCursorPos(&pt);

		int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			point.x, point.y, hWnd, NULL);

		if (iCmd > 0)
		{
			CMINVOKECOMMANDINFO cmi = { 0 };
			cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
			cmi.hwnd = hWnd;
			cmi.lpVerb = (LPCSTR)MAKEINTRESOURCE(iCmd - 1);
			cmi.nShow = SW_SHOWNORMAL;

			pContextMenu->InvokeCommand(&cmi);
		}

		DestroyMenu(hMenu);
	}

	// 清理
	for (int i = 0; i < arrFiles.GetCount(); i++)
	{
		if (pidlArray[i])
			CoTaskMemFree(pidlArray[i]);
	}
	delete[] pidlArray;

	pContextMenu->Release();
	pDesktopFolder->Release();
	CoUninitialize();
}

void CFIleExplorerDlg::ShowDesktopContextMenu(CPoint point)
{
    // 初始化 COM 库
    CoInitialize(NULL);

    // 获取桌面文件夹对象
    IShellFolder* pDesktopFolder = NULL;
    if (SHGetDesktopFolder(&pDesktopFolder) == S_OK)
    {
        // 获取桌面的 PIDL
        LPITEMIDLIST pidl = NULL;
        if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl) == S_OK)
        {
            // 获取桌面的 IContextMenu 接口
            IContextMenu* pContextMenu = NULL;
            if (pDesktopFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidl, IID_IContextMenu, NULL, (void**)&pContextMenu) == S_OK)
            {
                // 创建弹出菜单
                HMENU hMenu = CreatePopupMenu();
                if (hMenu)
                {
					UINT flags = CMF_NORMAL | CMF_EXPLORE | CMF_EXTENDEDVERBS; // 设置标志
                    // 填充菜单
                    pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, flags);

                    // 显示菜单
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, GetSafeHwnd(), NULL);

                    // 处理菜单项命令
                    if (cmd > 0)
                    {
                        CMINVOKECOMMANDINFO info = { 0 };
                        info.cbSize = sizeof(info);
                        info.hwnd = GetSafeHwnd();
                        info.lpVerb = (LPCSTR)MAKEINTRESOURCE(cmd - 1);
                        info.nShow = SW_SHOWNORMAL;
                        pContextMenu->InvokeCommand(&info);
                    }

                    // 销毁菜单
                    DestroyMenu(hMenu);
                }

                // 释放 IContextMenu 接口
                pContextMenu->Release();
            }

            // 释放 PIDL
            CoTaskMemFree(pidl);
        }

        // 释放 IShellFolder 接口
        pDesktopFolder->Release();
    }

    // 释放 COM 库
    CoUninitialize();
}
void CFIleExplorerDlg::OnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	CRegKey regKey;
	if (regKey.Create(HKEY_CURRENT_USER, _T("Software\\FileExplorer")) == ERROR_SUCCESS)
	{
		CString str;
		m_curpath.GetWindowTextW(str);
		regKey.SetStringValue(_T("path"), str);
		regKey.Close();
	}
}


void CFIleExplorerDlg::LoadSettings()
{
	CRegKey regKey;
	if (regKey.Open(HKEY_CURRENT_USER, _T("Software\\FileExplorer")) == ERROR_SUCCESS)
	{
		TCHAR szPath[256];
		ULONG nChars = _countof(szPath);
		if (regKey.QueryStringValue(_T("path"), szPath, &nChars) == ERROR_SUCCESS)
		{
			m_curpath.SetWindowTextW(szPath);
			ExpandToPath1(m_treeCtrl, szPath);
			ShowFileList(szPath);
		}
		regKey.Close();
	}
}
BOOL CFIleExplorerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// 处理回车键
		CWnd* pWnd = GetFocus();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
		{
			// 如果焦点在编辑框上，执行特定操作
			OnEditEnterPressed();
			return TRUE; // 已处理
		}

		// 返回TRUE表示已处理，不会关闭对话框
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CANCEL)
	{
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// YourDialog.cpp
void CFIleExplorerDlg::OnEditEnterPressed()
{
	// 获取当前焦点所在的控件
	CWnd* pWnd = GetFocus();

	// 检查是否是编辑框
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
	{
		CEdit* pEdit = (CEdit*)pWnd;

		// 获取编辑框ID
		UINT nID = pEdit->GetDlgCtrlID();

		// 根据不同的编辑框执行不同操作
		switch (nID)
		{
		case IDC_EDIT1:
			// 处理第一个编辑框的回车
			OnEdit1EnterPressed();
			break;

		default:
			// 默认处理
			NextDlgCtrl();  // 移动到下一个控件
			break;
		}
	}
}

void CFIleExplorerDlg::OnEdit1EnterPressed()
{
	// 示例1：验证并移动到下一个控件
	if (ValidateEdit1Content())
	{
		CString str;
		m_curpath.GetWindowTextW(str);
		ExpandToPath1(m_treeCtrl, str);
		ShowFileList(str);
	}
	else
	{
		// 验证失败，保持焦点
		GetDlgItem(IDC_EDIT1)->SetFocus();
	}
}

bool CFIleExplorerDlg::ValidateEdit1Content()
{
	CString str;
	m_curpath.GetWindowTextW(str);
	return IsValidFilePath(str);
}


BOOL CFIleExplorerDlg::IsPathFormatValid(LPCTSTR lpszPath)
{
	// Windows 路径正则表达式
	std::wregex winPathRegex(
		L"^([a-zA-Z]:\\\\|\\\\\\\\)"  // 驱动器或网络路径
		L"([^\\\\/:*?\"<>|]+\\\\?)*"  // 文件夹层级
		L"([^\\\\/:*?\"<>|]+)?$"      // 文件名
	);

	// Unix 风格路径正则表达式
	std::wregex unixPathRegex(
		L"^(/[^/]+)+/?$"
	);

	std::wstring path(lpszPath);

	// 检查是否符合其中一种格式
	return std::regex_match(path, winPathRegex) ||
		std::regex_match(path, unixPathRegex);
}


bool CFIleExplorerDlg::IsValidFilePath(LPCTSTR lpszFilePath)
{
	// 检查路径是否为空
	if (lpszFilePath == NULL || _tcslen(lpszFilePath) == 0)
		return false;

	// 替换正斜杠为反斜杠
	//lpszFilePath.Replace(_T('/'), _T('\\'));


	// 检查路径中是否包含非法字符
	LPCTSTR illegalChars = _T("*?\"<>|");
	for (int i = 0; i < _tcslen(illegalChars); i++) {
		if (_tcschr(lpszFilePath, illegalChars[i]) != NULL)
			return false;
	}

	if (!IsPathFormatValid(lpszFilePath))
	{
		return false;
	}
	// 检查相对路径
	if (PathIsRelative(lpszFilePath))
	{
		// 相对路径的特殊处理
		TCHAR szFullPath[MAX_PATH];
		if (!PathCanonicalize(szFullPath, lpszFilePath))
			return FALSE;
		lpszFilePath = szFullPath;
	}

	// 5. 可选：检查文件/目录是否存在
	DWORD dwAttrib = GetFileAttributes(lpszFilePath);
	if (dwAttrib == INVALID_FILE_ATTRIBUTES)
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND ||
			dwError == ERROR_PATH_NOT_FOUND ||
			dwError == ERROR_INVALID_NAME)
		{
			AfxMessageBox(L"检查文件/目录不存在");
			return FALSE;
		}			
	}

	// 检查是否是目录
	if (!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		// 尝试打开文件验证可访问性
		HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return false;
		}
		else
			CloseHandle(hFile);
	}
	
	return true;
}
void CFIleExplorerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_curpath.GetWindowTextW(str);
	// 遍历文件夹并添加到树形控件
	ExpandToPath1(m_treeCtrl, str);
	ShowFileList(str);
}


// 根据路径查找并展开节点
bool CFIleExplorerDlg::ExpandToPath1(CTreeCtrl& tree, LPCTSTR lpszPath)
{
	// 从根节点开始查找
	HTREEITEM hRoot = tree.GetRootItem();
	if (!hRoot)
		return false;

	// 分割路径为各个部分
	CString strPath(lpszPath);
	CStringArray pathParts;
	SplitPath(strPath, pathParts); // 实现见下文

	// 递归查找
	return FindItem(tree, hRoot, pathParts, 0);
}

// 分割路径函数
void CFIleExplorerDlg::SplitPath(LPCTSTR lpszPath, CStringArray& parts)
{
	CString strPath(lpszPath);
	int nPos = 0;
	CString strPart = strPath.Tokenize(_T("\\/"), nPos);

	while (!strPart.IsEmpty())
	{
		parts.Add(strPart);
		strPart = strPath.Tokenize(_T("\\/"), nPos);
	}
}

// 递归查找并展开节点
bool CFIleExplorerDlg::FindAndExpandNode(CTreeCtrl& tree, HTREEITEM hCurrent, const CStringArray& pathParts, int nIndex)
{
	if (nIndex >= pathParts.GetSize())
		return true; // 已匹配所有部分

	CString strTarget = pathParts.GetAt(nIndex);

	// 遍历当前节点的所有子节点
	HTREEITEM hChild = tree.GetChildItem(hCurrent);
	while (hChild)
	{
		CString strText = tree.GetItemText(hChild);

		// 比较节点文本或完整路径
		LPTSTR pszPath = (LPTSTR)tree.GetItemData(hChild);
		CString strNodePath(pszPath ? pszPath : _T(""));

		if (strText.CompareNoCase(strTarget) == 0 ||
			(!strNodePath.IsEmpty() && strNodePath.CompareNoCase(strTarget) == 0))
		{
			// 展开当前节点
			tree.Expand(hChild, TVE_EXPAND);

			// 递归查找下一级
			if (FindAndExpandNode(tree, hChild, pathParts, nIndex + 1))
			{
				// 确保目标节点可见
				tree.EnsureVisible(hChild);
				return true;
			}
		}

		hChild = tree.GetNextSiblingItem(hChild);
	}
	return false;
}

bool CFIleExplorerDlg::FindItem(CTreeCtrl& tree, HTREEITEM hCurrent, const CStringArray& pathParts, int nIndex)
{
	if (nIndex >= pathParts.GetSize())
		return true; // 已匹配所有部分

	CString strTarget = pathParts.GetAt(nIndex);
	if (strTarget.CompareNoCase(L"My Computer")  == 0)
	{
		return true;
	}

	// 遍历当前节点的所有子节点
	HTREEITEM hChild = tree.GetChildItem(hCurrent);
	while (hChild)
	{
		CString strText = tree.GetItemText(hChild);

		// 比较节点文本或完整路径
		LPTSTR pszPath = (LPTSTR)tree.GetItemData(hChild);
		CString strNodePath(pszPath ? pszPath : _T(""));

		if (strText.CompareNoCase(strTarget+"\\") == 0 ||
			(!strNodePath.IsEmpty() && strNodePath.CompareNoCase(strTarget) == 0))
		{
			// 展开当前节点
			tree.Expand(hChild, TVE_EXPAND);
			return FindAndExpandNode(tree, hChild, pathParts, nIndex + 1);
		}
		hChild = tree.GetNextSiblingItem(hCurrent);
	}
	
	return false;
}


void CFIleExplorerDlg::CleanupTreeData(CTreeCtrl& tree, HTREEITEM hItem)
{
	if (!hItem)
		hItem = tree.GetRootItem();

	while (hItem)
	{
		LPTSTR pszPath = (LPTSTR)tree.GetItemData(hItem);
		if (pszPath)
		{
			delete[] pszPath;
			tree.SetItemData(hItem, NULL);
		}

		// 递归清理子节点
		HTREEITEM hChild = tree.GetChildItem(hItem);
		if (hChild)
			CleanupTreeData(tree, hChild);

		hItem = tree.GetNextSiblingItem(hItem);
	}
}

// 添加树节点并保存路径信息 _In_z_ 
HTREEITEM CFIleExplorerDlg::AddTreeItem(CTreeCtrl& tree,  LPCTSTR lpszName, LPCTSTR lpszFullPath,
	_In_ int nImage, _In_ int nSelectedImage, _In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter)
{
	TVINSERTSTRUCT tvis = { 0 };
	tvis.hParent = hParent;
	tvis.hInsertAfter = hInsertAfter;
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.pszText = (LPTSTR)lpszName;
	tvis.item.iImage = nImage;
	tvis.item.iSelectedImage = nSelectedImage;

	// 分配内存保存完整路径
	LPTSTR pszPath = new TCHAR[_tcslen(lpszFullPath) + 1];
	_tcscpy_s(pszPath, _tcslen(lpszFullPath) + 1, lpszFullPath);

	tvis.item.lParam = (LPARAM)pszPath;

	return tree.InsertItem(&tvis);
}

void CFIleExplorerDlg::LoadAndDisplayDrives()
{
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_DRIVES); // 假设有一个列表控件

	// 获取驱动器字符串所需的缓冲区大小
	DWORD dwSize = GetLogicalDriveStrings(0, NULL);

	if (dwSize == 0)
	{
		AfxMessageBox(_T("无法获取驱动器列表"));
		return;
	}

	// 分配缓冲区
	TCHAR* pDriveStrings = new TCHAR[dwSize + 1];
	GetLogicalDriveStrings(dwSize, pDriveStrings);

	// 清空列表控件
	pListCtrl->DeleteAllItems();

	// 添加列标题（如果是列表控件）
	pListCtrl->InsertColumn(0, _T("驱动器"), LVCFMT_LEFT, 50);
	pListCtrl->InsertColumn(1, _T("类型"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(2, _T("卷标"), LVCFMT_LEFT, 80); 
	pListCtrl->InsertColumn(3, _T("总空间"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(4, _T("可用空间"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(5, _T("文件系统"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(6, _T("序列号"), LVCFMT_LEFT, 80);

	// 遍历驱动器字符串
	TCHAR* pDrive = pDriveStrings;
	int nItem = 0;

	while (*pDrive)
	{
		// 获取驱动器类型
		UINT nType = GetDriveType(pDrive);
		CString strType;

		switch (nType)
		{
		case DRIVE_FIXED:     strType = _T("本地磁盘"); break;
		case DRIVE_REMOVABLE: strType = _T("可移动磁盘"); break;
		case DRIVE_CDROM:     strType = _T("光盘驱动器"); break;
		case DRIVE_REMOTE:    strType = _T("网络驱动器"); break;
		case DRIVE_RAMDISK:   strType = _T("RAM磁盘"); break;
		default:              strType = _T("未知类型"); break;
		}

		// 获取卷标
		TCHAR szVolumeName[MAX_PATH] = { 0 };
		GetVolumeInformation(pDrive, szVolumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0);

		// 添加到列表控件
		pListCtrl->InsertItem(nItem, pDrive);
		pListCtrl->SetItemText(nItem, 1, strType);
		pListCtrl->SetItemText(nItem, 2, szVolumeName);
		GetDriveDetails(pDrive, pListCtrl, nItem);

		// 移动到下一个驱动器
		pDrive += _tcslen(pDrive) + 1;
		nItem++;
	}

	delete[] pDriveStrings;
}

void CFIleExplorerDlg::GetDriveDetails(const CString& strDrive, CListCtrl* pListCtrl, int nItem)
{
	// 获取磁盘空间信息
	ULARGE_INTEGER ulFreeBytes, ulTotalBytes, ulTotalFreeBytes;
	if (GetDiskFreeSpaceEx(strDrive, &ulFreeBytes, &ulTotalBytes, &ulTotalFreeBytes))
	{
		CString strInfo;
		strInfo.Format(_T("%.2f GB"),
			ulTotalBytes.QuadPart / (1024.0 * 1024 * 1024)
			);
		pListCtrl->SetItemText(nItem, 3, strInfo);
		strInfo.Format(_T("%.2f GB"),
			ulFreeBytes.QuadPart / (1024.0 * 1024 * 1024)
			);
		pListCtrl->SetItemText(nItem, 4, strInfo);

		if (ulFreeBytes.QuadPart / (1024.0 * 1024 * 1024) < 10)
		{
			// 根据条件设置不同的颜色标识
			COLORREF clrText, clrBkgnd;
			clrText = RGB(255, 255, 255);    // 红色
			clrBkgnd = RGB(255, 0, 0); // 浅黄色
			// 将颜色信息存储在项数据中
			//DWORD_PTR dwData = (clrBkgnd << 16) | clrText;
			pListCtrl->SetItemData(nItem, 1);

			// 强制重绘该项
			CRect rect;
			pListCtrl->GetItemRect(nItem, &rect, LVIR_BOUNDS);
			pListCtrl->InvalidateRect(&rect);
		}	
		else
		{
			pListCtrl->SetItemData(nItem, 0);
		}
	}

	// 获取文件系统信息
	TCHAR szFileSystem[MAX_PATH];
	DWORD dwSerialNumber;
	if (GetVolumeInformation(strDrive, NULL, 0, &dwSerialNumber, NULL, NULL, szFileSystem, MAX_PATH))
	{
		CString strInfo;
		strInfo.Format(_T("%s"), szFileSystem);
		pListCtrl->SetItemText(nItem, 5, strInfo);
			strInfo.Format(_T("%08X"), dwSerialNumber);
		pListCtrl->SetItemText(nItem, 6, strInfo);
	}
}

void CFIleExplorerDlg::OnNMCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
	{
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		// 根据条件设置不同行的颜色
		/*
		if (nItem % 2 == 0)  // 示例：偶数行
		{
			pLVCD->clrText = RGB(255, 0, 0);    // 红色文本
			pLVCD->clrTextBk = RGB(240, 240, 240); // 浅灰色背景
		}
		else  // 奇数行
		{
			pLVCD->clrText = RGB(0, 0, 255);    // 蓝色文本
			pLVCD->clrTextBk = RGB(255, 255, 255); // 白色背景
		}
		*/
		// 或者根据特定条件设置颜色
		  // 从你的数据源获取文本
		CString strText = m_pListCtrl.GetItemText(nItem, 4);
		DWORD_PTR ulFreeBytes = m_pListCtrl.GetItemData(nItem);
		if (ulFreeBytes)
		{
			pLVCD->clrText = RGB(255, 255, 255);  // 白色文本
			pLVCD->clrTextBk = RGB(255, 0, 0);    // 红色背景
		}

		*pResult = CDRF_NEWFONT;
	}
	break;
	}

}

void CFIleExplorerDlg::InitializeSpecialFoldersTree()
{
	CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE_FAST); // 树形控件ID

	/*
	// 初始化图像列表
	CImageList imgList;
	imgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 1);
	if (imgList.GetSafeHandle() != NULL)
	{
		imgList.Detach();
	}
	// 添加系统图标
	SHFILEINFO sfi = { 0 };
	HIMAGELIST hSysImgList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), 0, &sfi,
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	imgList.Attach(hSysImgList);
	pTree->SetImageList(&imgList, TVSIL_NORMAL);
	imgList.Detach();
	*/
	pTree->SetImageList(&this->m_systemImageList, TVSIL_NORMAL);
	// 添加根项 - 快速访问
	HTREEITEM hRoot = AddShellItemToTree(pTree, TVI_ROOT, FOLDERID_QuickLaunch, _T("快速访问"));

	// 添加其他特殊文件夹
	AddShellItemToTree(pTree, hRoot, FOLDERID_Desktop, _T("桌面"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Downloads, _T("下载"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Documents, _T("文档"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Pictures, _T("图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_NetworkFolder, _T("网络资源管理器文件夹"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SearchHistory, _T("历史记录"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ComputerFolder, _T("Computer"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_InternetFolder, _T("Internet"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ControlPanelFolder, _T("控制面板"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PrintersFolder, _T("打印机"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SyncManagerFolder, _T("同步中心"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SyncSetupFolder, _T("同步设置"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ConflictFolder, _T("冲突"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SyncResultsFolder, _T("同步结果"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RecycleBinFolder, _T("回收站"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ConnectionsFolder, _T("网络连接"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Fonts, _T("字体"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Startup, _T("启动"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Programs, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_StartMenu, _T("开始菜单"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Recent, _T("最近使用的项目"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SendTo, _T("SendTo"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Favorites, _T("收藏夹"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_NetHood, _T("网络快捷方式"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PrintHood, _T("打印机快捷方式"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Templates, _T("模板"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonStartup, _T("启动"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonPrograms, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonStartMenu, _T("“开始”菜单"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicDesktop, _T("公用桌面"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramData, _T("ProgramData"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonTemplates, _T("模版"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicDocuments, _T("公用文檔"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RoamingAppData, _T("漫游"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalAppData, _T("Local"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalAppDataLow, _T("LocalLow"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_InternetCache, _T("Internet 临时文件"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Cookies, _T("Cookie"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_History, _T("历史记录"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_System, _T("System32"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SystemX86, _T("System32"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Windows, _T("Windows"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Profile, m_strUserName);
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesX86, _T("Program FilesX86"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesCommonX86, _T("通用文件X86"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesX64, _T("Program FilesX64"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesCommonX64, _T("通用文件X64"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFiles, _T("Program Files"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesCommon, _T("通用文件"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserProgramFiles, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserProgramFilesCommon, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AdminTools, _T("管理工具"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonAdminTools, _T("管理工具"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Music, _T("音乐"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Videos, _T("视频"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Ringtones, _T("铃声"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicPictures, _T("公用图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicMusic, _T("公用音乐"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicVideos, _T("公用视频"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicRingtones, _T("公用铃声"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ResourceDir, _T("资源"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalizedResourcesDir, _T("LocalizedResourcesDir"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonOEMLinks, _T("OEM 链接"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CDBurning, _T("临时燃烧文件夹"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserProfiles, _T("用户"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Playlists, _T("播放列表"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SamplePlaylists, _T("示例播放列表"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SampleMusic, _T("示例音乐"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SamplePictures, _T("示例图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SampleVideos, _T("示例視頻"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PhotoAlbums, _T("幻灯片放映"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Public, _T("图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ChangeRemovePrograms, _T("程序和功能"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppUpdates, _T("已安装更新"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AddNewPrograms, _T("获取程序"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicDownloads, _T("公用下载"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedSearches, _T("保存的搜索"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_QuickLaunch, _T("快速启动"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Contacts, _T("联系人"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SidebarParts, _T("产品"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SidebarDefaultParts, _T("产品"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicGameTasks, _T("GameExplorer"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_GameTasks, _T("GameExplorer"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedGames, _T("保存的游戏"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Games, _T("游戏"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SEARCH_MAPI, _T("Microsoft Office Outlook"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SEARCH_CSC, _T("脱机文件"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Links, _T("链接"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UsersFiles, _T("UsersFiles"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UsersLibraries, _T("库"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SearchHome, _T("搜索结果"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_OriginalImages, _T("原始图像"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_DocumentsLibrary, _T("文档库"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_MusicLibrary, _T("音樂庫"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PicturesLibrary, _T("圖片庫"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_VideosLibrary, _T("視頻庫"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RecordedTVLibrary, _T("录制的电视"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_HomeGroup, _T("家庭组"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_HomeGroupCurrentUser, m_strUserName);
	AddShellItemToTree(pTree, hRoot, FOLDERID_DeviceMetadataStore, _T("DeviceMetadataStore"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Libraries, _T("库"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicLibraries, _T("公共庫"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserPinned, _T("用户已固定"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ImplicitAppShortcuts, _T("ImplicitAppShortcuts"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AccountPictures, _T("帐户图片AddNewPrograms"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicUserTiles, _T("公共帐户图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppsFolder, _T("应用程序"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_StartMenuAllPrograms, _T("StartMenuAllPrograms"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonStartMenuPlaces, _T("CommonStartMenuPlaces"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ApplicationShortcuts, _T("应用程序快捷方式"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RoamingTiles, _T("RoamingTiles"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RoamedTileImages, _T("RoamedTileImages"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Screenshots, _T("屏幕截图"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CameraRoll, _T("相机胶卷"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDrive, _T("OneDrive"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_OneDrive, _T("OneDrive"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDriveDocuments, _T("OneDrive文档"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDrivePictures, _T("OneDrive照片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDriveMusic, _T("OneDrive音樂"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDriveCameraRoll, _T("OneDrive相機"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SearchTemplates, _T("模板"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CameraRollLibrary, _T("CameraRollLibrary"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedPictures, _T("保存的图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedPicturesLibrary, _T("保存的图片庫"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RetailDemo, _T("零售體驗模型"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Device, _T("驅動器"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_DevelopmentFiles, _T("開發者文件夾"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Objects3D, _T("3D 对象"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalDocuments, _T("本地文檔"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalPictures, _T("本地图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalVideos, _T("本地視頻"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalMusic, _T("本地音樂"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalDownloads, _T("本地下載"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RecordedCalls, _T("录制的呼叫文件夹"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AllAppMods, _T("“所有应用修改”文件夹"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CurrentAppMods, _T("图片"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataDesktop, _T("AppDataDesktop"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataDocuments, _T("AppDataDocuments"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataFavorites, _T("AppDataFavorites"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataProgramData, _T("AppDataProgramData"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalStorage, _T("本地緩存"));


	// 展开根节点
	pTree->Expand(hRoot, TVE_EXPAND);
}

HTREEITEM CFIleExplorerDlg::AddShellItemToTree(CTreeCtrl* pTree, HTREEITEM hParent, REFKNOWNFOLDERID folderId, LPCTSTR lpszDisplayName)
{
	HTREEITEM hItem = NULL;
	LPITEMIDLIST pidl = NULL;

	if (SHGetKnownFolderIDList(folderId, 0, NULL, &pidl) == S_OK)
	{
		// 获取显示信息和图标
		SHFILEINFO sfi = { 0 };
		if (SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(sfi),
			SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX))
		{
			// 添加到树形控件
			TVINSERTSTRUCT tvis = { 0 };
			tvis.hParent = hParent;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvis.item.pszText = lpszDisplayName ? (LPTSTR)lpszDisplayName : sfi.szDisplayName;
			tvis.item.iImage = sfi.iIcon;
			tvis.item.iSelectedImage = sfi.iIcon;
			tvis.item.lParam = (LPARAM)ILClone(pidl); // 存储PIDL

			hItem = pTree->InsertItem(&tvis);
		}
		CoTaskMemFree(pidl);
	}

	return hItem;
}

CString CFIleExplorerDlg::GetCurrentUserNameWMI()
{
	HRESULT hres;

	// 初始化COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return _T("COM初始化失败");
	}

	// 检查是否已经初始化了安全设置
	HRESULT hr = CoInitializeSecurity(
		NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IDENTIFY,
		NULL, EOAC_NONE, NULL);

	if (hr == RPC_E_TOO_LATE)
	{
		// 安全设置已经被初始化，可以继续或使用CoSetProxyBlanket设置特定接口
		TRACE("安全设置已经初始化\n");
	}
	else if (FAILED(hr))
	{
		// 其他错误处理
		CoUninitialize();
		return _T("安全初始化失败");
	}

	// 创建WMI连接
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*)&pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		return _T("创建WMI定位器失败");
	}

	// 连接到WMI
	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc);

	if (FAILED(hres))
	{
		pLoc->Release();
		CoUninitialize();
		return _T("连接WMI失败");
	}

	// 设置安全级别
	hres = CoSetProxyBlanket(
		pSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return _T("设置安全代理失败");
	}

	// 执行查询
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_ComputerSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return _T("查询失败");
	}

	// 获取结果
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	CString strUserName;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (uReturn == 0)
		{
			break;
		}

		VARIANT vtProp;
		hr = pclsObj->Get(L"UserName", 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr))
		{
			strUserName = vtProp.bstrVal;
			VariantClear(&vtProp);
		}
		pclsObj->Release();
	}

	// 清理
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return strUserName.IsEmpty() ? _T("未知用户") : strUserName;
}

void CFIleExplorerDlg::CleanupTreeData(CTreeCtrl* pTree, HTREEITEM hItem)
{
	if (!hItem)
		hItem = pTree->GetRootItem();

	while (hItem)
	{
		LPITEMIDLIST pidl = (LPITEMIDLIST)pTree->GetItemData(hItem);
		if (pidl)
			CoTaskMemFree(pidl);

		HTREEITEM hChild = pTree->GetChildItem(hItem);
		if (hChild)
			CleanupTreeData(pTree, hChild);

		hItem = pTree->GetNextSiblingItem(hItem);
	}
}

void CFIleExplorerDlg::CleanupListData(CListCtrl* pList)
{
	for (int i = 0; i < pList->GetItemCount(); i++)
	{
		LPITEMIDLIST pidl = (LPITEMIDLIST)pList->GetItemData(i);
		if (pidl)
			CoTaskMemFree(pidl);
	}
}

BOOL CFIleExplorerDlg::DestroyWindow()
{
	//CleanupTreeData((CTreeCtrl*)GetDlgItem(IDC_TREE_FAST), NULL);
	//CleanupListData((CListCtrl*)GetDlgItem(IDC_LIST1));
	//CleanupListData((CListCtrl*)GetDlgItem(IDC_LIST_DRIVES));
	m_systemImageList.Detach();
	m_imageList.Detach();

	return CDialogEx::DestroyWindow();
}
void CFIleExplorerDlg::OnSelchangedTreeFast(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2); // 列表控件ID
	m_listCtrl.ShowWindow(SW_HIDE);
	pList->ShowWindow(SW_NORMAL);
	// 清空列表
	pList->DeleteAllItems();

	// 获取选中的PIDL
	LPITEMIDLIST pidl = (LPITEMIDLIST)pNMTreeView->itemNew.lParam;
	if (!pidl) return;

	// 获取IShellFolder接口
	IShellFolder* pDesktopFolder = NULL;
	if (SHGetDesktopFolder(&pDesktopFolder) != S_OK) return;

	IShellFolder* pSelectedFolder = NULL;
	if (pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&pSelectedFolder) == S_OK)
	{
		// 枚举文件夹内容
		IEnumIDList* pEnum = NULL;
		if (pSelectedFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum) == S_OK)
		{
			LPITEMIDLIST pidlItem = NULL;
			ULONG celtFetched = 0;

			// 设置列表控件为报表模式
			pList->SetView(LV_VIEW_DETAILS);
			pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
			// 获取系统图像列表
				SHFILEINFO listsfi = { 0 };
			HIMAGELIST hListSystemImageList = (HIMAGELIST)SHGetFileInfo(
				_T("C:\\"), 0, &listsfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
			pList->SetImageList(CImageList::FromHandle(hListSystemImageList), LVSIL_SMALL);
			CHeaderCtrl* pHeaderCtrl = pList->GetHeaderCtrl();
			if (pHeaderCtrl != NULL)
			{
				int  nColumnCount = pHeaderCtrl->GetItemCount();
				for (int i = 0; i < nColumnCount; i++)
				{
					pList->DeleteColumn(0);
				}
			}
			pList->InsertColumn(0, _T("名称"), LVCFMT_LEFT, 200);
			pList->InsertColumn(1, _T("类型"), LVCFMT_LEFT, 100);
			pList->InsertColumn(2, _T("大小"), LVCFMT_RIGHT, 80);
			pList->InsertColumn(3, _T("修改日期"), LVCFMT_LEFT, 120);

			while (pEnum->Next(1, &pidlItem, &celtFetched) == S_OK && celtFetched == 1)
			{
				// 获取显示名称
				STRRET strret;
				if (pSelectedFolder->GetDisplayNameOf(pidlItem, SHGDN_NORMAL, &strret) == S_OK)
				{
					TCHAR szName[MAX_PATH];
					StrRetToBuf(&strret, pidlItem, szName, MAX_PATH);

					// 获取文件属性信息
					SHFILEINFO sfi = { 0 };
					SHGetFileInfo((LPCTSTR)pidlItem, 0, &sfi, sizeof(sfi),
						SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);

					// 添加到列表控件
					int nItem = pList->InsertItem(pList->GetItemCount(), szName, sfi.iIcon);

					// 设置子项文本
					pList->SetItemText(nItem, 1, sfi.szTypeName);

					// 如果是文件夹，设置大小为"<文件夹>"
					if (sfi.dwAttributes & SFGAO_FOLDER)
					{
						pList->SetItemText(nItem, 2, _T("<文件夹>"));
					}
					else
					{
						// 获取文件大小等信息（需要更复杂的处理）
						pList->SetItemText(nItem, 2, _T(""));
					}

					// 存储PIDL以便后续操作
					pList->SetItemData(nItem, (DWORD_PTR)ILClone(pidlItem));
				}
				CoTaskMemFree(pidlItem);
			}
			pEnum->Release();
		}
		pSelectedFolder->Release();
	}
	pDesktopFolder->Release();

	*pResult = 0;
}

void CFIleExplorerDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	this->OnDblclkTree1(pNMHDR, pResult);
}

void CFIleExplorerDlg::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2);
	CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE_FAST);

	if (pNMItemActivate->iItem != -1)
	{
		// 获取列表项的PIDL
		LPITEMIDLIST pidlItem = (LPITEMIDLIST)pList->GetItemData(pNMItemActivate->iItem);

		// 获取树形控件当前选中项的PIDL（父文件夹）
		HTREEITEM hSelectedTreeItem = pTree->GetSelectedItem();
		LPITEMIDLIST pidlParent = (LPITEMIDLIST)pTree->GetItemData(hSelectedTreeItem);

		if (pidlItem && pidlParent)
		{
			// 组合完整PIDL
			LPITEMIDLIST pidlFull = ILCombine(pidlParent, pidlItem);
			if (pidlFull)
			{
				// 使用Shell执行打开操作
				SHELLEXECUTEINFO sei = { 0 };
				sei.cbSize = sizeof(SHELLEXECUTEINFO);
				sei.fMask = SEE_MASK_IDLIST;
				sei.lpIDList = pidlFull;
				sei.nShow = SW_SHOWNORMAL;

				ShellExecuteEx(&sei);

				CoTaskMemFree(pidlFull);
			}
		}
	}
	*pResult = 0;
}
