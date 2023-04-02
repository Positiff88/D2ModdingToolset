/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2023 Vladimir Makeev.
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

#include "menurandomscenariosingle.h"
#include "mempool.h"
#include "menuphase.h"
#include "menurandomscenario.h"
#include "utils.h"

namespace hooks {

/** Menu for single-player random scenario generation. */
struct CMenuRandomScenarioSingle : public CMenuRandomScenario
{
    CMenuRandomScenarioSingle(game::CMenuPhase* menuPhase);
    ~CMenuRandomScenarioSingle() = default;
};

static void startScenarioLocal(CMenuRandomScenario* menu)
{
    prepareToStartRandomScenario(menu);

    // Switch to the next menu
    game::CMenuPhaseApi::get().setTransition(menu->menuBaseData->menuPhase, 0);
}

CMenuRandomScenarioSingle::CMenuRandomScenarioSingle(game::CMenuPhase* menuPhase)
    : CMenuRandomScenario(menuPhase, startScenarioLocal, "DLG_RANDOM_SCENARIO_SINGLE")
{ }

game::CMenuBase* __stdcall createMenuRandomScenarioSingle(game::CMenuPhase* menuPhase)
{
    const auto& allocateMem{game::Memory::get().allocate};

    auto menu = (CMenuRandomScenarioSingle*)allocateMem(sizeof(CMenuRandomScenarioSingle));
    new (menu) CMenuRandomScenarioSingle(menuPhase);

    return menu;
}

} // namespace hooks
