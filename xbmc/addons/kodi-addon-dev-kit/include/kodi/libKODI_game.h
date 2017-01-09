/*
 *      Copyright (C) 2014-2016 Team Kodi
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

#include "libXBMC_addon.h"
#include "kodi_game_types.h"

#include <string>
#include <stdio.h>

#if defined(ANDROID)
  #include <sys/stat.h>
#endif

typedef struct CB_GameLib
{
  void (*CloseGame)(void* addonData);
  game_stream_handle* (*OpenStream)(const game_stream_details* info)
  bool (*ChangeStreamDetails)(game_stream_handle* stream, const game_stream_details* info);
  void (*AddStreamData)(game_stream_handle* stream, const uint8_t* data, unsigned int size);
  void (*CloseStream)(game_stream_handle* stream);
  void (*EnableHardwareRendering)(void* addonData, const game_hw_info* hw_info);
  uintptr_t (*HwGetCurrentFramebuffer)(void* addonData);
  game_proc_address_t (*HwGetProcAddress)(void* addonData, const char* symbol);
  void (*RenderFrame)(void* addonData);
  bool (*OpenPort)(void* addonData, unsigned int port);
  void (*ClosePort)(void* addonData, unsigned int port);
  bool (*InputEvent)(void* addonData, const game_input_event* event);

} CB_GameLib;

class CHelper_libKODI_game
{
public:
  CHelper_libKODI_game(void) :
    m_handle(nullptr),
    m_callbacks(nullptr)
  {
  }

  ~CHelper_libKODI_game(void)
  {
    if (m_handle && m_callbacks)
    {
      m_handle->GameLib_UnRegisterMe(m_handle->addonData, m_callbacks);
    }
  }

  /*!
    * @brief Resolve all callback methods
    * @param handle Pointer to the add-on
    * @return True when all methods were resolved, false otherwise.
    */
  bool RegisterMe(void* handle)
  {
    m_handle = static_cast<AddonCB*>(handle);
    if (m_handle)
      m_callbacks = (CB_GameLib*)m_handle->GameLib_RegisterMe(m_handle->addonData);
    if (!m_callbacks)
      fprintf(stderr, "libKODI_game-ERROR: GameLib_RegisterMe can't get callback table from Kodi !!!\n");

    return m_callbacks != nullptr;
  }

  // --- Gameplay callbacks --------------------------------------------------------

  /*!
   * \brief Requests the frontend to stop the current game
   */
  void CloseGame(void)
  {
    return m_callbacks->CloseGame(m_handle->addonData);
  }

  // Stream callbacks

  /*!
   * \brief Create a stream for game data
   *
   * \param info The stream details of a valid type
   *
   * \return the stream handle, NULL on error. Must be closed if non-NULL is returned.
   */
  game_stream_handle* OpenStream(const game_stream_details& info)
  {
    return m_callbacks->OpenStream(m_handle->addonData, &info);
  }

  /*!
   * \brief Change the details of an open stream
   *
   * \param info The stream details of a valid type
   *
   * \return true if the stream was updated, false if the stream failed to update
   */
  bool ChangeStreamDetails(game_stream_handle* stream, const game_stream_details& info)
  {
    return m_callbacks->ChangeStreamDetails(m_handle->addonData, stream, &info);
  }

  /*!
   * \brief Add data to an open stream
   *
   * \param stream The handle returned from OpenStream()
   * \param data The data buffer
   * \param size The size of the buffer
   */
  void AddStreamData(game_stream_handle* stream, const uint8_t* data, unsigned int size)
  {
    m_callbacks->AddStreamData(m_handle->addonData, stream, data, size);
  }

  /*!
   * \brief Close an opened stream
   *
   * \param stream The handle returned from OpenStream()
   */
  void CloseStream(game_stream_handle* stream)
  {
    m_callbacks->CloseStream(m_handle->addonData, stream);
  }

  // -- Hardware rendering callbacks -------------------------------------------

  /*!
   * \brief Enable hardware rendering
   *
   * \param hw_info A struct of properties for the hardware rendering system
   */
  void EnableHardwareRendering(const struct game_hw_info* hw_info)
  {
    return m_callbacks->EnableHardwareRendering(m_handle->addonData, hw_info);
  }

  /*!
   * \brief Get the framebuffer for rendering
   *
   * \return The framebuffer
   */
  uintptr_t HwGetCurrentFramebuffer(void)
  {
    return m_callbacks->HwGetCurrentFramebuffer(m_handle->addonData);
  }

  /*!
   * \brief Get a symbol from the hardware context
   *
   * \param symbol The symbol's name
   *
   * \return A function pointer for the specified symbol
   */
  game_proc_address_t HwGetProcAddress(const char* sym)
  {
    return m_callbacks->HwGetProcAddress(m_handle->addonData, sym);
  }

  /*!
   * \brief Called when a frame is being rendered
   */
  void RenderFrame()
  {
    return m_callbacks->RenderFrame(m_handle->addonData);
  }

  // --- Input callbacks -------------------------------------------------------

  /*!
   * \brief Begin reporting events for the specified joystick port
   *
   * \param port The zero-indexed port number
   *
   * \return true if the port was opened, false otherwise
   */
  bool OpenPort(unsigned int port)
  {
    return m_callbacks->OpenPort(m_handle->addonData, port);
  }

  /*!
   * \brief End reporting events for the specified port
   *
   * \param port The port number passed to OpenPort()
   */
  void ClosePort(unsigned int port)
  {
    return m_callbacks->ClosePort(m_handle->addonData, port);
  }

  /*!
  * \brief Notify the port of an input event
  *
  * \param event The input event
  *
  * Input events can arrive for the following sources:
  *   - GAME_INPUT_EVENT_MOTOR
  *
  * \return true if the event was handled, false otherwise
  */
  bool InputEvent(const game_input_event& event)
  {
    return m_callbacks->InputEvent(m_handle->addonData, &event);
  }

private:
  AddonCB* m_handle;
  CB_GameLib* m_callbacks;
};
