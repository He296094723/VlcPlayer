#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "vlc_player.h"

class QPushButton;
class QLabel;
class QSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = Q_NULLPTR);
    ~MainWindow();

private Q_SLOTS:
    // 打开文件
    void onLocalFileTriggered();
    // 打开文件
    void onNetworkTriggered();
    // 播放
    void onPlayBtnClicked();
    // 暂停
    void onPauseBtnClicked();
    // 停止
    void onStopBtnClicked();
    // 静音
    void onMuteBtnClicked();
    // 改变音量
    void onVolumeChanged(int value);
    void onVolumeSliderReleased();
    // 改变进度
    void onMediaChanged();
    // 更新时间
    void onTimeChanged(int position, int length);

private:
    // 初始化 UI
    void initMenuBar();
    void initCentralWidget();
    // 初始化设置
    void initSettings();

private:
    VlcPlayer m_vlcPlayer;
    bool m_muteFlag;
    int64_t m_length;
    int m_lastVolume;

    // 基本部件
    QPushButton *m_pPlayButton;
    QPushButton *m_pPauseButton;
    QPushButton *m_pStopButton;
    QPushButton *m_pMuteButton;
    QLabel *m_pPathLabel;
    QLabel *m_pVlcControlLabel;
    QLabel *m_pVolumeLevelLabel;
    QLabel *m_pMediaPositionLabel;
    QSlider *m_pMediaSlider;
    QSlider *m_pVolumeSlider;
};

#endif // MAIN_WINDOW_H

