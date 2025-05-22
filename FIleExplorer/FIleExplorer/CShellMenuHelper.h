#pragma once
class CShellMenuHelper
{
public:
    CShellMenuHelper()
        : m_pDesktopFolder(NULL), m_pContextMenu(NULL), m_pidlDesktop(NULL)
    {
    }

    ~CShellMenuHelper()
    {
        Release();
    }

    BOOL GetDesktopMenu()
    {
        Release();

        if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &m_pidlDesktop) != S_OK)
            return FALSE;

        if (SHGetDesktopFolder(&m_pDesktopFolder) != S_OK)
            return FALSE;

        if (m_pDesktopFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&m_pidlDesktop,
            IID_IContextMenu, NULL, (void**)&m_pContextMenu) != S_OK)
            return FALSE;

        return TRUE;
    }

    HMENU CreateMenu()
    {
        if (!m_pContextMenu)
            return NULL;

        HMENU hMenu = CreatePopupMenu();
        if (!hMenu)
            return NULL;

        if (m_pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL) == 0)
        {
            DestroyMenu(hMenu);
            return NULL;
        }

        return hMenu;
    }

    void Release()
    {
        if (m_pContextMenu)
        {
            m_pContextMenu->Release();
            m_pContextMenu = NULL;
        }

        if (m_pDesktopFolder)
        {
            m_pDesktopFolder->Release();
            m_pDesktopFolder = NULL;
        }

        if (m_pidlDesktop)
        {
            CoTaskMemFree(m_pidlDesktop);
            m_pidlDesktop = NULL;
        }
    }

    IContextMenu* GetContextMenu() { return m_pContextMenu; }

private:
    IShellFolder* m_pDesktopFolder;
    IContextMenu* m_pContextMenu;
    LPITEMIDLIST m_pidlDesktop;
};

