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

#include "IRetroPlayerVideoStreamCallback.h"
#include "cores/VideoPlayer/VideoPlayer.h"

class IPlayerCallback;

namespace RETROPLAYER
{
  class CRetroPlayerVideoStream : public IRetroPlayerVideoCallback
  {
  public:
    CRetroPlayerVideoStream(IPlayerCallback& callback, CProcessInfo& m_processInfo);

    virtual ~CRetroPlayerVideoStream();

    // implementation of IRetroPlayerVideoStreamCallback
    virtual bool SetPixelFormat(AVPixelFormat pixfmt, unsigned int width, unsigned int height, double aspect, unsigned int orientationDeg, double framerate) override;
    virtual bool SetEncodedStream(AVCodecID codec) override;
    virtual bool OpenStream() override;
    virtual void AddStreamData(const uint8_t* data, unsigned int size) override;
    virtual void CloseStream() override;

    bool OpenFile(const CFileItem& file, const CPlayerOptions &options);
    bool CloseFile(bool reopen = false);

    void FrameMove() { m_videoPlayer->FrameMove(); }
    void Render(bool clear, uint32_t alpha = 255, bool gui = true) { m_videoPlayer->Render(clear, alpha, gui); }
    void FlushRenderer() { m_videoPlayer->FlushRenderer(); }
    void SetRenderViewMode(int mode) { m_videoPlayer->SetRenderViewMode(mode); }
    float GetRenderAspectRatio() { return m_videoPlayer->GetRenderAspectRatio(); }
    void TriggerUpdateResolution() { m_videoPlayer->TriggerUpdateResolution(); }
    bool IsRenderingVideo() { return m_videoPlayer->IsRenderingVideo(); }
    bool IsRenderingGuiLayer() { return m_videoPlayer->IsRenderingGuiLayer(); }
    bool IsRenderingVideoLayer() { return m_videoPlayer->IsRenderingVideoLayer(); }
    bool Supports(EINTERLACEMETHOD method) { return m_videoPlayer->IsRenderingVideoLayer(); }
    EINTERLACEMETHOD GetDeinterlacingMethodDefault() { return m_videoPlayer->GetDeinterlacingMethodDefault(); }
    bool Supports(ESCALINGMETHOD method) { return m_videoPlayer->Supports(method); }
    bool Supports(ERENDERFEATURE feature) { return m_videoPlayer->Supports(feature); }
    unsigned int RenderCaptureAlloc() { return m_videoPlayer->RenderCaptureAlloc(); }
    void RenderCaptureRelease(unsigned int captureId) { m_videoPlayer->RenderCaptureRelease(captureId); }
    void RenderCapture(unsigned int captureId, unsigned int width, unsigned int height, int flags) { m_videoPlayer->RenderCapture(captureId, width, height, flags); }
    bool RenderCaptureGetPixels(unsigned int captureId, unsigned int millis, uint8_t *buffer, unsigned int size) { return m_videoPlayer->RenderCaptureGetPixels(captureId, millis, buffer, size); }

  private:
    enum class VIDEO_TYPE
    {
      INVALID,
      PIXEL,
      ENCODED,
    };

    bool IsOpen() const;

    const std::unique_ptr<CVideoPlayer> m_videoPlayer;

    // Stream parameters
    VIDEO_TYPE m_type;
    AVPixelFormat m_pixelFormat;
    AVCodecID m_videoCodec;
    unsigned int m_width;
    unsigned int m_height;
    double m_aspect;
    unsigned int m_orientationDeg;
    double m_framerate;
  };
}
