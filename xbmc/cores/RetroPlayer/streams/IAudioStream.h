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

#include "cores/AudioEngine/Utils/AEChannelData.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

#include <stdint.h>

class CAEChannelInfo;
class IAudioStreamHandler;
class IStreamHandler;

namespace RETROPLAYER
{
  class IAudioStream
  {
    IAudioStream() :
      m_handler(nullptr)
    {
    }

    virtual ~IAudioStream() = default;

    virtual bool SetPCMStream(AEDataFormat format, unsigned int samplerate, const CAEChannelInfo& channelLayout) = 0;
    virtual bool SetEncodedStream(AVCodecID codec, unsigned int samplerate, const CAEChannelInfo& channelLayout) = 0;

    virtual bool OpenStream() = 0;
    virtual void AddStreamData(const uint8_t* data, unsigned int size) = 0;
    virtual void CloseStream() = 0;

    void RegisterStream(IGameClientStream* handler) { m_handler = handler; }
    void UnregisterStreamHandler() { m_handler = nullptr; }
    IStreamHandler* GetStreamHandler() { return m_handler; }

  private:
    IStreamHandler* m_handler;
  };
}
