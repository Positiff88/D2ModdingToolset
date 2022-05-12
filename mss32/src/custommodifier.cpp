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

#include "custommodifier.h"
#include "attackimpl.h"
#include "attackutils.h"
#include "customattacks.h"
#include "customattackutils.h"
#include "custommodifierfunctions.h"
#include "deathanimcat.h"
#include "dynamiccast.h"
#include "game.h"
#include "groundcat.h"
#include "immunecat.h"
#include "leaderabilitycat.h"
#include "mempool.h"
#include "midunit.h"
#include "restrictions.h"
#include "unitcat.h"
#include "unitutils.h"
#include "ussoldierimpl.h"
#include "utils.h"
#include <mutex>
#include <set>

namespace hooks {

#define GET_VALUE(_FUNC_, _PREV_) getValue(functions->##_FUNC_, #_FUNC_, _PREV_)
#define THIZ_GET_VALUE(_FUNC_, _PREV_) thiz->getValue(thiz->functions->##_FUNC_, #_FUNC_, _PREV_)
#define THIZ_GET_VALUE_AS(_FUNC_, _PREV_)                                                          \
    thiz->getValueAs(thiz->functions->##_FUNC_, #_FUNC_, _PREV_)
#define THIZ_GET_VALUE_PARAM(_FUNC_, _PARAM_, _PREV_)                                              \
    thiz->getValueParam(thiz->functions->##_FUNC_, #_FUNC_, _PARAM_, _PREV_)
#define THIZ_GET_VALUE_NO_PARAM(_FUNC_, _DEF_)                                                     \
    thiz->getValueNoParam(thiz->functions->##_FUNC_, #_FUNC_, _DEF_)

static struct
{
    game::RttiInfo<game::IUsUnitVftable> usUnit;
    game::RttiInfo<game::IUsSoldierVftable> usSoldier;
    game::RttiInfo<game::CUmModifierVftable> umModifier;
    game::RttiInfo<game::IUsStackLeaderVftable> usStackLeader;
    game::RttiInfo<game::IAttackVftable> attack;
    game::RttiInfo<game::IAttackVftable> attack2;
} rttiInfo;

void initRttiInfo();
void initVftable(CCustomModifier* thisptr);

static inline CCustomModifier* castUnitToCustomModifier(const game::IUsUnit* unit)
{
    return (CCustomModifier*)unit;
}

static inline CCustomModifier* castSoldierToCustomModifier(const game::IUsSoldier* soldier)
{
    if (soldier == nullptr)
        return nullptr;

    return reinterpret_cast<CCustomModifier*>((uintptr_t)soldier
                                              - offsetof(CCustomModifier, usSoldier));
}

CCustomModifier* castModifierToCustomModifier(const game::CUmModifier* modifier)
{
    if (!modifier || modifier->vftable != &rttiInfo.umModifier.vftable)
        return nullptr;

    return reinterpret_cast<CCustomModifier*>((uintptr_t)modifier
                                              - offsetof(CCustomModifier, umModifier));
}

static inline CCustomModifier* castStackLeaderToCustomModifier(
    const game::IUsStackLeader* stackLeader)
{
    if (stackLeader == nullptr)
        return nullptr;

    return reinterpret_cast<CCustomModifier*>((uintptr_t)stackLeader
                                              - offsetof(CCustomModifier, usStackLeader));
}

CCustomModifier* castAttackToCustomModifier(const game::IAttack* attack)
{
    if (attack == nullptr)
        return nullptr;

    if (attack->vftable == &rttiInfo.attack.vftable) {
        return reinterpret_cast<CCustomModifier*>((uintptr_t)attack
                                                  - offsetof(CCustomModifier, attack));
    } else if (attack->vftable == &rttiInfo.attack2.vftable) {
        return reinterpret_cast<CCustomModifier*>((uintptr_t)attack
                                                  - offsetof(CCustomModifier, attack2));
    }

    return nullptr;
}

Ids IdVectorToIds(const game::IdVector* src)
{
    Ids value;
    for (const game::CMidgardID* it = src->bgn; it != src->end; it++) {
        value.push_back(it);
    }

    return value;
}

void IdsToIdVector(const Ids& src, game::IdVector* dst)
{
    using namespace game;

    const auto& idVectorApi = IdVectorApi::get();

    IdVector tmp{};
    idVectorApi.reserve(&tmp, 1);
    for (const auto& id : src) {
        idVectorApi.pushBack(&tmp, &(const CMidgardID&)id);
    }
    idVectorApi.copy(dst, tmp.bgn, tmp.end);
    idVectorApi.destructor(&tmp);
}

const game::IUsUnit* CCustomModifier::getPrev() const
{
    return umModifier.data->prev;
}

const game::IUsSoldier* CCustomModifier::getPrevSoldier() const
{
    return game::gameFunctions().castUnitImplToSoldier(getPrev());
}

const game::IUsStackLeader* CCustomModifier::getPrevStackLeader() const
{
    return game::gameFunctions().castUnitImplToStackLeader(getPrev());
}

const game::IAttack* CCustomModifier::getPrevAttack(const game::IAttack* thisptr) const
{
    if (thisptr == &attack) {
        return prevAttack;
    } else if (thisptr == &attack2) {
        return prevAttack2;
    }

    return nullptr;
}

const CCustomModifier* CCustomModifier::getPrevCustomModifier() const
{
    using namespace game;

    const auto& rtti = RttiApi::rtti();
    const auto dynamicCast = RttiApi::get().dynamicCast;

    auto current = getPrev();
    while (current) {
        auto modifier = (CUmModifier*)dynamicCast(current, 0, rtti.IUsUnitType,
                                                  rtti.CUmModifierType, 0);

        auto customModifier = castModifierToCustomModifier(modifier);
        if (customModifier)
            return customModifier;

        current = modifier ? modifier->data->prev : nullptr;
    }

    return nullptr;
}

const game::IAttack* CCustomModifier::getAttackToWrap(bool primary) const
{
    auto prev = getPrevSoldier();

    auto prevValue = hooks::getAttack(prev, primary, false);

    bindings::IdView prevId{prevValue ? prevValue->id : game::emptyId};
    auto value = primary ? GET_VALUE(getAttackId, prevId) : GET_VALUE(getAttack2Id, prevId);
    if (value != prevId) {
        auto attack = getGlobalAttack(&value.id);
        if (attack)
            return attack;
    }

    return prevValue;
}

game::IAttack* CCustomModifier::wrap(const game::IAttack* prev, bool primary)
{
    if (primary) {
        attack.id = prev->id;
        prevAttack = prev;
        return &attack;
    } else {
        attack2.id = prev->id;
        prevAttack2 = prev;
        return &attack2;
    }
}

CustomAttackData CCustomModifier::getCustomAttackData(const game::IAttack* thisptr) const
{
    auto prev = getPrevAttack(thisptr);

    auto value = hooks::getCustomAttackData(prev);
    if (thisptr == &attack) {
        value.damageRatio = GET_VALUE(getAttackDamRatio, value.damageRatio);
        value.damageRatioPerTarget = GET_VALUE(getAttackDrRepeat, value.damageRatioPerTarget);
        value.damageSplit = GET_VALUE(getAttackDrSplit, value.damageSplit);
        value.critDamage = GET_VALUE(getAttackCritDamage, value.critDamage);
        value.critPower = std::clamp(GET_VALUE(getAttackCritPower, value.critPower), (uint8_t)0,
                                     (uint8_t)100);
    } else if (thisptr == &attack2) {
        value.damageRatio = GET_VALUE(getAttack2DamRatio, value.damageRatio);
        value.damageRatioPerTarget = GET_VALUE(getAttack2DrRepeat, value.damageRatioPerTarget);
        value.damageSplit = GET_VALUE(getAttack2DrSplit, value.damageSplit);
        value.critDamage = GET_VALUE(getAttack2CritDamage, value.critDamage);
        value.critPower = std::clamp(GET_VALUE(getAttack2CritPower, value.critPower), (uint8_t)0,
                                     (uint8_t)100);
    }

    return value;
}

void CCustomModifier::setUnit(const game::CMidUnit* value)
{
    unit = value;
}

const char* CCustomModifier::getFormattedGlobalText(const game::CMidgardID& formatId,
                                                    const game::CMidgardID& baseId) const
{
    static std::set<std::string> globals;
    static std::mutex globalsMutex;

    auto format = getGlobalText(formatId);
    auto base = getGlobalText(baseId);

    std::string formatted = format;
    if (!replace(formatted, "%BASE%", base))
        return format;

    // No iterators or references are invalidated on insert
    // so it is safe to return raw char pointer.
    const std::lock_guard<std::mutex> lock(globalsMutex);
    return globals.insert(formatted).first->c_str();
}

game::CMidgardID CCustomModifier::getNameTxt() const
{
    auto prev = getPrevCustomModifier();

    bindings::IdView prevValue{prev ? prev->getNameTxt() : getBaseNameTxt()};
    return GET_VALUE(getNameTxt, prevValue);
}

game::CMidgardID CCustomModifier::getBaseNameTxt() const
{
    auto soldierImpl = getSoldierImpl(getPrev());
    return soldierImpl ? soldierImpl->data->name.id : game::invalidId;
}

game::CMidgardID CCustomModifier::getDescTxt() const
{
    auto prev = getPrevCustomModifier();

    bindings::IdView prevValue{prev ? prev->getDescTxt() : getBaseDescTxt()};
    return GET_VALUE(getDescTxt, prevValue);
}

game::CMidgardID CCustomModifier::getBaseDescTxt() const
{
    auto soldierImpl = getSoldierImpl(getPrev());
    return soldierImpl ? soldierImpl->data->description.id : game::invalidId;
}

game::CMidgardID CCustomModifier::getAttackNameTxt(const game::IAttack* thisptr) const
{
    auto prev = getPrevCustomModifier();

    bindings::IdView prevValue{prev ? prev->getAttackNameTxt(thisptr)
                                    : getAttackBaseNameTxt(thisptr)};
    return thisptr == &attack ? GET_VALUE(getAttackNameTxt, prevValue)
                              : GET_VALUE(getAttack2NameTxt, prevValue);
}

game::CMidgardID CCustomModifier::getAttackBaseNameTxt(const game::IAttack* thisptr) const
{
    auto attackImpl = getAttackImpl(thisptr);
    return attackImpl ? attackImpl->data->name.id : game::invalidId;
}

game::CMidgardID CCustomModifier::getAttackDescTxt(const game::IAttack* thisptr) const
{
    auto prev = getPrevCustomModifier();

    bindings::IdView prevValue{prev ? prev->getAttackDescTxt(thisptr)
                                    : getAttackBaseDescTxt(thisptr)};
    return thisptr == &attack ? GET_VALUE(getAttackDescTxt, prevValue)
                              : GET_VALUE(getAttack2DescTxt, prevValue);
}

game::CMidgardID CCustomModifier::getAttackBaseDescTxt(const game::IAttack* thisptr) const
{
    auto attackImpl = getAttackImpl(thisptr);
    return attackImpl ? attackImpl->data->description.id : game::invalidId;
}

CCustomModifier* customModifierCtor(CCustomModifier* thisptr,
                                    const CustomModifierFunctions* functions,
                                    const char* scriptFileName,
                                    const game::CMidgardID* id,
                                    const game::GlobalData** globalData)
{
    using namespace game;

    // Lazy init makes sure that vftable hooks (if any) are already applied
    initRttiInfo();

    thisptr->usUnit.id = emptyId;
    CUmModifierApi::get().constructor(&thisptr->umModifier, id, globalData);

    thisptr->unit = nullptr;
    thisptr->prevAttack = nullptr;
    thisptr->prevAttack2 = nullptr;
    thisptr->lastElementQuery = ModifierElementTypeFlag::None;
    thisptr->functions = functions;
    new (&thisptr->scriptFileName) std::string(scriptFileName);

    thisptr->wards = {};
    IdVectorApi::get().reserve(&thisptr->wards, 1);

    initVftable(thisptr);

    return thisptr;
}

CCustomModifier* customModifierCopyCtor(CCustomModifier* thisptr, const CCustomModifier* src)
{
    using namespace game;

    thisptr->usUnit.id = src->usUnit.id;
    CUmModifierApi::get().copyConstructor(&thisptr->umModifier, &src->umModifier);

    thisptr->unit = src->unit;
    thisptr->lastElementQuery = src->lastElementQuery;
    thisptr->functions = src->functions;
    new (&thisptr->scriptFileName) std::string(src->scriptFileName);

    thisptr->wards = {};
    IdVectorApi::get().reserve(&thisptr->wards, 1);
    IdVectorApi::get().copy(&thisptr->wards, src->wards.bgn, src->wards.end);

    initVftable(thisptr);

    return thisptr;
}

void customModifierDtor(CCustomModifier* thisptr, char flags)
{
    using namespace game;

    thisptr->scriptFileName.~basic_string();

    IdVectorApi::get().destructor(&thisptr->wards);

    CUmModifierApi::get().destructor(&thisptr->umModifier);

    if (flags & 1) {
        Memory::get().freeNonZero(thisptr);
    }
}

void __fastcall unitDtor(game::IUsUnit* thisptr, int /*%edx*/, char flags)
{
    auto thiz = castUnitToCustomModifier(thisptr);
    customModifierDtor(thiz, flags);
}

game::IUsUnitExtension* __fastcall unitCast(const game::IUsUnit* thisptr,
                                            int /*%edx*/,
                                            const char* rawTypeName)
{
    using namespace game;

    const auto& rtti = RttiApi::rtti();
    const auto& rttiApi = RttiApi::get();
    auto& typeInfoRawName = *rttiApi.typeInfoRawName;

    auto thiz = castUnitToCustomModifier(thisptr);
    auto prev = thiz->getPrev();

    if (!strcmp(rawTypeName, typeInfoRawName(rtti.IUsSoldierType))) {
        return (IUsUnitExtension*)&thiz->usSoldier;
    } else if (!strcmp(rawTypeName, typeInfoRawName(rtti.IUsStackLeaderType))) {
        auto prevStackLeader = prev->vftable->cast(prev, rawTypeName);
        return prevStackLeader ? (IUsUnitExtension*)&thiz->usStackLeader : nullptr;
    }

    return prev->vftable->cast(prev, rawTypeName);
}

const game::LUnitCategory* __fastcall unitGetCategory(const game::IUsUnit* thisptr, int /*%edx*/)
{
    auto prev = castUnitToCustomModifier(thisptr)->getPrev();
    return prev->vftable->getCategory(prev);
}

void __fastcall soldierDtor(game::IUsSoldier* thisptr, int /*%edx*/, char flags)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    customModifierDtor(thiz, flags);
}

const char* __fastcall soldierGetName(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    return thiz->getFormattedGlobalText(thiz->getNameTxt(), thiz->getBaseNameTxt());
}

const char* __fastcall soldierGetDescription(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    return thiz->getFormattedGlobalText(thiz->getDescTxt(), thiz->getBaseDescTxt());
}

const game::CMidgardID* __fastcall soldierGetRaceId(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getRaceId(prev);
}

const game::LSubRaceCategory* __fastcall soldierGetSubrace(const game::IUsSoldier* thisptr,
                                                           int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getSubrace(prev);
}

const game::LUnitBranch* __fastcall soldierGetBranch(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getBranch(prev);
}

bool __fastcall soldierGetSizeSmall(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getSizeSmall(prev);
}

bool __fastcall soldierGetSexM(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getSexM(prev);
}

int __fastcall soldierGetLevel(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getLevel(prev);
}

int __fastcall soldierGetHitPoints(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    auto value = THIZ_GET_VALUE(getHitPoint, prev->vftable->getHitPoints(prev));
    return std::clamp(value, restrictions.unitHp->min, restrictions.unitHp->max);
}

int* __fastcall soldierGetArmor(const game::IUsSoldier* thisptr, int /*%edx*/, int* armor)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    auto value = THIZ_GET_VALUE(getArmor, *prev->vftable->getArmor(prev, armor));
    *armor = std::clamp(value, restrictions.unitArmor->min, restrictions.unitArmor->max);
    return armor;
}

const game::CMidgardID* __fastcall soldierGetBaseUnitImplId(const game::IUsSoldier* thisptr,
                                                            int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getBaseUnitImplId(prev);
}

const game::LDeathAnimCategory* __fastcall soldierGetDeathAnim(const game::IUsSoldier* thisptr,
                                                               int /*%edx*/)
{
    using namespace game;

    const auto& annimations = DeathAnimCategories::get();

    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    auto prevValue = prev->vftable->getDeathAnim(prev);
    auto value = THIZ_GET_VALUE(getDeathAnim, (int)prevValue->id);
    switch ((DeathAnimationId)value) {
    case DeathAnimationId::Human:
        return annimations.human;
    case DeathAnimationId::Heretic:
        return annimations.heretic;
    case DeathAnimationId::Dwarf:
        return annimations.dwarf;
    case DeathAnimationId::Undead:
        return annimations.undead;
    case DeathAnimationId::Neutral:
        return annimations.neutral;
    case DeathAnimationId::Dragon:
        return annimations.dragon;
    case DeathAnimationId::Ghost:
        return annimations.ghost;
    case DeathAnimationId::Elf:
        return annimations.elf;
    default:
        return prevValue;
    }
}

int* __fastcall soldierGetRegen(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    auto value = THIZ_GET_VALUE(getRegen, *prev->vftable->getRegen(prev));
    thiz->regen = std::clamp(value, 0, 100);
    return &thiz->regen;
}

int __fastcall soldierGetXpNext(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    return THIZ_GET_VALUE(getXpNext, prev->vftable->getXpNext(prev));
}

int __fastcall soldierGetXpKilled(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    return THIZ_GET_VALUE(getXpKilled, prev->vftable->getXpKilled(prev));
}

const game::LImmuneCat* getImmuneCatById(int categoryId, const game::LImmuneCat* default)
{
    using namespace game;

    const auto& immunities = ImmuneCategories::get();

    switch ((ImmuneId)categoryId) {
    case ImmuneId::Notimmune:
        return immunities.notimmune;
    case ImmuneId::Once:
        return immunities.once;
    case ImmuneId::Always:
        return immunities.always;
    default:
        return default;
    }
}

const game::LImmuneCat* __fastcall soldierGetImmuneByAttackClass(
    const game::IUsSoldier* thisptr,
    int /*%edx*/,
    const game::LAttackClass* attackClass)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    auto prevValue = prev->vftable->getImmuneByAttackClass(prev, attackClass);
    auto value = THIZ_GET_VALUE_PARAM(getImmuneToAttack, (int)attackClass->id, (int)prevValue->id);
    return getImmuneCatById(value, prevValue);
}

const game::LImmuneCat* __fastcall soldierGetImmuneByAttackSource(
    const game::IUsSoldier* thisptr,
    int /*%edx*/,
    const game::LAttackSource* attackSource)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    auto prevValue = prev->vftable->getImmuneByAttackSource(prev, attackSource);
    auto value = THIZ_GET_VALUE_PARAM(getImmuneToSource, (int)attackSource->id, (int)prevValue->id);
    return getImmuneCatById(value, prevValue);
}

