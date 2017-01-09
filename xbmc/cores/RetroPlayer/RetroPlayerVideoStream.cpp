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

#include "RetroPlayerDefines.h"
#include "PixelConverter.h"
#include "PixelConverterRBP.h"
#include "cores/VideoPlayer/VideoPlayer.h"
#include "utils/log.h"

#include <atomic> //! @todo
#include "RetroPlayerVideoStream.h"

using namespace RETROPLAYER;

CRetroPlayerVideo::CRetroPlayerVideo(IPlayerCallback& callback, CProcessInfo& processInfo) :
  m_videoPlayer(new CVideoPlayer(callback, processInfo)),
  m_type(VIDEO_TYPE::INVALID),
  m_pixelFormat(AV_PIX_FMT_NONE),
  m_videoCodec(AV_CODEC_ID_NONE),
  m_width(0),
  m_height(0),
  m_orientationDeg(0),
  m_framerate(0.0),
  m_aspect(0.0)
{
}

CRetroPlayerVideo::~CRetroPlayerVideo()
{
  CloseStream();
}

bool CRetroPlayerVideo::IsOpen() const
{
  return m_videoPlayer->IsPlaying();
}

bool CRetroPlayerVideo::OpenFile(const CFileItem& file, const CPlayerOptions &options)
{
  CFileItem fileCopy(file);

  fileCopy.GetGameInfoTag()->RegisterStreamSource(this);

  return m_videoPlayer->OpenFile(file, options);
}

bool CRetroPlayerVideo::CloseFile(bool reopen = false)
{
  return m_videoPlayer->CloseFile(reopen);

  fileCopy.GetGameInfoTag()->UnregisterStreamSource(this);
}

bool CRetroPlayerVideo::SetPixelFormat(AVPixelFormat pixfmt, unsigned int width, unsigned int height, double aspect, unsigned int orientationDeg, double framerate)
{
  bool bSuccess = true;
  bool bChanged = false;

  if (m_type != VIDEO_TYPE::PIXEL ||
      m_pixelFormat != pixfmt ||
      m_width != width ||
      m_height != height ||
      m_aspect != aspect ||
      m_orientationDeg != orientationDeg ||
      m_framerate != framerate)
  {
    m_type = VIDEO_TYPE::PIXEL;
    m_pixelFormat = pixfmt;
    m_width = width;
    m_height = height;
    m_aspect = aspect;
    m_orientationDeg = orientationDeg;
    m_framerate = framerate;
    bChanged = true;
  }

  if (IsOpen() && bChanged)
  {
    CloseStream();
    bSuccess = OpenStream();
  }

  return bSuccess;
}

bool CRetroPlayerVideo::SetEncodedStream(AVCodecID codec)
{
  bool bSuccess = true;
  bool bChanged = false;

  if (m_type != VIDEO_TYPE::ENCODED ||
      m_videoCodec != codec)
  {
    m_type = VIDEO_TYPE::ENCODED;
    m_videoCodec = codec;
    bChanged = true;
  }

  if (IsOpen() && bChanged)
  {
    CloseStream();
    bSuccess = OpenStream();
  }

  return bSuccess;
}

bool CRetroPlayerVideo::OpenStream()
{
  bool bSuccess = false;

  if (IsOpen())
    CloseStream();


  /*
  CDemuxStreamVideo videoStream;

  // Stream
  videoStream.uniqueId = GAME_STREAM_VIDEO_ID;
  videoStream.codec = codec;
  videoStream.type = STREAM_VIDEO;
  videoStream.source = STREAM_SOURCE_DEMUX;
  videoStream.realtime = true;
  */
`
  // Video
  //! @todo Needed?
  /*
  videoStream.iFpsScale = 1000;
  videoStream.iFpsRate = static_cast<int>(framerate * 1000);
  videoStream.iHeight = height;
  videoStream.iWidth = width;
  videoStream.fAspect = static_cast<float>(width) / static_cast<float>(height);
  videoStream.iOrientation = orientationDeg;
  */

  //! @todo

  return false;


  switch (m_type)
  {
  case VIDEO_TYPE::PIXELS:
  {
    CLog::Log(LOGINFO, "RetroPlayerVideo: Creating PCM audio stream, format = %d, samplerate = %u", m_format, m_samplerate);

    AEVideoFormat audioFormat;
    audioFormat.m_dataFormat = m_format;
    audioFormat.m_sampleRate = m_samplerate;
    audioFormat.m_channelLayout = m_channelLayout;
    m_pAudioStream = CServiceBroker::GetActiveAE().MakeStream(audioFormat);

    if (m_pAudioStream)
      bSuccess = true;
    else
      CLog::Log(LOGERROR, "RetroPlayerVideo: Failed to create audio stream");

    break;
  }
  case AUDIO_TYPE::ENCODED:
  {
    CLog::Log(LOGINFO, "RetroPlayerVideo: Creating encoded audio stream, codec = %d, samplerate = %u", m_codec, m_samplerate);

    CDemuxStreamVideo audioStream;

    // Stream
    audioStream.uniqueId = GAME_STREAM_VIDEO_ID;
    audioStream.codec = m_codec;
    audioStream.type = STREAM_AUDIO;
    audioStream.source = STREAM_SOURCE_DEMUX;
    audioStream.realtime = true;

    // Video
    audioStream.iChannels = m_channelLayout.Count();
    audioStream.iSampleRate = m_samplerate;
    audioStream.iChannelLayout = CAEUtil::GetAVChannelLayout(m_channelLayout);

    CDVDStreamInfo hint(audioStream);
    m_pVideoCodec.reset(CDVDFactoryCodec::CreateVideoCodec(hint, m_processInfo, false));

    if (!m_pVideoCodec)
    {
      CLog::Log(LOGERROR, "RetroPlayerVideo: Failed to create audio codec", m_codec, m_samplerate);
      return false;
    }

    return true;
  }
  default:
    break;
  }

  return bSuccess;
}

void CRetroPlayerVideo::AddData(const uint8_t* data, unsigned int size)
{
  DVDVideoPicture picture = { };

  if (GetPicture(data, size, picture))
  {
    if (!Configure(picture))
    {
      CLog::Log(LOGERROR, "RetroPlayerVideo: Failed to configure renderer");
      CloseStream();
    }
    else
    {
      SendPicture(picture);
    }
  }
}

void CRetroPlayerAudio::AddStreamData(const uint8_t* data, unsigned int size)
{
  if (m_bAudioEnabled)
  {
    if (m_pAudioCodec)
    {
      // FIXME
      int consumed = m_pAudioCodec->AddData(const_cast<uint8_t*>(data), size, DVD_NOPTS_VALUE, DVD_NOPTS_VALUE);
      if (consumed < 0)
      {
        CLog::Log(LOGERROR, "CRetroPlayerAudio::AddData - Decode Error (%d)", consumed);
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
            CLog::Log(LOGERROR, "RetroPlayerAudio: Failed to create audio stream, closing codec");
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

void CRetroPlayerVideo::CloseStream()
{
}
