#include "vlc_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    VlcWidget w;
    w.show();

    return a.exec();
}