const game::IAttack* __fastcall soldierGetAttackById(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getAttackToWrap(true);

    if (*prev->vftable->getAltAttackId(prev) != game::emptyId)
        return prev;

    return thiz->wrap(prev, true);
}

const game::IAttack* __fastcall soldierGetSecondAttackById(const game::IUsSoldier* thisptr,
                                                           int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getAttackToWrap(false);

    return prev ? thiz->wrap(prev, false) : nullptr;
}

bool __fastcall soldierGetAttackTwice(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    return THIZ_GET_VALUE(getAtckTwice, prev->vftable->getAttackTwice(prev));
}

const game::Bank* __fastcall soldierGetEnrollCost(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    bindings::CurrencyView prevValue{*prev->vftable->getEnrollCost(prev)};
    auto value = THIZ_GET_VALUE(getEnrollCost, prevValue);

    thiz->enrollCost = value.bank;
    return &thiz->enrollCost;
}

const game::Bank* __fastcall soldierGetReviveCost(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    bindings::CurrencyView prevValue{*prev->vftable->getReviveCost(prev)};
    auto value = THIZ_GET_VALUE(getReviveCost, prevValue);

    thiz->reviveCost = value.bank;
    return &thiz->reviveCost;
}

const game::Bank* __fastcall soldierGetHealCost(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    bindings::CurrencyView prevValue{*prev->vftable->getHealCost(prev)};
    auto value = THIZ_GET_VALUE(getHealCost, prevValue);

    thiz->healCost = value.bank;
    return &thiz->healCost;
}

