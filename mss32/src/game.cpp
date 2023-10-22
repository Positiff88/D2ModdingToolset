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

#include "game.h"
#include "version.h"
#include <array>

namespace game {

// clang-format off
static std::array<Functions, 4> functions = {{
    // Akella
    Functions{
        (RespopupInitFunc)0x4893ad,
        (ToggleShowBannersInitFunc)0x5b4015,
        (AddUnitToHireList)0x5d5bb4,
        (AddSideshowUnitToHireList)0x5d5ab6,
        (AddPlayerUnitsToHireList)0x5d59e8,
        (CreateBuildingType)0x58bf67,
        (AddObjectAndCheckDuplicates)0x59c0a8,
        (ChooseUnitLane)0x5d6abb,
        (GetLordByPlayer)0x5e6323,
        (IsTurnValid)0x5879f3,
        (GetAllyOrEnemyGroupId)0x65afb2,
        (FindUnitById)0x625180,
        (CastUnitImplToSoldier)0x40a79d,
        (CastUnitImplToStackLeader)0x40a7c3,
        (CreateBatAttack)0x64630e,
        (GetAttackById)0x645df6,
        (IsUnitImmuneToAttack)0x65baa8,
        (GetAttackClassWardFlagPosition)0x623e12,
        (AttackClassToString)0x5c7af0,
        (GetAttackSourceWardFlagPosition)0x622dfe,
        (GetStackFortRuinGroup)0x5f6304,
        (GetStackFortRuinGroupForChange)0x5f6349,
        (GetStackFortRuinForChange)0x5f6255,
        (GetStackFortRuinId)0x5eba8e,
        (GetStackIdByUnitId)0x5ebabe,
        (GetFortIdByUnitId)0x5ebbc8,
        (GetRuinIdByUnitId)0x5ebccb,
        (DeletePlayerBuildings)0x422498,
        (GetInterfaceText)0x5c8f38,
        (ComputePlayerDailyIncome)0x5db82a,
        (ComputeDamage)0x65ccc7,
        (ComputeDamageMax)0x61f05c,
        (ComputeDamageWithBuffs)0x5d3e80,
        (ComputeArmor)0x5d3d5e,
        (MarkMapPosition)0x5cdf6f,
        (GetUnitLevelByImplId)0x599581,
        (GetAttackPower)0x628b0a,
        (IsGroupOwnerPlayerHuman)0x628165,
        (AttackShouldMiss)0x626b5f,
        (GenerateRandomNumber)0x548e10,
        (GenerateRandomNumberStd)0x548e67,
        (GetUnitPositionInGroup)0x65fb1e,
        (GetSummonUnitImplIdByAttack)0x661fe0,
        (GetSummonUnitImplId)0x664b5c,
        (GetAttackImplMagic)0x647561,
        (GetUnitHealAttackNumber)0x6283fb,
        (GetAttackQtyDamageOrHeal)0x61efc4,
        (ComputeUnitEffectiveHpForAi)0x5d0053,
        (ApplyDynUpgradeToAttackData)0x59a76b,
        (ComputeUnitDynUpgrade)0x5998fc,
        (ShowMovementPath)0x4cd533,
        (GetMidgardPlan)0x5f756f,
        (CastUnitImplToNoble)0x41b8cc,
        (GetBlockingPathNearbyStackId)0x5ce204,
        (GetFortOrRuinEntrance)0x4cdb5a,
        (StackCanMoveToPosition)0x4cdae4,
        (IsWaterTileSurroundedByWater)0x5e773e,
        (GetStackPositionById)0x5eecf2,
        (ApplyPercentModifiers)0x577515,
        (GenerateAttackDescription)0x57652b,
        (CreateMenuAnimation)0x4eb465,
        (GetAttackSourceText)0x61e1df,
        (AppendAttackSourceText)0x61e923,
        (IsRaceCategoryUnplayable)0x5e6691,
        (ValidateRaces)0x582ecb,
        (CheckRaceExist)0x582f6f,
        (GetUnitAttackSourceImmunities)0x408b0d,
        (GetSoldierAttackSourceImmunities)0x61e3c8,
        (GetTerrainByAbbreviation)0x5a6fbb,
        (GetTerrainByRace)0x5f105e,
        (GetTerrainByRace)0x5e6449,
        (GetRaceByTerrain)0x5e64c4,
        (GetRaceByTerrain)0x5db0ab,
        (GetNumberByTerrainGround)0x5cbf2f,
        (ThrowGenericException)0x596690,
        (IgnorePlayerEvents)0x5e66ec,
        (GetRacePreviewImage)0x4f1c4c,
        (GetSoldierImmunityAiRating)0x5de4e6,
        (GetAttackClassAiRating)0x5de83f,
        (GetAttackReachAiRating)0x5dea5c,
        (GetUnitPositionDistance)0x65c053,
        (GetUnitRaceCategory)0x65472b,
        (IsGroupSuitableForAiNobleMisfit)0x45c8e5,
        (IsUnitSuitableForAiNobleDuel)0x45c9b3,
        (IsAttackEffectiveAgainstGroup)0x5d269c,
        (IsAttackBetterThanItemUsage)0x452aa9,
        (ComputeAttackDamageCheckAltAttack)0x5d400c,
        (FillAttackTransformIdList)0x5989b2,
        (IsSmallMeleeFighter)0x44c5de,
        (CAiHireUnitEval)0x44c89a,
        (GetMeleeUnitToHireAiRating)0x44cad9,
        (ComputeTargetUnitAiPriority)0x5d00c0,
        (IsPlayerRaceUnplayable)0x5e6643,
        (IsSupportAttackClass)0x631e50,
        (GetUnitAttacks)0x645fe5,
        (GetItemAttack)0x646182,
        (IsUnitUseAdditionalAnimation)0x5c4f1a,
        (CheckUnitForHire)0x4b53e6,
        (CheckUnitForHire)0x4b58d2,
        (CastUnitImplToRacialSoldier)0x4482f6,
        (BuildLordSpecificBuildings)0x422411,
        (AddCapitalBuilding)0x4224ec,
        (CastUnitImplToLeader)0x424a87,
        (GetBaseUnitImplId)0x64ae7f,
        (GetUnitImplDamageMax)0x61f0a7,
        (ReadGlobalAttacks)0x593d63,
        (GetAltAttackIdCheckClass)0x59a965,
        (UnitHasDoppelgangerAttack)0x62a88c,
        (GetDeathAnimationByUnitOrItemId)0x6492e3,
        (OemToCharA)0x6ce410,
        (ThrowScenarioException)0x60b3b9,
        (LoadScenarioMap)0x429dc6,
        (CreateUnitFaceImage)0x5b18e9,
        (CanApplyPotionToUnit)0x5df760,
        (GetUpgradeUnitImplCheckXp)0x5d9e09,
        (ChangeUnitXpCheckUpgrade)0x5d8704,
        (IsUnitTierMax)0x5da04a,
        (IsUnitLevelNotMax)0x5da1bc,
        (IsUnitUpgradePending)0x61fb9d,
        (GetUnitImplIdForIsoUnitImage)0x684101,
        (GetUnitRequiredBuildings)0x61f7fe,
        (ComputeMovementCost)0x603bc6,
    },
    // Russobit
    Functions{
        (RespopupInitFunc)0x4893ad,
        (ToggleShowBannersInitFunc)0x5b4015,
        (AddUnitToHireList)0x5d5bb4,
        (AddSideshowUnitToHireList)0x5d5ab6,
        (AddPlayerUnitsToHireList)0x5d59e8,
        (CreateBuildingType)0x58bf67,
        (AddObjectAndCheckDuplicates)0x59c0a8,
        (ChooseUnitLane)0x5d6abb,
        (GetLordByPlayer)0x5e6323,
        (IsTurnValid)0x5879f3,
        (GetAllyOrEnemyGroupId)0x65afb2,
        (FindUnitById)0x625180,
        (CastUnitImplToSoldier)0x40a79d,
        (CastUnitImplToStackLeader)0x40a7c3,
        (CreateBatAttack)0x64630e,
        (GetAttackById)0x645df6,
        (IsUnitImmuneToAttack)0x65baa8,
        (GetAttackClassWardFlagPosition)0x623e12,
        (AttackClassToString)0x5c7af0,
        (GetAttackSourceWardFlagPosition)0x622dfe,
        (GetStackFortRuinGroup)0x5f6304,
        (GetStackFortRuinGroupForChange)0x5f6349,
        (GetStackFortRuinForChange)0x5f6255,
        (GetStackFortRuinId)0x5eba8e,
        (GetStackIdByUnitId)0x5ebabe,
        (GetFortIdByUnitId)0x5ebbc8,
        (GetRuinIdByUnitId)0x5ebccb,
        (DeletePlayerBuildings)0x422498,
        (GetInterfaceText)0x5c8f38,
        (ComputePlayerDailyIncome)0x5db82a,
        (ComputeDamage)0x65ccc7,
        (ComputeDamageMax)0x61f05c,
        (ComputeDamageWithBuffs)0x5d3e80,
        (ComputeArmor)0x5d3d5e,
        (MarkMapPosition)0x5cdf6f,
        (GetUnitLevelByImplId)0x599581,
        (GetAttackPower)0x628b0a,
        (IsGroupOwnerPlayerHuman)0x628165,
        (AttackShouldMiss)0x626b5f,
        (GenerateRandomNumber)0x548e10,
        (GenerateRandomNumberStd)0x548e67,
        (GetUnitPositionInGroup)0x65fb1e,
        (GetSummonUnitImplIdByAttack)0x661fe0,
        (GetSummonUnitImplId)0x664b5c,
        (GetAttackImplMagic)0x647561,
        (GetUnitHealAttackNumber)0x6283fb,
        (GetAttackQtyDamageOrHeal)0x61efc4,
        (ComputeUnitEffectiveHpForAi)0x5d0053,
        (ApplyDynUpgradeToAttackData)0x59a76b,
        (ComputeUnitDynUpgrade)0x5998fc,
        (ShowMovementPath)0x4cd533,
        (GetMidgardPlan)0x5f756f,
        (CastUnitImplToNoble)0x41b8cc,
        (GetBlockingPathNearbyStackId)0x5ce204,
        (GetFortOrRuinEntrance)0x4cdb5a,
        (StackCanMoveToPosition)0x4cdae4,
        (IsWaterTileSurroundedByWater)0x5e773e,
        (GetStackPositionById)0x5eecf2,
        (ApplyPercentModifiers)0x577515,
        (GenerateAttackDescription)0x57652b,
        (CreateMenuAnimation)0x4eb465,
        (GetAttackSourceText)0x61e1df,
        (AppendAttackSourceText)0x61e923,
        (IsRaceCategoryUnplayable)0x5e6691,
        (ValidateRaces)0x582ecb,
        (CheckRaceExist)0x582f6f,
        (GetUnitAttackSourceImmunities)0x408b0d,
        (GetSoldierAttackSourceImmunities)0x61e3c8,
        (GetTerrainByAbbreviation)0x5a6fbb,
        (GetTerrainByRace)0x5f105e,
        (GetTerrainByRace)0x5e6449,
        (GetRaceByTerrain)0x5e64c4,
        (GetRaceByTerrain)0x5db0ab,
        (GetNumberByTerrainGround)0x5cbf2f,
        (ThrowGenericException)0x596690,
        (IgnorePlayerEvents)0x5e66ec,
        (GetRacePreviewImage)0x4f1c4c,
        (GetSoldierImmunityAiRating)0x5de4e6,
        (GetAttackClassAiRating)0x5de83f,
        (GetAttackReachAiRating)0x5dea5c,
        (GetUnitPositionDistance)0x65c053,
        (GetUnitRaceCategory)0x65472b,
        (IsGroupSuitableForAiNobleMisfit)0x45c8e5,
        (IsUnitSuitableForAiNobleDuel)0x45c9b3,
        (IsAttackEffectiveAgainstGroup)0x5d269c,
        (IsAttackBetterThanItemUsage)0x452aa9,
        (ComputeAttackDamageCheckAltAttack)0x5d400c,
        (FillAttackTransformIdList)0x5989b2,
        (IsSmallMeleeFighter)0x44c5de,
        (CAiHireUnitEval)0x44c89a,
        (GetMeleeUnitToHireAiRating)0x44cad9,
        (ComputeTargetUnitAiPriority)0x5d00c0,
        (IsPlayerRaceUnplayable)0x5e6643,
        (IsSupportAttackClass)0x631e50,
        (GetUnitAttacks)0x645fe5,
        (GetItemAttack)0x646182,
        (IsUnitUseAdditionalAnimation)0x5c4f1a,
        (CheckUnitForHire)0x4b53e6,
        (CheckUnitForHire)0x4b58d2,
        (CastUnitImplToRacialSoldier)0x4482f6,
        (BuildLordSpecificBuildings)0x422411,
        (AddCapitalBuilding)0x4224ec,
        (CastUnitImplToLeader)0x424a87,
        (GetBaseUnitImplId)0x64ae7f,
        (GetUnitImplDamageMax)0x61f0a7,
        (ReadGlobalAttacks)0x593d63,
        (GetAltAttackIdCheckClass)0x59a965,
        (UnitHasDoppelgangerAttack)0x62a88c,
        (GetDeathAnimationByUnitOrItemId)0x6492e3,
        (OemToCharA)0x6ce410,
        (ThrowScenarioException)0x60b3b9,
        (LoadScenarioMap)0x429dc6,
        (CreateUnitFaceImage)0x5b18e9,
        (CanApplyPotionToUnit)0x5df760,
        (GetUpgradeUnitImplCheckXp)0x5d9e09,
        (ChangeUnitXpCheckUpgrade)0x5d8704,
        (IsUnitTierMax)0x5da04a,
        (IsUnitLevelNotMax)0x5da1bc,
        (IsUnitUpgradePending)0x61fb9d,
        (GetUnitImplIdForIsoUnitImage)0x684101,
        (GetUnitRequiredBuildings)0x61f7fe,
        (ComputeMovementCost)0x603bc6,
    },
    // Gog
    Functions{
        (RespopupInitFunc)0x488f96,
        (ToggleShowBannersInitFunc)0x5b32db,
        (AddUnitToHireList)0x5d4add,
        (AddSideshowUnitToHireList)0x5d49df,
        (AddPlayerUnitsToHireList)0x5d4911,
        (CreateBuildingType)0x58b0c2,
        (AddObjectAndCheckDuplicates)0x59b275,
        (ChooseUnitLane)0x5d59e4,
        (GetLordByPlayer)0x5e5038,
        (IsTurnValid)0x586ba8,
        (GetAllyOrEnemyGroupId)0x659a32,
        (FindUnitById)0x623cc0,
        (CastUnitImplToSoldier)0x40a3f9,
        (CastUnitImplToStackLeader)0x40a41f,
        (CreateBatAttack)0x644b3e,
        (GetAttackById)0x644626,
        (IsUnitImmuneToAttack)0x65a528,
        (GetAttackClassWardFlagPosition)0x6229a2,
        (AttackClassToString)0x5c6ad9,
        (GetAttackSourceWardFlagPosition)0x62198e,
        (GetStackFortRuinGroup)0x5f4f87,
        (GetStackFortRuinGroupForChange)0x5f4fcc,
        (GetStackFortRuinForChange)0x5f4ed8,
        (GetStackFortRuinId)0x5ea791,
        (GetStackIdByUnitId)0x5ea7c1,
        (GetFortIdByUnitId)0x5ea8cb,
        (GetRuinIdByUnitId)0x5ea9ce,
        (DeletePlayerBuildings)0x421fb6,
        (GetInterfaceText)0x5c7f06,
        (ComputePlayerDailyIncome)0x5da55f,
        (ComputeDamage)0x65b747,
        (ComputeDamageMax)0x61db98,
        (ComputeDamageWithBuffs)0x5d2db1,
        (ComputeArmor)0x5d2c8f,
        (MarkMapPosition)0x5cce8b,
        (GetUnitLevelByImplId)0x59870b,
        (GetAttackPower)0x62764a,
        (IsGroupOwnerPlayerHuman)0x626ca5,
        (AttackShouldMiss)0x62569f,
        (GenerateRandomNumber)0x54851f,
        (GenerateRandomNumberStd)0x548576,
        (GetUnitPositionInGroup)0x65e59e,
        (GetSummonUnitImplIdByAttack)0x660a60,
        (GetSummonUnitImplId)0x6635dc,
        (GetAttackImplMagic)0x645d91,
        (GetUnitHealAttackNumber)0x626f3b,
        (GetAttackQtyDamageOrHeal)0x61db00,
        (ComputeUnitEffectiveHpForAi)0x5cef84,
        (ApplyDynUpgradeToAttackData)0x5998f5,
        (ComputeUnitDynUpgrade)0x598a86,
        (ShowMovementPath)0x4ccc2c,
        (GetMidgardPlan)0x5f61f2,
        (CastUnitImplToNoble)0x41b399,
        (GetBlockingPathNearbyStackId)0x5cd11f,
        (GetFortOrRuinEntrance)0x4cd253,
        (StackCanMoveToPosition)0x4cd1dd,
        (IsWaterTileSurroundedByWater)0x5e6453,
        (GetStackPositionById)0x5ed9b2,
        (ApplyPercentModifiers)0x576b6a,
        (GenerateAttackDescription)0x575b80,
        (CreateMenuAnimation)0x4ea917,
        (GetAttackSourceText)0x61cd1b,
        (AppendAttackSourceText)0x61d45f,
        (IsRaceCategoryUnplayable)0x5e53a6,
        (ValidateRaces)0x582121,
        (CheckRaceExist)0x5821c5,
        (GetUnitAttackSourceImmunities)0x408790,
        (GetSoldierAttackSourceImmunities)0x61cf04,
        (GetTerrainByAbbreviation)0x5a621c,
        (GetTerrainByRace)0x5efd35,
        (GetTerrainByRace)0x5e515e,
        (GetRaceByTerrain)0x5e51d9,
        (GetRaceByTerrain)0x5d9de0,
        (GetNumberByTerrainGround)0x5cae4b,
        (ThrowGenericException)0x5957b5,
        (IgnorePlayerEvents)0x5e5401,
        (GetRacePreviewImage)0x4f1095,
        (GetSoldierImmunityAiRating)0x5dd21b,
        (GetAttackClassAiRating)0x5dd574,
        (GetAttackReachAiRating)0x5dd791,
        (GetUnitPositionDistance)0x65aad3,
        (GetUnitRaceCategory)0x652f1b,
        (IsGroupSuitableForAiNobleMisfit)0x45c1e5,
        (IsUnitSuitableForAiNobleDuel)0x45c2b3,
        (IsAttackEffectiveAgainstGroup)0x5d15cd,
        (IsAttackBetterThanItemUsage)0x452475,
        (ComputeAttackDamageCheckAltAttack)0x5d2f3d,
        (FillAttackTransformIdList)0x597b00,
        (IsSmallMeleeFighter)0x44bff2,
        (CAiHireUnitEval)0x44c2ae,
        (GetMeleeUnitToHireAiRating)0x44c4e7,
        (ComputeTargetUnitAiPriority)0x5ceff1,
        (IsPlayerRaceUnplayable)0x5e5358,
        (IsSupportAttackClass)0x630890,
        (GetUnitAttacks)0x644815,
        (GetItemAttack)0x6449b2,
        (IsUnitUseAdditionalAnimation)0x5c3f03,
        (CheckUnitForHire)0x4b4a7f,
        (CheckUnitForHire)0x4b4f6b,
        (CastUnitImplToRacialSoldier)0x447ee5,
        (BuildLordSpecificBuildings)0x421f2f,
        (AddCapitalBuilding)0x42200a,
        (CastUnitImplToLeader)0x424565,
        (GetBaseUnitImplId)0x6497bf,
        (GetUnitImplDamageMax)0x61dbe3,
        (ReadGlobalAttacks)0x592e47,
        (GetAltAttackIdCheckClass)0x599aef,
        (UnitHasDoppelgangerAttack)0x6293cc,
        (GetDeathAnimationByUnitOrItemId)0x647b63,
        (OemToCharA)0x6cc40c,
        (ThrowScenarioException)0x609e84,
        (LoadScenarioMap)0x4297e3,
        (CreateUnitFaceImage)0x5b0bdd,
        (CanApplyPotionToUnit)0x5de495,
        (GetUpgradeUnitImplCheckXp)0x5d8b17,
        (ChangeUnitXpCheckUpgrade)0x5d7412,
        (IsUnitTierMax)0x5d8d58,
        (IsUnitLevelNotMax)0x5d8eca,
        (IsUnitUpgradePending)0x61e6d9,
        (GetUnitImplIdForIsoUnitImage)0x682a8b,
        (GetUnitRequiredBuildings)0x61e33a,
        (ComputeMovementCost)0x6027f3,
    },
    // Scenario Editor
    Functions{
        (RespopupInitFunc)nullptr,
        (ToggleShowBannersInitFunc)nullptr,
        (AddUnitToHireList)nullptr,
        (AddSideshowUnitToHireList)nullptr,
        (AddPlayerUnitsToHireList)nullptr,
        (CreateBuildingType)0x538b2d,
        (AddObjectAndCheckDuplicates)0x53cd9c,
        (ChooseUnitLane)0x4d8148,
        (GetLordByPlayer)nullptr,
        (IsTurnValid)0x52d22e,
        (GetAllyOrEnemyGroupId)nullptr,
        (FindUnitById)0x526a00,
        (CastUnitImplToSoldier)0x4088f7,
        (CastUnitImplToStackLeader)0x40b6c8,
        (CreateBatAttack)nullptr,
        (GetAttackById)nullptr,
        (IsUnitImmuneToAttack)nullptr,
        (GetAttackClassWardFlagPosition)0x525282,
        (AttackClassToString)nullptr,
        (GetAttackSourceWardFlagPosition)0x52426e,
        (GetStackFortRuinGroup)nullptr,
        (GetStackFortRuinGroupForChange)0x4f0d8e,
        (GetStackFortRuinForChange)0x4f0c9a,
        (GetStackFortRuinId)0x4ee371,
        (GetStackIdByUnitId)0x4ee3a1,
        (GetFortIdByUnitId)0x4ee4ab,
        (GetRuinIdByUnitId)0x4ee5ae,
        (DeletePlayerBuildings)nullptr,
        (GetInterfaceText)0x4d0a02,
        (ComputePlayerDailyIncome)nullptr,
        (ComputeDamage)nullptr,
        (ComputeDamageMax)0x522a74,
        (ComputeDamageWithBuffs)nullptr,
        (ComputeArmor)nullptr,
        (MarkMapPosition)nullptr,
        (GetUnitLevelByImplId)0x54416b,
        (GetAttackPower)nullptr,
        (IsGroupOwnerPlayerHuman)nullptr,
        (AttackShouldMiss)nullptr,
        (GenerateRandomNumber)nullptr,
        (GenerateRandomNumberStd)nullptr,
        (GetUnitPositionInGroup)nullptr,
        (GetSummonUnitImplIdByAttack)nullptr,
        (GetSummonUnitImplId)nullptr,
        (GetAttackImplMagic)nullptr,
        (GetUnitHealAttackNumber)nullptr,
        (GetAttackQtyDamageOrHeal)0x5229dc,
        (ComputeUnitEffectiveHpForAi)nullptr,
        (ApplyDynUpgradeToAttackData)0x545355,
        (ComputeUnitDynUpgrade)0x5444e6,
        (ShowMovementPath)nullptr,
        (GetMidgardPlan)0x4e5752,
        (CastUnitImplToNoble)0x4296d0,
        (GetBlockingPathNearbyStackId)nullptr,
        (GetFortOrRuinEntrance)nullptr,
        (StackCanMoveToPosition)nullptr,
        (IsWaterTileSurroundedByWater)0x4e41cf,
        (GetStackPositionById)0x4f227f,
        (ApplyPercentModifiers)0x4c7fb1,
        (GenerateAttackDescription)0x4c6fc7,
        (CreateMenuAnimation)nullptr,
        (GetAttackSourceText)0x521bf7,
        (AppendAttackSourceText)0x52233b,
        (IsRaceCategoryUnplayable)0x4e1d1a,
        (ValidateRaces)0x528524,
        (CheckRaceExist)0x5285c8,
        (GetUnitAttackSourceImmunities)nullptr,
        (GetSoldierAttackSourceImmunities)0x521de0,
        (GetTerrainByAbbreviation)0x53a5e3,
        (GetTerrainByRace)0x4e1b7a,
        (GetTerrainByRace)0x4e8519,
        (GetRaceByTerrain)nullptr,
        (GetRaceByTerrain)nullptr,
        (GetNumberByTerrainGround)0x4d3cb3,
        (ThrowGenericException)0x536c62,
        (IgnorePlayerEvents)nullptr,
        (GetRacePreviewImage)nullptr,
        (GetSoldierImmunityAiRating)0x4d7666,
        (GetAttackClassAiRating)0x4d79bf,
        (GetAttackReachAiRating)0x4d7bdc,
        (GetUnitPositionDistance)nullptr,
        (GetUnitRaceCategory)nullptr,
        (IsGroupSuitableForAiNobleMisfit)nullptr,
        (IsUnitSuitableForAiNobleDuel)nullptr,
        (IsAttackEffectiveAgainstGroup)nullptr,
        (IsAttackBetterThanItemUsage)nullptr,
        (ComputeAttackDamageCheckAltAttack)nullptr,
        (FillAttackTransformIdList)nullptr,
        (IsSmallMeleeFighter)nullptr,
        (CAiHireUnitEval)nullptr,
        (GetMeleeUnitToHireAiRating)nullptr,
        (ComputeTargetUnitAiPriority)nullptr,
        (IsPlayerRaceUnplayable)0x4e1ccc,
        (IsSupportAttackClass)nullptr,
        (GetUnitAttacks)nullptr,
        (GetItemAttack)nullptr,
        (IsUnitUseAdditionalAnimation)0x564ae7,
        (CheckUnitForHire)nullptr,
        (CheckUnitForHire)nullptr,
        (CastUnitImplToRacialSoldier)0x4cfc0a,
        (BuildLordSpecificBuildings)nullptr,
        (AddCapitalBuilding)nullptr,
        (CastUnitImplToLeader)0x4453c3,
        (GetBaseUnitImplId)nullptr,
        (GetUnitImplDamageMax)0x522abf,
        (ReadGlobalAttacks)0x537e05,
        (GetAltAttackIdCheckClass)0x54554f,
        (UnitHasDoppelgangerAttack)nullptr,
        (GetDeathAnimationByUnitOrItemId)nullptr,
        (OemToCharA)0x5ca344,
        (ThrowScenarioException)0x506057,
        (LoadScenarioMap)nullptr,
        (CreateUnitFaceImage)0x558cd6,
        (CanApplyPotionToUnit)nullptr,
        (GetUpgradeUnitImplCheckXp)nullptr,
        (ChangeUnitXpCheckUpgrade)nullptr,
        (IsUnitTierMax)0x4d585b,
        (IsUnitLevelNotMax)0x4d59cd,
        (IsUnitUpgradePending)0x523370,
        (GetUnitImplIdForIsoUnitImage)0x59acea,
        (GetUnitRequiredBuildings)0x52314a,
        (ComputeMovementCost)nullptr,
    },
}};

static std::array<Variables, 4> variables = {{
    // Akella
    Variables{
        (int*)0x837ac8,
        (unsigned char*)0x837acc
    },
    // Russobit
    Variables{
        (int*)0x837ac8,
        (unsigned char*)0x837acc
    },
    // Gog
    Variables{
        (int*)0x835a78,
        (unsigned char*)0x835a7c
    },
    // Scenario Editor
    Variables{
        nullptr,
        nullptr
    },
}};
// clang-format on

Functions& gameFunctions()
{
    return functions[static_cast<int>(hooks::gameVersion())];
}

Variables& gameVariables()
{
    return variables[static_cast<int>(hooks::gameVersion())];
}

} // namespace game
