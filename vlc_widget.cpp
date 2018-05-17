#include "vlc_widget.h"
#include <QGroupBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QTime>
#include <QGridLayout>
#include <QtDebug>

VlcWidget::VlcWidget(QWidget *parent)
    : QWidget(parent),
      m_length(0),
      m_lastVolume(80)
{
    resize(600, 400);

    initUI();
    initSettings();

    connect(&m_vlcPlayer, SIGNAL(timeChanged(int,int)), this, SLOT(onTimeChanged(int,int)));
}

VlcWidget::~VlcWidget()
{

}

void VlcWidget::initUI()
{
    // 创建部件
    m_pControlGroup = new QGroupBox(this);
    m_pPathLabel = new QLabel(this);
    m_pVlcControlLabel = new QLabel(this);
    m_pVolumeLevelLabel = new QLabel(this);
    m_pMediaPositionLabel = new QLabel(this);
    m_pPlayButton = new QPushButton(this);
    m_pPauseButton = new QPushButton(this);
    m_pStopButton = new QPushButton(this);
    m_pMuteButton = new QPushButton(this);
    m_pOpenButton = new QPushButton(this);
    m_pVolumeSlider = new QSlider(this);
    m_pMediaSlider = new QSlider(this);

    // 界面布局
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(m_pPathLabel);
    hLayout->addStretch();
    hLayout->addWidget(m_pMediaPositionLabel);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(0, 0, 0, 0);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addLayout(hLayout, 0, 0, 1, 8);
    gridLayout->addWidget(m_pMediaSlider, 1, 0, 1, 8);
    gridLayout->addWidget(m_pOpenButton, 2, 0);
    gridLayout->addWidget(m_pPlayButton, 2, 1);
    gridLayout->addWidget(m_pPauseButton, 2, 2);
    gridLayout->addWidget(m_pStopButton, 2, 3);
    gridLayout->addWidget(m_pMuteButton, 2, 5);
    gridLayout->addWidget(m_pVolumeSlider, 2, 6);
    gridLayout->addWidget(m_pVolumeLevelLabel, 2, 7);
    gridLayout->setColumnStretch(4, 1000);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);
    m_pControlGroup->setLayout(gridLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_pVlcControlLabel);
    mainLayout->addWidget(m_pControlGroup);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    setLayout(mainLayout);

    m_pVlcControlLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pPathLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pVolumeSlider->setOrientation(Qt::Horizontal);
    m_pMediaSlider->setOrientation(Qt::Horizontal);

    // 连接信号槽
    connect(m_pPlayButton, SIGNAL(clicked()), this, SLOT(onPlayBtnClicked()));
    connect(m_pPauseButton, SIGNAL(clicked()), this, SLOT(onPauseBtnClicked()));
    connect(m_pStopButton, SIGNAL(clicked()), this, SLOT(onStopBtnClicked()));
    connect(m_pMuteButton, SIGNAL(clicked()), this, SLOT(onMuteBtnClicked()));
    connect(m_pOpenButton, SIGNAL(clicked()), this, SLOT(onLoadBtnClicked()));
    connect(m_pVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
    connect(m_pVolumeSlider, SIGNAL(sliderReleased()), this, SLOT(onVolumeSliderReleased()));
    connect(m_pMediaSlider, SIGNAL(sliderReleased()), this, SLOT(onMediaChanged()));
}

void VlcWidget::initSettings()
{
    m_pPathLabel->setText(("VLC Player"));
    m_pControlGroup->setTitle(QStringLiteral("多媒体控制"));
    m_pOpenButton->setText(QStringLiteral("打开"));
    m_pPlayButton->setText(QStringLiteral("播放"));
    m_pPauseButton->setText(QStringLiteral("暂停"));
    m_pStopButton->setText(QStringLiteral("停止"));
    m_pMuteButton->setText(QStringLiteral("静音"));
    m_pVolumeSlider->setToolTip(QStringLiteral("调节音量"));

    m_pVolumeSlider->setRange(0, 100);
    m_pMediaSlider->setRange(0, 100);
    m_pMediaPositionLabel->setText("00:00:00/00:00:00");

    m_vlcPlayer.setOutputWindow((void*)(m_pVlcControlLabel->winId()));
    //m_vlcPlayer.setEventHandler(&handleVLCEvents, this);

    m_vlcPlayer.setVolume(m_lastVolume);
    m_pVolumeSlider->setValue(m_lastVolume);
    m_pVolumeLevelLabel->setText(QString::number(m_lastVolume));
}

void VlcWidget::onLoadBtnClicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                QDir::homePath(),
                                                tr("Multimedia files(*)"));
    if (!file.isEmpty()) {
        m_pPathLabel->setText(file);
        QString localFile = file;
        localFile = QDir::toNativeSeparators(localFile);
        m_vlcPlayer.openMedia(localFile.toUtf8().data());
        m_vlcPlayer.play();
    }
}

void VlcWidget::onPlayBtnClicked()
{
    m_vlcPlayer.play();
}

void VlcWidget::onPauseBtnClicked()
{
    m_vlcPlayer.pause();
}

void VlcWidget::onStopBtnClicked()
{
    m_vlcPlayer.stop();
    m_length = 0;
    m_pMediaSlider->setValue(0);

    // 总时长不变，只重置前面的进度
    QString length = m_pMediaPositionLabel->text();
    length = QString("00:00:00") + length.right(9);
    m_pMediaPositionLabel->setText(length);
}

void VlcWidget::onMuteBtnClicked()
{
    // 该接口也可以设置静音
    // m_vlcPlayer.setMute(true);

    int volume = m_vlcPlayer.volume();

    // 若有声音，设置为静音；否则，还原之前的音量
    if (volume > 0) {
        m_pVolumeSlider->setValue(0);
    } else {
        m_pVolumeSlider->setValue(m_lastVolume);
    }
}

void VlcWidget::onVolumeChanged(int value)
{
    m_vlcPlayer.setVolume(value);
    m_pVolumeLevelLabel->setText(QString::number(value));

    // 更改文本或者图标
    if (value > 0) {
        m_pMuteButton->setText(QStringLiteral("静音"));
    } else {
        m_pMuteButton->setText(QStringLiteral("非静音"));
    }
}

// 最后一次释放时的声音大小
void VlcWidget::onVolumeSliderReleased()
{
    int volume = m_pVolumeSlider->value();
    if (volume > 0)
        m_lastVolume = volume;
}

void VlcWidget::onMediaChanged()
{
    int value = m_pMediaSlider->value();
    m_vlcPlayer.setTime(m_length/100*value);
}

void VlcWidget::onTimeChanged(int position, int length)
{
    m_length = length;

    QTime n1(0, 0, 0);
    QTime t1;
    t1 = n1.addMSecs(position);

    QTime n2(0, 0, 0);
    QTime t2;
    t2 = n2.addMSecs(length);

    QString strLength = QString("%1/%2").arg(t1.toString("hh:mm:ss")).arg(t2.toString("hh:mm:ss"));
    m_pMediaPositionLabel->setText(strLength);

    int newSliderPos = length ? static_cast<int>((static_cast<double>(position)/static_cast<double>(length) * 100)) : 0;
    m_pMediaSlider->setValue(newSliderPos);
}