const game::Bank* __fastcall soldierGetTrainingCost(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto thiz = castSoldierToCustomModifier(thisptr);
    auto prev = thiz->getPrevSoldier();

    bindings::CurrencyView prevValue{*prev->vftable->getTrainingCost(prev)};
    auto value = THIZ_GET_VALUE(getTrainingCost, prevValue);

    thiz->trainingCost = value.bank;
    return &thiz->trainingCost;
}

const game::CMidgardID* __fastcall soldierGetDynUpg1(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getDynUpg1(prev);
}

int __fastcall soldierGetDynUpgLvl(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getDynUpgLvl(prev);
}

const game::CMidgardID* __fastcall soldierGetDynUpg2(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getDynUpg2(prev);
}

bool __fastcall soldierGetWaterOnly(const game::IUsSoldier* thisptr, int /*%edx*/)
{
    auto prev = castSoldierToCustomModifier(thisptr)->getPrevSoldier();
    return prev->vftable->getWaterOnly(prev);
}

void __fastcall modifierDtor(game::CUmModifier* thisptr, int /*%edx*/, char flags)
{
    auto thiz = castModifierToCustomModifier(thisptr);
    customModifierDtor(thiz, flags);
}

game::CUmModifier* __fastcall modifierCopy(const game::CUmModifier* thisptr, int /*%edx*/)
{
    using namespace game;

    auto thiz = castModifierToCustomModifier(thisptr);

    auto copy = (CCustomModifier*)Memory::get().allocate(sizeof(CCustomModifier));
    customModifierCopyCtor(copy, thiz);
    return &copy->umModifier;
}

