
#include "KrienGraphiXToolbox.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	kgxt::KrienGraphiXToolbox w;
	w.show();
	return a.exec();
}
