
#include "CTerraFoliageFactory.h"
#include "CFoliage.h"


unsigned short TerraFoliage::GetFoliageType ( CFoliage* pFoliage )
{
	string tname = pFoliage->GetTypeName();

	unsigned short val = GetFoliageType( tname );
	if ( val == (unsigned short)(-1) )
	{
		std::cout << "Warning: Given object '" << tname << "' has no valid Foliage type." << std::endl;
	}
	return val;
}
unsigned short TerraFoliage::GetFoliageType ( string const& stName )
{
	if ( stName == "CFoliage" )
		return 0;
	else if ( stName == "CTreeBase" )
		return 1;
	else if ( stName == "FoliageBushGreen" )
		return 2;
	else if ( stName == "FoliageTreePine" )
		return 3;
	else if ( stName == "FoliageCactusDefault" )
		return 4;
	else if ( stName == "FoliageTreeWarped" )
		return 5;

	return (unsigned short)(-1);
}

#include "CTreeBase.h"
#include "after/entities/foliage/nottrees/FoliageBushGreen.h"
#include "after/entities/foliage/trees/FoliageTreePine.h"
#include "after/entities/foliage/nottrees/FoliageCactusDefault.h"
#include "after/entities/foliage/trees/FoliageTreeWarped.h"

CFoliage* TerraFoliage::CreateFoliage ( unsigned short iType, Vector3d const& vInPos, const char* iInData )
{
	CFoliage* result = NULL;
	if ( iType == 0 ) {
		std::cout << "Invalid foliage type" << std::endl;
	}
	else if ( iType == 1 )
	{
		result = new CTreeBase();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 2 )
	{
		result = new FoliageBushGreen();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 3 )
	{
		result = new FoliageTreePine();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 4 )
	{
		result = new FoliageCactusDefault();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 5 )
	{
		result = new FoliageTreeWarped();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}

	return result;
}
