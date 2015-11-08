#include "CVoxelTerrain.h"

bool CVoxelTerrain::SideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, unsigned int const i, int const width, EFaceDir const dir )
{
	// TODO (not done but will work 4 now)
	/*
	if (( data[i+1024] == 0 ))
		glCallList( iFaceList+TOP-1 );
	if (( data[i-1024] == 0 ))
		glCallList( iFaceList+BOTTOM-1 );
	if (( data[i+32] == 0 ))
		glCallList( iFaceList+BACK-1 );
	if (( data[i-32] == 0 ))
		glCallList( iFaceList+FRONT-1 );
	if (( data[i+1] == 0 ))
		glCallList( iFaceList+RIGHT-1 );
	if (( data[i-1] == 0 ))
		glCallList( iFaceList+LEFT-1 );
		*/
	static unsigned char checkdata = EB_NONE;
	switch ( dir )
	{
	case FRONT:
		if ( i%width != width-1 )
		{
			checkdata = data[i+1];
		}
		else
		{
			if ( index8%2 != 1 )
			{
				checkdata = pBoob->data[index16].data[index8+1].data[i-(width-1)].block;
			}
			else if ( index16%2 != 1 )
			{
				checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-1)].block;
			}
			else if ( pBoob->front != NULL )
			{
				checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-1)].block;
			}
		}
		break;
	case BACK:
		if ( i%width != 0 )
		{
			checkdata = data[i-1];
		}
		else
		{
			if ( index8%2 != 0 )
			{
				checkdata = pBoob->data[index16].data[index8-1].data[i+(width-1)].block;
			}
			else if ( index16%2 != 0 )
			{
				checkdata = pBoob->data[index16-1].data[index8+1].data[i+(width-1)].block;
			}
			else if ( pBoob->back != NULL )
			{
				checkdata = pBoob->back->data[index16+1].data[index8+1].data[i+(width-1)].block;
			}
		}
		break;
	case LEFT:
		if ( ((i/width)%width) != width-1 )
		{
			checkdata = data[i+width];
		}
		else
		{
			if ( ((index8/2)%2) != 1 )
			{
				checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-1))].block;
			}
			else if ( ((index16/2)%2) != 1 )
			{
				checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-1))].block;
			}
			else if ( pBoob->left != NULL )
			{
				checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-1))].block;
			}
		}
		break;
	case RIGHT:
		if ( ((i/width)%width) != 0 )
		{
			checkdata = data[i-width];
		}
		else
		{
			if ( ((index8/2)%2) != 0 )
			{
				checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-1))].block;
			}
			else if ( ((index16/2)%2) != 0 )
			{
				checkdata = pBoob->data[index16-2].data[index8+2].data[i+(width*(width-1))].block;
			}
			else if ( pBoob->right != NULL )
			{
				checkdata = pBoob->right->data[index16+2].data[index8+2].data[i+(width*(width-1))].block;
			}
		}
		break;
	case TOP:
		if ( ((i/width/width)%width) != width-1 )
		{
			checkdata = data[i+(width*width)];
		}
		else
		{
			if ( ((index8/4)%2) != 1 )
			{
				checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-1))].block;
			}
			else if ( ((index16/4)%2) != 1 )
			{
				checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-1))].block;
			}
			else if ( pBoob->top != NULL )
			{
				checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-1))].block;
			}
		}
		break;
	case BOTTOM:
		if ( ((i/width/width)%width) != 0 )
		{
			checkdata = data[i-(width*width)];
		}
		else
		{
			if ( ((index8/4)%2) != 0 )
			{
				checkdata = pBoob->data[index16].data[index8-4].data[i+(width*width*(width-1))].block;
			}
			else if ( ((index16/4)%2) != 0 )
			{
				checkdata = pBoob->data[index16-4].data[index8+4].data[i+(width*width*(width-1))].block;
			}
			else if ( pBoob->bottom != NULL )
			{
				checkdata = pBoob->bottom->data[index16+4].data[index8+4].data[i+(width*width*(width-1))].block;
			}
		}
		break;
	/*case LEFT:
		if ( ((i/width)%width) != 0 )
		{
			checkdata = data[i-width];
		}
		else
		{
			if ( ((index8/2)%2) != 0 )
			{
				checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-1))];
			}
		}
		break;
	case RIGHT:
		if ( ((i/width)%width) != width-1 )
		{
			checkdata = data[i+width];
		}
		else
		{
			if ( ((index8/2)%2) != 1 )
			{
				checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-1))];
			}
		}
		break;*/
	/*case FRONT:
		if ( ((i/width)%width) != 0 )
		{
			checkdata = data[i-width];
		}
		else
		{
			if ( ((index8/2)%2) != 0 )
			{
				checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-1))];
			}
		}
		break;
	case LEFT:
		if ( i%width != 0 )
		{
			checkdata = data[i-1];
		}
		else
		{
			if ( index8%2 != 0 )
			{
				checkdata = pBoob->data[index16].data[index8-1].data[i+(width-1)];
			}
		}
		break;
	case RIGHT:
		if ( i%width != width-1 )
		{
			checkdata = data[i+1];
		}
		else
		{
			if ( index8%2 != 1 )
			{
				checkdata = pBoob->data[index16].data[index8+1].data[i-(width-1)];
			}
		}
		break;*/
	}
	return ((checkdata==EB_NONE)||(checkdata==EB_WATER)||(checkdata==EB_TOP_SNOW));
}

