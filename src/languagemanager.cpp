/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "languagemanager.h"
#include "json.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

LanguageManager::LanguageManager()
{

}

LanguageManager::~LanguageManager()
{

}

LanguageManager& LanguageManager::GetInstance()
{
    static LanguageManager _instance;
    return _instance;
}

std::string LanguageManager::GetLanguageFolder()
{
    return GetInstance().languageFolder;
}

std::string LanguageManager::GetString(const std::string & stringId)
{
    auto stringIter = GetInstance().stringValues.find(stringId);
    if(stringIter != GetInstance().stringValues.end())
        return stringIter->second;
    else
        return stringId + "_MISSING";
}

bool LanguageManager::LoadLanguage(const std::string & languageJSONPath)
{
    return GetInstance()._loadLanguage(languageJSONPath);
}

bool LanguageManager::_loadLanguage(const std::string & languageJSONPath)
{
    std::ifstream in(languageJSONPath);
    if(in.is_open() == false)
        return false;

    languageFolder = fs::path(languageJSONPath).parent_path();

    json j;
    try
    {
        in >> j;
        in.close();
    }
    catch(...)
    {
        in.close();
        std::cerr << "Could not parse language json." << std::endl;
        return false;
    }

    stringValues = j.get<std::map<std::string, std::string>>();

    return true;
}
