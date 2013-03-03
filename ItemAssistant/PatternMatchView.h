#ifndef PATTERNMATCHVIEW
#define PATTERNMATCHVIEW

#include "FilterPanel.h"
#include "resource.h"
#include "shared/Mutex.h"
#include "shared/Thread.h"
#include "shared/aopackets.h"
#include <Shared/IDB.h>
#include <PluginSDK/IContainerManager.h>
#include <PluginSDK/IGuiServices.h>
#include <PluginSDK/ItemAssistView.h>
#include "WebView.h"

// Forward declarations
class PatternMatchView;

/**
 * \brief
 * This class spawns a new thread that will search for pocketboss patterns 
 * given the current search criterias.
 * 
 * To trigger a search, set the appropriate filters and call Begin().
 * To cancel a search call StopPlease() followed by End().
 * Updates are posted to the owner as \e WM_UPDATE_PBLIST messages.
 */
class AvailCalcThread : public Thread
{
public:
    AvailCalcThread(sqlite::IDBPtr db) : m_db(db), m_index(0), m_term(false), m_dimensionid(0), m_toon(0), m_excludeAssembled(false) { }
    virtual ~AvailCalcThread() { }

    void SetOwner(PatternMatchView* owner) { m_pOwner = owner; }
    void setDimensionId(unsigned int dimensionid) { m_dimensionid = dimensionid; }
    void SetToon(unsigned int toon = 0) { m_toon = toon; }
    unsigned int getDimensionId() const { return m_dimensionid; }
    unsigned int Toon() const { return m_toon; }
    void SetExcludeAssembled(bool newVal) { m_excludeAssembled = newVal; }
    bool ExcludeAssembled() const { return m_excludeAssembled; }
    void StopPlease() { if (IsRunning()) { m_term = true; } }

    virtual DWORD ThreadProc();

    static float CalcPbAvailability(sqlite::IDBPtr db, unsigned int dimensionid, unsigned int pbid, unsigned int toonid, bool excludeassembled);

private:
    PatternMatchView* m_pOwner;
    int m_index;
    unsigned int m_dimensionid;
    unsigned int m_toon;
    bool m_term;
    bool m_excludeAssembled;
    sqlite::IDBPtr m_db;
};


#define WM_UPDATE_PBLIST WM_USER+1


class PatternMatchView
    : public ItemAssistView<PatternMatchView>
{
    typedef ItemAssistView<PatternMatchView> inherited;
public:
    DECLARE_WND_CLASS(NULL)

    PatternMatchView(sqlite::IDBPtr db, aoia::IContainerManagerPtr containerManager, aoia::IGuiServicesPtr gui, aoia::ISettingsPtr settings);
    virtual ~PatternMatchView();

    BEGIN_MSG_MAP_EX(PatternMatchView)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
        COMMAND_ID_HANDLER(ID_RECALCULATE, OnRecalculate)
        COMMAND_ID_HANDLER(ID_HELP, OnHelp)
        MESSAGE_HANDLER(WM_UPDATE_PBLIST, OnUpdatePbListView)
        NOTIFY_CODE_HANDLER_EX(LVN_COLUMNCLICK, OnColumnClick)
        NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGING, OnItemChanging)
        CHAIN_MSG_MAP(inherited)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    virtual void OnActive(bool doActivation);
    virtual bool PreTranslateMsg(MSG* pMsg);

    // Pocket boss list and access methods
    struct PbItem {
        unsigned int pbid;
        std::tstring pbname;
        float pbavailability;
    };
    typedef std::vector<boost::shared_ptr<PbItem> > PbList;
    PbList const& BossList() { return m_pblist; }
    void SetBossAvail(unsigned int pbid, float avail);

    void UpdatePbListView(unsigned int pbid = 0);

    PbList& PbListRef() { return m_pblist; }
    Mutex& PbListMutex() { return m_pblistMutex; }

protected:
    enum {
        AVAIL_TIMER = 1
    };

    LRESULT OnCreate(LPCREATESTRUCT createStruct);
    void OnDestroy();
    LRESULT OnSize(UINT wParam, CSize newSize);
    LRESULT OnRecalculate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnColumnClick(LPNMHDR lParam);
    LRESULT OnUpdatePbListView(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnItemChanging(LPNMHDR lParam);

    void UpdateFilterProgress(unsigned short percent);

    void UpdateLayout(CSize newSize);
    static int CALLBACK CompareStr(LPARAM param1, LPARAM param2, LPARAM sort);

    void SetColumns(std::vector<std::tstring> &headings);
    void AddRow(unsigned int rowid, std::vector<std::tstring> &data);
    void UpdateRow(unsigned int rowid, std::vector<std::tstring> &data);

    void onFilterSettingsChanged();

private:
    PbList m_pblist;

    unsigned int m_dimensionid;
    float m_availfilter;
    unsigned int m_toonid;

    bool  m_sortDesc;
    int   m_sortColumn;

    CListViewCtrl     m_listview;
    aoia::WebView     m_webview;
    CAccelerator      m_accelerators;
    AvailCalcThread   m_availCalc;
    Mutex             m_pblistMutex;

    PatternMatcher::FilterPanel m_filterPanel;
    PatternMatcher::FilterPanel::Connection m_filterConnection;

    sqlite::IDBPtr m_db;
    aoia::IContainerManagerPtr m_containerManager;
    aoia::IGuiServicesPtr m_gui;
};


#endif // PATTERNMATCHVIEW
