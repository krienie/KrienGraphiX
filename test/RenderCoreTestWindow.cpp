
#include "RenderCoreTestWindow.h"

#include "Core/GpuRenderEngine.h"

namespace RenderCore::Test
{
    RenderCoreTestWindow::RenderCoreTestWindow( QWidget *parent )
        : QMainWindow( parent ), m_ui()
    {
        m_ui.setupUi(this);

        Core::GpuRenderEngine::get()->initialize();

        // Create render window target and start the main loop
        //kgx::KGXCore::get()->startMainLoop( kgx::KGXCore::get()->createRenderWindow(HWND(m_ui.renderWidget1->winId())) );
    }

    RenderCoreTestWindow::~RenderCoreTestWindow()
    {
        Core::GpuRenderEngine::shutdown();
    }
}
