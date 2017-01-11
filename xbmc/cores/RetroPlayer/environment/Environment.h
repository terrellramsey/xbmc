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
#pragma once

#include "threads/Thread.h"

//#include <atomic>
#include <stdint.h>

namespace RETROPLAYER
{
  class CAction;
  class CGoal;
  class CReward;
  class CState;

  class CEnvironment : private CThread
  {
  public:
    CEnvironment();

    virtual ~CEnvironment() { Destroy(); }

    bool Create();
    void Destroy();

    bool GetState(uint64_t timestamp, CState& state);
    bool GetReward(uint64_t timestamp, CReward& reward);
    bool GetGoal(uint64_t timestamp, CGoal& goal);
    bool GetAction(uint64_t timestamp, CAction& action);

    //void Seek(int64_t relativeFrameCount) { m_relativeFrameSeek = relativeFrameCount; }

  private:
    // implementation of CThread
    virtual void Process() override;

    static void UpdateState( uint64_t t,       CState& state, const CReward& reward, const CGoal& goal, const CAction& action);
    static void UpdateReward(uint64_t t, const CState& state,       CReward& reward, const CGoal& goal, const CAction& action);
    static void UpdateGoal(  uint64_t t, const CState& state, const CReward& reward,       CGoal& goal, const CAction& action);
    static void UpdateAction(uint64_t t, const CState& state, const CReward& reward, const CGoal& goal,       CAction& action);

    std::unique_ptr<CState> m_state;
    std::unique_ptr<CReward> m_reward;
    std::unique_ptr<CGoal> m_goal;
    std::unique_ptr<CAction> m_action;
  };
}
