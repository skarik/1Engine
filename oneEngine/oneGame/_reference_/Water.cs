// Sea and Storm
//(C) 2011

using UnityEngine;
using System.Collections;

public class Water
{
    public static float WaveHeight = 0.034f; // not used???

    public static bool PositionInside ( Vector3 pos )
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
		if ( pos.y < OceanHeight( pos ) )
		{
			return true;
		}

        // OMG LOOP THROUGH THE WATER VOLUME
        //CWaterVolume [] waterVolumes = (CWaterVolume[])GameObject.FindObjectsOfType( typeof( CWaterVolume ) );
        CWaterVolume [] waterVolumes = Volumes.globalWaterVolumes.getCleanArray();
        if ( waterVolumes != null )
        foreach ( CWaterVolume waterVolume in waterVolumes )
        {
            if ( waterVolume.collider.bounds.Contains( pos ) )
            {
                return true;
            }
        }

		return false;
	}

    // Get the height of ocean at the current position
	public static float OceanHeight ( Vector3 pos )
	{
		float _WaveHeight = WaveHeight;
		float _Period = 0.08f;
		float _Time = (Time.time/1f);
		float centerDist = Mathf.Sqrt(pos.x*pos.x+pos.z*pos.z)/(100f/_WaveHeight);
		centerDist *= centerDist*4f;
		float currentY = 0;
		currentY += Mathf.Sin( (_Time*12f+(pos.x*0.4f+pos.z)*0.1f)*1.7f*_Period )*8f*centerDist;
		currentY += Mathf.Cos( (_Time*10f+(pos.x-pos.z*0.7f)*0.1f)*1.2f*_Period )*10f*centerDist;
		currentY += Mathf.Cos( (_Time*23f+(-pos.x+pos.z*0.1f)*0.1f)*1.2f*_Period )*7f*centerDist;
		/*if ( pos.y < currentY )
		{
			return true;
		}
		return false;*/
		return currentY;
	}

    // Flow fields
	public static Vector3 GetFlowField ( Vector3 pos )
	{
		Vector3 result = new Vector3 ( 0,0,0 );
		float _WaveHeight = WaveHeight;
		float _Period = 0.08f;
		float _Time = (Time.time/1f);
		float centerDist = Mathf.Sqrt(pos.x*pos.x+pos.z*pos.z)/(100f/_WaveHeight);
		centerDist *= centerDist*4f;
		float currentY = 0;
		currentY += Mathf.Cos( (_Time*12f+(pos.x*0.4f+pos.z)*0.1f)*1.7f*_Period )*8f*centerDist;
		currentY -= Mathf.Sin( (_Time*10f+(pos.x-pos.z*0.7f)*0.1f)*1.2f*_Period )*10f*centerDist;
		currentY -= Mathf.Sin( (_Time*23f+(-pos.x+pos.z*0.1f)*0.1f)*1.2f*_Period )*7f*centerDist;
		/*if ( pos.y < currentY )
		{
			return true;
		}
		return false;*/
		result.y = currentY;
		return result * Time.smoothDeltaTime;
	}
}
