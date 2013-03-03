#include "StdAfx.h"
#include "AOManager.h"
#include <boost/filesystem.hpp>
#include <Shared/UnicodeSupport.h>
#include <Shared/FileUtils.h>


namespace bfs = boost::filesystem;
using namespace aoia;


SINGLETON_IMPL(AOManager);


AOManager::AOManager()
{
}


AOManager::AOManager(const AOManager&)
{
}


AOManager::~AOManager()
{
}


std::tstring AOManager::getAOFolder() const
{
    assert(m_settings);

    if (m_aofolder.empty())
    {
        bfs::tpath AODir;
        bool requestFolder = true;

        // Get AO folder from settings
        std::tstring dir_setting = m_settings->getValue(_T("AOPath"));
        if (!dir_setting.empty())
        {
            AODir = dir_setting;
            if (bfs::exists(AODir / _T("anarchy.exe")))
            {
                requestFolder = false;
            }
        }

        if (requestFolder)
        {
            AODir = BrowseForFolder(NULL, _T("Please locate the AO directory:"));
            if (AODir.empty()) {
                return _T("");
            }

            if (!bfs::exists(AODir / _T("anarchy.exe"))) {
                MessageBox( NULL, _T("The specified directory doesn't contain a valid Anarchy Online installation."),
                    _T("Error - AO Item Assistant"), MB_OK | MB_ICONERROR);
                return _T("");
            }

            // Store the new AO directory in the settings
            m_settings->setValue(_T("AOPath"), AODir.native());
        }

        m_aofolder = AODir.native();
    }

    return m_aofolder;
}


std::vector<std::tstring> AOManager::getAccountNames() const
{
    std::vector<std::tstring> result;

    bfs::path path(to_ascii_copy(getAOFolder()));
    path = path / "Prefs";

    if (bfs::exists(path)) {
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(path); itr != end_itr; ++itr ) {
            if (bfs::is_directory(itr->status())) {
                result.push_back(itr->path().leaf().c_str());
            }
        }
    }

    return result;
}


void AOManager::SettingsManager( aoia::ISettingsPtr settings )
{
    m_settings = settings;
}
