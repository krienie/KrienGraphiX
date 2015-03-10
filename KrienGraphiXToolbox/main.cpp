
#include "KrienGraphiXToolbox.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	//TODO: put toolbox classes in a namespace

	QApplication a(argc, argv);
	KrienGraphiXToolbox w;
	w.show();
	return a.exec();
}
