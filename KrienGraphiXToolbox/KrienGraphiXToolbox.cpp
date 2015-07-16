
#include <iostream>

#include <KgParser.h>
#include <KGXCore.h>
#include <Camera.h>
#include <RenderableObject.h>
#include <PixelShader.h>
#include <TextureManager.h>

#include "KrienGraphiXToolbox.h"


namespace kgxt
{
	KrienGraphiXToolbox::KrienGraphiXToolbox( const std::string &kgoScene, QWidget *parent )
		: QMainWindow(parent), m_materialEditorWin(nullptr), m_mainCam(nullptr), m_defaultScene(nullptr),
		m_leftMouseBtnDown(false), m_wKeyDown(false), m_sKeyDown(false), m_aKeyDown(false), m_dKeyDown(false)
	{
		m_ui.setupUi( this );


		m_ui.renderWidget1->initialize();
		m_ui.renderWidget1->addFrameListener( this );
		m_ui.renderWidget1->addMouseListener( this );
		m_ui.renderWidget1->addKeyboardListener( this );


		QRect widgetGeom = m_ui.renderWidget1->geometry();
		float aspectRatio = static_cast<float>(widgetGeom.width()) / widgetGeom.height();

		m_mainCam = new kgx::Camera( DirectX::XM_PIDIV4, aspectRatio, 0.001f, 3000.0f,
								   DirectX::XMFLOAT3( 50.0f, 50.0f, 50.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f ) );
		m_defaultScene = new kgx::Scene();
		m_mainCam->setParentScene( m_defaultScene );

		m_ui.renderWidget1->getRenderWindow()->setViewport( m_mainCam );


		if ( kgoScene.size() > 0 )
		{
			kgx::RenderableObject *renObj = kgx::KgParser::loadKGO( kgoScene );

			//TODO: temporary
			/*kgx::Texture *tex = kgx::TextureManager::getInst()->loadTexture( L"uvCheckerboard.jpg" );
			kgx::RenderableObject::ObjectIterator it = renObj->getChildrenBegin();
			for ( it; it != renObj->getChildrenEnd(); ++it )
			{
				kgx::PixelShader *pixShader = it->mat->getPixelShader();
				pixShader->addTexture( tex );
			}*/

			// add RenderableObject to scene
			m_defaultScene->claimRenderableObject( renObj );
		}

		m_ui.renderWidget1->startRendering();

		QObject::connect( m_ui.actionMaterial_Editor, &QAction::triggered, this, &KrienGraphiXToolbox::openMaterialEditor );
	}

	KrienGraphiXToolbox::~KrienGraphiXToolbox()
	{
		if ( m_materialEditorWin )
		{
			m_materialEditorWin->close();
			delete m_materialEditorWin;
		}

		if ( m_mainCam )
			delete m_mainCam;
		if ( m_defaultScene )
			delete m_defaultScene;

		kgx::KGXCore::destroy();
	}


	void KrienGraphiXToolbox::setupTestScene()
	{

	}


	void KrienGraphiXToolbox::openMaterialEditor()
	{
		if ( !m_materialEditorWin )
			m_materialEditorWin = new MaterialEditor( this );

		m_materialEditorWin->show();
	}


	void KrienGraphiXToolbox::frameUpdate( double deltaTime )
	{
		float speed = static_cast<float>(500.0 * deltaTime);
		
		if ( m_wKeyDown )
			m_mainCam->moveForward( speed );
		if ( m_sKeyDown )
			m_mainCam->moveBackward( speed );
		if ( m_aKeyDown )
			m_mainCam->moveLeft( speed );
		if ( m_dKeyDown )
			m_mainCam->moveRight( speed );
	}


	//TODO: migrate these input methods to a separate ToolboxScene class or something like that
	void KrienGraphiXToolbox::mouseMoved( const MouseEvent &evt )
	{
		if ( m_leftMouseBtnDown )
		{
			m_mainCam->rotateRight( float(-evt.X().rel()) * 0.5f );
			m_mainCam->rotateUp( float(evt.Y().rel()) * 0.5f );
		}
	}

	void KrienGraphiXToolbox::mousePressed( const MouseEvent &evt )
	{
		if ( evt.button() == MouseEvent::Button::LeftButton )
			m_leftMouseBtnDown = true;
	}

	void KrienGraphiXToolbox::mouseReleased( const MouseEvent &evt )
	{
		if ( evt.button() == MouseEvent::Button::LeftButton )
			m_leftMouseBtnDown = false;
	}

	void KrienGraphiXToolbox::wheelEvent( const MouseEvent &evt )
	{

	}

	void KrienGraphiXToolbox::keyPressed( const KeyEvent &evt )
	{
		switch ( evt.keyCode() )
		{
			case KeyEvent::Key::Key_W:
				m_wKeyDown = true;
				break;
			case KeyEvent::Key::Key_S:
				m_sKeyDown = true;
				break;
			case KeyEvent::Key::Key_A:
				m_aKeyDown = true;
				break;
			case KeyEvent::Key::Key_D:
				m_dKeyDown = true;
				break;
			case KeyEvent::Key::Key_Return:
				if ( (evt.modifiers() & KeyEvent::KeyModifiers::Alt) == KeyEvent::KeyModifiers::Alt )
					m_ui.renderWidget1->toggleFullscreen();
				break;
			default:
				break;
		}
	}

	void KrienGraphiXToolbox::keyReleased( const KeyEvent &evt )
	{
		switch ( evt.keyCode() )
		{
			case KeyEvent::Key::Key_W:
				m_wKeyDown = false;
				break;
			case KeyEvent::Key::Key_S:
				m_sKeyDown = false;
				break;
			case KeyEvent::Key::Key_A:
				m_aKeyDown = false;
				break;
			case KeyEvent::Key::Key_D:
				m_dKeyDown = false;
				break;
			default:
				break;
		}
	}
}
