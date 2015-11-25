
#include <QtWidgets/QApplication>

#include "KrienGraphiXToolbox.h"

int main( int argc, char *argv[] )
{
	QApplication a(argc, argv);

	kgt::KrienGraphiXToolbox w;
	w.show();
	return a.exec();
}
