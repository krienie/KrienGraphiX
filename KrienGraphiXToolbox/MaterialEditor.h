
#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_MaterialEditor.h"

namespace kgxt
{
	class MaterialEditor : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit MaterialEditor( QWidget *parent = nullptr );
		~MaterialEditor();

	private:
		void setupTestScene();

		Ui::MaterialEditorWindow m_ui;
	};
}
