
// FIleExplorerDlg.cpp : ʵ���ļ�
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


#define  ID_INDICATOR_PROGRESS  10001  // ������ʾ
#define  ID_INDICATOR_TIME  10003      // ʱ����ʾ
#define  ID_INDICATOR_IP  10004      // ʱ����ʾ

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CFIleExplorerDlg �Ի���




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


// CFIleExplorerDlg ��Ϣ�������

BOOL CFIleExplorerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	SetWindowText(L"�ļ�������");

	 //initToolbar();
	
	 // ����״̬��
    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return FALSE;
    }
	// ����״̬������
    static UINT indicators[] =
    {
        ID_SEPARATOR,           // ״̬����һ��������Ĭ����ʾ��Ϣ��
        ID_INDICATOR_PROGRESS,  // ������ʾ
        ID_INDICATOR_TIME,      // ʱ����ʾ
		ID_INDICATOR_IP,		// �������
    };
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	 // ���÷������
    CRect rect;
    GetClientRect(&rect);
    int nWidth = rect.Width() / 4; // ��״̬����Ϊ 3 ���ȿ����
    m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, nWidth);
    m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, nWidth);
    m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TIME, SBPS_NORMAL, nWidth);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_IP, SBPS_NORMAL, nWidth);
    // ����״̬��λ��
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

	// �����б�ؼ���ʽ
	//m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES|LVS_REPORT);
	 // ��ʼ�� ListCtrl Ϊ����ģʽ
    m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_pListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_listCtrlFast.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	// �����
    m_listCtrl.InsertColumn(0, _T("����"), LVCFMT_LEFT, 200);
    m_listCtrl.InsertColumn(1, _T("����"), LVCFMT_LEFT, 120);
    m_listCtrl.InsertColumn(2, _T("��С"), LVCFMT_RIGHT, 100);
    m_listCtrl.InsertColumn(3, _T("�޸�����"), LVCFMT_LEFT, 150);
	// ��������
	m_treeCtrl.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	m_treeCtrl.ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

	

	// ����ͼ���б�
	m_systemImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 4);
	// ȷ�� CImageList �����ǿյ�
	m_imageList.Create(16, 16, ILC_COLOR32, 4, 4);

	if (m_systemImageList.GetSafeHandle() != NULL)
	{
		m_systemImageList.Detach();
	}

	// ��ȡϵͳͼ���б�
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

	// ��ϵͳͼ���б��ӵ�CImageList
	m_systemImageList.Attach(hSystemImageList);

	// ��ȡϵͳͼ���б�
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
	// ��ͼ���б������CTreeCtrl
	m_treeCtrl.SetImageList(&m_systemImageList, TVSIL_NORMAL);
	m_listCtrl.SetImageList(CImageList::FromHandle(hListSystemImageList), LVSIL_SMALL);
	m_listCtrlFast.SetImageList(CImageList::FromHandle(hListSystemImageList), LVSIL_SMALL);
	// ��Ӹ��ڵ�
	// HTREEITEM m_hRoot = m_treeCtrl.InsertItem(_T("My Computer"), TVI_ROOT);
	HTREEITEM m_hRoot = AddTreeItem(m_treeCtrl, _T("My Computer"), L"", listsfi.iIcon, listsfi.iIcon, TVI_ROOT);

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2); // �б�ؼ�ID
	m_listCtrl.ShowWindow(SW_NORMAL);
	pList->ShowWindow(SW_HIDE);

	m_strUserName = GetCurrentUserNameWMI();

	InitializeSpecialFoldersTree();

	m_strUserName = GetCurrentUserNameWMI();
	LoadAndDisplayDrives();

	// �����̷������οؼ�
	LoadDrivesToTreeCtrl();

	// ��ȡ����
	LoadSettings();

	// ����״̬��
	UpdateStatusBar(m_strUserName, 1);

	// �������� ip��ַ
	UpdateStatusBarNetwork();
	

	// ���ö�ʱ����ÿ�����һ�Σ�
    SetTimer(1, 1000, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFIleExplorerDlg::UpdateStatusBarNetwork()
{
	if (IsInternetAvailable())
	{
		CStringArray ips;
		GetLocalIPAddressesEx(ips);
		ips.InsertAt(0, CString("������ "));
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
		UpdateStatusBar(_T("δ��⵽��������"), 3);
	}
}

 void CFIleExplorerDlg::GetLocalIPAddressesEx(CStringArray& ipArray)
{
	ULONG ulOutBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;

	// ��һ�ε��û�ȡ��������С
	dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &ulOutBufLen);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
		pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(ulOutBufLen);
		if (pAddresses == NULL) {
			return ;
		}
	}

	// �ڶ��ε��û�ȡʵ������
	if ((dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &ulOutBufLen)) == NO_ERROR) {
		PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
		while (pCurrAddresses) {
			PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
			while (pUnicast) {
				if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
					// IPv4��ַ
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


	//TBSTYLE_TRANSPARENT��ʹCToolBar͸����������ʾCReBar�ı�����
	//TBSTYLE_LIST�������ð�ť����ʱ�������ڰ�ť���ұߣ�Ĭ��������������ڰ�ť���²�
	m_wndToolBar.ModifyStyle(0, TBSTYLE_TRANSPARENT | TBSTYLE_LIST);

	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);//����������ͷ��ʽ

	m_wndToolBar.SetButtons(NULL, 5);//����ToolBar ��ť����
	m_reBar.AddBar(&m_wndToolBar);

	//2����Ӱ�ť��

		CRect rect;
	int nIndex = -1;

	//����ı�
	m_wndToolBar.SetButtonInfo(++nIndex, 0, TBSTYLE_BUTTON | BTNS_AUTOSIZE | TBSTYLE_AUTOSIZE | TBBS_DISABLED, -1);
	//�˴���Ϊ�����Ӱ�ť�Ŀ�ȣ����Ը����Լ�����Ҫ�ʵ��ĵ����������ǲ��ɼ��ַ��������͸����
	m_wndToolBar.SetButtonText(nIndex, _T(" "));
	m_wndToolBar.GetItemRect(nIndex, &rect);
	rect.top += 3;//�˴���Ϊ�����ı���ֱ����
	m_static.Create(_T("������"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SIMPLE, rect, &m_wndToolBar);
	m_static.SetFont(m_wndToolBar.GetFont());

	//�����Ͽ�
	m_wndToolBar.SetButtonInfo(++nIndex, 0, TBSTYLE_SEP, 80);
	m_wndToolBar.GetItemRect(nIndex, &rect);
	rect.bottom += 100;
	m_combo.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, rect, &m_wndToolBar, 0);m_combo.SetFont(m_wndToolBar.GetFont());

	//���
	m_wndToolBar.SetButtonInfo(++nIndex, 0,TBSTYLE_BUTTON | TBBS_DISABLED | TBSTYLE_AUTOSIZE, -1);
	m_wndToolBar.SetButtonText(nIndex, _T(" "));
	m_wndToolBar.GetItemRect(nIndex, &rect);
	m_btn.Create(_T("���"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, &m_wndToolBar, 0);
	m_btn.SetFont(m_wndToolBar.GetFont());
	//���������ť�����-2��ʾ����ʾ��ťλͼ��������ʾ�����֣������ͨ������IE8��CToolBar��ť�õ��ģ�΢����ȫû��˵����
	//�������-2����ô���ģ��һ�����һ��������˵����
	m_wndToolBar.SetButtonInfo(++nIndex, 0, TBSTYLE_AUTOSIZE | BTNS_WHOLEDROPDOWN, -2);
	m_wndToolBar.SetButtonText(nIndex, _T("�����б�"));

	//��ʾReBar�͹�����
	CRect rcWnd;
	GetClientRect(&rcWnd);
	m_reBar.SetWindowPos(NULL, 0, 0, rcWnd.Width(), 30, SWP_SHOWWINDOW);//��ʾCReBar
	return TRUE;
}

void CFIleExplorerDlg::RepositionControls()
{
    if (m_wndToolBar.GetSafeHwnd())
    {
        CRect rectClient;
        GetClientRect(rectClient);

        // ���ù�����λ��
        m_wndToolBar.SetWindowPos(NULL, 0, 0, rectClient.Width(), 30, SWP_NOZORDER);

        // ���������ؼ���λ�ã����磬�������ؼ����ƹ������ĸ߶ȣ�
        CRect rectToolBar;
        m_wndToolBar.GetWindowRect(rectToolBar);
        int toolbarHeight = rectToolBar.Height();

        // ������һ���ؼ���Ҫ����λ��
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
	// ��ȡ����ʾ������Ϣ
	HMONITOR hMonitor = MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo = { 0 };
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &monitorInfo);

	// ��ȡ��ʾ���ľ�������
	CRect rectMonitor = monitorInfo.rcWork;

	// ��ȡ���ڵĿ�Ⱥ͸߶�
	CRect rectWindow;
	GetWindowRect(&rectWindow);
	int cxWindow = rectWindow.Width();
	int cyWindow = rectWindow.Height();

	// �������λ��
	int x = rectMonitor.left + (rectMonitor.Width() - cxWindow) / 2;
	int y = rectMonitor.top + (rectMonitor.Height() - cyWindow) / 2;

	// ���ô���λ��
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFIleExplorerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFIleExplorerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFIleExplorerDlg::LoadDrivesToTreeCtrl()
{
	// ��ȡ�����̷�
	std::vector<std::wstring> drives = GetLogicalDrives();

	// ������οؼ�
	m_treeCtrl.DeleteAllItems();

	// ����̷������οؼ�
	for(size_t i = 0; i < drives.size(); ++i)
	{	
		// ��ȡϵͳͼ���б�
		SHFILEINFO sfi;
		SHGetFileInfo(
			drives[i].c_str(), 0, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
			);

		// ��ӽڵ㲢����ͼ��
		HTREEITEM hDrive = AddTreeItem(m_treeCtrl, drives[i].c_str(), drives[i].c_str(), sfi.iIcon, sfi.iIcon, TVI_ROOT);
		// ���һ���յ��ӽڵ㣨���ڶ�̬�������ļ��У�
		//m_treeCtrl.InsertItem(drives[i].c_str(), hDrive);
		
		AddTreeItem(m_treeCtrl, drives[i].c_str(), GetFullPath(hDrive), sfi.iIcon, sfi.iIcon, hDrive, NULL);
	}
	//SelectFirstChildItem();
	
}

void CFIleExplorerDlg::SelectFirstChildItem()
{
    HTREEITEM hRootItem = m_treeCtrl.GetRootItem(); // ��ȡ���ڵ�

    if (hRootItem != NULL)
    {
        HTREEITEM hFirstChild = m_treeCtrl.GetChildItem(hRootItem); // ��ȡ��һ���ӽڵ�
        if (hFirstChild != NULL)
        {
            m_treeCtrl.SelectItem(hFirstChild); // ѡ���һ���ӽڵ�
            m_treeCtrl.EnsureVisible(hFirstChild); // ȷ���ӽڵ�ɼ�
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

	// ��ȡ�߼��������ַ���
	TCHAR szDrives[MAX_PATH];
	DWORD dwResult = GetLogicalDriveStrings(MAX_PATH, szDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		TCHAR* pDrive = szDrives;
		while (*pDrive)
		{
			drives.push_back(pDrive); // ���̷���ӵ� vector
			pDrive += _tcslen(pDrive) + 1;
		}
	}

	return drives;
}


// ��ʾ�ļ��б�
void CFIleExplorerDlg::ShowFileList(const CString& strPath)
{

	// �����б�ؼ�Ϊ����ģʽ
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2); // �б�ؼ�ID
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

	// �����
	m_listCtrl.InsertColumn(0, _T("����"), LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(1, _T("����"), LVCFMT_LEFT, 120);
	m_listCtrl.InsertColumn(2, _T("��С"), LVCFMT_RIGHT, 100);
	m_listCtrl.InsertColumn(3, _T("�޸�����"), LVCFMT_LEFT, 150);

	m_listCtrl.DeleteAllItems();
	m_strCurrentPath = strPath;	
	m_curpath.SetWindowTextW(m_strCurrentPath);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + _T("\\*.*"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// ��������Ŀ¼��. �� ..��
		if (finder.IsDots())
		{
			// ��� "�����ϼ�" �������Ǹ�Ŀ¼��
			CString name = L"";
			if (strPath[3] != _T('\0')) { // �Ǹ�Ŀ¼���� C:\Test��
				SHFILEINFO sfi = { 0 };
				if(_T("..") == finder.GetFileName())
				{
					name = _T("�ϼ�Ŀ¼");
				}
				if(_T(".") == finder.GetFileName())
				{
					name = _T("��ǰĿ¼");
				}
				
				SHGetFileInfo(finder.GetFileName(), 0, &sfi, sizeof(SHFILEINFO), 
					SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);
				int nIndex = m_listCtrl.InsertItem(0, finder.GetFileName(), sfi.iIcon);
				m_listCtrl.SetItemText(nIndex, 1, name);
			}
			continue;
		}

		// ��ȡ�ļ����ļ�������
		CString strName = finder.GetFileName();

		// �ж����ļ������ļ���
		BOOL bIsFolder = finder.IsDirectory();

		// ��ȡͼ������
		int nIconIndex = GetIconIndex(finder.GetFilePath(),strName, bIsFolder);

		int nItem = m_listCtrl.InsertItem(0, strName, nIconIndex);
		m_listCtrl.SetItemText(nItem, 1, bIsFolder ? _T("�ļ���") : _T("�ļ�"));

		// ������ļ�����ʾ��С
		if (!bIsFolder)
		{
			CString strSize;
			strSize.Format(_T("%lld KB"), finder.GetLength() / 1024);
			m_listCtrl.SetItemText(nItem, 2, strSize);
		}
		 // �޸�ʱ��
        CTime timeModified;
		finder.GetLastWriteTime(timeModified);
        m_listCtrl.SetItemText(nItem, 3, timeModified.Format(_T("%Y-%m-%d %H:%M")));
	}
}

void CFIleExplorerDlg::LoadDirectory(LPCTSTR lpszPath)
{
    m_listCtrl.DeleteAllItems(); // ����б�

    // ��� "�����ϼ�" �������Ǹ�Ŀ¼��
    if (lpszPath[3] != _T('\0')) { // �Ǹ�Ŀ¼���� C:\Test��
        SHFILEINFO sfi = { 0 };
        SHGetFileInfo(_T(".."), 0, &sfi, sizeof(SHFILEINFO), 
            SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);
        
        int nIndex = m_listCtrl.InsertItem(0, _T(".."), sfi.iIcon);
        m_listCtrl.SetItemText(nIndex, 1, _T("�ϼ�Ŀ¼"));
    }

    // ����Ŀ¼
    CString strSearchPath = lpszPath;
    strSearchPath += _T("*.*");

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(strSearchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        //AfxMessageBox(_T("Ŀ¼�޷����ʣ�"), _T("����"), MB_ICONERROR);
        return;
    }

    int nItemIndex = 0;
    do {
        if (wcscmp(findData.cFileName, _T(".")) == 0 || 
            wcscmp(findData.cFileName, _T("..")) == 0) 
            continue;

		 // ��ȡ�ļ���Ϣ
        SHFILEINFO sfi = { 0 };
		int nIndex = 0;
		CString cFileName = findData.cFileName;
		 // ��������������
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			 // �����б���
			int nImageIndex = GetIconIndex(lpszPath+cFileName, cFileName, TRUE);
			nIndex = m_listCtrl.InsertItem(nItemIndex, findData.cFileName, nImageIndex);
		}
		else{
			int nImageIndex = GetIconIndex(lpszPath+cFileName, cFileName, FALSE);
			nIndex = m_listCtrl.InsertItem(nItemIndex, findData.cFileName, nImageIndex);
		}              

        // ��������������
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            m_listCtrl.SetItemText(nIndex, 1, _T("�ļ���"));
            m_listCtrl.SetItemText(nIndex, 2, _T(""));
        } else {
            // �ļ�����
            m_listCtrl.SetItemText(nIndex, 1, sfi.szTypeName);

            // �ļ���С
            CString strSize;
            strSize.Format(_T("%.2f KB"), (double)findData.nFileSizeLow / 1024);
            m_listCtrl.SetItemText(nIndex, 2, strSize);
        }

        // �޸�ʱ��
        CTime timeModified(findData.ftLastWriteTime);
        m_listCtrl.SetItemText(nIndex, 3, timeModified.Format(_T("%Y-%m-%d %H:%M")));

        nItemIndex++;
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
}

// ��ȡ�ļ����ļ��е�ͼ������
int CFIleExplorerDlg::GetIconIndex(const CString& strPath, const CString& cFileName, BOOL bIsFolder)
{
	SHFILEINFO sfi = {0};
	DWORD dwFlags = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;

	if (bIsFolder)
	{
		// ������ļ��У�ֱ�ӻ�ȡ�ļ���ͼ��
		SHGetFileInfo(
			strPath, 0, &sfi, sizeof(SHFILEINFO),
			dwFlags | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
			);
	}
	else
	{
		// ������ļ���������չ����ȡͼ��
		SHGetFileInfo(
			strPath, 0, &sfi, sizeof(SHFILEINFO),
			dwFlags | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME
			);
	}

	return sfi.iIcon;
}

// ��ȡ���ڵ������·��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// �����Ӧ��֧
	m_treeCtrl.SetRedraw(FALSE); // �����ػ�
	HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		// �ͷŹ������Զ�������
		DWORD_PTR pData = m_treeCtrl.GetItemData(hChildItem);
		if (pData != NULL)
		{
			delete reinterpret_cast<DWORD_PTR*>(pData); // �滻Ϊʵ����������
		}

		// ɾ����ǰ����
		HTREEITEM hNextItem = m_treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
		m_treeCtrl.DeleteItem(hChildItem);
		hChildItem = hNextItem;
	}

	m_treeCtrl.SetRedraw(TRUE); // �����ػ�
	m_treeCtrl.Invalidate(); // ǿ���ػ�
}


void CFIleExplorerDlg::OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// ����ڵ�����չ��
	if (pNMTreeView->action == TVE_EXPAND)
	{
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;

		// ɾ���յ��ӽڵ�
		deleteChild(hItem);

		// ��ȡ�ڵ�·��
		CString strPath = GetItemPath(hItem);

		// �����ļ��в���ӵ����οؼ�
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
			// �����ӽڵ�
			SHFILEINFO sfiFolder;
			SHGetFileInfo(
				finder.GetFilePath(), 0, &sfiFolder, sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
				);

			HTREEITEM hDrive = AddTreeItem(m_treeCtrl, finder.GetFileName(), GetFullPath(hParent)+"\\"+ finder.GetFileName(), sfiFolder.iIcon, sfiFolder.iIcon, hParent, NULL);
			// ���һ���յ��ӽڵ㣨���ڶ�̬�������ļ��У�
			//m_treeCtrl.InsertItem(_T(""), hDrive);  
			AddTreeItem(m_treeCtrl, finder.GetFileName(), GetFullPath(hParent) + "\\" + finder.GetFileName(), sfiFolder.iIcon, sfiFolder.iIcon, hDrive, NULL);
		}
		else
		{
			CString strName = finder.GetFileName();
			// �����ӽڵ�
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

	// �ӵ�ǰ�ڵ㿪ʼ����·��
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
			// �����ӽڵ�
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

	// TODO: �ڴ˴������Ϣ����������
	if (nType != SIZE_MINIMIZED) // ������ڲ�����С��
	{
		if (m_wndStatusBar.GetSafeHwnd())
		{
			// ����״̬��λ��
			RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

			// �����������
			CRect rect;
			GetClientRect(&rect);
			int nWidth = rect.Width() / 3;
			m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, nWidth);
			m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, nWidth);
			m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TIME, SBPS_NORMAL, rect.Width() / 5);
		}

		if (m_wndToolBar.GetSafeHwnd())
		{
			// ����������λ��
			RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		}

		if(m_recttree && m_listCtrl)
		{
			// ����ؼ�����λ�úʹ�С
			CRect rc;
			m_wndStatusBar.GetWindowRect(&rc);
			int dx = cx - m_recttree.right  - 2;  // ˮƽ�仯��
			int dy = cy - m_recttree.bottom  - rc.Height(); // ��ֱ�仯��
			
			// �����༭���С
			m_treeCtrl.MoveWindow(
				m_recttree.left, m_recttree.top,
				m_recttree.Width(), m_recttree.Height()  + dy
				);

			m_treeCtrlFast.MoveWindow(
				m_recttreeFast.left, m_recttreeFast.top,
				m_recttreeFast.Width(), m_recttreeFast.Height() + dy
			);
			// �����б�ؼ���С
			m_listCtrl.MoveWindow(
				m_rectList.left, m_rectList.top,
				m_rectList.Width() + dx, m_rectList.Height() + dy
				);
			// �����б�ؼ���С
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
        // ����״̬��λ��
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

        int nWidth = (pRect->right - pRect->left) / 3;
        m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, nWidth);
        m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, nWidth);
        m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_TIME, SBPS_NORMAL, nWidth);
    }

	if(m_recttree && m_listCtrl)
		{
			// ����ؼ�����λ�úʹ�С
			int dx = pRect->right - pRect->left - m_recttree.right - 2;  // ˮƽ�仯��
			CRect rc;
			m_wndStatusBar.GetWindowRect(&rc);
			int dy = pRect->bottom - pRect->top - m_recttree.bottom  - rc.Height(); // ��ֱ�仯	

			// �ؼ���С
			m_treeCtrl.MoveWindow(
				m_recttree.left, m_recttree.top,
				m_recttree.Width(), m_recttree.Height()  + dy
				);

			m_treeCtrlFast.MoveWindow(
				m_recttreeFast.left, m_recttreeFast.top,
				m_recttreeFast.Width(), m_recttreeFast.Height() + dy
			);

			// �����б�ؼ���С
			m_listCtrl.MoveWindow(
				m_rectList.left, m_rectList.top,
				m_rectList.Width() + dx, m_rectList.Height() + dy
				);
			// �����б�ؼ���С
			m_listCtrlFast.MoveWindow(
				m_rectListFast.left, m_rectListFast.top,
				m_rectListFast.Width() + dx, m_rectListFast.Height() + dy
			);
		}     	
}


