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

	if (!processUnbufferedInput(fe))
		return false;

	return ret;
}

bool TutorialApplication::processUnbufferedInput(const FrameEvent& fe)
{
	static bool mouseDownLastFrame = false;
	static Real toggleTimer = 0.0;
	static Real rotate = .13;
	static Real move = 250;

	bool leftMouseDown = mMouse->getMouseState().buttonDown(OIS::MB_Left);

	if (leftMouseDown && !mouseDownLastFrame)
	{
		Light* light = mSceneMgr->getLight("PointLight");
		light->setVisible(!light->isVisible());
	}

	mouseDownLastFrame = leftMouseDown;

	toggleTimer -= fe.timeSinceLastFrame;

	if ((toggleTimer < 0) && mMouse->getMouseState().buttonDown(OIS::MB_Right))
	{
		toggleTimer = 0.5;

		Light* light = mSceneMgr->getLight("PointLight");
		light->setVisible(!light->isVisible());
	}

	Vec3 dirVec = Vec3::ZERO;

	if (mKeyboard->isKeyDown(OIS::KC_I))
		dirVec.z -= move;
	if (mKeyboard->isKeyDown(OIS::KC_K))
		dirVec.z += move;
	if (mKeyboard->isKeyDown(OIS::KC_U))
		dirVec.y += move;
	if (mKeyboard->isKeyDown(OIS::KC_O))
		dirVec.y -= move;
	if (mKeyboard->isKeyDown(OIS::KC_J))
	{
		if (mKeyboard->isKeyDown(OIS::KC_LSHIFT)) // Rotate if shift held
			mSceneMgr->getSceneNode("NinjaNode")->yaw(Ogre::Degree(5 * rotate));
		else
			dirVec.x -= move;
	}

	if (mKeyboard->isKeyDown(OIS::KC_L))
	{
		if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
			mSceneMgr->getSceneNode("NinjaNode")->yaw(Ogre::Degree(-5 * rotate));
		else
			dirVec.x += move;
	}

	mSceneMgr->getSceneNode("NinjaNode")->translate(dirVec * fe.timeSinceLastFrame, Node::TS_LOCAL);

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
