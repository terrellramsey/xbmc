/*
 *      Copyright (C) 2015-2016 Team Kodi
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

#include "GameClientStreamAudio.h"
#include "cores/RetroPlayer/IRetroPlayerAudioCallback.h"
#include "utils/log.h"

#include "addons/kodi-addon-dev-kit/include/kodi/kodi_game_types.h"

#include <assert.h>

using namespace GAME;

CGameClientAudioStream::CGameClientAudioStream(const game_stream_details& info, IAudioStream* audio) :
  m_audio(audio)
{
  assert(m_audio != nullptr);

  SetStreamDetails(info);
}

bool CGameClientAudioStream::OpenStream()
{
  return m_audio->OpenStream();
}

bool CGameClientAudioStream::SetStreamDetails(const game_stream_details& info)
{
  bool bSuccess = false;

  m_info = info;

  switch (m_info.type)
  {
  case GAME_STREAM_AUDIO:
  {
    const unsigned int sampleRate = m_info.audio.sample_rate;
    const CAEChannelInfo channelLayout = CGameClientTranslator::TranslateAudioChannels(m_info.audio.channel_map);

    if (sampleRate > 0 && channelLayout.Count() > 0)
    {
      const AEDataFormat pcmFormat = CGameClientTranslator::TranslatePCMFormat(m_info.audio.format);
      const AVCodecID codec = CGameClientTranslator::TranslateAudioCodec(m_info.audio.codec);

      if (codec != AV_CODEC_ID_NONE)
        bSuccess = m_audio->SetEncodedStream(codec, sampleRate, channelLayout);
      else if (pcmFormat != AE_FMT_INVALID)
        bSuccess = m_audio->SetPCMStream(pcmFormat, sampleRate, channelLayout);
      else
      {
        CLog::Log(LOGERROR, "Invalid format (%d) or codec (%d)",
            m_info.audio.format, m_info.audio.codec);
      }
    }

    break;
  }
  default:
    break;
  }

  return bSuccess;
}

void CGameClientAudioStream::AddStreamData(const uint8_t* data, unsigned int size)
{
  m_audio->AddStreamData(data, size);
}

void CGameClientAudioStream::CloseStream()
{
  m_audio->CloseStream();
}