bool __fastcall modifierCanApplyWithLeadership(const game::CUmModifier* thisptr,
                                               int /*%edx*/,
                                               const int* leadership)
{
    return true;
}

bool __fastcall modifierCanApplyToUnit(const game::CUmModifier* thisptr,
                                       int /*%edx*/,
                                       const game::IUsUnit* unit)
{
    auto thiz = castModifierToCustomModifier(thisptr);

    auto f = thiz->functions->canApplyToUnit;
    try {
        if (f) {
            bindings::UnitImplView unitImplView{unit};
            return (*f)(unitImplView);
        }
    } catch (const std::exception& e) {
        showErrorMessageBox(fmt::format("Failed to run '{:s}' script.\n"
                                        "Function: 'canApplyToUnit'\n"
                                        "Reason: '{:s}'",
                                        thiz->scriptFileName, e.what()));
    }

    return true;
}

bool __fastcall modifierCanApplyToUnitCategory(const game::CUmModifier* thisptr,
                                               int /*%edx*/,
                                               const game::LUnitCategory* unitCategory)
{
    auto thiz = castModifierToCustomModifier(thisptr);

    auto f = thiz->functions->canApplyToUnitType;
    try {
        if (f) {
            return (*f)((int)unitCategory->id);
        }
    } catch (const std::exception& e) {
        showErrorMessageBox(fmt::format("Failed to run '{:s}' script.\n"
                                        "Function: 'canApplyToUnitType'\n"
                                        "Reason: '{:s}'",
                                        thiz->scriptFileName, e.what()));
    }

    return true;
}

bool __fastcall modifierIsLower(const game::CUmModifier* thisptr, int /*%edx*/)
{
    auto thiz = castModifierToCustomModifier(thisptr);
    return THIZ_GET_VALUE_NO_PARAM(canApplyAsLowerSpell, true);
}

bool __fastcall modifierIsBoost(const game::CUmModifier* thisptr, int /*%edx*/)
{
    auto thiz = castModifierToCustomModifier(thisptr);
    return THIZ_GET_VALUE_NO_PARAM(canApplyAsBoostSpell, true);
}

bool __fastcall modifierHasElement(const game::CUmModifier* thisptr,
                                   int /*%edx*/,
                                   game::ModifierElementTypeFlag type)
{
    using namespace game;

    auto thiz = castModifierToCustomModifier(thisptr);

    bool result = false;
    thiz->lastElementQuery = result ? type : ModifierElementTypeFlag::None;
    return result;
}

