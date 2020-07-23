#pragma once

#include <algorithm>
#include <OgrePrerequisites.h>

namespace Ogre_glTF
{
	namespace internal_utils
	{
		template <class inputContainer, class outputContainer>
		void container_double_to_real(inputContainer& in, outputContainer& out)
		{
			std::transform(std::begin(in),
						   std::end(in),
						   std::begin(out),
						   [](double n) {
							   return static_cast<Ogre::Real>(n);
						   });
		}
	}
}