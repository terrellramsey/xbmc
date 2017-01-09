/*
 *      Copyright (C) 2016 Team Kodi
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

#include "InputStreamGame.h"
#include "cores/AudioEngine/Utils/AEChannelInfo.h"
#include "cores/AudioEngine/Utils/AEUtil.h"
#include "cores/RetroPlayer/IRetroPlayerVideoCallback.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxPacket.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.h"
#include "games/addons/GameClient.h"

// --- CInputStreamGame --------------------------------------------------------

CInputStreamGame::CInputStreamGame(const CFileItem &fileitem) :
  CDVDInputStream(DVDSTREAM_TYPE_GAME, fileitem),
  m_pDemux(new CInputStreamGameDemux(fileitem))
{
  // initialize CDVDInputStream
  SetRealtime(true);
}

CInputStreamGame::~CInputStreamGame()
{
  Close();
}

CDVDInputStream::ENextStream CInputStreamGame::NextStream()
{
  return NEXTSTREAM_NONE;
}

// --- CInputStreamGameDisplayTime ---------------------------------------------

CInputStreamGameDisplayTime::CInputStreamGameDisplayTime(const GAME::GameClientPtr &gameClient) :
  m_gameClient(gameClient)
{
}

int CInputStreamGameDisplayTime::GetTotalTime()
{
  return 0; //! @todo
}

int CInputStreamGameDisplayTime::GetTime()
{
  return 0; //! @todo
}

// --- CInputStreamGamePosTime -------------------------------------------------

CInputStreamGamePosTime::CInputStreamGamePosTime(const GAME::GameClientPtr &gameClient) :
  m_gameClient(gameClient)
{
}

bool CInputStreamGamePosTime::PosTime(int ms)
{
  return false; //! @todo
}

// --- CInputStreamGameDemux ---------------------------------------------------

CInputStreamGameDemux::CInputStreamGameDemux(const CFileItem &fileItem)
{
  if (fileItem.HasGameInfoTag())
    fileItem.GetGameInfoTag()->GetInputStream()->;
}

CInputStreamGameDemux::~CInputStreamGameDemux()
{
}

bool CInputStreamGameDemux::OpenDemux()
{
  CDemuxStreamVideo
  return true;
}
