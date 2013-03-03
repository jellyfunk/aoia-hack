#include "StdAfx.h"
#include "FindPanel.h"
#include "InventoryView.h"
#include <boost/algorithm/string.hpp>

namespace ba = boost::algorithm;
using namespace aoia;

FindView::FindView(sqlite::IDBPtr db, aoia::ISettingsPtr settings)
    : m_db(db)
    , m_settings(settings)
    , m_lastQueryChar(-1)
    , m_lastQueryQlMin(-1)
    , m_lastQueryQlMax(-1)
    , m_lastQueryDimension(0)
    , m_pParent(NULL) {}


void FindView::SetParent(InventoryView* parent)
{
    m_pParent = parent;
}


LRESULT FindView::OnInitDialog(UINT/*uMsg*/, WPARAM/*wParam*/, LPARAM/*lParam*/, BOOL&/*bHandled*/)
{
    this->SetWindowText(_T("Find View"));

    updateDimensionList();

    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);

    if (cb.GetCount() > 0)
    {
        int index = CB_ERR;
        if (!m_settings->getValue(_T("DefaultDimension")).empty())
        {
            index = cb.FindStringExact(-1, m_settings->getValue(_T("DefaultDimension")).c_str());
        }
        if (index == CB_ERR)
        {
            index = 0;
        }
        cb.SetCurSel(index);
        updateCharList(cb.GetItemData(index));
        m_lastQueryDimension = cb.GetItemData(index);

        CComboBox toon_combo = GetDlgItem(IDC_CHARCOMBO);
        if (toon_combo.GetCount() > 0)
        {
            toon_combo.SetCurSel(0);
        }
    }

    DlgResize_Init(false, true, WS_CLIPCHILDREN);
    return 0;
}


LRESULT FindView::OnForwardMsg(UINT/*uMsg*/, WPARAM/*wParam*/, LPARAM lParam, BOOL&/*bHandled*/)
{
    LPMSG pMsg = (LPMSG)lParam;
    return this->PreTranslateMsg(pMsg);
}


BOOL FindView::PreTranslateMsg(MSG* pMsg)
{
    return IsDialogMessage(pMsg);
}


LRESULT FindView::onDimensionFocus(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    KillTimer(1);

    WTL::CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    int oldselection = cb.GetCurSel();

    updateDimensionList();

    if (oldselection >= 0)
    {
        cb.SetCurSel(oldselection);
    }
    else if (oldselection == -1)
    {
        cb.SetCurSel(0);
    }

    return 0;
}


LRESULT FindView::onDimensionSelection(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    unsigned int dimension_id = 0;
    int item = -1;
    if ((item = cb.GetCurSel()) != CB_ERR)
    {
        dimension_id = (unsigned int)cb.GetItemData(item);
        TCHAR buffer[256];
        cb.GetLBText(item, buffer);
        m_settings->setValue(_T("DefaultDimension"), buffer);
    }

    updateCharList(dimension_id);
    CComboBox toon_combo = GetDlgItem(IDC_CHARCOMBO);
    if (toon_combo.GetCount() > 0)
    {
        toon_combo.SetCurSel(0);
    }

    if (dimension_id != m_lastQueryDimension)
    {
        UpdateFindQuery();
    }

    return 0;
}


LRESULT FindView::OnEnChangeItemtext(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    SetTimer(1, 1500);
    return 0;
}


LRESULT FindView::OnCbnSelChangeCharcombo(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    UpdateFindQuery();
    return 0;
}


LRESULT FindView::OnCbnDropdown(WORD/*wNotifyCode*/, WORD/*wID*/, HWND/*hWndCtl*/, BOOL&/*bHandled*/)
{
    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    int item;
    if ((item = cb.GetCurSel()) != CB_ERR)
    {
        unsigned int dimension_id = (unsigned int)cb.GetItemData(item);

        CComboBox toon_combo = GetDlgItem(IDC_CHARCOMBO);
        unsigned int char_id = toon_combo.GetItemData(toon_combo.GetCurSel());

        updateCharList(dimension_id);

        bool found = false;
        for (int i = 0; i < toon_combo.GetCount(); ++i)
        {
            unsigned int data = toon_combo.GetItemData(i);
            if (data == char_id)
            {
                toon_combo.SetCurSel(i);
                found = true;
                break;
            }
        }

        if (!found)
        {
            toon_combo.SetCurSel(0);
        }
    }
    return 0;
}


LRESULT FindView::OnTimer(UINT wParam)
{
    if (wParam == 1)
    {
        UpdateFindQuery();
        KillTimer(1);
    }
    return 0;
}


