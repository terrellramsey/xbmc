/*
 *      Copyright (C) 2014-2017 Team Kodi
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

#include "DefaultJoystick.h"
#include "KeymapHandler.h"
#include "JoystickEasterEgg.h"
#include "JoystickIDs.h"
#include "JoystickTranslator.h"
#include "input/Key.h"
#include "Application.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#define ANALOG_DIGITAL_THRESHOLD   0.5f

using namespace KODI;
using namespace JOYSTICK;

CDefaultJoystick::CDefaultJoystick(void) :
  m_handler(new CKeymapHandler)
{
}

CDefaultJoystick::~CDefaultJoystick(void)
{
  delete m_handler;
}

std::string CDefaultJoystick::ControllerID(void) const
{
  return GetControllerID();
}

bool CDefaultJoystick::HasFeature(const FeatureName& feature) const
{
  if (GetKeyID(feature) != 0)
    return true;

  // Try analog stick directions
  if (GetKeyID(feature, ANALOG_STICK_DIRECTION::UP)    != 0 ||
      GetKeyID(feature, ANALOG_STICK_DIRECTION::DOWN)  != 0 ||
      GetKeyID(feature, ANALOG_STICK_DIRECTION::RIGHT) != 0 ||
      GetKeyID(feature, ANALOG_STICK_DIRECTION::LEFT)  != 0)
    return true;

  return false;
}

bool CDefaultJoystick::AcceptsInput()
{
  return g_application.IsAppFocused();
}

INPUT_TYPE CDefaultJoystick::GetInputType(const FeatureName& feature) const
{
  return m_handler->GetInputType(GetKeyID(feature));
}

bool CDefaultJoystick::OnButtonPress(const FeatureName& feature, bool bPressed)
{
  if (bPressed && m_easterEgg && m_easterEgg->OnButtonPress(feature))
    return true;

  const unsigned int keyId = GetKeyID(feature);

  if (m_handler->GetInputType(keyId) == INPUT_TYPE::DIGITAL)
  {
    m_handler->OnDigitalKey(keyId, bPressed);
    return true;
  }

  return false;
}

void CDefaultJoystick::OnButtonHold(const FeatureName& feature, unsigned int holdTimeMs)
{
  const unsigned int keyId = GetKeyID(feature);
  m_handler->OnDigitalKey(keyId, true, holdTimeMs);
}

bool CDefaultJoystick::OnButtonMotion(const FeatureName& feature, float magnitude)
{
  const unsigned int keyId = GetKeyID(feature);

  if (m_handler->GetInputType(keyId) == INPUT_TYPE::ANALOG)
  {
    m_handler->OnAnalogKey(keyId, magnitude);
    return true;
  }

  return false;
}

bool CDefaultJoystick::OnAnalogStickMotion(const FeatureName& feature, float x, float y, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the direction of the stick's position
  const ANALOG_STICK_DIRECTION analogStickDir = CJoystickTranslator::VectorToAnalogStickDirection(x, y);

  // Calculate the magnitude projected onto that direction
  const float magnitude = std::max(std::abs(x), std::abs(y));

  // Deactivate directions in which the stick is not pointing first
  for (ANALOG_STICK_DIRECTION dir : GetDirections())
  {
    if (dir != analogStickDir)
      DeactivateDirection(feature, dir);
  }

  // Now activate direction the analog stick is pointing
  if (magnitude != 0.0f)
    bHandled = ActivateDirection(feature, magnitude, analogStickDir, motionTimeMs);

  return bHandled;
}

bool CDefaultJoystick::OnAccelerometerMotion(const FeatureName& feature, float x, float y, float z)
{
  return false; //! @todo implement
}

int CDefaultJoystick::GetActionID(const FeatureName& feature)
{
  const unsigned int keyId = GetKeyID(feature);

  if (keyId > 0)
    return m_handler->GetActionID(keyId);

  return ACTION_NONE;
}

bool CDefaultJoystick::ActivateDirection(const FeatureName& feature, float magnitude, ANALOG_STICK_DIRECTION dir, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the button key ID and input type for the analog stick's direction
  const unsigned int  keyId     = GetKeyID(feature, dir);
  const INPUT_TYPE    inputType = m_handler->GetInputType(keyId);

  if (inputType == INPUT_TYPE::DIGITAL)
  {
    unsigned int holdTimeMs = 0;

    const bool bIsPressed = (magnitude >= ANALOG_DIGITAL_THRESHOLD);
    if (bIsPressed)
    {
      const bool bIsHeld = (m_holdStartTimes.find(keyId) != m_holdStartTimes.end());
      if (bIsHeld)
        holdTimeMs = motionTimeMs - m_holdStartTimes[keyId];
      else
        m_holdStartTimes[keyId] = motionTimeMs;
    }
    else
    {
      m_holdStartTimes.erase(keyId);
    }

    m_handler->OnDigitalKey(keyId, bIsPressed, holdTimeMs);
    bHandled = true;
  }
  else if (inputType == INPUT_TYPE::ANALOG)
  {
    m_handler->OnAnalogKey(keyId, magnitude);
    bHandled = true;
  }

  if (bHandled)
    m_currentDirections[feature] = dir;

  return bHandled;
}

void CDefaultJoystick::DeactivateDirection(const FeatureName& feature, ANALOG_STICK_DIRECTION dir)
{
  if (m_currentDirections[feature] == dir)
  {
    // Calculate the button key ID and input type for this direction
    const unsigned int  keyId     = GetKeyID(feature, dir);
    const INPUT_TYPE    inputType = m_handler->GetInputType(keyId);

    if (inputType == INPUT_TYPE::DIGITAL)
    {
      m_handler->OnDigitalKey(keyId, false);
    }
    else if (inputType == INPUT_TYPE::ANALOG)
    {
      m_handler->OnAnalogKey(keyId, 0.0f);
    }

    m_holdStartTimes.erase(keyId);
    m_currentDirections[feature] = ANALOG_STICK_DIRECTION::UNKNOWN;
  }
}

const std::vector<ANALOG_STICK_DIRECTION>& CDefaultJoystick::GetDirections(void)
{
  static std::vector<ANALOG_STICK_DIRECTION> directions;
  if (directions.empty())
  {
    directions.push_back(ANALOG_STICK_DIRECTION::UP);
    directions.push_back(ANALOG_STICK_DIRECTION::DOWN);
    directions.push_back(ANALOG_STICK_DIRECTION::RIGHT);
    directions.push_back(ANALOG_STICK_DIRECTION::LEFT);
  }
  return directions;
}
