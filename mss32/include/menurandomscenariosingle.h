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

#ifndef MENURANDOMSCENARIOSINGLE_H
#define MENURANDOMSCENARIOSINGLE_H

#include "mapgenerator.h"
#include "maptemplate.h"
#include "maptemplatereader.h"
#include "menubase.h"
#include "uievent.h"
#include <array>
#include <thread>
#include <utility>

namespace game {
struct CButtonInterf;
struct CMenuPhase;
} // namespace game

namespace hooks {

struct WaitGenerationInterf;

enum class GenerationStatus : int
{
    NotStarted,    /**< Random scenario generation has not started yet. */
    InProcess,     /**< Generation is in process, generator thread is running. */
    Canceled,      /**< Generation was canceled by player. */
    Done,          /**< Generation successfully done, scenario can be serialized. */
    LimitExceeded, /**< Generation could not succeed in specified number of attempts. */
    Error,         /**< Generation was aborted with an error. */
};

/** Menu for single-player random scenario generation. */
struct CMenuRandomScenarioSingle : public game::CMenuBase
{
    CMenuRandomScenarioSingle(game::CMenuPhase* menuPhase);
    ~CMenuRandomScenarioSingle();

    game::UiEvent uiEvent{};
    std::thread generatorThread;
    rsg::MapTemplate scenarioTemplate;
    rsg::MapPtr scenario;

    // Tracks which button shows which race image
    using RaceIndices = std::array<std::pair<game::CButtonInterf*, int /* index */>, 4>;
    RaceIndices raceIndices;

    WaitGenerationInterf* popup{};
    GenerationStatus generationStatus{GenerationStatus::NotStarted};
    bool cancelGeneration{false};
};

game::CMenuBase* __stdcall createMenuRandomScenarioSingle(game::CMenuPhase* menuPhase);

} // namespace hooks

#endif // MENURANDOMSCENARIOSINGLE_H
