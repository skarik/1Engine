
#include "Rotator.h"


// Out stream overload
std::ostream& operator<< (std::ostream& out, Rotator const& matx)
{
	out << matx.pData[0] << "\t" << matx.pData[1] << "\t" << matx.pData[2] << std::endl;
	out << matx.pData[3] << "\t" << matx.pData[4] << "\t" << matx.pData[5] << std::endl;
	out << matx.pData[6] << "\t" << matx.pData[7] << "\t" << matx.pData[8] << std::endl;
	return out;
}