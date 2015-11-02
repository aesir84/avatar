#pragma once

namespace avatar
{
	namespace fpv_utils
	{

		struct TwoAxisFpvAngleCalculator
		{

			enum class Axis { X, Y };

			int operator()(Axis axis, float eulerAngle) const
			{
				// It is more convenient to deal with angles in degrees.
				//
				float eulerAngleInDegrees = qRadiansToDegrees(eulerAngle);


				//
				// Limit the angles that will be passed to the servo.
				//

				// Define the minimum and the maximum limitations.
				// Usually the servos have a rotation range
				// of 180 degrees, i.e. 90 degrees in both sides.
				// Though edge values should be avoided, as it leads
				// to heavy current loads on the servos.
				//
				float const minEulerAngleInDegrees = -88.0f;
				float const maxEulerAngleInDegrees = 88.0f;

				// If an incoming angle exceeds these limitations,
				// then the corresponding extreme value is used.
				//
				if (eulerAngleInDegrees > maxEulerAngleInDegrees)
				{
					eulerAngleInDegrees = maxEulerAngleInDegrees;
				}
				else if (eulerAngleInDegrees < minEulerAngleInDegrees)
				{
					eulerAngleInDegrees = minEulerAngleInDegrees;
				}


				//
				// Transform euler angle to SG90 angle value.
				//
				//
				// Difference between Euler and this particular FPV angle representation.
				//
				//  Euler (CCW system, right handed):
				//
				//           180         90         0         -90       -180
				//            |----------|----------|----------|----------|
				//
				//  Y axis:
				//
				//                      180        90          0
				//                       |----------|----------|
				//                      left                 right
				//
				//  X axis:
				//
				//                       0        90          180
				//                       |----------|----------|
				//                       up                  down
				//

				int fpvAxisAngle = 0;

				if (axis == Axis::X)
				{
					fpvAxisAngle = static_cast<int>(90.0f - eulerAngleInDegrees);
				}
				else if (axis == Axis::Y)
				{
					fpvAxisAngle = static_cast<int>(90.0f + eulerAngleInDegrees);
				}
				else
				{
					Q_ASSERT(false);
				}
				return fpvAxisAngle;
			}

		};
	}
}
