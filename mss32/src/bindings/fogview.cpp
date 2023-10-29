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

#include "fogview.h"
#include "midgardmapfog.h"
#include "point.h"
#include <sol/sol.hpp>

namespace bindings {

FogView::FogView(const game::CMidgardMapFog* mapFog)
    : mapFog(mapFog)
{ }

void FogView::bind(sol::state& lua)
{
    auto view = lua.new_usertype<FogView>("FogView");
    view["getFog"] = sol::overload<>(&FogView::getFogByCoordinates, &FogView::getFogByPoint);
}

IdView FogView::getId() const
{
    return IdView{mapFog->id};
}

bool FogView::getFogByCoordinates(int x, int y) const
{
    const game::CMqPoint mapPosition{x, y};

    bool fog = false;
    if (game::CMidgardMapFogApi::get().getFog(mapFog, &fog, &mapPosition)) {
        return fog;
    }

    return false;
}

bool FogView::getFogByPoint(const Point& p) const
{
    return getFogByCoordinates(p.x, p.y);
}

} // namespace bindings
