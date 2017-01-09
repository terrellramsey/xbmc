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

#include "GameClientStreamVideo.h"
#include "cores/RetroPlayer/streams/IVideoStream.h"
#include "utils/log.h"

#include "addons/kodi-addon-dev-kit/include/kodi/kodi_game_types.h"

#include <assert.h>

using namespace GAME;

// --- CGameClientVideoStream --------------------------------------------------

CGameClientVideoStream::CGameClientVideoStream(const game_stream_details& info, RETROPLAYER::IVideoStream* video) :
  m_video(video)
{
  assert(m_video != nullptr);

  SetStreamDetails(info);
}

bool CGameClientVideoStream::OpenStream()
{
  return m_video->OpenStream();
}

bool CGameClientVideoStream::SetStreamDetails(const game_stream_details& info)
{
  bool bSuccess = false;

  m_info = info;

  switch (m_info.type)
  {
  case GAME_STREAM_VIDEO:
  {
    const AVPixelFormat pixFormat = CGameClientTranslator::TranslatePixelFormat(m_info.video.format);
    const AVCodecID codec = CGameClientTranslator::TranslateVideoCodec(m_info.video.codec);

    if (pixFormat != AV_PIX_FMT_NONE)
    {
      const unsigned int width = m_info.video.width;
      const unsigned int height = m_info.video.height;
      const double aspect = m_info.video.aspect;
      const unsigned int orientationDegCC = CGameClientTranslator::TranslateRotation(m_info.video.rotation);
      const double framerate = m_info.video.fps;

      m_video->SetPixelStream(pixFormat, width, height, aspect, orientationDegCC, framerate);
    }
    else if (codec != AV_CODEC_ID_NONE)
    {
      m_video->SetEncodedStream(codec);
    }
    else
    {
      CLog::Log(LOGERROR, "Invalid pixel format (%d) or codec (%d)",
          m_info.video.format, m_info.video.codec);
    }

    break;
  }
  default:
    break;
  }

  return bSuccess;
}

void CGameClientVideoStream::AddStreamData(const uint8_t* data, unsigned int size)
{
  m_video->AddStreamData(data, size);
}

void CGameClientVideoStream::CloseStream()
{
  m_video->CloseStream();
}