int __fastcall modifierGetFirstElementValue(const game::CUmModifier* thisptr, int /*%edx*/)
{
    using namespace game;

    auto thiz = castModifierToCustomModifier(thisptr);

    // The values are used to check if this modifier can be applied, to apply spell effects, etc.
    // Custom modifiers do not have defined element values.
    // The goal here is to provide universal static values to pass most of the checks,
    // while having neutral effect if the values are applied directly.
    switch (thiz->lastElementQuery) {
    case ModifierElementTypeFlag::Leadership:
        // Used by GetCombinedLeadership (Akella 0x4a7d6d) to check if Leadership leader upgrade can
        // be offered.
        return 0;
    case ModifierElementTypeFlag::MoveAbility:
        // Used by CanUseItem (Akella 0x5ed7cf) to check if leader already has this movement bonus.
        return emptyCategoryId; // game::GroundId
    case ModifierElementTypeFlag::LeaderAbility:
        // Used by CanUseItem (Akella 0x5ed7cf) to check if leader already has this ability.
        return emptyCategoryId; // game::LeaderAbilityId
    case ModifierElementTypeFlag::QtyDamage:
    case ModifierElementTypeFlag::Power:
    case ModifierElementTypeFlag::Initiative:
        // Used by ApplyPercentModifiers (Akella 0x577515) to exclude editor modifiers when
        // calculating bonus damage/power/ini in GenerateAttackDescription (Akella 0x57652b).
        return 0;
    case ModifierElementTypeFlag::Hp:
        // Used by CEffectModifierApply (Akella 0x439cd1) to apply Gallean's Boon G000UM7545.
        return 0;
    case ModifierElementTypeFlag::Armor:
        // Used by ApplyFortificationArmor (Akella 0x4212a2).
        // Used by ApplyAbsoluteModifiers (Akella 0x57646e) to exclude editor modifiers when
        // calculating bonus armor in GenerateUnitDescription (Akella 0x5757eb).
        return 0;
    case ModifierElementTypeFlag::ImmunityAlways:
    case ModifierElementTypeFlag::ImmunityOnce:
        // Used by CanUseItem (Akella 0x5ed7cf) to check if unit already immune to attack source.
        return emptyCategoryId; // game::AttackSourceId
    case ModifierElementTypeFlag::ImmunityclassAlways:
    case ModifierElementTypeFlag::ImmunityclassOnce:
        // Used by CanUseItem (Akella 0x5ed7cf) to check if unit already immune to attack class.
        return emptyCategoryId; // game::AttackClassId
    case ModifierElementTypeFlag::MoveAllowance:
    case ModifierElementTypeFlag::ScoutingRange:
    case ModifierElementTypeFlag::Regeneration:
    case ModifierElementTypeFlag::AttackDrain:
    default:
        return 0;
    }
}

const char* __fastcall modifierGetDescription(const game::CUmModifier* thisptr, int /*%edx*/)
{
    using namespace game;

    auto thiz = castModifierToCustomModifier(thisptr);

    CMidgardID id = THIZ_GET_VALUE_NO_PARAM(getModifierDescTxt, bindings::IdView(emptyId));
    return getGlobalText(id);
}

void __fastcall modifierUpdateUnitImplId(game::CUmModifier* thisptr, int /*%edx*/)
{
    auto thiz = castModifierToCustomModifier(thisptr);
    thiz->usUnit.id = thiz->getPrev()->id;
}

void __fastcall stackLeaderDtor(game::IUsStackLeader* thisptr, int /*%edx*/, char flags)
{
    auto thiz = castStackLeaderToCustomModifier(thisptr);
    customModifierDtor(thiz, flags);
}

int __fastcall stackLeaderGetMovement(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    auto value = THIZ_GET_VALUE(getMovement, prev->vftable->getMovement(prev));
    return std::clamp(value, restrictions.stackMovement->min, restrictions.stackMovement->max);
}

const char* __fastcall stackLeaderGetAbilityName(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    auto prev = castStackLeaderToCustomModifier(thisptr)->getPrevStackLeader();
    return prev->vftable->getAbilityName(prev);
}

bool __fastcall stackLeaderHasMovementBonus(const game::IUsStackLeader* thisptr,
                                            int /*%edx*/,
                                            const game::LGroundCategory* ground)
{
    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    return THIZ_GET_VALUE_PARAM(hasMovementBonus, (int)ground->id,
                                prev->vftable->hasMovementBonus(prev, ground));
}

int __fastcall stackLeaderGetScout(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    auto value = THIZ_GET_VALUE(getScout, prev->vftable->getScout(prev));
    return std::clamp(value, restrictions.stackScoutRange->min, restrictions.stackScoutRange->max);
}

int __fastcall stackLeaderGetLeadership(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    auto value = THIZ_GET_VALUE(getLeadership, prev->vftable->getLeadership(prev));
    return std::clamp(value, restrictions.stackLeadership->min, restrictions.stackLeadership->max);
}

int __fastcall stackLeaderGetNegotiate(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    return THIZ_GET_VALUE(getNegotiate, prev->vftable->getNegotiate(prev));
}

bool __fastcall stackLeaderHasAbility(const game::IUsStackLeader* thisptr,
                                      int /*%edx*/,
                                      const game::LLeaderAbility* ability)
{
    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    return THIZ_GET_VALUE_PARAM(hasAbility, (int)ability->id,
                                prev->vftable->hasAbility(prev, ability));
}

bool __fastcall stackLeaderGetFastRetreat(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    return THIZ_GET_VALUE(getFastRetreat, prev->vftable->getFastRetreat(prev));
}

int __fastcall stackLeaderGetLowerCost(const game::IUsStackLeader* thisptr, int /*%edx*/)
{
    auto thiz = castStackLeaderToCustomModifier(thisptr);
    auto prev = thiz->getPrevStackLeader();

    auto value = THIZ_GET_VALUE(getLowerCost, prev->vftable->getLowerCost(prev));
    return std::clamp(value, 0, 100);
}

void __fastcall attackDtor(game::IAttack* thisptr, int /*%edx*/, char flags)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    customModifierDtor(thiz, flags);
}

const char* __fastcall attackGetName(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    return thiz->getFormattedGlobalText(thiz->getAttackNameTxt(thisptr),
                                        thiz->getAttackBaseNameTxt(thisptr));
}

const char* __fastcall attackGetDescription(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    return thiz->getFormattedGlobalText(thiz->getAttackDescTxt(thisptr),
                                        thiz->getAttackBaseDescTxt(thisptr));
}

