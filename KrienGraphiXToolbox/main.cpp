
#include <QtWidgets/QApplication>

#include "KrienGraphiXToolbox.h"

int main( int argc, char *argv[] )
{
	QApplication a(argc, argv);

	// /loadScene ..\\..\\Assets\\sponzaNoBanner.kgo

	std::string sceneToLoad;
	if ( argc > 1 )
		sceneToLoad = argv[1];

	kgxt::KrienGraphiXToolbox w(sceneToLoad);
	w.show();
	return a.exec();
}
