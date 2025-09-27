/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Stanislav Egorov.
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

#include "attack.h"
#include "batattacklowerinitiative.h"
#include "battleattackinfo.h"
#include "game.h"
#include "idvector.h"
#include "lowerinitiativehooks.h"
#include "midgardobjectmap.h"
#include "modifierutils.h"
#include "originalfunctions.h"
#include "ussoldier.h"
#include "usunit.h"

namespace hooks {

static bool __fastcall hasLoweredInitiative(const game::CMidgardID unitId,
                                            const game::BattleAttackInfo* attackInfo)
{
    for (const auto& unitInfo : attackInfo->unitsInfo)
        if (unitId == unitInfo.unitId && !unitInfo.attackMissed && !unitInfo.targetImmune)
            return true;
    return false;
}

static int __fastcall getUnitInitiative(game::CMidgardID* unitId,
                                        game::IMidgardObjectMap* objectMap,
                                        game::BattleAttackInfo** attackInfo,
                                        const game::GlobalVariables* vars,
                                        const game::Functions& functions)
{
    auto targetUnit = functions.findUnitById(objectMap, unitId);
    if (!targetUnit)
        return -1;
    auto targetSoldier = functions.castUnitImplToSoldier(targetUnit->unitImpl);
    if (!targetSoldier)
        return -1;
    auto soldierAttack = targetSoldier->vftable->getAttackById(targetSoldier);
    if (!soldierAttack)
        return -1;
    int unitInitiative = soldierAttack->vftable->getInitiative(soldierAttack);
    if (hasLoweredInitiative(*unitId, *attackInfo))
        unitInitiative = unitInitiative * (100 - vars->battleLowerIni) / 100;
    unitInitiative += functions.generateRandomNumber(vars->additionalBattleIni);
    return unitInitiative;
}

void __fastcall lowerInitiativeOnHitHooked(game::CBatAttackLowerInitiative* batAttack,
                                           int /*%edx*/,
                                           game::IMidgardObjectMap* objectMap,
                                           game::BattleMsgData* battleMsgData,
                                           game::CMidgardID* targetUnitId,
                                           game::BattleAttackInfo** attackInfo)
{
    getOriginalFunctions().batAttackLowerInitiativeOnHit(batAttack, objectMap, battleMsgData,
                                                         targetUnitId, attackInfo);
    
    using namespace game;
    const auto& funcs = gameFunctions();
    const auto data = *GlobalDataApi::get().getGlobalData();
    const auto vars = *data->globalVariables;
    
    int targetIni = 0;
    bool targetFound = false;
    BattleTurn targetUnit{};
    for (int index = 1; index < 12; index++) {
        BattleTurn currentUnit = battleMsgData->turnsOrder[index];
        if (targetFound) {
            int unitIni = getUnitInitiative(&currentUnit.unitId, objectMap, attackInfo, vars, funcs);
            if (unitIni > targetIni || (unitIni == targetIni && funcs.generateRandomNumber(2) == 1)) {
                battleMsgData->turnsOrder[index - 1] = currentUnit;
                battleMsgData->turnsOrder[index] = targetUnit;
            } else
                break;
        } else if (currentUnit.unitId == *targetUnitId) {
            targetFound = true;
            targetUnit = currentUnit;
            targetIni = getUnitInitiative(targetUnitId, objectMap, attackInfo, vars, funcs);
        }
    }
}

} // namespace hooks
