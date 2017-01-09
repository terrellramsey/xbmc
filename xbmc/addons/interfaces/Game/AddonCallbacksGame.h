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
#pragma once

#include "addons/interfaces/AddonInterfaces.h"
#include "addons/kodi-addon-dev-kit/include/kodi/libKODI_game.h"

namespace GAME { class CGameClient; }

namespace KodiAPI
{

namespace Game
{

/*!
 * Callbacks for a game add-on to Kodi
 */
class CAddonCallbacksGame
{
public:
  CAddonCallbacksGame(ADDON::CAddon* addon);
  ~CAddonCallbacksGame(void);

  /*!
   * @return The callback table.
   */
  CB_GameLib* GetCallbacks() const { return m_callbacks; }

  static void CloseGame(void* addonData);
  static game_stream_handle* OpenStream(void* addonData, const game_stream_details& info);
  static bool ChangeStreamDetails(void* addonData, game_stream_handle* stream, const game_stream_details& info);
  static void AddStreamData(void* addonData, game_stream_handle* stream, const uint8_t* data, unsigned int size);
  static void CloseStream(void* addonData, game_stream_handle* stream);
  static void EnableHardwareRendering(void* addonData, const game_hw_info* hw_info);
  static uintptr_t HwGetCurrentFramebuffer(void* addonData);
  static game_proc_address_t HwGetProcAddress(void* addonData, const char* sym);
  static void RenderFrame(void* addonData);
  static bool OpenPort(void* addonData, unsigned int port);
  static void ClosePort(void* addonData, unsigned int port);
  static bool InputEvent(void* addonData, const game_input_event* event);

private:
  static GAME::CGameClient* GetGameClient(void* addonData, const char* strFunction);

  ADDON::CAddon* m_addon; /*!< the addon */
  CB_GameLib*  m_callbacks; /*!< callback addresses */
};

} /* namespace Game */

} /* namespace KodiAPI */
