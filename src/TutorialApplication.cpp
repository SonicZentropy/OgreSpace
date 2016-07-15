/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#include "TutorialApplication.h"


TutorialApplication::TutorialApplication()
{
}

TutorialApplication::~TutorialApplication()
{
}

void TutorialApplication::createScene()
{
	mSceneMgr->setAmbientLight(ColourValue(.25, .25, .25));

	Light* pointLight = mSceneMgr->createLight("PointLight");
	pointLight->setType(Light::LT_POINT);
	pointLight->setPosition(250, 150, 250);
	pointLight->setDiffuseColour(ColourValue::White);
	pointLight->setSpecularColour(ColourValue::White);

	Entity* ninjaEntity = mSceneMgr->createEntity("ninja.mesh");
	SceneNode* ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
	ninjaNode->attachObject(ninjaEntity);

}

bool TutorialApplication::frameRenderingQueued(const FrameEvent& fe)
{
	bool ret = BaseApplication::frameRenderingQueued(fe);

	return ret;
}

bool TutorialApplication::processUnbufferedInput(const FrameEvent& fe)
{
	return true;
}
//---------------------------------------------------------------------------
//==============================================================================
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch(Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
