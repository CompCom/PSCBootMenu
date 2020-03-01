/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef LANGUAGEMANAGER_H_
#define LANGUAGEMANAGER_H_

#include <map>

class LanguageManager
{
public:
    ~LanguageManager();
    static LanguageManager& GetInstance();
    static std::string GetLanguageFolder();
    static std::string GetString(const std::string & stringId);
    static bool LoadLanguage(const std::string & languageJSONPath);
private:
    LanguageManager();
    LanguageManager(const LanguageManager&) = delete;
    LanguageManager& operator =(const LanguageManager&) = delete;
    bool _loadLanguage(const std::string & languageJSONPath);
    std::string languageFolder;
    std::map<std::string, std::string> stringValues;
};

#endif // LANGUAGEMANAGER_H_
