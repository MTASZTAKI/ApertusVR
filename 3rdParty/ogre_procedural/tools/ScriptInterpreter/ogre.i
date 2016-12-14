%module Ogre
%include "std_map.i"
 %include "std_string.i"
 %include "std_vector.i"
 %{ 
 #include "Ogre.h" 
 #include <hash_map>
 %}
 %include "OgreBuildSettings.h"
 %include "OgrePlatform.h"
 %include "OgrePrerequisites.h"  
 #if OGRE_THREAD_PROVIDER == 0
	%include "Threading\OgreThreadDefinesNone.h"
#elif OGRE_THREAD_PROVIDER == 1
	%include "Threading\OgreThreadDefinesBoost.h"
#elif OGRE_THREAD_PROVIDER == 2
	%include "Threading\OgreThreadDefinesPoco.h"
#elif OGRE_THREAD_PROVIDER == 3
	%include "Threading\OgreThreadDefinesTBB.h"
#endif
 %include "OgreVector2.h"
 %include "OgreVector3.h"
 %include "OgreVector4.h"
 %include "OgreQuaternion.h"
 %include "OgreMath.h" 
 %include "OgreCommon.h" 
 %template(Rect) Ogre::TRect<long>;
%template(RealRect) Ogre::TRect<Ogre::Real>;
%include "OgreColourValue.h"