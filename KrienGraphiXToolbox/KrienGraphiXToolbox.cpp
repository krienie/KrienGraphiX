
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include <qfiledialog.h>

#include <KGXCore.h>
#include <KGObjectParser.h>
#include <IOManager.h>
#include <Camera.h>
#include <RenderableObject.h>
#include <PixelShader.h>
#include <ConfigManager.h>
#include <TextureManager.h>

#include "parsers/KGTProjectParser.h"
#include "parsers/KGSceneParser.h"
#include "generators/KGTProjectGenerator.h"
#include "generators/KGSceneGenerator.h"
#include "KrienGraphiXToolbox.h"

namespace bfs = boost::filesystem;

namespace
{
	const std::string SCENE_FOLDER    = "scenes";
	const std::string RESOURCE_FOLDER = "resources";
}


namespace kgt
{
	KrienGraphiXToolbox::KrienGraphiXToolbox( QWidget *parent )
		: QMainWindow(parent), m_projectDir(), m_mainCam(nullptr), m_currentScene(nullptr),
			m_leftMouseBtnDown(false), m_wKeyDown(false), m_sKeyDown(false),
			m_aKeyDown(false), m_dKeyDown(false)
	{
		m_ui.setupUi( this );
		m_ui.renderWidget1->initialize();
		m_ui.renderWidget1->addFrameListener( this );
		m_ui.renderWidget1->addMouseListener( this );
		m_ui.renderWidget1->addKeyboardListener( this );

		QRect widgetGeom = m_ui.renderWidget1->geometry();
		float aspectRatio = float(widgetGeom.width()) / widgetGeom.height();

		//try and load project directory from KGConfig
		m_projectDir = kgx::ConfigManager::getInst()->getProperty<std::string>( "projectFolder" );
		if ( m_projectDir.size() == 0 )
			setProjectFolder();

		// add project work directory to IOManager
		kgx::IOManager::getInst()->addSearchPath( m_projectDir );

		m_ui.renderWidget1->startRendering();

		QObject::connect( m_ui.actionNew, &QAction::triggered, this, &KrienGraphiXToolbox::createNewScene );
		QObject::connect( m_ui.actionOpen, &QAction::triggered, this, &KrienGraphiXToolbox::openSceneFile );
		QObject::connect( m_ui.actionSave, &QAction::triggered, this, &KrienGraphiXToolbox::saveSceneFile );
		QObject::connect( m_ui.actionSave_as, &QAction::triggered, this, &KrienGraphiXToolbox::saveSceneAsNewFile );
		QObject::connect( m_ui.actionSetProjectFolder, &QAction::triggered, this, &KrienGraphiXToolbox::setProjectFolder );
		QObject::connect( m_ui.actionExit, &QAction::triggered, this, &KrienGraphiXToolbox::exitProgram );
	}

	KrienGraphiXToolbox::~KrienGraphiXToolbox()
	{
		if ( m_currentScene )
			delete m_currentScene;

		kgx::KGXCore::destroy();
	}


	void KrienGraphiXToolbox::frameUpdate( double deltaTime )
	{
		if ( !m_mainCam )
			return;

		float speed = static_cast<float>(150.0 * deltaTime);
		
		if ( m_wKeyDown )
			m_mainCam->moveForward( speed );
		if ( m_sKeyDown )
			m_mainCam->moveBackward( speed );
		if ( m_aKeyDown )
			m_mainCam->moveLeft( speed );
		if ( m_dKeyDown )
			m_mainCam->moveRight( speed );
	}


	void KrienGraphiXToolbox::mouseMoved( const MouseEvent &evt )
	{
		if ( m_mainCam && m_leftMouseBtnDown )
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
		//TODO: implement
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


	void KrienGraphiXToolbox::createNewScene()
	{
		std::cout << "Error (KrienGraphiXToolbox::createNewScene): Not implemented." << std::endl;
	}
	void KrienGraphiXToolbox::openSceneFile()
	{
		// get scene filename
		bfs::path sceneDirPath = bfs::path( m_projectDir ).append( SCENE_FOLDER );
		std::string sceneFile = QFileDialog::getOpenFileName( this, tr( "Open scene file" ), tr(sceneDirPath.string().c_str()),
															  tr( "KrienGraphiX Scene File (*.kgscene)" ) ).toStdString();
		if ( sceneFile.size() <= 0 )
			return;
		
		// unload current scene
		if ( m_currentScene )
			delete m_currentScene;
		kgx::KGXCore::getInst()->clearManagers();

		// add project work directory to IOManager
		kgx::IOManager::getInst()->addSearchPath( m_projectDir );


		// parse and load scene file
		m_currentScene = kgx::KGSceneParser::loadKGScene( sceneFile, m_ui.renderWidget1->getRenderWindow() );
		if ( m_currentScene && m_currentScene->getDefaultCamera() )
		{
			m_mainCam = m_currentScene->getDefaultCamera();

			//TODO: change interface for setting output for a camera below to something like: m_mainCam->setRenderTarget( m_ui.renderWidget1->getRenderWindow() );
			m_ui.renderWidget1->getRenderWindow()->setViewport( m_mainCam );
		}

		//TODO: enable code below
		// set window title
		/*std::stringstream titleSS;
		titleSS << "KrienGraphiX Toolbox - " << projData.projectName;
		setWindowTitle( QString( titleSS.str().c_str() ) );*/

	}
	void KrienGraphiXToolbox::saveSceneFile()
	{
		std::cout << "Error (KrienGraphiXToolbox::saveSceneFile): Not implemented." << std::endl;
	}
	void KrienGraphiXToolbox::saveSceneAsNewFile()
	{
		std::cout << "Error (KrienGraphiXToolbox::saveSceneAsNewFile): Not implemented." << std::endl;
	}

	void KrienGraphiXToolbox::setProjectFolder()
	{
		std::string selectedDir = QFileDialog::getExistingDirectory( this, tr( "Select project directory" ), "" ).toStdString();
		if ( !bfs::is_directory( selectedDir ) )
		{
			std::cout << "Error (KrienGraphiXToolbox::setProjectFolder): Not a valid directory: " << selectedDir << std::endl;
			return;
		}

		// remove existing project directory from IOManager, if present
		if ( m_projectDir.size() > 0 )
			kgx::IOManager::getInst()->removeSearchPath( m_projectDir );

		// save project directory in KG configuration file
		kgx::ConfigManager::getInst()->setProperty( "projectFolder", selectedDir );
		m_projectDir = selectedDir;

		// create scene folders
		bfs::path scenePath = bfs::path( m_projectDir ).append( SCENE_FOLDER );
		if ( !bfs::exists(scenePath) )
			bfs::create_directory( scenePath );

		// create resource folder
		bfs::path resourcePath = bfs::path( m_projectDir ).append( RESOURCE_FOLDER );
		if ( !bfs::exists( resourcePath ) )
			bfs::create_directory( resourcePath );
	}

	void KrienGraphiXToolbox::exitProgram()
	{
		this->close();
	}


	void KrienGraphiXToolbox::loadScene( const std::string &sceneFile )
	{
		
	}
}
