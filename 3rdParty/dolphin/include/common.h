/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef COMMON_H
#define COMMON_H

namespace Dolphin
{
	class Position
	{
		double x, y, z;
		Position(double x, double y, double z) : x(x), y(y), z(z) {}
	};
}
#endif //COMMON_H