bool CVoxelTerrain::SideVisible ( CBoob * pBoob, char const index16, char const index8, terra_t * data, int const i, int const width, EFaceDir const dir, unsigned char ofs )
{
	//static unsigned short checkdata;
	//checkdata = EB_DIRT;
	//static Vector3d vDir;
	//vDir = pos - vCameraPos;
	static terra_t checkdata;
	checkdata.raw = 0;
	checkdata.block = EB_ANC_GRAVITY_JOKE;

	switch ( dir )
	{
	case FRONT:
		//if ( dot.x < 0.0f )
		{
			if ( i%width < width-ofs )
			{
				checkdata = data[i+ofs];
			}
			else
			{
				if ( index8%2 != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+1].data[i-(width-ofs)];
				}
				else if ( index16%2 != 1 )
				{
					checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-ofs)];
				}
				else if ( pBoob->front != NULL )
				{
					checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-ofs)];
				}
				else
					return false;
			}
		}
		//else
		//	return false;
		break;
	case BACK:
		//if ( dot.x > 0 )
		{
			if ( i%width >= ofs )
			{
				checkdata = data[i-ofs];
			}
			else
			{
				if ( index8%2 != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-1].data[i+(width-ofs)];
				}
				else if ( index16%2 != 0 )
				{
					checkdata = pBoob->data[index16-1].data[index8+1].data[i+(width-ofs)];
				}
				else if ( pBoob->back != NULL )
				{
					checkdata = pBoob->back->data[index16+1].data[index8+1].data[i+(width-ofs)];
				}
				else
					return false;
			}
		}
		//else
		//	return false;
		break;
	case LEFT:
		//if ( dot.y < 0 )
		{
			if ( ((i/width)%width) < width-ofs )
			{
				checkdata = data[i+(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-ofs))];
				}
				else if ( ((index16/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-ofs))];
				}
				else if ( pBoob->left != NULL )
				{
					checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-ofs))];
				}
				else
					return false;
			}
		}
		//else
		//	return false;
		break;
	case RIGHT:
		//if ( dot.y > 0 )
		{
			if ( ((i/width)%width) >= ofs )
			{
				checkdata = data[i-(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-ofs))];
				}
				else if ( ((index16/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16-2].data[index8+2].data[i+(width*(width-ofs))];
				}
				else if ( pBoob->right != NULL )
				{
					checkdata = pBoob->right->data[index16+2].data[index8+2].data[i+(width*(width-ofs))];
				}
				else
					return false;
			}
		}
		//else
		//	return false;
		break;
	case TOP:
		//if ( dot.z < 0 )
		{
			if ( ((i/width/width)%width) != width-ofs )
			{
				checkdata = data[i+(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-ofs))];
				}
				else if ( ((index16/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-ofs))];
				}
				else if ( pBoob->top != NULL )
				{
					checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-ofs))];
				}
				else
					return false;
			}
		}
		//else
		//	return false;
		break;
	case BOTTOM:
		//if ( dot.z > 0 )
		{
			if ( ((i/width/width)%width) >= ofs )
			{
				checkdata = data[i-(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-4].data[i+(width*width*(width-ofs))];
				}
				else if ( ((index16/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16-4].data[index8+4].data[i+(width*width*(width-ofs))];
				}
				else if ( pBoob->bottom != NULL )
				{
					checkdata = pBoob->bottom->data[index16+4].data[index8+4].data[i+(width*width*(width-ofs))];
				}
				else
					return false;
			}
		}
		//else
		//	return false;
		break;
	}
	cLastCheckedBlock = checkdata.block;
	return ((cLastCheckedBlock==EB_NONE)||(cLastCheckedBlock==EB_WATER)||(cLastCheckedBlock==EB_TOP_SNOW));
}