const game::LAttackClass* __fastcall attackGetAttackClass(const game::IAttack* thisptr,
                                                          int /*%edx*/)
{
    using namespace game;

    const auto& classes = AttackClassCategories::get();

    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getAttackClass(prev);

    bool primary = thisptr == &thiz->attack;
    auto value = primary ? THIZ_GET_VALUE(getAttackClass, (int)prevValue->id)
                         : THIZ_GET_VALUE(getAttack2Class, (int)prevValue->id);
    switch ((AttackClassId)value) {
    case AttackClassId::Damage:
        return classes.damage;
    case AttackClassId::Drain:
        return classes.drain;
    case AttackClassId::Paralyze:
        return classes.paralyze;
    case AttackClassId::Heal:
        return classes.heal;
    case AttackClassId::Fear:
        return classes.fear;
    case AttackClassId::BoostDamage:
        return classes.boostDamage;
    case AttackClassId::Petrify:
        return classes.petrify;
    case AttackClassId::LowerDamage:
        return classes.lowerDamage;
    case AttackClassId::LowerInitiative:
        return classes.lowerInitiative;
    case AttackClassId::Poison:
        return classes.poison;
    case AttackClassId::Frostbite:
        return classes.frostbite;
    case AttackClassId::Revive:
        return classes.revive;
    case AttackClassId::DrainOverflow:
        return classes.drainOverflow;
    case AttackClassId::Cure:
        return classes.cure;
    case AttackClassId::Summon:
        return classes.summon;
    case AttackClassId::DrainLevel:
        return classes.drainLevel;
    case AttackClassId::GiveAttack:
        return classes.giveAttack;
    case AttackClassId::Doppelganger:
        return classes.doppelganger;
    case AttackClassId::TransformSelf:
        return classes.transformSelf;
    case AttackClassId::TransformOther:
        return classes.transformOther;
    case AttackClassId::Blister:
        return classes.blister;
    case AttackClassId::BestowWards:
        return classes.bestowWards;
    case AttackClassId::Shatter:
        return classes.shatter;
    default:
        return prevValue;
    }
}

const game::LAttackSource* __fastcall attackGetAttackSource(const game::IAttack* thisptr,
                                                            int /*%edx*/)
{
    using namespace game;

    const auto& sources = AttackSourceCategories::get();

    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getAttackSource(prev);

    bool primary = thisptr == &thiz->attack;
    auto value = primary ? THIZ_GET_VALUE(getAttackSource, (int)prevValue->id)
                         : THIZ_GET_VALUE(getAttack2Source, (int)prevValue->id);
    switch ((AttackSourceId)value) {
    case AttackSourceId::Weapon:
        return sources.weapon;
    case AttackSourceId::Mind:
        return sources.mind;
    case AttackSourceId::Life:
        return sources.life;
    case AttackSourceId::Death:
        return sources.death;
    case AttackSourceId::Fire:
        return sources.fire;
    case AttackSourceId::Water:
        return sources.water;
    case AttackSourceId::Earth:
        return sources.earth;
    case AttackSourceId::Air:
        return sources.air;
    default:
        for (const auto& custom : getCustomAttacks().sources) {
            if (custom.source.id == (AttackSourceId)value)
                return &custom.source;
        }
        return prevValue;
    }
}

int __fastcall attackGetInitiative(const game::IAttack* thisptr, int /*%edx*/)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getInitiative(prev);
    if (thisptr != &thiz->attack)
        return prevValue;

    auto value = THIZ_GET_VALUE(getAttackInitiative, prevValue);
    return std::clamp(value, restrictions.attackInitiative->min,
                      restrictions.attackInitiative->max);
}

int* __fastcall attackGetPower(const game::IAttack* thisptr, int /*%edx*/, int* power)
{
    const auto& restrictions = game::gameRestrictions();

    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = *prev->vftable->getPower(prev, power);

    bool primary = thisptr == &thiz->attack;
    auto value = primary ? THIZ_GET_VALUE(getAttackPower, prevValue)
                         : THIZ_GET_VALUE(getAttack2Power, prevValue);
    *power = std::clamp(value, restrictions.attackPower->min, restrictions.attackPower->max);
    return power;
}

const game::LAttackReach* __fastcall attackGetAttackReach(const game::IAttack* thisptr,
                                                          int /*%edx*/)
{
    using namespace game;

    const auto& reaches = AttackReachCategories::get();

    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getAttackReach(prev);
    if (thisptr != &thiz->attack)
        return prevValue;

    auto value = THIZ_GET_VALUE(getAttackReach, (int)prevValue->id);
    switch ((AttackReachId)value) {
    case AttackReachId::All:
        return reaches.all;
    case AttackReachId::Any:
        return reaches.any;
    case AttackReachId::Adjacent:
        return reaches.adjacent;
    default:
        for (const auto& custom : getCustomAttacks().reaches) {
            if (custom.reach.id == (AttackReachId)value)
                return &custom.reach;
        }
        return prevValue;
    }
}

int __fastcall attackGetQtyDamage(const game::IAttack* thisptr, int /*%edx*/)
{
    const auto& restrictions = game::gameRestrictions();
    const auto& fn = game::gameFunctions();

    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getQtyDamage(prev);

    bool primary = thisptr == &thiz->attack;
    auto value = primary ? THIZ_GET_VALUE(getAttackDamage, prevValue)
                         : THIZ_GET_VALUE(getAttack2Damage, prevValue);
    return std::clamp(value, restrictions.unitDamage->min,
                      fn.getUnitImplDamageMax(&thiz->unit->unitImpl->id));
}

int __fastcall attackGetQtyHeal(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getQtyHeal(prev);

    bool primary = thisptr == &thiz->attack;
    return primary ? THIZ_GET_VALUE(getAttackHeal, prevValue)
                   : THIZ_GET_VALUE(getAttack2Heal, prevValue);
}

int __fastcall attackGetDrain(const game::IAttack* thisptr, int /*%edx*/, int damage)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getDrain(prev, damage);

    bool primary = thisptr == &thiz->attack;
    return primary ? THIZ_GET_VALUE_PARAM(getAttackDrain, damage, prevValue)
                   : THIZ_GET_VALUE_PARAM(getAttack2Drain, damage, prevValue);
}

int __fastcall attackGetLevel(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getLevel(prev);

    bool primary = thisptr == &thiz->attack;
    return primary ? THIZ_GET_VALUE(getAttackLevel, prevValue)
                   : THIZ_GET_VALUE(getAttack2Level, prevValue);
}