void CFIleExplorerDlg::OnDblclkList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


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
    CString strCurrentPath = m_strCurrentPath; // ��Ա�������浱ǰ·��
	m_curpath.SetWindowTextW(m_strCurrentPath);

	if (strName == _T(".")) {
		// ��ǰĿ¼
	}
    else if (strName == _T("..")) {
        // �����ϼ�Ŀ¼
		 if (strCurrentPath.Right(1) == _T("\\"))
			 strCurrentPath = strCurrentPath.Left(strCurrentPath.GetLength()  - 1);
        int nPos = strCurrentPath.ReverseFind(_T('\\'));
        if (nPos != -1) {
            strCurrentPath = strCurrentPath.Left(nPos);
            if (strCurrentPath.Right(1) != _T("\\")) 
                strCurrentPath += _T("\\");
        }
    } else {
        // ����Ƿ�Ϊ�ļ���
        CString strFullPath = strCurrentPath + strName;
        DWORD dwAttrib = GetFileAttributes(strFullPath);
        if (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
            strCurrentPath = strFullPath + _T("\\");
        } else {
            // �����ļ��򿪲������������ ShellExecute��
            ShellExecute(NULL, _T("open"), strFullPath, NULL, NULL, SW_SHOWNORMAL);
            return;
        }
    }

    // ���¼���Ŀ¼
    ShowFileList(strCurrentPath);

	*pResult = 0;
}


void CFIleExplorerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1)
    {
        // ����ʱ����ʾ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CFIleExplorerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: �ڴ˴������Ϣ����������
	// ȷ���Ҽ�������� ListCtrl
     // ��ʾ�����Ҽ��˵�
	 if (pWnd == &m_listCtrl)
    {
		// ShowDesktopContextMenu(point);
		 // ��ȡ��ǰѡ�����������-1��ʾû��ѡ��
		 

		 // ��ȡ��ǰѡ��������
		 int nSelectedCount = m_listCtrl.GetSelectedCount();
		 CString strPath = m_strCurrentPath;
		 strPath.Replace(_T("\\\\"), _T("\\"));

		 if (nSelectedCount > 1)
		 {
			 CStringArray arr;
			 // ��ȡ����ѡ�����
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
			 // ��ȡ������ı�
			 CString strText = m_listCtrl.GetItemText(nSelectedItem, 0); // 0��ʾ��һ��
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
		AfxMessageBox(_T("�޷���ȡ����˵�"));
		return;
	}

	HMENU hMenu = helper.CreateMenu();
	if (!hMenu)
	{
		AfxMessageBox(_T("�޷������˵�"));
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
	// 1. ��ʼ�� COM
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
		return;

	// 2. ��ȡ�����ļ��нӿ�
	IShellFolder* pDesktopFolder = NULL;
	hr = SHGetDesktopFolder(&pDesktopFolder);
	if (FAILED(hr))
	{
		CoUninitialize();
		return;
	}

	// 3. ���ļ�·��ת��Ϊ���ַ�
	LPITEMIDLIST pidl = NULL;
	//STRRET strret;
	WCHAR wszPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, CW2A(strFilePath), -1, wszPath, MAX_PATH);
	ULONG chEaten = 0;
	ULONG dwAttributes = 0;
	// 4. ����·����ȡ PIDL
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

	// 5. ��ȡ�����Ĳ˵��ӿ�
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

	// 6. ���������˵�
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		// ��ȡĬ�ϲ˵�
		hr = pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL);
		if (SUCCEEDED(hr))
		{
			// ��ȡ���λ��
			//POINT pt;
			//GetCursorPos(&pt);

			// ��ʾ�˵�
			int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, hWnd, NULL);

			// ����˵�ѡ��
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

	// 7. �ͷ���Դ
	pContextMenu->Release();
	CoTaskMemFree(pidl);
	pDesktopFolder->Release();
	CoUninitialize();
}

