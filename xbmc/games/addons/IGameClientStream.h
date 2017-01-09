/*
 *      Copyright (C) 2016-2017 Team Kodi
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

#include <stdint.h>

struct game_stream_details;

namespace GAME
{
  /*!
   * \ingroup games
   * \brief Types of game client streams
   */
  enum class GAME_STREAM
  {
    VIDEO,
    AUDIO,
    INPUT,
    SAVESTATE,
  };

  /*!
   * \ingroup games
   * \brief Abstraction of a game client stream
   */
  class IGameClientStream
  {
  public:
    virtual ~IGameClientStream() = default;

    virtual GAME_STREAM StreamType() const = 0;
    virtual bool OpenStream() = 0;
    virtual bool SetStreamDetails(const game_stream_details& info) = 0;
    virtual void AddStreamData(const uint8_t* data, unsigned int size) = 0;
    virtual void CloseStream() = 0;
  };
}
