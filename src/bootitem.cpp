/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "bootitem.h"
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

using json = nlohmann::json;

void to_json(json& j, const BootItem& b) {
    j = json{
               {"texture", b.texture},
               {"hoverTexture", b.hoverTexture},
               {"launchCommand", b.launchCommand},
               {"default", b.isDefault},
               {"autoBoot", b.autoBoot},
            };
}

void from_json(const json& j, BootItem& b) {
    j.at("texture").get_to(b.texture);
    j.at("hoverTexture").get_to(b.hoverTexture);
    j.at("launchCommand").get_to(b.launchCommand);

    auto isDefaultItem = j.find("default");
    if(isDefaultItem != j.end())
        b.isDefault = *isDefaultItem;

    auto autoBoot = j.find("autoBoot");
    if(autoBoot != j.end())
        b.autoBoot = *autoBoot;
}

bool BootItem::Launch()
{
    std::ofstream out("/tmp/launchfilecommand");
    if(out.is_open())
    {
        out << launchCommand;
        out.close();
        return true;
    }
    return false;
}

int BootItem::Save()
{
    std::ofstream out(filename);
    if(out.is_open())
    {
        try
        {
            json j = *this;
            out << std::setw(2) << j << std::endl;
            out.close();
            return 0;
        }
        catch(...)
        {
            out.close();
            return 2;
        }
    }
    else
        return 1;
}

std::list<std::unique_ptr<BootItem>> GetBootItems(const fs::path & bootItemDirectory)
{
    std::list<std::unique_ptr<BootItem>> bootItems;
    for(auto & entry : fs::directory_iterator(bootItemDirectory))
    {
        const auto & path = entry.path();
        try
        {
            if(fs::is_regular_file(path) && path.extension().compare(".json") == 0)
            {
                json j;
                std::ifstream in(path.string());
                if(in.is_open())
                {
                    in >> j;
                    in.close();
                    std::unique_ptr<BootItem> b = std::make_unique<BootItem>(j.get<BootItem>());
                    b->filename = fs::absolute(path).c_str();
                    bootItems.push_back(std::move(b));
                }
                else
                {
                    std::cerr << "Error opening boot item json: " << path << std::endl;
                }
            }
        }
        catch(...)
        {
            std::cerr << "Error parsing boot item json: " << path << std::endl;
        }
    }
    return bootItems;
}