void CFIleExplorerDlg::ShowShellContextMenuForMultipleFiles(HWND hWnd, const CStringArray& arrFiles, CPoint point)
{
	// ��ʼ�� COM
	CoInitialize(NULL);

	// ��ȡ�����ļ���
	IShellFolder* pDesktopFolder = NULL;
	SHGetDesktopFolder(&pDesktopFolder);

	// ���� PIDL ����
	LPITEMIDLIST* pidlArray = new LPITEMIDLIST[arrFiles.GetCount()];
	ZeroMemory(pidlArray, sizeof(LPITEMIDLIST) * arrFiles.GetCount());

	// ת��ÿ���ļ�·��Ϊ PIDL
	for (int i = 0; i < arrFiles.GetCount(); i++)
	{
		WCHAR wszPath[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, CW2A(arrFiles[i]), -1, wszPath, MAX_PATH);
		pDesktopFolder->ParseDisplayName(NULL, NULL, wszPath, NULL, &pidlArray[i], NULL);
	}

	// ��ȡ�����Ĳ˵�
	IContextMenu* pContextMenu = NULL;
	pDesktopFolder->GetUIObjectOf(hWnd, arrFiles.GetCount(), (LPCITEMIDLIST*)pidlArray,
		IID_IContextMenu, NULL, (void**)&pContextMenu);

	// ��������ʾ�˵�
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

	// ����
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
    // ��ʼ�� COM ��
    CoInitialize(NULL);

    // ��ȡ�����ļ��ж���
    IShellFolder* pDesktopFolder = NULL;
    if (SHGetDesktopFolder(&pDesktopFolder) == S_OK)
    {
        // ��ȡ����� PIDL
        LPITEMIDLIST pidl = NULL;
        if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl) == S_OK)
        {
            // ��ȡ����� IContextMenu �ӿ�
            IContextMenu* pContextMenu = NULL;
            if (pDesktopFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidl, IID_IContextMenu, NULL, (void**)&pContextMenu) == S_OK)
            {
                // ���������˵�
                HMENU hMenu = CreatePopupMenu();
                if (hMenu)
                {
					UINT flags = CMF_NORMAL | CMF_EXPLORE | CMF_EXTENDEDVERBS; // ���ñ�־
                    // ���˵�
                    pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, flags);

                    // ��ʾ�˵�
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, GetSafeHwnd(), NULL);

                    // ����˵�������
                    if (cmd > 0)
                    {
                        CMINVOKECOMMANDINFO info = { 0 };
                        info.cbSize = sizeof(info);
                        info.hwnd = GetSafeHwnd();
                        info.lpVerb = (LPCSTR)MAKEINTRESOURCE(cmd - 1);
                        info.nShow = SW_SHOWNORMAL;
                        pContextMenu->InvokeCommand(&info);
                    }

                    // ���ٲ˵�
                    DestroyMenu(hMenu);
                }

                // �ͷ� IContextMenu �ӿ�
                pContextMenu->Release();
            }

            // �ͷ� PIDL
            CoTaskMemFree(pidl);
        }

        // �ͷ� IShellFolder �ӿ�
        pDesktopFolder->Release();
    }

    // �ͷ� COM ��
    CoUninitialize();
}
void CFIleExplorerDlg::OnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
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
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// ����س���
		CWnd* pWnd = GetFocus();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
		{
			// ��������ڱ༭���ϣ�ִ���ض�����
			OnEditEnterPressed();
			return TRUE; // �Ѵ���
		}

		// ����TRUE��ʾ�Ѵ�������رնԻ���
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
	// ��ȡ��ǰ�������ڵĿؼ�
	CWnd* pWnd = GetFocus();

	// ����Ƿ��Ǳ༭��
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
	{
		CEdit* pEdit = (CEdit*)pWnd;

		// ��ȡ�༭��ID
		UINT nID = pEdit->GetDlgCtrlID();

		// ���ݲ�ͬ�ı༭��ִ�в�ͬ����
		switch (nID)
		{
		case IDC_EDIT1:
			// �����һ���༭��Ļس�
			OnEdit1EnterPressed();
			break;

		default:
			// Ĭ�ϴ���
			NextDlgCtrl();  // �ƶ�����һ���ؼ�
			break;
		}
	}
}