const game::CMidgardID* __fastcall attackGetAltAttackId(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    if (!attackHasAltAttack(prev->vftable->getAttackClass(prev)))
        return &game::emptyId;

    auto prevValue = prev->vftable->getAltAttackId(prev);
    if (thisptr != &thiz->attack)
        return prevValue;

    bindings::IdView prevId{prevValue};
    thiz->altAttackId = THIZ_GET_VALUE(getAltAttackId, prevId);
    return &thiz->altAttackId;
}

bool __fastcall attackGetInfinite(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getInfinite(prev);

    bool primary = thisptr == &thiz->attack;
    return primary ? THIZ_GET_VALUE(getAttackInfinite, prevValue)
                   : THIZ_GET_VALUE(getAttack2Infinite, prevValue);
}

game::IdVector* __fastcall attackGetWards(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getWards(prev);
    Ids prevIds = IdVectorToIds(prevValue);

    bool primary = thisptr == &thiz->attack;
    auto value = primary ? THIZ_GET_VALUE_AS(getAttackWards, prevIds)
                         : THIZ_GET_VALUE_AS(getAttack2Wards, prevIds);
    if (value == prevIds) {
        return prevValue;
    } else {
        IdsToIdVector(value, &thiz->wards);
        return &thiz->wards;
    }
}

bool __fastcall attackGetCritHit(const game::IAttack* thisptr, int /*%edx*/)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    auto prevValue = prev->vftable->getCritHit(prev);

    bool primary = thisptr == &thiz->attack;
    return primary ? THIZ_GET_VALUE(getAttackCritHit, prevValue)
                   : THIZ_GET_VALUE(getAttack2CritHit, prevValue);
}

void __fastcall attackGetData(const game::IAttack* thisptr, int /*%edx*/, game::CAttackData* value)
{
    auto thiz = castAttackToCustomModifier(thisptr);
    auto prev = thiz->getPrevAttack(thisptr);

    prev->vftable->getData(prev, value);
}

void initUnitRttiInfo()
{
    using namespace game;

    auto& info = rttiInfo.usUnit;
    replaceRttiInfo(info, CUmUnitApi::vftable().usUnit, false);

    auto& vftable = info.vftable;
    vftable.destructor = (IMidObjectVftable::Destructor)&unitDtor;
    vftable.cast = (IUsUnitVftable::Cast)&unitCast;
    vftable.getCategory = (IUsUnitVftable::GetCategory)&unitGetCategory;
}

void initSoldierRttiInfo()
{
    using namespace game;

    auto& info = rttiInfo.usSoldier;
    replaceRttiInfo(info, CUmUnitApi::vftable().usSoldier, false);

    auto& vftable = info.vftable;
    vftable.destructor = (IUsUnitExtensionVftable::Destructor)&soldierDtor;
    vftable.getName = (IUsSoldierVftable::GetCStr)&soldierGetName;
    vftable.getDescription = (IUsSoldierVftable::GetCStr)&soldierGetDescription;
    vftable.getRaceId = (IUsSoldierVftable::GetId)&soldierGetRaceId;
    vftable.getSubrace = (IUsSoldierVftable::GetSubrace)&soldierGetSubrace;
    vftable.getBranch = (IUsSoldierVftable::GetBranch)&soldierGetBranch;
    vftable.getSizeSmall = (IUsSoldierVftable::GetBool)&soldierGetSizeSmall;
    vftable.getSexM = (IUsSoldierVftable::GetBool)&soldierGetSexM;
    vftable.getLevel = (IUsSoldierVftable::GetInt)&soldierGetLevel;
    vftable.getHitPoints = (IUsSoldierVftable::GetInt)&soldierGetHitPoints;
    vftable.getArmor = (IUsSoldierVftable::GetArmor)&soldierGetArmor;
    vftable.getBaseUnitImplId = (IUsSoldierVftable::GetId)&soldierGetBaseUnitImplId;
    vftable.getDeathAnim = (IUsSoldierVftable::GetDeathAnim)&soldierGetDeathAnim;
    vftable.getRegen = (IUsSoldierVftable::GetRegen)&soldierGetRegen;
    vftable.getXpNext = (IUsSoldierVftable::GetInt)&soldierGetXpNext;
    vftable.getXpKilled = (IUsSoldierVftable::GetInt)&soldierGetXpKilled;
    vftable.getImmuneByAttackClass = (IUsSoldierVftable::
                                          GetImmuneByAttackClass)&soldierGetImmuneByAttackClass;
    vftable.getImmuneByAttackSource = (IUsSoldierVftable::
                                           GetImmuneByAttackSource)&soldierGetImmuneByAttackSource;
    vftable.getAttackById = (IUsSoldierVftable::GetAttackById)&soldierGetAttackById;
    vftable.getSecondAttackById = (IUsSoldierVftable::GetAttackById)&soldierGetSecondAttackById;
    vftable.getAttackTwice = (IUsSoldierVftable::GetBool)&soldierGetAttackTwice;
    vftable.getEnrollCost = (IUsSoldierVftable::GetBank)&soldierGetEnrollCost;
    vftable.getReviveCost = (IUsSoldierVftable::GetBank)&soldierGetReviveCost;
    vftable.getHealCost = (IUsSoldierVftable::GetBank)&soldierGetHealCost;
    vftable.getTrainingCost = (IUsSoldierVftable::GetBank)&soldierGetTrainingCost;
    vftable.getDynUpg1 = (IUsSoldierVftable::GetId)&soldierGetDynUpg1;
    vftable.getDynUpgLvl = (IUsSoldierVftable::GetDynUpgLvl)&soldierGetDynUpgLvl;
    vftable.getDynUpg2 = (IUsSoldierVftable::GetId)&soldierGetDynUpg2;
    vftable.getWaterOnly = (IUsSoldierVftable::GetBool)&soldierGetWaterOnly;
}