void FindView::UpdateFindQuery()
{
    KillTimer(1);

    int item = -1;
    CComboBox cb = GetDlgItem(IDC_CHARCOMBO);
    CComboBox dimension_combo = GetDlgItem(IDC_DIMENSION_COMBO);
    CEdit eb = GetDlgItem(IDC_ITEMTEXT);
    CEdit qlmin = GetDlgItem(IDC_QLMIN);
    CEdit qlmax = GetDlgItem(IDC_QLMAX);

    unsigned int charid = 0;
    item = -1;
    if ((item = cb.GetCurSel()) != CB_ERR)
    {
        charid = (unsigned int)cb.GetItemData(item);
    }

    unsigned int dimension_id = 0;
    item = -1;
    if ((item = dimension_combo.GetCurSel()) != CB_ERR)
    {
        dimension_id = (unsigned int)dimension_combo.GetItemData(item);
    }

    TCHAR buffer[MAX_PATH];
    ZeroMemory(buffer, MAX_PATH);
    eb.GetWindowText(buffer, MAX_PATH);
    std::tstring text(buffer);
    ba::trim_if(text, ba::is_any_of(" \n\r\t"));

    int minql = -1;
    ZeroMemory(buffer, MAX_PATH);
    qlmin.GetWindowText(buffer, MAX_PATH);
    std::tstring qlminText(buffer);
    if (!qlminText.empty())
    {
        try
        {
            minql = boost::lexical_cast<int>(qlminText);
        }
        catch (boost::bad_lexical_cast&/*e*/)
        {
            // Go with the default value
        }
    }

    int maxql = -1;
    ZeroMemory(buffer, MAX_PATH);
    qlmax.GetWindowText(buffer, MAX_PATH);
    std::tstring qlmaxText(buffer);
    if (!qlmaxText.empty())
    {
        try
        {
            maxql = boost::lexical_cast<int>(qlmaxText);
        }
        catch (boost::bad_lexical_cast&/*e*/)
        {
            // Go with the default value
        }
    }

    if (text.size() > 2
        && (m_lastQueryText != text
        || m_lastQueryChar != charid
        || m_lastQueryQlMin != minql
        || m_lastQueryQlMax != maxql
        || m_lastQueryDimension != dimension_id))
    {
        m_lastQueryText = text;
        m_lastQueryChar = charid;
        m_lastQueryQlMin = minql;
        m_lastQueryQlMax = maxql;
        m_lastQueryDimension = dimension_id;
        std::tstringstream sql;

        if (charid > 0)
        {
            sql << _T("I.owner = ") << charid << _T(" AND ");
        }
        else
        {
            sql << _T("T.dimensionid = ") << dimension_id << _T(" AND ");
        }
        if (minql > -1)
        {
            sql << _T("I.ql >= ") << minql << _T(" AND ");
        }
        if (maxql > -1)
        {
            sql << _T("I.ql <= ") << maxql << _T(" AND ");
        }

        sql << _T("keylow IN (SELECT aoid FROM aodb.tblAO WHERE name LIKE \"%") << text << _T("%\")");

        m_pParent->UpdateListView(sql.str());
    }
}


void FindView::updateCharList(unsigned int dimension_id)
{
    CComboBox cb = GetDlgItem(IDC_CHARCOMBO);

    cb.ResetContent();
    int item = cb.AddString(_T("-"));
    cb.SetItemData(item, 0);

    boost::format sql("SELECT DISTINCT owner FROM tItems I JOIN tToons T ON I.owner = T.charid WHERE dimensionid = %1% ORDER BY T.charname");
    sql % dimension_id;

    g_DBManager.Lock();
    sqlite::ITablePtr pT = m_db->ExecTable(sql.str());
    g_DBManager.UnLock();

    if (pT != NULL)
    {
        for (unsigned int i = 0; i < pT->Rows(); i++)
        {
            try
            {
                unsigned int id = boost::lexical_cast<unsigned int>(pT->Data(i, 0));

                g_DBManager.Lock();
                std::tstring name = g_DBManager.GetToonName(id);
                g_DBManager.UnLock();

                if (name.empty())
                {
                    name = from_ascii_copy(pT->Data(i, 0));
                }

                if ((item = cb.AddString(name.c_str())) != CB_ERR)
                {
                    cb.SetItemData(item, id);
                }
            }
            catch (boost::bad_lexical_cast&/*e*/)
            {
                // This is here because of a wierd but that appears to be SQLite's fault.
                LOG("Error in updateCharList(). Bad lexical cast at row " << i << ".");
                continue;
            }
        }
    }
}


void FindView::updateDimensionList()
{
    int item = -1;

    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    cb.ResetContent();

    std::map<unsigned int, std::tstring> dimensionNames;
    g_DBManager.Lock();
    g_DBManager.GetDimensions(dimensionNames);
    sqlite::ITablePtr pT = m_db->ExecTable(_T("SELECT DISTINCT dimensionid FROM tToons"));
    g_DBManager.UnLock();

    // Add named dimensions.
    for (std::map<unsigned int, std::tstring>::iterator it = dimensionNames.begin(); it != dimensionNames.end(); ++it)
    {
        if ((item = cb.AddString(it->second.c_str())) != CB_ERR)
        {
            cb.SetItemData(item, it->first);
        }
    }

    // Add un-named dimensions.
    for (unsigned int i = 0; i < pT->Rows(); ++i)
    {
        unsigned int dimId = boost::lexical_cast<unsigned int>(pT->Data(i, 0));
        std::tstring dimName;
        if (dimensionNames.find(dimId) != dimensionNames.end())
        {
            continue;   // Skip named ones.
        }
        else
        {
            dimName = _T("Unknown Dimension");
            if (dimId > 0)
            {
                dimName += STREAM2STR(" (0x" << std::hex << dimId << ")");
            }
        }

        if ((item = cb.AddString(dimName.c_str())) != CB_ERR)
        {
            cb.SetItemData(item, dimId);
        }
    }
}
