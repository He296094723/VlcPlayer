#include "vlc_player.h"
#include <QtDebug>

// VLC 类型定义
typedef struct libvlc_event_t       VLCEvent;                       // vlc 事件
typedef void (*VLCEventHandler)     (const VLCEvent *, void *);     // 事件处理程序回调

/********** VlcPlayerPrivate **********/
class VlcPlayerPrivate
{
public:
    VlcPlayerPrivate();
    ~VlcPlayerPrivate();

    void setOutputWindow(void* pHwnd);
    void setEventHandler(VLCEventHandler event, void* pUserData);
    void openMedia(const char* pMediaPathName);
    void play();
    void pause();
    void stop();
    int64_t length();
    int64_t time();
    void setTime(int64_t newTime);
    void setMute(bool mute = true);
    bool mute();
    int  volume();
    void setVolume(int volume);

private:
    libvlc_instance_t* m_pVlcInstance; // VLC 实例
    libvlc_media_player_t* m_pMediaPlayer; // VLC media player 对象
    libvlc_media_t* m_pMedia; // media player 播放的媒体
    libvlc_event_manager_t* m_pEventManager; // 已加载媒体文件的事件管理器
    VLCEventHandler m_eventHandler; // media player 的事件处理程序
};

VlcPlayerPrivate ::VlcPlayerPrivate()
    : m_pVlcInstance(0),
    m_pMediaPlayer(0),
    m_pMedia(0),
    m_pEventManager(0),
    m_eventHandler(0)
{
    const char * const vlc_args[] = {
        "-I", "dumy",      // 没有特别的接口
        "--ignore-config", // 不要使用 VLC 的配置
        "--plugin-path=./plugins" };

    // 初始化 vlc 模块，应该只执行一次
    m_pVlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

    // 创建一个 media player 播放环境
    m_pMediaPlayer = libvlc_media_player_new(m_pVlcInstance);

    // 为 player 创建一个事件管理器，以处理时间变更事件
    m_pEventManager = libvlc_media_player_event_manager(m_pMediaPlayer);
}

VlcPlayerPrivate::~VlcPlayerPrivate()
{
    // 释放 media player
    libvlc_media_player_release(m_pMediaPlayer);
    libvlc_release(m_pVlcInstance);
}

// 设置输出窗口
void VlcPlayerPrivate ::setOutputWindow(void* pHwnd)
{
    libvlc_media_player_set_hwnd(m_pMediaPlayer, pHwnd);
}

// 设置事件处理程序
void VlcPlayerPrivate ::setEventHandler(VLCEventHandler event, void* pUserData)
{
    m_eventHandler = event;
    libvlc_event_attach(m_pEventManager,
                        libvlc_MediaPlayerTimeChanged,
                        m_eventHandler,
                        pUserData);
}

// 加载一个新项
void VlcPlayerPrivate ::openMedia(const char* pMediaPathName)
{
    m_pMedia = libvlc_media_new_path(m_pVlcInstance, pMediaPathName);
    libvlc_media_player_set_media(m_pMediaPlayer, m_pMedia);
}

// 播放 media player
void VlcPlayerPrivate ::play()
{
    libvlc_media_player_play(m_pMediaPlayer);
}

// 暂停播放
void VlcPlayerPrivate::pause()
{
    libvlc_media_player_pause(m_pMediaPlayer);
}

// 停止播放
void VlcPlayerPrivate::stop()
{
    libvlc_media_player_stop(m_pMediaPlayer);
}

// 获取媒体的长度（单位：毫秒）
int64_t VlcPlayerPrivate::length()
{
    int64_t length = libvlc_media_player_get_length(m_pMediaPlayer);
    return length;
}

// 获取媒体的实际位置（单位：毫秒）
int64_t VlcPlayerPrivate::time()
{
    int64_t time = libvlc_media_player_get_time(m_pMediaPlayer);
    return time;
}

// 设置媒体的新位置（单位：毫秒）
void VlcPlayerPrivate::setTime(int64_t newTime)
{
    libvlc_media_player_set_time(m_pMediaPlayer,(libvlc_time_t)newTime);
}

// 设置静音状态
void VlcPlayerPrivate::setMute(bool mute)
{
    libvlc_audio_set_mute(m_pMediaPlayer, mute);
}

// 获得静音状态
bool VlcPlayerPrivate::mute()
{
    bool bMuteState = !!libvlc_audio_get_mute(m_pMediaPlayer);
    return bMuteState;
}

// 设置实际音量
void VlcPlayerPrivate::setVolume(int volume)
{
    libvlc_audio_set_volume(m_pMediaPlayer, volume);
}

// 返回实际音量
int VlcPlayerPrivate::volume()
{
    int volume = libvlc_audio_get_volume(m_pMediaPlayer);
    return volume;
}

// 回调函数，用于事件处理
static void handleVLCEvents(const VLCEvent* pEvt, void* pUserData)
{
    VlcPlayer* player = reinterpret_cast<VlcPlayer*>(pUserData);
    switch (pEvt->type) {
    case libvlc_MediaPlayerTimeChanged:
        qDebug() << "MediaPlayerTimeChanged - new_time: " << pEvt->u.media_player_time_changed.new_time;
        if (player != Q_NULLPTR)
            player->updatePosition();
        break;
    }
}

/********** VlcPlayer **********/
VlcPlayer::VlcPlayer()
    : d_ptr(new VlcPlayerPrivate())
{
    d_ptr->setEventHandler(&handleVLCEvents, this);
}

VlcPlayer::~VlcPlayer()
{
    delete d_ptr;
}

void VlcPlayer::setOutputWindow(void* pHwnd)
{
    d_ptr->setOutputWindow(pHwnd);
}

void VlcPlayer::play()
{
    d_ptr->play();
}

void VlcPlayer::pause()
{
    d_ptr->pause();
}

void VlcPlayer::stop()
{
    d_ptr->stop();
}

int64_t VlcPlayer::length()
{
    return d_ptr->length();
}

int64_t VlcPlayer::time()
{
    return d_ptr->time();
}

void VlcPlayer::setTime(int64_t newTime)
{
    d_ptr->setTime(newTime);
}

void VlcPlayer::setMute(bool mute)
{
    d_ptr->setMute(mute);
}

bool VlcPlayer::mute()
{
    return d_ptr->mute();
}

int VlcPlayer::volume()
{
    return d_ptr->volume();
}

void VlcPlayer::setVolume(int volume)
{
    d_ptr->setVolume(volume);
}

void VlcPlayer::openMedia(const char* pMediaPathName)
{
    d_ptr->openMedia(pMediaPathName);
}

void VlcPlayer::updatePosition()
{
    // 当前进度、总时长
    emit timeChanged(time(), length());
}
