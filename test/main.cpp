
#include <QtWidgets/QApplication>

#include "RenderCoreTestWindow.h"

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    RenderCore::Test::RenderCoreTestWindow w;
    w.show();
    return a.exec();
}