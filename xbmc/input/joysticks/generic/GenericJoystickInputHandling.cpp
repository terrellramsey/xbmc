/*
 *      Copyright (C) 2014-2015 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "GenericJoystickInputHandling.h"
#include "DigitalAnalogButtonConverter.h"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "input/joysticks/IJoystickInputHandler.h"
#include "input/joysticks/JoystickTranslator.h"
#include "utils/log.h"

#include <algorithm>

using namespace JOYSTICK;

CGenericJoystickInputHandling::CGenericJoystickInputHandling(IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap)
 : m_handler(new CDigitalAnalogButtonConverter(handler)),
   m_buttonMap(buttonMap)
{
}

CGenericJoystickInputHandling::~CGenericJoystickInputHandling(void)
{
  delete m_handler;
}

bool CGenericJoystickInputHandling::OnButtonMotion(unsigned int buttonIndex, bool bPressed)
{
  const char pressed = bPressed ? 1 : 0;

  // Ensure buttonIndex will fit in vector
  if (m_buttonStates.size() <= buttonIndex)
    m_buttonStates.resize(buttonIndex + 1);

  bool bHandled = false;

  FeatureName feature;
  if (m_buttonMap->GetFeature(CDriverPrimitive(buttonIndex), feature))
  {
    char& wasPressed = m_buttonStates[buttonIndex];

    if (!wasPressed && pressed)
    {
      bHandled = OnPress(feature);
    }
    else if (wasPressed && !pressed)
    {
      OnRelease(feature);
      bHandled = true;
    }

    wasPressed = pressed;
  }
  else if (bPressed)
  {
    CLog::Log(LOGDEBUG, "Joystick handling: No feature mapped to button %u", buttonIndex);
  }

  return bHandled;
}

bool CGenericJoystickInputHandling::OnHatMotion(unsigned int hatIndex, HAT_STATE newState)
{
  // Ensure hatIndex will fit in vector
  if (m_hatStates.size() <= hatIndex)
    m_hatStates.resize(hatIndex + 1);

  HAT_STATE& oldState = m_hatStates[hatIndex];

  bool bHandled = false;

  bHandled |= ProcessHatDirection(hatIndex, oldState, newState, HAT_DIRECTION::UP);
  bHandled |= ProcessHatDirection(hatIndex, oldState, newState, HAT_DIRECTION::RIGHT);
  bHandled |= ProcessHatDirection(hatIndex, oldState, newState, HAT_DIRECTION::DOWN);
  bHandled |= ProcessHatDirection(hatIndex, oldState, newState, HAT_DIRECTION::LEFT);

  oldState = newState;

  return bHandled;
}

bool CGenericJoystickInputHandling::ProcessHatDirection(int index,
    HAT_STATE oldState, HAT_STATE newState, HAT_DIRECTION targetDir)
{
  bool bHandled = false;

  if (((int)oldState & (int)targetDir) != ((int)newState & (int)targetDir))
  {
    const bool bActivated = ((int)newState & (int)targetDir) != (int)HAT_STATE::UNPRESSED;

    FeatureName feature;
    if (m_buttonMap->GetFeature(CDriverPrimitive(index, targetDir), feature))
    {
      if (bActivated)
      {
        bHandled = OnPress(feature);
      }
      else
      {
        OnRelease(feature);
        bHandled = true;
      }
    }
  }

  return bHandled;
}

bool CGenericJoystickInputHandling::OnAxisMotion(unsigned int axisIndex, float newPosition)
{
  // Ensure axisIndex will fit in vector
  if (m_axisStates.size() <= axisIndex)
    m_axisStates.resize(axisIndex + 1);

  if (m_axisStates[axisIndex] == 0.0f && newPosition == 0.0f)
    return false;

  float oldPosition = m_axisStates[axisIndex];
  m_axisStates[axisIndex] = newPosition;

  CDriverPrimitive positiveAxis(axisIndex, SEMIAXIS_DIRECTION::POSITIVE);
  CDriverPrimitive negativeAxis(axisIndex, SEMIAXIS_DIRECTION::NEGATIVE);

  FeatureName positiveFeature;
  FeatureName negativeFeature;

  bool bHasFeaturePositive = m_buttonMap->GetFeature(positiveAxis, positiveFeature);
  bool bHasFeatureNegative = m_buttonMap->GetFeature(negativeAxis, negativeFeature);

  bool bHandled = false;

  if (bHasFeaturePositive || bHasFeatureNegative)
  {
    bHandled = true;

    // If the positive and negative semiaxis correspond to the same feature,
    // then we must be dealing with an analog stick or accelerometer. These both
    // require multiple axes, so record the axis and batch-process later during
    // ProcessAxisMotions()

    bool bNeedsMoreAxes = (positiveFeature == negativeFeature);

    if (bNeedsMoreAxes)
    {
      if (std::find(m_featuresWithMotion.begin(), m_featuresWithMotion.end(), positiveFeature) == m_featuresWithMotion.end())
        m_featuresWithMotion.push_back(positiveFeature);
    }
    else
    {
      if (bHasFeaturePositive)
      {
        // If new position passes through the origin, 0.0f is sent exactly once
        // until the position becomes positive again
        if (newPosition > 0)
          m_handler->OnButtonMotion(positiveFeature, newPosition);
        else if (oldPosition > 0)
          m_handler->OnButtonMotion(positiveFeature, 0.0f);
      }

      if (bHasFeatureNegative)
      {
        // If new position passes through the origin, 0.0f is sent exactly once
        // until the position becomes negative again
        if (newPosition < 0)
          m_handler->OnButtonMotion(negativeFeature, -1.0f * newPosition); // magnitude is >= 0
        else if (oldPosition < 0)
          m_handler->OnButtonMotion(negativeFeature, 0.0f);
      }
    }
  }

  return bHandled;
}

void CGenericJoystickInputHandling::ProcessAxisMotions(void)
{
  std::vector<FeatureName> featuresToProcess;
  featuresToProcess.swap(m_featuresWithMotion);

  // Invoke callbacks for features with motion
  for (std::vector<FeatureName>::const_iterator it = featuresToProcess.begin(); it != featuresToProcess.end(); ++it)
  {
    const FeatureName& feature = *it;

    CDriverPrimitive up;
    CDriverPrimitive down;
    CDriverPrimitive right;
    CDriverPrimitive left;

    CDriverPrimitive positiveX;
    CDriverPrimitive positiveY;
    CDriverPrimitive positiveZ;

    if (m_buttonMap->GetAnalogStick(feature, up, down, right,  left))
    {
      float horizPos = 0.0f;
      float vertPos = 0.0f;

      if (right.Type() == CDriverPrimitive::SemiAxis)
        horizPos = GetAxisState(right.Index()) * static_cast<int>(right.SemiAxisDirection());

      if (up.Type() == CDriverPrimitive::SemiAxis)
        vertPos  = GetAxisState(up.Index())  * static_cast<int>(up.SemiAxisDirection());

      m_handler->OnAnalogStickMotion(feature, horizPos, vertPos);
    }
    else if (m_buttonMap->GetAccelerometer(feature, positiveX, positiveY, positiveZ))
    {
      float xPos = 0.0f;
      float yPos = 0.0f;
      float zPos = 0.0f;

      if (positiveX.Type() == CDriverPrimitive::SemiAxis)
        xPos = GetAxisState(positiveX.Index()) * static_cast<int>(positiveX.SemiAxisDirection());

      if (positiveY.Type() == CDriverPrimitive::SemiAxis)
        yPos = GetAxisState(positiveY.Index()) * static_cast<int>(positiveY.SemiAxisDirection());

      if (positiveZ.Type() == CDriverPrimitive::SemiAxis)
        zPos = GetAxisState(positiveZ.Index()) * static_cast<int>(positiveZ.SemiAxisDirection());

      m_handler->OnAccelerometerMotion(feature, xPos, yPos, zPos);
    }
  }

  // Digital buttons emulating analog buttons need to be repeated every frame
  for (std::vector<FeatureName>::const_iterator it = m_repeatingFeatures.begin(); it != m_repeatingFeatures.end(); ++it)
    m_handler->OnButtonPress(*it, true);
}

bool CGenericJoystickInputHandling::OnPress(const FeatureName& feature)
{
  bool bHandled = false;

  CLog::Log(LOGDEBUG, "CGenericJoystickInputHandling: %s feature [ %s ] pressed",
            m_handler->ControllerID().c_str(), feature.c_str());

  const INPUT inputType = m_handler->GetInputType(feature);

  if (inputType == INPUT::DIGITAL)
  {
    bHandled = m_handler->OnButtonPress(feature, true);
  }
  else if (inputType == INPUT::ANALOG)
  {
    StartDigitalRepeating(feature); // Analog actions repeat every frame
    bHandled = true;
  }

  return bHandled;
}

void CGenericJoystickInputHandling::OnRelease(const FeatureName& feature)
{
  CLog::Log(LOGDEBUG, "CGenericJoystickInputHandling: %s feature [ %s ] released",
            m_handler->ControllerID().c_str(), feature.c_str());

  m_handler->OnButtonPress(feature, false);
  StopDigitalRepeating(feature);
}

void CGenericJoystickInputHandling::StartDigitalRepeating(const FeatureName& feature)
{
  m_repeatingFeatures.push_back(feature);
}

void CGenericJoystickInputHandling::StopDigitalRepeating(const FeatureName& feature)
{
  m_repeatingFeatures.erase(std::remove(m_repeatingFeatures.begin(), m_repeatingFeatures.end(), feature), m_repeatingFeatures.end());
}

float CGenericJoystickInputHandling::GetAxisState(int axisIndex) const
{
  if (0 <= axisIndex && axisIndex < (int)m_axisStates.size())
    return m_axisStates[axisIndex];

  return 0;
}