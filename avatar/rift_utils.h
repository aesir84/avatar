#pragma once

#include "stdafx.h"

namespace avatar
{

	namespace rift_utils
	{

		/// \brief A template function to conveniently process both eyes.
		///
		/// The template parameter is meant to be a lambda, so that it is possible to
		/// loop over both eyes applying some processing which is specified by the lambda.
		///
		template <typename F>
		void foreach_eye(F f)
		{
			for (int i = 0; i < ovrEye_Count; ++i)
			{
				ovrEyeType currentEye = static_cast<ovrEyeType>(i);
				f(currentEye);
			}
		}

	}

}
