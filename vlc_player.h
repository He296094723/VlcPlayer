#ifndef VLC_PLAYER_H
#define VLC_PLAYER_H

// 避免 ssize_t 引发的错误（VS2010 及更高版本会出现）
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <memory>
#include <QObject>
#include <vlc/vlc.h>

class VlcPlayerPrivate;

class VlcPlayer : public QObject
{
    Q_OBJECT

public:
    VlcPlayer();
    ~VlcPlayer();

    // 更新位置（时间变了）
    void updatePosition();

    // 为媒体输出设置窗口
    void setOutputWindow(void* pHwnd);

    // 打开一个媒体文件
    void openMedia(const QString &url, bool localFile = true);

    // 设置媒体的新位置（单位：毫秒）
    void setTime(int64_t newTime);

    // 设置静音状态
    void setMute(bool mute = true);

    // 获得静音状态
    bool mute();

    // 设置实际音量
    void setVolume(int volume);

    // 返回实际音量
    int volume();

signals:
    // 时间改变
    void timeChanged(int position, int length);

public Q_SLOTS:
    // 开始播放
    void play();

    // 暂停播放
    void pause();

    // 停止播放
    void stop();

private:
    // 获取媒体的长度（单位：毫秒）。需要在事件处理程序中调用，否则结果不可靠！
    int64_t length();

    // 获取媒体的实际位置（单位：毫秒）。需要在事件处理程序中调用，否则结果不可靠！
    int64_t time();

private:
    Q_DECLARE_PRIVATE(VlcPlayer)
    VlcPlayerPrivate* d_ptr;
};

#endif // VLC_PLAYER_H
