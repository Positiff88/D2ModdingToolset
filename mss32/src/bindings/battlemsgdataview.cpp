/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2022 Stanislav Egorov.
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

#include "battlemsgdataview.h"
#include "battlemsgdata.h"
#include "idview.h"
#include <sol/sol.hpp>

namespace bindings {

BattleMsgDataView::BattleMsgDataView(const game::BattleMsgData* battleMsgData)
    : battleMsgData{battleMsgData}
{ }

void BattleMsgDataView::bind(sol::state& lua)
{
    auto view = lua.new_usertype<BattleMsgDataView>("BattleView");
    view["getUnitStatus"] = &BattleMsgDataView::getUnitStatus;
    view["currentRound"] = sol::property(&BattleMsgDataView::getCurrentRound);
}

bool BattleMsgDataView::getUnitStatus(const IdView& unitId, int status) const
{
    using namespace game;

    return BattleMsgDataApi::get().getUnitStatus(battleMsgData, &unitId.id, (BattleStatus)status);
}

int BattleMsgDataView::getCurrentRound() const
{
    return battleMsgData->currentRound;
}

} // namespace bindings
