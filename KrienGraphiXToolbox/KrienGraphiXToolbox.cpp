
#define NOMINMAX

#include "KrienGraphiXToolbox.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <qfiledialog.h>

#include <kriengraphix/Core/KGXCore.h>
#include <kriengraphix/Core/PhysXManager.h>
#include <kriengraphix/Core/ConfigManager.h>
#include <kriengraphix/IO/Filesystem.h>
#include <kriengraphix/IO/KGSceneParser.h>
#include <kriengraphix/Simulation/Camera.h>

namespace bfs = boost::filesystem;

namespace
{
    const std::string SCENE_FOLDER = "scenes";
    const std::string RESOURCE_FOLDER = "resources";
    const unsigned int MOUSE_BUFFER_FILTER_SIZE = 10u;

    bool animateScene = false;
}


namespace kgt
{
    KrienGraphiXToolbox::KrienGraphiXToolbox( QWidget *parent )
        : QMainWindow( parent ), m_projectDir(), m_mainCam( nullptr ), m_currentScene( nullptr ),
        m_mouseXFilterBuffer(), m_mouseYFilterBuffer(), m_leftMouseBtnDown( false ), m_wKeyDown( false ), m_sKeyDown( false ),
        m_aKeyDown( false ), m_dKeyDown( false )
    {
        m_ui.setupUi( this );
        m_ui.renderWidget1->initialize();
        m_ui.renderWidget1->addFrameListener( this );
        m_ui.renderWidget1->addMouseListener( this );
        m_ui.renderWidget1->addKeyboardListener( this );

        // reserve space for mouse movement filtering
        m_mouseXFilterBuffer.reserve( MOUSE_BUFFER_FILTER_SIZE );
        m_mouseYFilterBuffer.reserve( MOUSE_BUFFER_FILTER_SIZE );

        //try and load project directory from KGConfig
        m_projectDir = kgx::ConfigManager::get()->getProperty<std::string>( "projectFolder" );
        if ( m_projectDir.size() == 0 )
            setProjectFolder();

        m_ui.renderWidget1->startRendering();

        connect( m_ui.actionNew, &QAction::triggered, this, &KrienGraphiXToolbox::createNewScene );
        connect( m_ui.actionOpen, &QAction::triggered, this, &KrienGraphiXToolbox::openSceneFile );
        connect( m_ui.actionSave, &QAction::triggered, this, &KrienGraphiXToolbox::saveSceneFile );
        connect( m_ui.actionSave_as, &QAction::triggered, this, &KrienGraphiXToolbox::saveSceneAsNewFile );
        connect( m_ui.actionSetProjectFolder, &QAction::triggered, this, &KrienGraphiXToolbox::setProjectFolder );
        connect( m_ui.actionExit, &QAction::triggered, this, &KrienGraphiXToolbox::exitProgram );
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

        float speed = static_cast<float>(15.0 * deltaTime);

        if ( m_wKeyDown )
            m_mainCam->moveForward( speed );
        if ( m_sKeyDown )
            m_mainCam->moveBackward( speed );
        if ( m_aKeyDown )
            m_mainCam->moveLeft( speed );
        if ( m_dKeyDown )
            m_mainCam->moveRight( speed );

        if ( animateScene )
            kgx::PhysXManager::getInst()->advance( deltaTime );
    }


    void KrienGraphiXToolbox::mouseMoved( const MouseEvent &evt )
    {
        if ( m_mainCam && m_leftMouseBtnDown )
        {
            const float MOVE_WEIGHT_MODIFIER = 0.2f;
            const float MOVE_SPEED_MODIFIER = 1.5f;

            // make sure the buffers with mouse movement history do not exceed the max length
            if ( m_mouseXFilterBuffer.size() >= MOUSE_BUFFER_FILTER_SIZE )
                m_mouseXFilterBuffer.erase( m_mouseXFilterBuffer.begin() );
            if ( m_mouseYFilterBuffer.size() >= MOUSE_BUFFER_FILTER_SIZE )
                m_mouseYFilterBuffer.erase( m_mouseYFilterBuffer.begin() );

            float relX = float( -evt.X().rel() ) * MOVE_SPEED_MODIFIER;
            float relY = float( evt.Y().rel() ) * MOVE_SPEED_MODIFIER;

            float filteredX = relX;
            float filteredY = relY;
            float weight = 1.0;
            for ( unsigned int i = 0u; i < m_mouseXFilterBuffer.size(); ++i )
            {
                weight *= MOVE_WEIGHT_MODIFIER;
                filteredX += relX * weight;
                filteredY += relY * weight;
            }
            filteredX /= std::max( 1.0f, static_cast<float>(m_mouseXFilterBuffer.size()) );
            filteredY /= std::max( 1.0f, static_cast<float>(m_mouseYFilterBuffer.size()) );

            m_mouseXFilterBuffer.push_back( relX );
            m_mouseYFilterBuffer.push_back( relY );

            // rotate camera with the filtered movement vector
            m_mainCam->rotateRight( filteredX );
            m_mainCam->rotateUp( filteredY );
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
            case KeyEvent::Key::Key_Space:
                animateScene = !animateScene;
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
        std::string sceneFile = QFileDialog::getOpenFileName( this, tr( "Open scene file" ), tr( sceneDirPath.string().c_str() ),
                                                              tr( "KrienGraphiX Scene File (*.kgscene)" ) ).toStdString();
        if ( sceneFile.empty() )
            return;

        // unload current scene
        if ( m_currentScene )
            delete m_currentScene;
        kgx::KGXCore::get()->clearManagers();

        // add project work directory to KGX filesystem
        kgx::filesystem::addSearchPath( m_projectDir );


        // parse and load scene file
        m_currentScene = kgx::KGSceneParser::loadKGScene( sceneFile, m_ui.renderWidget1->getRenderWindow() );
        if ( m_currentScene && m_currentScene->getDefaultCamera() )
        {
            m_mainCam = m_currentScene->getDefaultCamera();

            //TODO: change interface for setting output for a camera below to something like: m_mainCam->setRenderTarget( m_ui.renderWidget1->getRenderWindow() );
            m_ui.renderWidget1->getRenderWindow()->setViewport( m_mainCam );
        }

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

        // remove existing project directory from KGX filesystem, if present
        if ( m_projectDir.size() > 0 )
            kgx::filesystem::removeSearchPath( m_projectDir );

        // save project directory in KG configuration file
        kgx::ConfigManager::get()->setProperty( "projectFolder", selectedDir );
        m_projectDir = selectedDir;

        // create scene folders
        bfs::path scenePath = bfs::path( m_projectDir ).append( SCENE_FOLDER );
        if ( !bfs::exists( scenePath ) )
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
}
