/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2022 Vladimir Makeev.
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

#ifndef INTERFCURSOR_H
#define INTERFCURSOR_H

namespace game {

struct IInterfCursorVftable;

struct IInterfCursor
{
    IInterfCursorVftable* vftable;
};

// Virtual table does not contain destructor
struct IInterfCursorVftable
{
    void* method1;
    void* method2;
    void* method3;
    void* method4;
};

static_assert(sizeof(IInterfCursorVftable) == 4 * sizeof(void*),
              "IInterfCursor vftable must have exactly 4 methods");

} // namespace game

#endif // INTERFCURSOR_H