#pragma once
#ifndef OGRE_ZENHEADER_INCLUDED
#define OGRE_ZENHEADER_INCLUDED

#include "ZenVectorMath.hpp"

using Vec3 = Ogre::Vector3;
using Vec2 = Ogre::Vector2;

#ifdef _DEBUG
// #define DBG( s )            \
// {                             \
//    std::ostringstream os_;    \
//    os_ << s << "\n";                   \
//    OutputDebugString( os_.str().c_str() );  \
// }

#define DBG( s ) \
{							\
	OutputDebugString( s );   \
	OutputDebugString("\n"); \
}
#else
#define DBG( s )
#endif //_DEBUG
#endif