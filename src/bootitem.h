/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef BOOTITEM_H_
#define BOOTITEM_H_

#include <experimental/filesystem>
#include <list>
#include <memory>
#include <string>

namespace fs = std::experimental::filesystem;

struct BootItem
{
    std::string filename;
    std::string texture;
    std::string hoverTexture;
    std::string launchCommand;
    bool isDefault = false;
    bool autoBoot = false;
    bool Launch();
    int Save();
};

std::list<std::unique_ptr<BootItem>> GetBootItems(const fs::path & bootItemDirectory);

#endif // BOOTITEM_H_
