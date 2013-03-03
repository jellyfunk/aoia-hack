#include "StdAfx.h"
#include "PatternMatchView.h"
#include "DBManager.h"
#include "PatternReport.h"
#include "Version.h"
#include <sstream>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>


PatternMatchView::PatternMatchView(sqlite::IDBPtr db, aoia::IContainerManagerPtr containerManager, aoia::IGuiServicesPtr gui, aoia::ISettingsPtr settings)
    : m_db(db)
    , m_containerManager(containerManager)
    , m_gui(gui)
    , m_availCalc(db)
    , m_availfilter(-1.0f)
    , m_dimensionid(0)
    , m_toonid(0)
    , m_sortDesc(true)
    , m_sortColumn(1)
    , m_webview(STREAM2STR("http://ia-help.frellu.net/?topic=patternmatcher&version=" << g_versionNumber))
    , m_filterPanel(db, settings)
{
    m_availCalc.SetOwner(this);
}


PatternMatchView::~PatternMatchView() {}


LRESULT PatternMatchView::OnCreate(LPCREATESTRUCT createStruct)
{
    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    m_listview.Create(m_hWnd, rcDefault, NULL, style | LVS_REPORT | LVS_SINGLESEL, WS_EX_CLIENTEDGE);

    m_filterConnection = m_filterPanel.connectSettingsChanged(boost::bind(&PatternMatchView::onFilterSettingsChanged,
        this));
    m_filterPanel.Create(m_hWnd);
    m_filterPanel.ShowWindow(SW_SHOWNOACTIVATE);

    m_webview.Create(m_hWnd);
    m_webview.ShowWindow(SW_SHOWNOACTIVATE);

    m_accelerators.LoadAccelerators(IDR_PB_ACCEL);

    // Build table of all PBs
    g_DBManager.Lock();
    sqlite::ITablePtr pT = m_db->ExecTable(_T("SELECT pbid, name FROM tblPocketBoss ORDER BY name"));
    g_DBManager.UnLock();

    if (pT != NULL)
    {
        for (unsigned int row = 0; row < pT->Rows(); row++)
        {
            boost::shared_ptr<PbItem> pPb = boost::shared_ptr<PbItem>(new PbItem());
            pPb->pbid = boost::lexical_cast<unsigned int>(pT->Data(row, 0));
            pPb->pbname = from_ascii_copy(pT->Data(row, 1));
            pPb->pbavailability = -1.0f;
            m_pblist.push_back(pPb);
        }

        UpdatePbListView();
    }

    // Build the toolbar
    TBBUTTON buttons[3];
    buttons[0].iBitmap = 0;
    buttons[0].idCommand = ID_RECALCULATE;
    buttons[0].fsState = TBSTATE_ENABLED;
    buttons[0].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_AUTOSIZE;
    buttons[0].dwData = NULL;
    buttons[0].iString = (INT_PTR)_T("Recalculate");
    buttons[1].iBitmap = 3;
    buttons[1].idCommand = 0;
    buttons[1].fsState = 0;
    buttons[1].fsStyle = BTNS_SEP;
    buttons[1].dwData = NULL;
    buttons[1].iString = NULL;
    buttons[2].iBitmap = 1;
    buttons[2].idCommand = ID_HELP;
    buttons[2].fsState = TBSTATE_ENABLED;
    buttons[2].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_AUTOSIZE;
    buttons[2].dwData = NULL;
    buttons[2].iString = (INT_PTR)_T("Help");

    CImageList imageList;
    imageList.CreateFromImage(IDB_PATTERNMATCH_VIEW, 16, 1, CLR_DEFAULT, IMAGE_BITMAP,
        LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

    m_toolbar.Create(GetTopLevelWindow(), NULL, _T("PatternMatchViewToolBar"),
        ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST,
        TBSTYLE_EX_MIXEDBUTTONS);
    m_toolbar.SetButtonStructSize();
    m_toolbar.SetImageList(imageList);
    m_toolbar.AddButtons(ARRAYSIZE(buttons), buttons);
    m_toolbar.AutoSize();

    return 0;
}


void PatternMatchView::OnDestroy()
{
    // Stop the bloddy worker thread while we still can count ourselves as a window.
    m_availCalc.StopPlease();
    m_availCalc.End();

    m_filterPanel.disconnect(m_filterConnection);
}


void PatternMatchView::SetBossAvail(unsigned int pbid, float avail)
{
    if (pbid == 0)
    {
        for (PbList::iterator it = m_pblist.begin(); it != m_pblist.end(); ++it)
        {
            (*it)->pbavailability = avail;
        }
    }
    else
    {
        for (PbList::iterator it = m_pblist.begin(); it != m_pblist.end(); ++it)
        {
            if ((*it)->pbid == pbid)
            {
                (*it)->pbavailability = avail;
                break;
            }
        }
    }
}


void PatternMatchView::onFilterSettingsChanged()
{
    unsigned int dimensionid = m_filterPanel.getDimensionId();
    unsigned int charid = m_filterPanel.getCharId();
    bool excludeAssembled = m_filterPanel.getExcludeAssembled();

    if (dimensionid != m_availCalc.getDimensionId() ||
        charid != m_availCalc.Toon() ||
        excludeAssembled != m_availCalc.ExcludeAssembled())
    {
        SetBossAvail(0, -1.0f);

        // Stop workerthread
        m_availCalc.StopPlease();
        m_availCalc.End();

        // Clean out pending messages from the old thread.
        MSG msg;
        while (PeekMessage(&msg, m_hWnd, WM_UPDATE_PBLIST, WM_UPDATE_PBLIST, PM_REMOVE))
        {
            continue;
        }

        // Restart thread
        m_availCalc.setDimensionId(dimensionid);
        m_availCalc.SetToon(charid);
        m_availCalc.SetExcludeAssembled(excludeAssembled);
        m_availCalc.Begin();
    }

    m_dimensionid = dimensionid;
    m_toonid = charid;
    m_availfilter = m_filterPanel.getAvailFilter();

    UpdatePbListView();
}


/**
* Updates the specified pocketboss entry. If pbid is zero, update all entries.
*/
void PatternMatchView::UpdatePbListView(unsigned int pbid)
{
    if (pbid == 0)
    {
        m_listview.DeleteAllItems();

        std::vector<std::tstring> cols;
        cols.push_back(_T("Pocket Boss"));
        cols.push_back(_T("Availability"));
        SetColumns(cols);

        for (PbList::iterator it = m_pblist.begin(); it != m_pblist.end(); ++it)
        {
            float avail = (*it)->pbavailability;
            if (avail >= m_availfilter)
            {
                cols.clear();
                cols.push_back((*it)->pbname);
                if ((*it)->pbavailability < 0.0f)
                {
                    cols.push_back(_T("calculating..."));
                }
                else
                {
                    cols.push_back(STREAM2STR(avail));
                }
                AddRow((*it)->pbid, cols);
            }
        }
    }
    else
    {
        for (PbList::iterator it = m_pblist.begin(); it != m_pblist.end(); ++it)
        {
            if (pbid == (*it)->pbid)
            {
                float avail = (*it)->pbavailability;
                if (avail >= m_availfilter)
                {
                    std::vector<std::tstring> cols;
                    cols.push_back((*it)->pbname);
                    float avail = (*it)->pbavailability;
                    if (avail < 0.0f)
                    {
                        cols.push_back(_T("calculating..."));
                    }
                    else
                    {
                        cols.push_back(STREAM2STR(avail));
                    }
                    UpdateRow((*it)->pbid, cols);
                }
                else
                {
                    LVFINDINFO findInfo;
                    findInfo.flags = LVFI_PARAM;
                    findInfo.lParam = pbid;

                    int indx = m_listview.FindItem(&findInfo, -1);

                    if (indx > -1)
                    {
                        m_listview.DeleteItem(indx);
                    }
                }
                break;
            }
        }
    }
    m_listview.SortItemsEx(CompareStr, (LPARAM)this);
}


void PatternMatchView::UpdateFilterProgress(unsigned short percent)
{
    m_filterPanel.setProgress(percent);
}


void PatternMatchView::SetColumns(std::vector<std::tstring>& headings)
{
    while (m_listview.GetHeader().GetItemCount() > 0)
    {
        m_listview.DeleteColumn(0);
    }

    for (int i = 0; i < (int)headings.size(); ++i)
    {
        int colid = m_listview.AddColumn(headings[i].c_str(), i);
        m_listview.SetColumnWidth(colid, 100);
    }
}


void PatternMatchView::AddRow(unsigned int rowid, std::vector<std::tstring>& data)
{
    int indx = m_listview.GetItemCount();

    for (int i = 0; i < (int)data.size(); ++i)
    {
        indx = m_listview.AddItem(indx, i, data[i].c_str());
        m_listview.SetItemData(indx, rowid);
    }
}


void PatternMatchView::UpdateRow(unsigned int rowid, std::vector<std::tstring>& data)
{
    LVFINDINFO findInfo;
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = rowid;

    int indx = m_listview.FindItem(&findInfo, -1);

    if (indx > -1)
    {
        for (int i = 0; i < (int)data.size(); ++i)
        {
            m_listview.SetItemText(indx, i, data[i].c_str());
        }
    }
    else
    {
        AddRow(rowid, data);
    }
}


void PatternMatchView::OnActive(bool doActivation)
{
    if (doActivation)
    {
        m_filterPanel.updateCharList();

        // Start up the calc thread
        m_availCalc.Begin();
    }
    else
    {
        // Shut down the calc thread
        m_availCalc.StopPlease();
        m_availCalc.End();

        // Clean out pending messages from the old thread.
        MSG msg;
        while (PeekMessage(&msg, m_hWnd, WM_UPDATE_PBLIST, WM_UPDATE_PBLIST, PM_REMOVE))
        {
            continue;
        }
    }
}


LRESULT PatternMatchView::OnColumnClick(LPNMHDR lParam)
{
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;

    if (m_sortColumn != pnmv->iSubItem)
    {
        m_sortColumn = pnmv->iSubItem;
        m_sortDesc = false;
    }
    else
    {
        m_sortDesc = !m_sortDesc;
    }

    m_listview.SortItemsEx(CompareStr, (LPARAM)this);

    return 0;
}


LRESULT PatternMatchView::OnItemChanging(LPNMHDR lParam)
{
    LPNMLISTVIEW pItem = (LPNMLISTVIEW)lParam;

    // Check to see if the change is a selection event.
    if (!(pItem->uOldState & LVIS_SELECTED) && (pItem->uNewState & LVIS_SELECTED))
    {
        unsigned int data = m_listview.GetItemData(pItem->iItem);
        PatternReport report(m_db, m_containerManager, m_dimensionid, data, m_toonid, m_availCalc.ExcludeAssembled());
        m_webview.SetHTML(report.toString());
    }

    return FALSE;
}


LRESULT PatternMatchView::OnSize(UINT wParam, CSize newSize)
{
    UpdateLayout(newSize);
    return 0;
}


LRESULT PatternMatchView::OnRecalculate(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    // Stop workerthread
    m_availCalc.StopPlease();
    m_availCalc.End();

    // Clean out pending messages from the old thread.
    MSG msg;
    while (PeekMessage(&msg, m_hWnd, WM_UPDATE_PBLIST, WM_UPDATE_PBLIST, PM_REMOVE))
    {
        continue;
    }

    // Reset calculations for ALL PBs
    SetBossAvail(0, -1.0f);

    // Restart thread
    m_availCalc.SetToon(m_toonid);
    m_availCalc.Begin();

    UpdatePbListView();

    return 0;
}


LRESULT PatternMatchView::OnHelp(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    m_gui->ShowHelp(_T("patternmatcher"));
    return 0;
}


LRESULT PatternMatchView::OnUpdatePbListView(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UpdatePbListView(lParam);
    UpdateFilterProgress(wParam);
    return 0;
}


void PatternMatchView::UpdateLayout(CSize newSize)
{
    int left = 0;
    int top = 0;

    if (::IsWindowVisible(m_filterPanel))
    {
        RECT fvRect;
        m_filterPanel.GetWindowRect(&fvRect);
        int width = fvRect.right - fvRect.left;
        int height = fvRect.bottom - fvRect.top;
        ::SetWindowPos(m_filterPanel, 0, 0, 0, width, height,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
        left += width;
        top += height;
    }

    m_listview.SetWindowPos(NULL, 0, top, left, newSize.cy - top,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
    m_webview.SetWindowPos(NULL, left, 0, newSize.cx - left, newSize.cy,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
}


bool PatternMatchView::PreTranslateMsg(MSG* pMsg)
{
    if (m_accelerators.TranslateAccelerator(m_hWnd, pMsg))
    {
        return true;
    }
    if (m_filterPanel.PreTranslateMsg(pMsg))
    {
        return true;
    }

    return false;
}


// Static callback function for sorting items.
int PatternMatchView::CompareStr(LPARAM param1, LPARAM param2, LPARAM sort)
{
    PatternMatchView* pThis = (PatternMatchView*)sort;

    int col = pThis->m_sortColumn;

    TCHAR name1[MAX_PATH];
    TCHAR name2[MAX_PATH];

    ZeroMemory(name1, sizeof(name1));
    ZeroMemory(name2, sizeof(name2));

    pThis->m_listview.GetItemText(param1, col, name1, sizeof(name1) - 1);
    pThis->m_listview.GetItemText(param2, col, name2, sizeof(name2) - 1);

    return pThis->m_sortDesc ? StrCmpI(name2, name1) : StrCmpI(name1, name2);
}


/*************************************************/
/** Worker Thread                               **/
/*************************************************/

DWORD AvailCalcThread::ThreadProc()
{
    m_index = 0;
    m_term = false;

    do
    {
        int lastUpdate = -1;

        m_pOwner->PbListMutex().MutexOn();

        PatternMatchView::PbList& list = m_pOwner->PbListRef();

        if (m_index < (int)list.size())
        {
            if (list[m_index]->pbavailability < 0)
            {
                unsigned int pbid = list[m_index]->pbid;
                m_pOwner->PbListMutex().MutexOff();
                float avail = CalcPbAvailability(m_db, m_dimensionid, pbid, m_toon, m_excludeAssembled);
                m_pOwner->PbListMutex().MutexOn();
                m_pOwner->SetBossAvail(pbid, avail);
                unsigned short percent = (unsigned int)(((m_index + 1) * 100) / list.size());
                if (percent > lastUpdate)
                {
                    m_pOwner->PostMessage(WM_UPDATE_PBLIST, percent, pbid);
                    lastUpdate = percent;
                }
            }
            ++m_index;
        }
        else
        {
            //m_index = 0;
            m_term = true;
        }

        m_pOwner->PbListMutex().MutexOff();

        ::Sleep(0);
    }
    while (!m_term);

    m_index = 0;
    m_term = false;

    return 0;
}


float AvailCalcThread::CalcPbAvailability(sqlite::IDBPtr db, unsigned int dimensionid, unsigned int pbid, unsigned int toonid, bool excludeassembled)
{
    std::map<std::tstring, unsigned int> vals;

    // Get a list of all pattern pieces for the specified pocket boss (optionally exclude ABCD assemblies)
    sqlite::ITablePtr pIDs;
    g_DBManager.Lock();
    {
        std::tstringstream sql;
        sql << _T("SELECT aoid, pattern FROM tblPatterns WHERE name = (SELECT name FROM tblPocketBoss WHERE pbid = ")
            << pbid << _T(")");
        if (excludeassembled)
        {
            sql << _T("AND pattern != 'ABCD'");
        }
        pIDs = db->ExecTable(sql.str());
    }
    g_DBManager.UnLock();

    if (pIDs == NULL)
    {
        return 0.0f;
    }

    // Loop all the pattern pieces and searches for recorded items.
    for (unsigned int idIdx = 0; idIdx < pIDs->Rows(); ++idIdx)
    {
        std::tstring pattern = from_ascii_copy(pIDs->Data(idIdx, 1));
        std::tstring id = from_ascii_copy(pIDs->Data(idIdx, 0));

        std::tstring sql =
            STREAM2STR("SELECT COUNT(itemidx) FROM tItems I JOIN tToons T ON I.owner = T.charid WHERE I.keylow = " <<
            id << " AND T.dimensionid = " << dimensionid);
        if (toonid > 0)
        {
            sql += STREAM2STR(" AND I.owner = " << toonid);
        }

        g_DBManager.Lock();
        sqlite::ITablePtr pItemCount = db->ExecTable(sql);
        g_DBManager.UnLock();

        if (pItemCount && pItemCount->Rows() > 0)
        {
            vals[pattern] += boost::lexical_cast<unsigned int>(pItemCount->Data(0, 0));
        }
    }

    float result = 0;

    if (vals[_T("AB")] > 0)
    {
        vals[_T("A")] += vals[_T("AB")];
        vals[_T("B")] += vals[_T("AB")];
    }
    if (vals[_T("ABC")] > 0)
    {
        vals[_T("A")] += vals[_T("ABC")];
        vals[_T("B")] += vals[_T("ABC")];
        vals[_T("C")] += vals[_T("ABC")];
    }

    result += vals[_T("ABCD")];

    unsigned int v = min(vals[_T("A")], min(vals[_T("B")], min(vals[_T("C")], vals[_T("D")])));

    if (v > 0)
    {
        result += v;
        vals[_T("A")] -= v;
        vals[_T("B")] -= v;
        vals[_T("C")] -= v;
        vals[_T("D")] -= v;
    }

    if (vals[_T("A")] > 0)
    {
        result += 0.25;
    }
    if (vals[_T("B")] > 0)
    {
        result += 0.25;
    }
    if (vals[_T("C")] > 0)
    {
        result += 0.25;
    }
    if (vals[_T("D")] > 0)
    {
        result += 0.25;
    }

    return result;
}
