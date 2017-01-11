/*
 *      Copyright (C) 2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "Environment.h"

using namespace RETROPLAYER;

CState::CState() :
  m_timestamp(0)
{
}

void CState::Update(uint64_t timestamp, const CReward& reward, const CGoal& goal, const CAction& action)
{
  if (timestamp == 0)
  {
    RunFrame();
  }
  else
  {
    if (emulator)
    {
      if (emulator.state.timestamp != timestamp)
        emulator.Deserialize();
      RunFrame();
    }

  }
}

void CState::Initialize()
{

}