bool CVoxelTerrain::InSideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs )
{
	static unsigned char checkdata = EB_DIRT;
	//static Vector3d vDir;
	//vDir = pos - vCameraPos;

	switch ( dir )
	{
	case FRONT:
		//if ( dot.x < 0.0f )
		{
			if ( i%width < width-ofs )
			{
				checkdata = data[i+ofs];
			}
			else
			{
				if ( index8%2 != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+1].data[i-(width-ofs)].block;
				}
				else if ( index16%2 != 1 )
				{
					checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-ofs)].block;
				}
				else if ( pBoob->front != NULL )
				{
					checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-ofs)].block;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case BACK:
		//if ( dot.x > 0 )
		{
			if ( i%width >= ofs )
			{
				checkdata = data[i-ofs];
			}
			else
			{
				if ( index8%2 != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-1].data[i+(width-ofs)].block;
				}
				else if ( index16%2 != 0 )
				{
					checkdata = pBoob->data[index16-1].data[index8+1].data[i+(width-ofs)].block;
				}
				else if ( pBoob->back != NULL )
				{
					checkdata = pBoob->back->data[index16+1].data[index8+1].data[i+(width-ofs)].block;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case LEFT:
		//if ( dot.y < 0 )
		{
			if ( ((i/width)%width) < width-ofs )
			{
				checkdata = data[i+(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-ofs))].block;
				}
				else if ( ((index16/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-ofs))].block;
				}
				else if ( pBoob->left != NULL )
				{
					checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-ofs))].block;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case RIGHT:
		//if ( dot.y > 0 )
		{
			if ( ((i/width)%width) >= ofs )
			{
				checkdata = data[i-(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-ofs))].block;
				}
				else if ( ((index16/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16-2].data[index8+2].data[i+(width*(width-ofs))].block;
				}
				else if ( pBoob->right != NULL )
				{
					checkdata = pBoob->right->data[index16+2].data[index8+2].data[i+(width*(width-ofs))].block;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case TOP:
		//if ( dot.z < 0 )
		{
			if ( ((i/width/width)%width) != width-ofs )
			{
				checkdata = data[i+(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-ofs))].block;
				}
				else if ( ((index16/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-ofs))].block;
				}
				else if ( pBoob->top != NULL )
				{
					checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-ofs))].block;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case BOTTOM:
		//if ( dot.z > 0 )
		{
			if ( ((i/width/width)%width) >= ofs )
			{
				checkdata = data[i-(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-4].data[i+(width*width*(width-ofs))].block;
				}
				else if ( ((index16/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16-4].data[index8+4].data[i+(width*width*(width-ofs))].block;
				}
				else if ( pBoob->bottom != NULL )
				{
					checkdata = pBoob->bottom->data[index16+4].data[index8+4].data[i+(width*width*(width-ofs))].block;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	}
	//return (checkdata==0);
	//return_block_type = checkdata;
	cLastCheckedBlock = checkdata;
	return ((checkdata!=EB_NONE)&&(checkdata!=EB_WATER));
}

