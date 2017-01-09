/*
 *      Copyright (C) 2012-2016 Team Kodi
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

#include "AddonCallbacksGame.h"
#include "cores/AudioEngine/Utils/AEChannelInfo.h"
#include "games/addons/GameClient.h"
#include "guilib/WindowIDs.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "utils/log.h"

#include <string>

using namespace ADDON;
using namespace GAME;

namespace KodiAPI
{

namespace Game
{

CAddonCallbacksGame::CAddonCallbacksGame(CAddon* addon) :
  m_addon(addon),
  m_callbacks(new CB_GameLib)
{
  /* write Kodi game specific add-on function addresses to callback table */
  m_callbacks->CloseGame                      = CloseGame;

  m_callbacks->OpenStream                     = OpenStream;
  m_callbacks->ChangeStreamDetails            = ChangeStreamDetails;
  m_callbacks->AddStreamData                  = AddStreamData;
  m_callbacks->CloseStream                    = CloseStream;
  m_callbacks->EnableHardwareRendering        = EnableHardwareRendering;
  m_callbacks->HwGetCurrentFramebuffer        = HwGetCurrentFramebuffer;
  m_callbacks->HwGetProcAddress               = HwGetProcAddress;
  m_callbacks->RenderFrame                    = RenderFrame;
  m_callbacks->OpenPort                       = OpenPort;
  m_callbacks->ClosePort                      = ClosePort;
  m_callbacks->InputEvent                     = InputEvent;
}

CAddonCallbacksGame::~CAddonCallbacksGame()
{
  /* delete the callback table */
  delete m_callbacks;
}

CGameClient* CAddonCallbacksGame::GetGameClient(void* addonData, const char* strFunction)
{
  CAddonInterfaces* addon = static_cast<CAddonInterfaces*>(addonData);
  if (!addon || !addon->GetHelperGame())
  {
    CLog::Log(LOGERROR, "GAME - %s - called with a null pointer", strFunction);
    return NULL;
  }

  return dynamic_cast<CGameClient*>(static_cast<CAddonCallbacksGame*>(addon->GetHelperGame())->m_addon);
}

void CAddonCallbacksGame::CloseGame(void* addonData)
{
  using namespace KODI::MESSAGING;

  CApplicationMessenger::GetInstance().PostMsg(TMSG_GUI_ACTION, WINDOW_INVALID, -1, static_cast<void*>(new CAction(ACTION_STOP)));
}

game_stream_handle* CAddonCallbacksGame::OpenStream(void* addonData, const game_stream_details& info)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return nullptr;

  return gameClient->OpenStream(info);
}

bool CAddonCallbacksGame::ChangeStreamDetails(void* addonData, game_stream_handle* stream, const game_stream_details& info)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return nullptr;

  return gameClient->ChangeStreamDetails(stream, info);
}

void CAddonCallbacksGame::AddStreamData(void* addonData, game_stream_handle* stream, const uint8_t* data, unsigned int size)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return;

  gameClient->AddStreamData(stream, data, size);
}

void CAddonCallbacksGame::CloseStream(void* addonData, game_stream_handle* stream)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return;

  gameClient->CloseStream(stream);
}

void CAddonCallbacksGame::EnableHardwareRendering(void* addonData, const game_hw_info *hw_info)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return;

  //! @todo
}

uintptr_t CAddonCallbacksGame::HwGetCurrentFramebuffer(void* addonData)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return 0;

  //! @todo
  return 0;
}

game_proc_address_t CAddonCallbacksGame::HwGetProcAddress(void* addonData, const char *sym)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return nullptr;

  //! @todo
  return nullptr;
}

void CAddonCallbacksGame::RenderFrame(void* addonData)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return;

  //! @todo
}

bool CAddonCallbacksGame::OpenPort(void* addonData, unsigned int port)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return false;

  return gameClient->OpenPort(port);
}

void CAddonCallbacksGame::ClosePort(void* addonData, unsigned int port)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return;

  gameClient->ClosePort(port);
}

bool CAddonCallbacksGame::InputEvent(void* addonData, const game_input_event* event)
{
  CGameClient* gameClient = GetGameClient(addonData, __FUNCTION__);
  if (!gameClient)
    return false;

  if (event == nullptr)
    return false;

  return gameClient->ReceiveInputEvent(*event);
}

} /* namespace Game */

} /* namespace KodiAPI */
