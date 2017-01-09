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

#include "IGameClientStream.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"
}

struct game_stream_details;

namespace RETROPLAYER
{
  class IVideoStream;
}

namespace GAME
{
  class CGameClientVideoStream : IGameClientStream
  {
    CGameClientVideoStream(const game_stream_details& info, RETROPLAYER::IVideoStream* video);

    virtual ~CGameClientVideoStream() = default;

    // implementation of IGameClientStream
    virtual GAME_STREAM StreamType() const override { return GAME_STREAM::VIDEO; }
    virtual bool OpenStream() override;
    virtual bool SetStreamDetails(const game_stream_details& info) override;
    virtual void AddStreamData(const uint8_t* data, unsigned int size) override;
    virtual void CloseStream() override;

  private:
    // Construction parameters
    game_stream_details m_info;
    RETROPLAYER::IVideoStream* const m_video;
  };
}
