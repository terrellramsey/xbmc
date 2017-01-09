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

#include "RetroPlayerAudioStream.h"
#include "RetroPlayerDefines.h"
#include "cores/AudioEngine/Interfaces/AE.h"
#include "cores/AudioEngine/Interfaces/AEStream.h"
#include "cores/AudioEngine/Utils/AEChannelInfo.h"
#include "cores/AudioEngine/Utils/AEUtil.h"
#include "cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodec.h"
#include "cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemux.h"
#include "cores/VideoPlayer/DVDStreamInfo.h"
#include "cores/VideoPlayer/Process/ProcessInfo.h"
#include "threads/Thread.h"
#include "utils/log.h"

using namespace RETROPLAYER;

CRetroPlayerAudioStream::CRetroPlayerAudioStream(CProcessInfo& processInfo) :
  m_processInfo(processInfo),
  m_type(AUDIO_TYPE::INVALID),
  m_format(AE_FMT_INVALID),
  m_codec(AV_CODEC_ID_NONE),
  m_samplerate(0),
  m_bAudioEnabled(true),
  m_pAudioStream(nullptr)
{
}

CRetroPlayerAudioStream::~CRetroPlayerAudioStream()
{
  CloseStream();
}

bool CRetroPlayerAudioStream::IsOpen() const
{
  return m_pAudioStream != nullptr &&
         m_pAudioCodec.get() != nullptr;
}

bool CRetroPlayerAudioStream::SetPCMStream(AEDataFormat format, unsigned int samplerate, const CAEChannelInfo& channelLayout)
{
  bool bSuccess = true;
  bool bChanged = false;

  if (m_type != AUDIO_TYPE::PCM ||
      m_format != format ||
      m_samplerate != samplerate ||
      m_channelLayout != channelLayout)
  {
    m_type = AUDIO_TYPE::PCM;
    m_format = format;
    m_codec = AV_CODEC_ID_NONE;
    m_samplerate = samplerate;
    m_channelLayout = channelLayout;
    bChanged = true;
  }

  if (IsOpen() && bChanged)
  {
    CloseStream();
    bSuccess = OpenStream();
  }

  return bSuccess;
}

bool CRetroPlayerAudioStream::SetEncodedStream(AVCodecID codec, unsigned int samplerate, const CAEChannelInfo& channelLayout)
{
  bool bSuccess = true;
  bool bChanged = false;

  if (m_type != AUDIO_TYPE::ENCODED ||
      m_codec != codec ||
      m_samplerate != samplerate ||
      m_channelLayout != channelLayout)
  {
    m_type = AUDIO_TYPE::ENCODED;
    m_format = AE_FMT_INVALID;
    m_codec = codec;
    m_samplerate = samplerate;
    m_channelLayout = channelLayout;
    bChanged = true;
  }

  if (IsOpen() && bChanged)
  {
    CloseStream();
    bSuccess = OpenStream();
  }

  return bSuccess;

}

bool CRetroPlayerAudioStream::OpenStream()
{
  bool bSuccess = false;

  if (IsOpen())
    CloseStream();

  switch (m_type)
  {
  case AUDIO_TYPE::PCM:
  {
    CLog::Log(LOGINFO, "RetroPlayerAudioStream: Creating PCM audio stream, format = %d, samplerate = %u", m_format, m_samplerate);

    AEAudioFormat audioFormat;
    audioFormat.m_dataFormat = m_format;
    audioFormat.m_sampleRate = m_samplerate;
    audioFormat.m_channelLayout = m_channelLayout;
    m_pAudioStream = CServiceBroker::GetActiveAE().MakeStream(audioFormat);

    if (m_pAudioStream)
      bSuccess = true;
    else
      CLog::Log(LOGERROR, "RetroPlayerAudioStream: Failed to create audio stream");

    break;
  }
  case AUDIO_TYPE::ENCODED:
  {
    CLog::Log(LOGINFO, "RetroPlayerAudioStream: Creating encoded audio stream, codec = %d, samplerate = %u", m_codec, m_samplerate);

    CDemuxStreamAudio audioStream;

    // Stream
    audioStream.uniqueId = GAME_STREAM_AUDIO_ID;
    audioStream.codec = m_codec;
    audioStream.type = STREAM_AUDIO;
    audioStream.source = STREAM_SOURCE_DEMUX;
    audioStream.realtime = true;

    // Audio
    audioStream.iChannels = m_channelLayout.Count();
    audioStream.iSampleRate = m_samplerate;
    audioStream.iChannelLayout = CAEUtil::GetAVChannelLayout(m_channelLayout);

    CDVDStreamInfo hint(audioStream);
    m_pAudioCodec.reset(CDVDFactoryCodec::CreateAudioCodec(hint, m_processInfo, false));

    if (!m_pAudioCodec)
    {
      CLog::Log(LOGERROR, "RetroPlayerAudioStream: Failed to create audio codec", m_codec, m_samplerate);
      return false;
    }

    return true;
  }
  default:
    break;
  }

  return bSuccess;
}

void CRetroPlayerAudioStream::AddStreamData(const uint8_t* data, unsigned int size)
{
  if (m_bAudioEnabled)
  {
    if (m_pAudioCodec)
    {
      // FIXME
      int consumed = m_pAudioCodec->AddData(const_cast<uint8_t*>(data), size, DVD_NOPTS_VALUE, DVD_NOPTS_VALUE);
      if (consumed < 0)
      {
        CLog::Log(LOGERROR, "CRetroPlayerAudioStream::AddData - Decode Error (%d)", consumed);
        m_pAudioCodec.reset();
        return;
      }

      DVDAudioFrame audioframe;
      m_pAudioCodec->GetData(audioframe);

      if (audioframe.nb_frames != 0)
      {
        // Open audio stream if not already open
        if (!m_pAudioStream)
        {
          m_pAudioStream = CServiceBroker::GetActiveAE().MakeStream(audioframe.format);
          if (m_pAudioStream == nullptr)
          {
            CLog::Log(LOGERROR, "RetroPlayerAudioStream: Failed to create audio stream, closing codec");
            CloseStream();
          }
        }

        // Add data to audio stream
        if (m_pAudioStream)
          m_pAudioStream->AddData(audioframe.data, 0, audioframe.nb_frames);
      }
    }
    else if (m_pAudioStream)
    {
      const unsigned int frameSize = m_pAudioStream->GetChannelCount() * (CAEUtil::DataFormatToBits(m_pAudioStream->GetDataFormat()) >> 3);
      m_pAudioStream->AddData(&data, 0, size / frameSize);
    }
  }
}

void CRetroPlayerAudioStream::CloseStream()
{
  if (m_pAudioCodec)
  {
    m_pAudioCodec->Dispose();
    m_pAudioCodec.reset();
  }
  if (m_pAudioStream)
  {
    CServiceBroker::GetActiveAE().FreeStream(m_pAudioStream);
    m_pAudioStream = nullptr;
  }
}