void CFIleExplorerDlg::OnEdit1EnterPressed()
{
	// ʾ��1����֤���ƶ�����һ���ؼ�
	if (ValidateEdit1Content())
	{
		CString str;
		m_curpath.GetWindowTextW(str);
		ExpandToPath1(m_treeCtrl, str);
		ShowFileList(str);
	}
	else
	{
		// ��֤ʧ�ܣ����ֽ���
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
	// Windows ·��������ʽ
	std::wregex winPathRegex(
		L"^([a-zA-Z]:\\\\|\\\\\\\\)"  // ������������·��
		L"([^\\\\/:*?\"<>|]+\\\\?)*"  // �ļ��в㼶
		L"([^\\\\/:*?\"<>|]+)?$"      // �ļ���
	);

	// Unix ���·��������ʽ
	std::wregex unixPathRegex(
		L"^(/[^/]+)+/?$"
	);

	std::wstring path(lpszPath);

	// ����Ƿ��������һ�ָ�ʽ
	return std::regex_match(path, winPathRegex) ||
		std::regex_match(path, unixPathRegex);
}


bool CFIleExplorerDlg::IsValidFilePath(LPCTSTR lpszFilePath)
{
	// ���·���Ƿ�Ϊ��
	if (lpszFilePath == NULL || _tcslen(lpszFilePath) == 0)
		return false;

	// �滻��б��Ϊ��б��
	//lpszFilePath.Replace(_T('/'), _T('\\'));


	// ���·�����Ƿ�����Ƿ��ַ�
	LPCTSTR illegalChars = _T("*?\"<>|");
	for (int i = 0; i < _tcslen(illegalChars); i++) {
		if (_tcschr(lpszFilePath, illegalChars[i]) != NULL)
			return false;
	}

	if (!IsPathFormatValid(lpszFilePath))
	{
		return false;
	}
	// ������·��
	if (PathIsRelative(lpszFilePath))
	{
		// ���·�������⴦��
		TCHAR szFullPath[MAX_PATH];
		if (!PathCanonicalize(szFullPath, lpszFilePath))
			return FALSE;
		lpszFilePath = szFullPath;
	}

	// 5. ��ѡ������ļ�/Ŀ¼�Ƿ����
	DWORD dwAttrib = GetFileAttributes(lpszFilePath);
	if (dwAttrib == INVALID_FILE_ATTRIBUTES)
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND ||
			dwError == ERROR_PATH_NOT_FOUND ||
			dwError == ERROR_INVALID_NAME)
		{
			AfxMessageBox(L"����ļ�/Ŀ¼������");
			return FALSE;
		}			
	}

	// ����Ƿ���Ŀ¼
	if (!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		// ���Դ��ļ���֤�ɷ�����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_curpath.GetWindowTextW(str);
	// �����ļ��в���ӵ����οؼ�
	ExpandToPath1(m_treeCtrl, str);
	ShowFileList(str);
}


// ����·�����Ҳ�չ���ڵ�
bool CFIleExplorerDlg::ExpandToPath1(CTreeCtrl& tree, LPCTSTR lpszPath)
{
	// �Ӹ��ڵ㿪ʼ����
	HTREEITEM hRoot = tree.GetRootItem();
	if (!hRoot)
		return false;

	// �ָ�·��Ϊ��������
	CString strPath(lpszPath);
	CStringArray pathParts;
	SplitPath(strPath, pathParts); // ʵ�ּ�����

	// �ݹ����
	return FindItem(tree, hRoot, pathParts, 0);
}

