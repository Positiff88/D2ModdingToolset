/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETCUSTOMSESSION_H
#define NETCUSTOMSESSION_H

#include "mqnetsession.h"
#include <string>
#include <vector>

namespace game {
struct IMqNetPlayer;
}

namespace hooks {

struct CNetCustomService;
struct CNetCustomPlayerClient;
struct CNetCustomPlayerServer;

struct CNetCustomSession : public game::IMqNetSession
{
    CNetCustomSession(CNetCustomService* service, const char* name, bool host)
        : name{name}
        , service{service}
        , server{nullptr}
        , host{host}
    { }

    std::string name;
    std::vector<CNetCustomPlayerClient*> players;
    CNetCustomPlayerServer* server;
    CNetCustomService* service;
    bool host;
};

game::IMqNetSession* createCustomNetSession(CNetCustomService* service,
                                            const char* sessionName,
                                            bool host);

} // namespace hooks

#endif // NETCUSTOMSESSION_H