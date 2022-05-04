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

#ifndef UNITVIEW_H
#define UNITVIEW_H

#include <optional>

namespace sol {
class state;
}

namespace game {
struct IUsUnit;
struct CMidUnit;
} // namespace game

namespace bindings {

class UnitImplView;

class UnitView
{
public:
    UnitView(const game::CMidUnit* unit);
    UnitView(const game::CMidUnit* unit, const game::IUsUnit* unitImpl);

    static void bind(sol::state& lua);

    /** Returns unit current implementation. */
    std::optional<UnitImplView> getImpl() const;
    /** Returns unit base implementation. */
    std::optional<UnitImplView> getBaseImpl() const;

    /** Returns unit's current experience points. */
    int getXp() const;
    int getHp() const;
    int getHpMax() const;

protected:
    const game::IUsUnit* getUnitImpl() const;

    const game::CMidUnit* unit;
    const game::IUsUnit* unitImpl;
};

} // namespace bindings

#endif // UNITVIEW_H
