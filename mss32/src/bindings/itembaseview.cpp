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

#include "itembaseview.h"
#include "currencyview.h"
#include "itembase.h"
#include "itemcategory.h"
#include "unitimplview.h"
#include "unitutils.h"
#include "usunitimpl.h"
#include <sol/sol.hpp>

namespace bindings {

ItemBaseView::ItemBaseView(const game::CItemBase* item, const game::IMidgardObjectMap* objectMap)
    : item{item}
    , objectMap{objectMap}
{ }

void ItemBaseView::bind(sol::state& lua)
{
    auto view = lua.new_usertype<ItemBaseView>("ItemBase");
    view["category"] = sol::property(&getCategory);
    view["name"] = sol::property(&getName);
    view["description"] = sol::property(&getDescription);
    view["value"] = sol::property(&getValue);
    view["unitImpl"] = sol::property(&getUnitImpl);
}

int ItemBaseView::getCategory() const
{
    auto category = item ? item->vftable->getCategory(item) : nullptr;
    return category ? (int)category->id : -1;
}

std::string ItemBaseView::getName() const
{
    return item ? item->vftable->getName(item) : "";
}

std::string ItemBaseView::getDescription() const
{
    return item ? item->vftable->getDescription(item) : "";
}

CurrencyView ItemBaseView::getValue() const
{
    using namespace game;

    Bank bank{};
    if (item)
        bank = *item->vftable->getValue(item);

    return CurrencyView{bank};
}

std::optional<UnitImplView> ItemBaseView::getUnitImpl() const
{
    using namespace game;

    auto unitImplId = item ? item->data->unitId : emptyId;
    if (unitImplId == emptyId)
        return std::nullopt;

    auto globalUnitImpl = hooks::getGlobalUnitImpl(&unitImplId);
    if (!globalUnitImpl)
        return std::nullopt;

    return {UnitImplView(globalUnitImpl)};
}

} // namespace bindings
