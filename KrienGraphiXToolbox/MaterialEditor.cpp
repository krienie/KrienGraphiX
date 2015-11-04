
#include <KGXCore.h>

#include "MaterialEditor.h"

namespace kgt
{
	MaterialEditor::MaterialEditor( QWidget *parent )
		: QMainWindow( parent )
	{
		m_ui.setupUi( this );
	}

	MaterialEditor::~MaterialEditor()
	{
	}
}
