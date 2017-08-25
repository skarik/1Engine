// Sea and Storm
//(C) 2011

using UnityEngine;
using System.Collections;

public class Volumes
{
    // Enumerations
    public enum Type
    {
        Water,
        Cold
    };

    // Volumes
    public static CArrayList <CWaterVolume> globalWaterVolumes  = new CArrayList<CWaterVolume>(3);
    public static CArrayList <CColdVolume>  globalColdVolume    = new CArrayList<CColdVolume> (3);


    public static bool PositionInside ( Vector3 pos, Type e_volume_type )
	{
        CPhysicsVolume [] genericVolumes = null;
        switch ( e_volume_type )
        {
            case Type.Water:
                genericVolumes = Volumes.globalWaterVolumes.getCleanArray();
            break;
            case Type.Cold:
                genericVolumes = Volumes.globalColdVolume.getCleanArray();
            break;
            default:
                genericVolumes = null;
            break;
        }

        if ( genericVolumes != null )
        foreach ( CPhysicsVolume genericVolume in genericVolumes )
        {
            if ( genericVolume.collider.bounds.Contains( pos ) )
            {
                return true;
            }
        }

		return false;
	}

    /*public static bool PositionInside ( Vector3 pos, Type e_volume_type )
	{
		/*float _WaveHeight = 0.034f;
		float _Period = 0.08f;
		float _Time = (Time.time/2f);
		float centerDist = Mathf.Sqrt(pos.x*pos.x+pos.z*pos.z)/(100f/_WaveHeight);
		centerDist *= centerDist*4f;
		float currentY = 0;
		currentY += Mathf.Sin( (_Time*12f+(pos.x*0.4f+pos.z)*0.1f)*1.7f*_Period )*8f*centerDist;
		currentY += Mathf.Cos( (_Time*10f+(pos.x-pos.z*0.7f)*0.1f)*1.2f*_Period )*10f*centerDist;
		currentY += Mathf.Cos( (_Time*23f+(-pos.x+pos.z*0.1f)*0.1f)*1.2f*_Period )*7f*centerDist;*/
		/*if ( pos.y < OceanHeight( pos ) )
		{
			return true;
		}*/

        // OMG LOOP THROUGH THE WATER VOLUME
        //CWaterVolume [] waterVolumes = (CWaterVolume[])GameObject.FindObjectsOfType( typeof( CWaterVolume ) );
        /*CWaterVolume [] waterVolumes = Volumes.globalWaterVolumes.getCleanArray();
        if ( waterVolumes != null )
        foreach ( CWaterVolume waterVolume in waterVolumes )
        {
            if ( waterVolume.collider.bounds.Contains( pos ) )
            {
                return true;
            }
        }

		return false;
	}*/

    /*public static bool InsideCold ( Vector3 pos )
    {
        CC [] waterVolumes = Volumes.globalWaterVolumes.getCleanArray();
        if ( waterVolumes != null )
        foreach ( CWaterVolume waterVolume in waterVolumes )
        {
            if ( waterVolume.collider.bounds.Contains( pos ) )
            {
                return true;
            }
        }

		return false;
    }*/
}
