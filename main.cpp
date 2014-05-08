#include "videowidget.h"
#include "gen-cpp/Tracking.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    VideoPlayer program;

    program.show();

    return app.exec();
}
