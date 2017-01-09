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

#include "cores/IPlayer.h"
#include "games/GameTypes.h"
#include "threads/CriticalSection.h"

#include <memory>

class CProcessInfo;

namespace RETROPLAYER
{
  class CRetroPlayerAudio;
  class CRetroPlayerVideo;

  class CRetroPlayer : public IPlayer
  {
  public:
    CRetroPlayer(IPlayerCallback& callback);
    virtual ~CRetroPlayer();

    // implementation of IPlayer
    //virtual bool Initialize(TiXmlElement* pConfig) override { return true; }
    virtual bool OpenFile(const CFileItem& file, const CPlayerOptions& options) override;
    //virtual bool QueueNextFile(const CFileItem &file) override { return false; }
    //virtual void OnNothingToQueueNotify() override { }
    virtual bool CloseFile(bool reopen = false) override;
    virtual bool IsPlaying() const override;
    virtual bool CanPause() override;
    virtual void Pause() override;
    virtual bool HasVideo() const override { return true; }
    virtual bool HasAudio() const override { return true; }
    virtual bool HasGame() const override { return true; }
    //virtual bool HasRDS() const override { return false; }
    //virtual bool IsPassthrough() const override { return false;}
    virtual bool CanSeek() override;
    virtual void Seek(bool bPlus = true, bool bLargeStep = false, bool bChapterOverride = false) override;
    //virtual bool SeekScene(bool bPlus = true) override { return false; }
    virtual void SeekPercentage(float fPercent = 0) override;
    virtual float GetPercentage() override;
    virtual float GetCachePercentage() override;
    virtual void SetMute(bool bOnOff) override;
    //virtual void SetVolume(float volume) override { }
    //virtual void SetDynamicRangeCompression(long drc) override { }
    //virtual bool CanRecord() override { return false; }
    //virtual bool IsRecording() override { return false; }
    //virtual bool Record(bool bOnOff) override { return false; }
    //virtual void SetAVDelay(float fValue = 0.0f) override { return; }
    //virtual float GetAVDelay() override { return 0.0f; }
    //virtual void SetSubTitleDelay(float fValue = 0.0f) override { }
    //virtual float GetSubTitleDelay() override { return 0.0f; }
    //virtual int GetSubtitleCount() override { return 0; }
    //virtual int GetSubtitle() override { return -1; }
    //virtual void GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info) override { }
    //virtual void SetSubtitle(int iStream) override { }
    //virtual bool GetSubtitleVisible() override { return false; }
    //virtual void SetSubtitleVisible(bool bVisible) override { }
    //virtual void AddSubtitle(const std::string& strSubPath) override { }
    //virtual int GetAudioStreamCount() override { return 0; }
    //virtual int GetAudioStream() override { return -1; }
    //virtual void SetAudioStream(int iStream) override { }
    //virtual void GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info) override { }
    //virtual int GetVideoStream() const override { return -1; }
    //virtual int GetVideoStreamCount() const override { return 0; }
    //virtual void GetVideoStreamInfo(int streamId, SPlayerVideoStreamInfo &info) override { }
    //virtual void SetVideoStream(int iStream) override { }
    //virtual TextCacheStruct_t* GetTeletextCache() override { return NULL; }
    //virtual void LoadPage(int p, int sp, unsigned char* buffer) override { }
    //virtual std::string GetRadioText(unsigned int line) override { return ""; }
    //virtual int GetChapterCount() override { return 0; }
    //virtual int GetChapter() override { return -1; }
    //virtual void GetChapterName(std::string& strChapterName, int chapterIdx = -1) override { return; }
    //virtual int64_t GetChapterPos(int chapterIdx = -1) override { return 0; }
    //virtual int SeekChapter(int iChapter) override { return -1; }
    //virtual float GetActualFPS() override { return 0.0f; }
    virtual void SeekTime(int64_t iTime = 0) override;
    virtual bool SeekTimeRelative(int64_t iTime) override;
    virtual int64_t GetTime() override;
    //virtual void SetTime(int64_t time) override { } // Only used by Air Tunes Server
    virtual int64_t GetTotalTime() override;
    //virtual void SetTotalTime(int64_t time) override { } // Only used by Air Tunes Server
    //virtual int GetSourceBitrate() override { return 0; }
    virtual bool GetStreamDetails(CStreamDetails &details) override;
    virtual void SetSpeed(float speed) override;
    virtual float GetSpeed() override;
    //virtual bool SkipNext() override { return false; }
    //virtual bool IsCaching() const override { return false; }
    //virtual int GetCacheLevel() const override { return -1; }
    //virtual bool IsInMenu() const override { return false; }
    //virtual bool HasMenu() const override { return false; }
    //virtual void DoAudioWork() override { }
    //virtual bool OnAction(const CAction &action) override { return false; }
    virtual std::string GetPlayerState() override;
    virtual bool SetPlayerState(const std::string& state) override;
    //virtual std::string GetPlayingTitle() override { return ""; }
    //virtual bool SwitchChannel(const PVR::CPVRChannelPtr &channel) override { return false; }
    //virtual void GetAudioCapabilities(std::vector<int> &audioCaps) override { audioCaps.assign(1,IPC_AUD_ALL); }
    //virtual void GetSubtitleCapabilities(std::vector<int> &subCaps) override { subCaps.assign(1,IPC_SUBS_ALL); }
    virtual void FrameMove() override;
    virtual void Render(bool clear, uint32_t alpha = 255, bool gui = true) override;
    virtual void FlushRenderer() override;
    virtual void SetRenderViewMode(int mode) override;
    virtual float GetRenderAspectRatio() override;
    virtual void TriggerUpdateResolution() override;
    virtual bool IsRenderingVideo() override;
    virtual bool IsRenderingGuiLayer() override;
    virtual bool IsRenderingVideoLayer() override;
    virtual bool Supports(EINTERLACEMETHOD method) override;
    virtual EINTERLACEMETHOD GetDeinterlacingMethodDefault() override;
    virtual bool Supports(ESCALINGMETHOD method) override;
    virtual bool Supports(ERENDERFEATURE feature) override;
    virtual unsigned int RenderCaptureAlloc() override;
    virtual void RenderCaptureRelease(unsigned int captureId) override;
    virtual void RenderCapture(unsigned int captureId, unsigned int width, unsigned int height, int flags) override;
    virtual bool RenderCaptureGetPixels(unsigned int captureId, unsigned int millis, uint8_t *buffer, unsigned int size) override;

  private:
    /**
     * \brief Dump game information (if any) to the debug log.
     */
    void PrintGameInfo(const CFileItem &file) const;

    std::unique_ptr<CProcessInfo>      m_processInfo;
    std::unique_ptr<CRetroPlayerAudio> m_audio;
    std::unique_ptr<CRetroPlayerVideo> m_video;
    GameClientPtr                      m_gameClient;
    CCriticalSection                   m_mutex;
  };
}
