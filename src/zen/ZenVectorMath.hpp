#pragma once
#ifndef ZENVECTOR3_H_INCLUDED
#define ZENVECTOR3_H_INCLUDED
#include <OgreVector3.h>

namespace Zen
{
/// Return a vector corresponding to this vector translated by the input vector
static inline Ogre::Vector3 getTranslatedVec(const Ogre::Vector3& inVector,
	const Ogre::Real x_, const Ogre::Real y_, const Ogre::Real z_)
{
	Ogre::Vector3 transVec(inVector.x + x_, inVector.y + y_, inVector.z + z_);
	return transVec;
}

class ZenVMath
{
public:

	//inline Vector3 translated(Vector3 translateVector)
	//{
	//	Vector3 transVec(x + translateVector.x, y + translateVector.y, z + translateVector.z);
	//	return transVec;
	//}
};
} // Namespace
#endif // ZENVECTOR3_H_INCLUDED
