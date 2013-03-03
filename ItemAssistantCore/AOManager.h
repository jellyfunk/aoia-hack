#ifndef AOMANAGER_H
#define AOMANAGER_H

#include <ItemAssistantCore/ItemAssistantCore.h>
#include <Shared/UnicodeSupport.h>
#include <Shared/Singleton.h>
#include <settings/ISettings.h>
#include <vector>
#include <exception>


class ITEMASSISTANTCORE_API AOManager
{
    SINGLETON(AOManager);
public:
    ~AOManager();

    struct ITEMASSISTANTCORE_API AOManagerException : public std::exception {
        AOManagerException(std::tstring const& message) : std::exception(to_ascii_copy(message).c_str()) {}
    };

    void SettingsManager(aoia::ISettingsPtr settings);
    std::tstring getAOFolder() const;
	std::tstring getAOPrefsFolder() const;
    std::vector<std::tstring> getAccountNames() const;

private:
    mutable std::tstring m_aofolder;
    mutable std::tstring m_prefsfolder;
    aoia::ISettingsPtr m_settings;
};

#endif // AOMANAGER_H