// �ָ�·������
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

// �ݹ���Ҳ�չ���ڵ�
bool CFIleExplorerDlg::FindAndExpandNode(CTreeCtrl& tree, HTREEITEM hCurrent, const CStringArray& pathParts, int nIndex)
{
	if (nIndex >= pathParts.GetSize())
		return true; // ��ƥ�����в���

	CString strTarget = pathParts.GetAt(nIndex);

	// ������ǰ�ڵ�������ӽڵ�
	HTREEITEM hChild = tree.GetChildItem(hCurrent);
	while (hChild)
	{
		CString strText = tree.GetItemText(hChild);

		// �ȽϽڵ��ı�������·��
		LPTSTR pszPath = (LPTSTR)tree.GetItemData(hChild);
		CString strNodePath(pszPath ? pszPath : _T(""));

		if (strText.CompareNoCase(strTarget) == 0 ||
			(!strNodePath.IsEmpty() && strNodePath.CompareNoCase(strTarget) == 0))
		{
			// չ����ǰ�ڵ�
			tree.Expand(hChild, TVE_EXPAND);

			// �ݹ������һ��
			if (FindAndExpandNode(tree, hChild, pathParts, nIndex + 1))
			{
				// ȷ��Ŀ��ڵ�ɼ�
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
		return true; // ��ƥ�����в���

	CString strTarget = pathParts.GetAt(nIndex);
	if (strTarget.CompareNoCase(L"My Computer")  == 0)
	{
		return true;
	}

	// ������ǰ�ڵ�������ӽڵ�
	HTREEITEM hChild = tree.GetChildItem(hCurrent);
	while (hChild)
	{
		CString strText = tree.GetItemText(hChild);

		// �ȽϽڵ��ı�������·��
		LPTSTR pszPath = (LPTSTR)tree.GetItemData(hChild);
		CString strNodePath(pszPath ? pszPath : _T(""));

		if (strText.CompareNoCase(strTarget+"\\") == 0 ||
			(!strNodePath.IsEmpty() && strNodePath.CompareNoCase(strTarget) == 0))
		{
			// չ����ǰ�ڵ�
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

		// �ݹ������ӽڵ�
		HTREEITEM hChild = tree.GetChildItem(hItem);
		if (hChild)
			CleanupTreeData(tree, hChild);

		hItem = tree.GetNextSiblingItem(hItem);
	}
}

// ������ڵ㲢����·����Ϣ _In_z_ 
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

	// �����ڴ汣������·��
	LPTSTR pszPath = new TCHAR[_tcslen(lpszFullPath) + 1];
	_tcscpy_s(pszPath, _tcslen(lpszFullPath) + 1, lpszFullPath);

	tvis.item.lParam = (LPARAM)pszPath;

	return tree.InsertItem(&tvis);
}

void CFIleExplorerDlg::LoadAndDisplayDrives()
{
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_DRIVES); // ������һ���б�ؼ�

	// ��ȡ�������ַ�������Ļ�������С
	DWORD dwSize = GetLogicalDriveStrings(0, NULL);

	if (dwSize == 0)
	{
		AfxMessageBox(_T("�޷���ȡ�������б�"));
		return;
	}

	// ���仺����
	TCHAR* pDriveStrings = new TCHAR[dwSize + 1];
	GetLogicalDriveStrings(dwSize, pDriveStrings);

	// ����б�ؼ�
	pListCtrl->DeleteAllItems();

	// ����б��⣨������б�ؼ���
	pListCtrl->InsertColumn(0, _T("������"), LVCFMT_LEFT, 50);
	pListCtrl->InsertColumn(1, _T("����"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(2, _T("���"), LVCFMT_LEFT, 80); 
	pListCtrl->InsertColumn(3, _T("�ܿռ�"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(4, _T("���ÿռ�"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(5, _T("�ļ�ϵͳ"), LVCFMT_LEFT, 80);
	pListCtrl->InsertColumn(6, _T("���к�"), LVCFMT_LEFT, 80);

	// �����������ַ���
	TCHAR* pDrive = pDriveStrings;
	int nItem = 0;

	while (*pDrive)
	{
		// ��ȡ����������
		UINT nType = GetDriveType(pDrive);
		CString strType;

		switch (nType)
		{
		case DRIVE_FIXED:     strType = _T("���ش���"); break;
		case DRIVE_REMOVABLE: strType = _T("���ƶ�����"); break;
		case DRIVE_CDROM:     strType = _T("����������"); break;
		case DRIVE_REMOTE:    strType = _T("����������"); break;
		case DRIVE_RAMDISK:   strType = _T("RAM����"); break;
		default:              strType = _T("δ֪����"); break;
		}

		// ��ȡ���
		TCHAR szVolumeName[MAX_PATH] = { 0 };
		GetVolumeInformation(pDrive, szVolumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0);

		// ��ӵ��б�ؼ�
		pListCtrl->InsertItem(nItem, pDrive);
		pListCtrl->SetItemText(nItem, 1, strType);
		pListCtrl->SetItemText(nItem, 2, szVolumeName);
		GetDriveDetails(pDrive, pListCtrl, nItem);

		// �ƶ�����һ��������
		pDrive += _tcslen(pDrive) + 1;
		nItem++;
	}

	delete[] pDriveStrings;
}

void CFIleExplorerDlg::GetDriveDetails(const CString& strDrive, CListCtrl* pListCtrl, int nItem)
{
	// ��ȡ���̿ռ���Ϣ
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
			// �����������ò�ͬ����ɫ��ʶ
			COLORREF clrText, clrBkgnd;
			clrText = RGB(255, 255, 255);    // ��ɫ
			clrBkgnd = RGB(255, 0, 0); // ǳ��ɫ
			// ����ɫ��Ϣ�洢����������
			//DWORD_PTR dwData = (clrBkgnd << 16) | clrText;
			pListCtrl->SetItemData(nItem, 1);

			// ǿ���ػ����
			CRect rect;
			pListCtrl->GetItemRect(nItem, &rect, LVIR_BOUNDS);
			pListCtrl->InvalidateRect(&rect);
		}	
		else
		{
			pListCtrl->SetItemData(nItem, 0);
		}
	}

	// ��ȡ�ļ�ϵͳ��Ϣ
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

		// �����������ò�ͬ�е���ɫ
		/*
		if (nItem % 2 == 0)  // ʾ����ż����
		{
			pLVCD->clrText = RGB(255, 0, 0);    // ��ɫ�ı�
			pLVCD->clrTextBk = RGB(240, 240, 240); // ǳ��ɫ����
		}
		else  // ������
		{
			pLVCD->clrText = RGB(0, 0, 255);    // ��ɫ�ı�
			pLVCD->clrTextBk = RGB(255, 255, 255); // ��ɫ����
		}
		*/
		// ���߸����ض�����������ɫ
		  // ���������Դ��ȡ�ı�
		CString strText = m_pListCtrl.GetItemText(nItem, 4);
		DWORD_PTR ulFreeBytes = m_pListCtrl.GetItemData(nItem);
		if (ulFreeBytes)
		{
			pLVCD->clrText = RGB(255, 255, 255);  // ��ɫ�ı�
			pLVCD->clrTextBk = RGB(255, 0, 0);    // ��ɫ����
		}

		*pResult = CDRF_NEWFONT;
	}
	break;
	}

}

void CFIleExplorerDlg::InitializeSpecialFoldersTree()
{
	CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE_FAST); // ���οؼ�ID

	/*
	// ��ʼ��ͼ���б�
	CImageList imgList;
	imgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 1);
	if (imgList.GetSafeHandle() != NULL)
	{
		imgList.Detach();
	}
	// ���ϵͳͼ��
	SHFILEINFO sfi = { 0 };
	HIMAGELIST hSysImgList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), 0, &sfi,
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	imgList.Attach(hSysImgList);
	pTree->SetImageList(&imgList, TVSIL_NORMAL);
	imgList.Detach();
	*/
	pTree->SetImageList(&this->m_systemImageList, TVSIL_NORMAL);
	// ��Ӹ��� - ���ٷ���
	HTREEITEM hRoot = AddShellItemToTree(pTree, TVI_ROOT, FOLDERID_QuickLaunch, _T("���ٷ���"));

	// ������������ļ���
	AddShellItemToTree(pTree, hRoot, FOLDERID_Desktop, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Downloads, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Documents, _T("�ĵ�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Pictures, _T("ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_NetworkFolder, _T("������Դ�������ļ���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SearchHistory, _T("��ʷ��¼"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ComputerFolder, _T("Computer"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_InternetFolder, _T("Internet"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ControlPanelFolder, _T("�������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PrintersFolder, _T("��ӡ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SyncManagerFolder, _T("ͬ������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SyncSetupFolder, _T("ͬ������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ConflictFolder, _T("��ͻ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SyncResultsFolder, _T("ͬ�����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RecycleBinFolder, _T("����վ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ConnectionsFolder, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Fonts, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Startup, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Programs, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_StartMenu, _T("��ʼ�˵�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Recent, _T("���ʹ�õ���Ŀ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SendTo, _T("SendTo"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Favorites, _T("�ղؼ�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_NetHood, _T("�����ݷ�ʽ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PrintHood, _T("��ӡ����ݷ�ʽ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Templates, _T("ģ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonStartup, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonPrograms, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonStartMenu, _T("����ʼ���˵�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicDesktop, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramData, _T("ProgramData"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonTemplates, _T("ģ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicDocuments, _T("�����ęn"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RoamingAppData, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalAppData, _T("Local"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalAppDataLow, _T("LocalLow"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_InternetCache, _T("Internet ��ʱ�ļ�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Cookies, _T("Cookie"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_History, _T("��ʷ��¼"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_System, _T("System32"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SystemX86, _T("System32"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Windows, _T("Windows"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Profile, m_strUserName);
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesX86, _T("Program FilesX86"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesCommonX86, _T("ͨ���ļ�X86"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesX64, _T("Program FilesX64"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesCommonX64, _T("ͨ���ļ�X64"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFiles, _T("Program Files"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ProgramFilesCommon, _T("ͨ���ļ�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserProgramFiles, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserProgramFilesCommon, _T("Programs"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AdminTools, _T("������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonAdminTools, _T("������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Music, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Videos, _T("��Ƶ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Ringtones, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicPictures, _T("����ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicMusic, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicVideos, _T("������Ƶ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicRingtones, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ResourceDir, _T("��Դ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalizedResourcesDir, _T("LocalizedResourcesDir"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonOEMLinks, _T("OEM ����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CDBurning, _T("��ʱȼ���ļ���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserProfiles, _T("�û�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Playlists, _T("�����б�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SamplePlaylists, _T("ʾ�������б�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SampleMusic, _T("ʾ������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SamplePictures, _T("ʾ��ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SampleVideos, _T("ʾ��ҕ�l"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PhotoAlbums, _T("�õ�Ƭ��ӳ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Public, _T("ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ChangeRemovePrograms, _T("����͹���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppUpdates, _T("�Ѱ�װ����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AddNewPrograms, _T("��ȡ����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicDownloads, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedSearches, _T("���������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_QuickLaunch, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Contacts, _T("��ϵ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SidebarParts, _T("��Ʒ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SidebarDefaultParts, _T("��Ʒ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicGameTasks, _T("GameExplorer"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_GameTasks, _T("GameExplorer"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedGames, _T("�������Ϸ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Games, _T("��Ϸ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SEARCH_MAPI, _T("Microsoft Office Outlook"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SEARCH_CSC, _T("�ѻ��ļ�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Links, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UsersFiles, _T("UsersFiles"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UsersLibraries, _T("��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SearchHome, _T("�������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_OriginalImages, _T("ԭʼͼ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_DocumentsLibrary, _T("�ĵ���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_MusicLibrary, _T("������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PicturesLibrary, _T("�DƬ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_VideosLibrary, _T("ҕ�l��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RecordedTVLibrary, _T("¼�Ƶĵ���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_HomeGroup, _T("��ͥ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_HomeGroupCurrentUser, m_strUserName);
	AddShellItemToTree(pTree, hRoot, FOLDERID_DeviceMetadataStore, _T("DeviceMetadataStore"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Libraries, _T("��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicLibraries, _T("������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_UserPinned, _T("�û��ѹ̶�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ImplicitAppShortcuts, _T("ImplicitAppShortcuts"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AccountPictures, _T("�ʻ�ͼƬAddNewPrograms"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_PublicUserTiles, _T("�����ʻ�ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppsFolder, _T("Ӧ�ó���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_StartMenuAllPrograms, _T("StartMenuAllPrograms"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CommonStartMenuPlaces, _T("CommonStartMenuPlaces"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_ApplicationShortcuts, _T("Ӧ�ó����ݷ�ʽ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RoamingTiles, _T("RoamingTiles"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RoamedTileImages, _T("RoamedTileImages"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Screenshots, _T("��Ļ��ͼ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CameraRoll, _T("�������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDrive, _T("OneDrive"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_OneDrive, _T("OneDrive"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDriveDocuments, _T("OneDrive�ĵ�"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDrivePictures, _T("OneDrive��Ƭ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDriveMusic, _T("OneDrive����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SkyDriveCameraRoll, _T("OneDrive���C"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SearchTemplates, _T("ģ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CameraRollLibrary, _T("CameraRollLibrary"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedPictures, _T("�����ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_SavedPicturesLibrary, _T("�����ͼƬ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RetailDemo, _T("�����w�ģ��"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Device, _T("����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_DevelopmentFiles, _T("�_�l���ļ��A"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_Objects3D, _T("3D ����"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalDocuments, _T("�����ęn"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalPictures, _T("����ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalVideos, _T("����ҕ�l"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalMusic, _T("��������"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalDownloads, _T("�������d"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_RecordedCalls, _T("¼�Ƶĺ����ļ���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AllAppMods, _T("������Ӧ���޸ġ��ļ���"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_CurrentAppMods, _T("ͼƬ"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataDesktop, _T("AppDataDesktop"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataDocuments, _T("AppDataDocuments"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataFavorites, _T("AppDataFavorites"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_AppDataProgramData, _T("AppDataProgramData"));
	AddShellItemToTree(pTree, hRoot, FOLDERID_LocalStorage, _T("���ؾ���"));


	// չ�����ڵ�
	pTree->Expand(hRoot, TVE_EXPAND);
}

HTREEITEM CFIleExplorerDlg::AddShellItemToTree(CTreeCtrl* pTree, HTREEITEM hParent, REFKNOWNFOLDERID folderId, LPCTSTR lpszDisplayName)
{
	HTREEITEM hItem = NULL;
	LPITEMIDLIST pidl = NULL;

	if (SHGetKnownFolderIDList(folderId, 0, NULL, &pidl) == S_OK)
	{
		// ��ȡ��ʾ��Ϣ��ͼ��
		SHFILEINFO sfi = { 0 };
		if (SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(sfi),
			SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX))
		{
			// ��ӵ����οؼ�
			TVINSERTSTRUCT tvis = { 0 };
			tvis.hParent = hParent;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvis.item.pszText = lpszDisplayName ? (LPTSTR)lpszDisplayName : sfi.szDisplayName;
			tvis.item.iImage = sfi.iIcon;
			tvis.item.iSelectedImage = sfi.iIcon;
			tvis.item.lParam = (LPARAM)ILClone(pidl); // �洢PIDL

			hItem = pTree->InsertItem(&tvis);
		}
		CoTaskMemFree(pidl);
	}

	return hItem;
}

CString CFIleExplorerDlg::GetCurrentUserNameWMI()
{
	HRESULT hres;

	// ��ʼ��COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return _T("COM��ʼ��ʧ��");
	}

	// ����Ƿ��Ѿ���ʼ���˰�ȫ����
	HRESULT hr = CoInitializeSecurity(
		NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IDENTIFY,
		NULL, EOAC_NONE, NULL);

	if (hr == RPC_E_TOO_LATE)
	{
		// ��ȫ�����Ѿ�����ʼ�������Լ�����ʹ��CoSetProxyBlanket�����ض��ӿ�
		TRACE("��ȫ�����Ѿ���ʼ��\n");
	}
	else if (FAILED(hr))
	{
		// ����������
		CoUninitialize();
		return _T("��ȫ��ʼ��ʧ��");
	}

	// ����WMI����
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
		return _T("����WMI��λ��ʧ��");
	}

	// ���ӵ�WMI
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
		return _T("����WMIʧ��");
	}

	// ���ð�ȫ����
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
		return _T("���ð�ȫ����ʧ��");
	}

	// ִ�в�ѯ
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
		return _T("��ѯʧ��");
	}

	// ��ȡ���
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

	// ����
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return strUserName.IsEmpty() ? _T("δ֪�û�") : strUserName;
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
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST2); // �б�ؼ�ID
	m_listCtrl.ShowWindow(SW_HIDE);
	pList->ShowWindow(SW_NORMAL);
	// ����б�
	pList->DeleteAllItems();

	// ��ȡѡ�е�PIDL
	LPITEMIDLIST pidl = (LPITEMIDLIST)pNMTreeView->itemNew.lParam;
	if (!pidl) return;

	// ��ȡIShellFolder�ӿ�
	IShellFolder* pDesktopFolder = NULL;
	if (SHGetDesktopFolder(&pDesktopFolder) != S_OK) return;

	IShellFolder* pSelectedFolder = NULL;
	if (pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&pSelectedFolder) == S_OK)
	{
		// ö���ļ�������
		IEnumIDList* pEnum = NULL;
		if (pSelectedFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum) == S_OK)
		{
			LPITEMIDLIST pidlItem = NULL;
			ULONG celtFetched = 0;

			// �����б�ؼ�Ϊ����ģʽ
			pList->SetView(LV_VIEW_DETAILS);
			pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
			// ��ȡϵͳͼ���б�
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
			pList->InsertColumn(0, _T("����"), LVCFMT_LEFT, 200);
			pList->InsertColumn(1, _T("����"), LVCFMT_LEFT, 100);
			pList->InsertColumn(2, _T("��С"), LVCFMT_RIGHT, 80);
			pList->InsertColumn(3, _T("�޸�����"), LVCFMT_LEFT, 120);

			while (pEnum->Next(1, &pidlItem, &celtFetched) == S_OK && celtFetched == 1)
			{
				// ��ȡ��ʾ����
				STRRET strret;
				if (pSelectedFolder->GetDisplayNameOf(pidlItem, SHGDN_NORMAL, &strret) == S_OK)
				{
					TCHAR szName[MAX_PATH];
					StrRetToBuf(&strret, pidlItem, szName, MAX_PATH);

					// ��ȡ�ļ�������Ϣ
					SHFILEINFO sfi = { 0 };
					SHGetFileInfo((LPCTSTR)pidlItem, 0, &sfi, sizeof(sfi),
						SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);

					// ��ӵ��б�ؼ�
					int nItem = pList->InsertItem(pList->GetItemCount(), szName, sfi.iIcon);

					// ���������ı�
					pList->SetItemText(nItem, 1, sfi.szTypeName);

					// ������ļ��У����ô�СΪ"<�ļ���>"
					if (sfi.dwAttributes & SFGAO_FOLDER)
					{
						pList->SetItemText(nItem, 2, _T("<�ļ���>"));
					}
					else
					{
						// ��ȡ�ļ���С����Ϣ����Ҫ�����ӵĴ���
						pList->SetItemText(nItem, 2, _T(""));
					}

					// �洢PIDL�Ա��������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		// ��ȡ�б����PIDL
		LPITEMIDLIST pidlItem = (LPITEMIDLIST)pList->GetItemData(pNMItemActivate->iItem);

		// ��ȡ���οؼ���ǰѡ�����PIDL�����ļ��У�
		HTREEITEM hSelectedTreeItem = pTree->GetSelectedItem();
		LPITEMIDLIST pidlParent = (LPITEMIDLIST)pTree->GetItemData(hSelectedTreeItem);

		if (pidlItem && pidlParent)
		{
			// �������PIDL
			LPITEMIDLIST pidlFull = ILCombine(pidlParent, pidlItem);
			if (pidlFull)
			{
				// ʹ��Shellִ�д򿪲���
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
