#include "StdAfx.h"
#include "AOManager.h"
#include <boost/filesystem.hpp>
#include <Shared/UnicodeSupport.h>
#include <Shared/FileUtils.h>
#include <Shlobj.h>

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

std::tstring AOManager::getAOPrefsFolder() const
{
    assert(m_settings);

    if (m_prefsfolder.empty())
    {
        bool requestFolder = true;
        bfs::tpath prefsDir;
        
        // Get prefs folder from settings
        std::tstring dir_setting = m_settings->getValue(_T("PrefsPath"));
        if (!dir_setting.empty())
        {
            prefsDir = dir_setting;
            if (bfs::exists(prefsDir / _T("Prefs.xml"))) {
                requestFolder = false;
            }
        }

        //try locate it ourselves
        if (requestFolder) 
        {
             TCHAR localAppData[MAX_PATH];
             SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, localAppData);
             prefsDir = localAppData;
             prefsDir /= _T("Funcom");
             prefsDir /= _T("Anarchy Online");
             
             if (bfs::exists(prefsDir))
             {
                 bfs::directory_iterator subdir(prefsDir);
                 if (subdir != bfs::directory_iterator()) {
                     prefsDir = *subdir / _T("Anarchy Online") / _T("Prefs");
                     if (bfs::exists(prefsDir / _T("Prefs.xml"))) {
                         requestFolder = false;
                     }
                 }
			}
            // Store the new AO directory in the settings
            m_settings->setValue(_T("PrefsPath"), prefsDir.native());
		}

        //give up and prompt
		if (prefsDir.empty()) {
			prefsDir = BrowseForFolder(NULL, _T("Please locate the AO prefs folder:"));
            if (prefsDir.empty()) {
                return _T("");
            }

            if (!bfs::exists(prefsDir / _T("Prefs.xml"))) {
                MessageBox( NULL, _T("The specified directory doesn't seem to contain a valid Anarchy Online prefs tree."),
                    _T("Error - AO Item Assistant"), MB_OK | MB_ICONERROR);
                return _T("");
            }
            // Store the new AO directory in the settings
            m_settings->setValue(_T("PrefsPath"), prefsDir.native());
        }
        m_prefsfolder = prefsDir.native();
    }

    return m_prefsfolder;
}

std::vector<std::tstring> AOManager::getAccountNames() const
{
    std::vector<std::tstring> result;

    bfs::path path(to_ascii_copy(getAOPrefsFolder()));

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
