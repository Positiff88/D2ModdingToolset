/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2020 Vladimir Makeev.
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

#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include "d2list.h"
#include "d2pair.h"
#include "midgardid.h"
#include "mq_c_s.h"
#include "smartptr.h"
#include <cstddef>

namespace game {

struct LAttackClassTable;
struct LAttackSourceTable;
struct LAttackReachTable;
struct LUnitBranchTable;
struct LUnitCategoryTable;
struct LBuildingCategoryTable;
struct LGroundCategoryTable;
struct LDifficultyLevelTable;
struct LItemCategoryTable;
struct LLordCategoryTable;
struct LModifierElementTable;
struct LOrderCategoryTable;
struct LRaceCategoryTable;
struct LSubRaceCategoryTable;
struct LResourceTypeTable;
struct LSiteCategoryTable;
struct LSpellCategoryTable;
struct LTerrainCategoryTable;
struct LLeaderAbilityTable;
struct LLeaderCategoryTable;
struct LAiSpellCategoryTable;
struct LModifGroupTable;
struct LAttitudesCategoryTable;
struct LAiMsgCategoryTable;
struct LNobleActionCatTable;
struct LImmuneCatTable;
struct LFortCategoryTable;
struct LEventCondCategoryTable;
struct LEventEffectCategoryTable;
struct LLandmarkCategoryTable;
struct LDeathAnimCategoryTable;

struct TBuildingType;
struct TRaceType;
struct TSubRaceType;
struct TLordType;
struct TStrategicSpell;
struct TUsUnitImpl;
struct TUnitModifier;
struct CAttackImpl;
struct TLandmark;
struct TItemTypeList;
struct GlobalVariables;
struct CUnitGenerator;
struct CItemBase;
struct CDynUpgrade;
struct CTileVariation;

using RacesMap = mq_c_s<Pair<CMidgardID, TRaceType*>>;
using DynUpgradeList = List<SmartPtr<CDynUpgrade>>;

/** Holds global game information. */
struct GlobalData
{
    mq_c_s<Pair<CMidgardID, char*>>* text;
    LAttackClassTable* attackClasses;
    LAttackSourceTable* attackSources;
    LAttackReachTable* attackReach;
    LUnitBranchTable* unitBranches;
    LUnitCategoryTable* unitCategories;
    LBuildingCategoryTable* buildingCategories;
    LGroundCategoryTable* groundCategories;
    LDifficultyLevelTable* difficultyLevels;
    LItemCategoryTable* itemCategories;
    LLordCategoryTable* lordCategories;
    LModifierElementTable* modifierElements;
    LOrderCategoryTable* orderCategories;
    LRaceCategoryTable* raceCategories;
    LSubRaceCategoryTable* subRaceCategories;
    LResourceTypeTable* resourceTypes;
    LSiteCategoryTable* siteCategories;
    LSpellCategoryTable* spellCategories;
    LTerrainCategoryTable* terrainCategories;
    LLeaderAbilityTable* leaderAbilities;
    LLeaderCategoryTable* leaderCategories;
    LAiSpellCategoryTable* aiSpellCategories;
    LModifGroupTable* modifGroups;
    LAttitudesCategoryTable* attitudesCategories;
    LAiMsgCategoryTable* aiMsgCategories;
    LNobleActionCatTable* nobleActionCategories;
    LImmuneCatTable* immuneCategories;
    LFortCategoryTable* fortCategories;
    LEventCondCategoryTable* eventCondCategories;
    LEventEffectCategoryTable* eventEffectCategories;
    LLandmarkCategoryTable* landmarkCategories;
    LDeathAnimCategoryTable* deathAnimCategories;
    mq_c_s<Pair<CMidgardID, TBuildingType*>>* buildings;
    RacesMap** races;
    mq_c_s<Pair<CMidgardID, TSubRaceType*>>* subRaces;
    mq_c_s<Pair<CMidgardID, TLordType*>>* lords;
    mq_c_s<Pair<CMidgardID, TStrategicSpell*>>* spells;
    mq_c_s<Pair<CMidgardID, TUsUnitImpl*>>** units;
    mq_c_s<Pair<CMidgardID, TUnitModifier*>>* modifiers;
    mq_c_s<Pair<CMidgardID, CAttackImpl*>>* attacks;
    mq_c_s<Pair<CMidgardID, TLandmark*>>** landmarks;
    TItemTypeList* itemTypes;
    int* actions;
    int* transf;
    DynUpgradeList* dynUpgrade;
    CTileVariation* tileVariation;
    int* aiAttitudes;
    int* aiMessages;
    GlobalVariables** globalVariables;
    CUnitGenerator* unitGenerator;
    int initialized;
};

static_assert(sizeof(GlobalData) == 204, "Size of GlobalData structure must be exactly 204 bytes");

static_assert(offsetof(GlobalData, dynUpgrade) == 176,
              "GlobalData::dynUpgrade offset must be 176 bytes");

namespace GlobalDataApi {

struct Api
{
    using GetGlobalData = GlobalData**(__cdecl*)();
    GetGlobalData getGlobalData;

    /** Searches an entity by its id. */
    using FindById = void*(__thiscall*)(void* entityCollection, const CMidgardID* id);
    FindById findById;

    using FindItemById = const CItemBase*(__thiscall*)(TItemTypeList* thisptr,
                                                       const CMidgardID* id);
    FindItemById findItemById;

    using FindDynUpgradeById = const CDynUpgrade*(__thiscall*)(DynUpgradeList* thisptr,
                                                               const CMidgardID* id);
    FindDynUpgradeById findDynUpgradeById;
};

Api& get();

} // namespace GlobalDataApi

} // namespace game

#endif // GLOBALDATA_H
