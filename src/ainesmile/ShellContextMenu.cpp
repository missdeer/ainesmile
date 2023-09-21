#include "stdafx.h"

#include "ShellContextMenu.h"

#define MIN_ID 1
#define MAX_ID 10000

void CShellContextMenu::ShowContextMenu(QMenu *menu, QWidget *parent, QPoint &pt, QString path)
{
    ITEMIDLIST *pItemIdList = 0;
    HRESULT     hResult     = SHParseDisplayName(path.replace(QChar('/'), QChar('\\')).toStdWString().c_str(), 0, &pItemIdList, 0, 0);
    if (!SUCCEEDED(hResult) || !pItemIdList)
    {
        return;
    }
    BOOST_SCOPE_EXIT(pItemIdList)
    {
        LPMALLOC lpMalloc = nullptr;
        SHGetMalloc(&lpMalloc);
        lpMalloc->Free(pItemIdList);
    }
    BOOST_SCOPE_EXIT_END
    IShellFolder *pIFolder = nullptr;

    LPCITEMIDLIST pcItemIdList = nullptr;
    hResult                    = SHBindToParent(pItemIdList, IID_IShellFolder, (void **)&pIFolder, &pcItemIdList);
    if (!SUCCEEDED(hResult) || !pIFolder)
    {
        return;
    }
    BOOST_SCOPE_EXIT(pIFolder)
    {
        pIFolder->Release();
    }
    BOOST_SCOPE_EXIT_END

    IContextMenu *imenu = nullptr;
    hResult             = pIFolder->GetUIObjectOf((HWND)parent->winId(), 1, (const ITEMIDLIST **)&pcItemIdList, IID_IContextMenu, 0, (void **)&imenu);
    if (!SUCCEEDED(hResult) || !pIFolder)
    {
        return;
    }
    BOOST_SCOPE_EXIT(imenu)
    {
        imenu->Release();
    }
    BOOST_SCOPE_EXIT_END

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
    {
        return;
    }
    if (SUCCEEDED(imenu->QueryContextMenu(hMenu, 0, MIN_ID, MAX_ID, CMF_NORMAL)))
    {
        QList<QAction *> actions = menu->actions();
        UINT_PTR         id      = MAX_ID + 1;
        ::AppendMenuW(hMenu, MF_SEPARATOR, id++, 0);
        for (QAction *action : actions)
        {
            if (action->text().isEmpty())
            {
                ::AppendMenuW(hMenu, MF_SEPARATOR, id++, 0);
            }
            else
            {
                ::AppendMenuW(hMenu, MF_STRING, id++, action->text().toStdWString().c_str());
            }
        }
        int iCmd = ::TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pt.x(), pt.y(), (HWND)parent->winId(), nullptr);
        if (iCmd > 0)
        {
            if (iCmd > MAX_ID + 1)
            {
                QAction *action = actions.at(iCmd - MAX_ID - 1 - 1);
                if (action)
                {
                    emit action->triggered();
                }
            }
            else
            {
                CMINVOKECOMMANDINFOEX info = {0};
                info.cbSize                = sizeof(info);
                info.fMask                 = CMIC_MASK_UNICODE;
                info.hwnd                  = (HWND)parent->winId();
                info.lpVerb                = MAKEINTRESOURCEA(iCmd - 1);
                info.lpVerbW               = MAKEINTRESOURCEW(iCmd - 1);
                info.nShow                 = SW_SHOWNORMAL;
                imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
            }
        }
    }
    DestroyMenu(hMenu);
}
