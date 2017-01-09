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

#include "streams/IAudioStream.h"

#include <memory>

class CDVDAudioCodec;
class CProcessInfo;
class IAEStream;

namespace RETROPLAYER
{
  class CRetroPlayerAudioStream : public IAudioStream
  {
  public:
    CRetroPlayerAudioStream(CProcessInfo& processInfo);
    virtual ~CRetroPlayerAudioStream();

    // implementation of IRetroPlayerAudioCallback
    virtual bool SetPCMStream(AEDataFormat format, unsigned int samplerate, const CAEChannelInfo& channelLayout) override;
    virtual bool SetEncodedStream(AVCodecID codec, unsigned int samplerate, const CAEChannelInfo& channelLayout) override;
    virtual bool OpenStream() override;
    virtual void AddStreamData(const uint8_t* data, unsigned int size) override;
    virtual void CloseStream() override;

    void Enable(bool bEnabled) { m_bAudioEnabled = bEnabled; }

  private:
    enum class AUDIO_TYPE
    {
      INVALID,
      PCM,
      ENCODED,
    };

    bool IsOpen() const;

    // Construction parameters
    CProcessInfo& m_processInfo;

    // Stream parameters
    AUDIO_TYPE m_type;
    AEDataFormat m_format;
    AVCodecID m_codec;
    unsigned int m_samplerate;
    CAEChannelInfo m_channelLayout;
    bool m_bAudioEnabled;
    IAEStream* m_pAudioStream;

    // Codec parameters
    std::unique_ptr<CDVDAudioCodec> m_pAudioCodec;
  };
}