void initModifierRttiInfo()
{
    using namespace game;

    auto& info = rttiInfo.umModifier;
    replaceRttiInfo(info, CUmUnitApi::vftable().umModifier, false);

    auto& vftable = info.vftable;
    vftable.destructor = (CUmModifierVftable::Destructor)&modifierDtor;
    vftable.copy = (CUmModifierVftable::Copy)&modifierCopy;
    vftable.canApplyWithLeadership = (CUmModifierVftable::
                                          CanApplyWithLeadership)&modifierCanApplyWithLeadership;
    vftable.canApplyToUnit = (CUmModifierVftable::CanApplyToUnit)&modifierCanApplyToUnit;
    vftable.canApplyToUnitCategory = (CUmModifierVftable::
                                          CanApplyToUnitCategory)&modifierCanApplyToUnitCategory;
    vftable.isLower = (CUmModifierVftable::GetBool)&modifierIsLower;
    vftable.isBoost = (CUmModifierVftable::GetBool)&modifierIsBoost;
    vftable.hasElement = (CUmModifierVftable::HasElement)&modifierHasElement;
    vftable.getFirstElementValue = (CUmModifierVftable::
                                        GetFirstElementValue)&modifierGetFirstElementValue;
    vftable.getDescription = (CUmModifierVftable::GetDescription)&modifierGetDescription;
    vftable.updateUnitImplId = (CUmModifierVftable::UpdateUnitImplId)&modifierUpdateUnitImplId;
}

void initStackLeaderRttiInfo()
{
    using namespace game;

    // None of the existing RTTI can be reused as this class has unique offset.
    // Lucky for us, the game is not using IUsStackLeader for dynamic casts so we should be fine
    // with base RTTI. Otherwise, we would need to either patch dynamicCast or create our own RTTI.
    auto& info = rttiInfo.usStackLeader;
    replaceRttiInfo(info, IUsStackLeaderApi::vftable(), false);

    auto& vftable = info.vftable;
    vftable.destructor = (IUsUnitExtensionVftable::Destructor)&stackLeaderDtor;
    vftable.getMovement = (IUsStackLeaderVftable::GetInt)&stackLeaderGetMovement;
    vftable.getAbilityName = (IUsStackLeaderVftable::GetAbilityName)&stackLeaderGetAbilityName;
    vftable
        .hasMovementBonus = (IUsStackLeaderVftable::HasMovementBonus)&stackLeaderHasMovementBonus;
    vftable.getScout = (IUsStackLeaderVftable::GetInt)&stackLeaderGetScout;
    vftable.getLeadership = (IUsStackLeaderVftable::GetInt)&stackLeaderGetLeadership;
    vftable.getNegotiate = (IUsStackLeaderVftable::GetInt)&stackLeaderGetNegotiate;
    vftable.hasAbility = (IUsStackLeaderVftable::HasAbility)&stackLeaderHasAbility;
    vftable.getFastRetreat = (IUsStackLeaderVftable::GetFastRetreat)&stackLeaderGetFastRetreat;
    vftable.getLowerCost = (IUsStackLeaderVftable::GetInt)&stackLeaderGetLowerCost;
}

void initAttackVftable(game::IAttackVftable& vftable)
{
    using namespace game;

    vftable.destructor = (IMidObjectVftable::Destructor)&attackDtor;
    vftable.getName = (IAttackVftable::GetCStr)&attackGetName;
    vftable.getDescription = (IAttackVftable::GetCStr)&attackGetDescription;
    vftable.getAttackClass = (IAttackVftable::GetAttackClass)&attackGetAttackClass;
    vftable.getAttackSource = (IAttackVftable::GetAttackSource)&attackGetAttackSource;
    vftable.getInitiative = (IAttackVftable::GetInitiative)&attackGetInitiative;
    vftable.getPower = (IAttackVftable::GetPower)&attackGetPower;
    vftable.getAttackReach = (IAttackVftable::GetAttackReach)&attackGetAttackReach;
    vftable.getQtyDamage = (IAttackVftable::GetInt)&attackGetQtyDamage;
    vftable.getQtyHeal = (IAttackVftable::GetInt)&attackGetQtyHeal;
    vftable.getDrain = (IAttackVftable::GetDrain)&attackGetDrain;
    vftable.getLevel = (IAttackVftable::GetInt)&attackGetLevel;
    vftable.getAltAttackId = (IAttackVftable::GetId)&attackGetAltAttackId;
    vftable.getInfinite = (IAttackVftable::GetBool)&attackGetInfinite;
    vftable.getWards = (IAttackVftable::GetWards)&attackGetWards;
    vftable.getCritHit = (IAttackVftable::GetBool)&attackGetCritHit;
    vftable.getData = (IAttackVftable::GetData)&attackGetData;
}

void initAttackRttiInfo()
{
    using namespace game;

    // None of the existing RTTI can be reused as this class has unique offset.
    // Lucky for us, the game is not using IAttack for dynamic casts so we should be fine
    // with base RTTI. Otherwise, we would need to either patch dynamicCast or create our own RTTI.
    auto& info = rttiInfo.attack;
    replaceRttiInfo(info, IAttackApi::vftable(), false);
    initAttackVftable(info.vftable);
}

void initAttack2RttiInfo()
{
    using namespace game;

    // None of the existing RTTI can be reused as this class has unique offset.
    // Lucky for us, the game is not using IAttack for dynamic casts so we should be fine
    // with base RTTI. Otherwise, we would need to either patch dynamicCast or create our own RTTI.
    auto& info = rttiInfo.attack2;
    replaceRttiInfo(info, IAttackApi::vftable(), false);
    initAttackVftable(info.vftable);
}

void initRttiInfo()
{
    static bool initialized = false;
    if (!initialized) {
        initUnitRttiInfo();
        initSoldierRttiInfo();
        initModifierRttiInfo();
        initStackLeaderRttiInfo();
        initAttackRttiInfo();
        initAttack2RttiInfo();
        initialized = true;
    }
}

void initVftable(CCustomModifier* thisptr)
{
    thisptr->usUnit.vftable = &rttiInfo.usUnit.vftable;
    thisptr->usSoldier.vftable = &rttiInfo.usSoldier.vftable;
    thisptr->umModifier.vftable = &rttiInfo.umModifier.vftable;
    thisptr->usStackLeader.vftable = &rttiInfo.usStackLeader.vftable;
    thisptr->attack.vftable = &rttiInfo.attack.vftable;
    thisptr->attack2.vftable = &rttiInfo.attack2.vftable;
}

game::CUmModifier* createCustomModifier(const CustomModifierFunctions* functions,
                                        const char* scriptFileName,
                                        const game::CMidgardID* id,
                                        const game::GlobalData** globalData)
{
    using namespace game;

    auto customModifier = (CCustomModifier*)Memory::get().allocate(sizeof(CCustomModifier));
    customModifierCtor(customModifier, functions, scriptFileName, id, globalData);
    return &customModifier->umModifier;
}

} // namespace hooks
