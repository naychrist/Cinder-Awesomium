#include "cinder/ImageIO.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "CinderAwesomium.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public App {
public:
	void setup();
	void shutdown();
	void update();
	void draw();
	
	void resize();
	
	void mouseMove( MouseEvent event );	
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	void mouseWheel( MouseEvent event );	
	
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );
private:
	Awesomium::WebCore*		mWebCorePtr;
	Awesomium::WebView*		mWebViewPtr;
	
	gl::TextureRef			mWebTexture;
	gl::TextureRef			mLoadingTexture;

	Font					mFont;
};

void _TBOX_PREFIX_App::setup()
{
	setWindowSize( 1280, 720 );
	// set Awesomium logging to verbose
	Awesomium::WebConfig cnf;
	cnf.log_level = Awesomium::kLogLevel_Verbose;
#if defined( CINDER_MAC )
	std::string frameworkPath = ( getAppPath() / "Contents" / "MacOS" ).string();
	cnf.package_path = Awesomium::WebString::CreateFromUTF8( frameworkPath.c_str(), frameworkPath.size() );
#endif

	// initialize the Awesomium web engine
	mWebCorePtr = Awesomium::WebCore::Initialize( cnf );

	// create a webview
	mWebViewPtr = mWebCorePtr->CreateWebView( getWindowWidth(), getWindowHeight() );
	mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( "http://libcinder.org" ) ) );
	mWebViewPtr->Focus();

	// load and create a "loading" icon
	try { mLoadingTexture = gl::Texture::create( loadImage( loadAsset( "loading.png" ) ) ); }
	catch( const std::exception &e ) { console() << "Error loading asset: " << e.what() << std::endl; }
}

void _TBOX_PREFIX_App::shutdown()
{
	// properly shutdown Awesomium on exit
	if( mWebViewPtr ) mWebViewPtr->Destroy();
	Awesomium::WebCore::Shutdown();
}

void _TBOX_PREFIX_App::update()
{
	// update the Awesomium engine
	mWebCorePtr->Update();

	// create or update our OpenGL Texture from the webview
	if( ph::awesomium::isDirty( mWebViewPtr ) ) 
	{
		try {
			// set texture filter to NEAREST if you don't intend to transform (scale, rotate) it
			gl::Texture::Format fmt; 
			fmt.setMagFilter( GL_NEAREST );
			fmt.loadTopDown(true);

			// get the texture using a handy conversion function
			mWebTexture = ph::awesomium::toTexture( mWebViewPtr, fmt );
		}
		catch( const std::exception &e ) {
			console() << e.what() << std::endl;
		}

		// update the window title to reflect the loaded content
		char title[1024];
		mWebViewPtr->title().ToUTF8( title, 1024 );

		app::getWindow()->setTitle( title );
	}
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear(); 

	if( mWebTexture )
	{
		gl::color( Color::white() );
		gl::draw( mWebTexture );
	}

	// show spinner while loading 
	if( mLoadingTexture && mWebViewPtr && mWebViewPtr->IsLoading() )
	{
		gl::pushModelView();

		gl::translate( 0.5f * vec2( getWindowSize() ) );
		gl::scale( 0.5f, 0.5f );
		gl::rotate( 180.0f * float( getElapsedSeconds() ) );
		gl::translate( -0.5f * vec2( mLoadingTexture->getSize() ) );
		
		gl::color( Color::white() );
		gl::enableAlphaBlending();
		gl::draw( mLoadingTexture );
		gl::disableAlphaBlending();

		gl::popModelView();
	}
}

void _TBOX_PREFIX_App::resize()
{
	// resize webview if window resizes
	if( mWebViewPtr )
		mWebViewPtr->Resize( getWindowWidth(), getWindowHeight() );
}

void _TBOX_PREFIX_App::mouseMove( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseMove( mWebViewPtr, event );
}

void _TBOX_PREFIX_App::mouseDown( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseDown( mWebViewPtr, event );
}

void _TBOX_PREFIX_App::mouseDrag( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseDrag( mWebViewPtr, event );
}

void _TBOX_PREFIX_App::mouseUp( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseUp( mWebViewPtr, event );
}

void _TBOX_PREFIX_App::mouseWheel( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseWheel( mWebViewPtr, event );
}

void _TBOX_PREFIX_App::keyDown( KeyEvent event )
{
	// send key events to Awesomium
	ph::awesomium::handleKeyDown( mWebViewPtr, event );
}

void _TBOX_PREFIX_App::keyUp( KeyEvent event )
{
	// send key events to Awesomium
	ph::awesomium::handleKeyUp( mWebViewPtr, event );
}

CINDER_APP( _TBOX_PREFIX_App, RendererGl )
