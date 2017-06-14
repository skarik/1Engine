// Sea and Storm
//(C) 2011

using UnityEngine;
using System.Collections;

public class Raytracer
{
	
	public const int kIslandLayers = (1 << 10)|(1 << 11);
	//public static float WaveHeight = 0.034f;
	
	private static RaycastHit localhitInfo;
	
	public static bool CheckRayIntersection ( Ray ray )
	{
		return CheckRayIntersection ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersection ( Ray ray, float indistance )
	{
		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = (1 << 8 )|(1 << 9)|(1 << 10)|(1 << 11);
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		return tempResult;
	}

	public static bool CheckRayIntersectionIsland ( Ray ray )
	{
		return CheckRayIntersectionIsland ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersectionIsland ( Ray ray, float indistance )
	{
        if ( indistance <= 0.01f ) // ERROR CHECK
            return false;

		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = (1 << 10)|(1 << 11);
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		return tempResult;
	}

    public static bool CheckRayIntersectionGlass ( Ray ray )
	{
		return CheckRayIntersectionGlass ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersectionGlass ( Ray ray, float indistance )
	{
        if ( indistance <= 0.01f ) // ERROR CHECK
            return false;

		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = (1 << 12);
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		return tempResult;
	}

	public static bool CheckRayIntersectionRock ( Ray ray )
	{
		return CheckRayIntersectionRock ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersectionRock ( Ray ray, float indistance )
	{
        if ( indistance <= 0.01f ) // ERROR CHECK
            return false;

		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = 1 << 10;
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		return tempResult;
	}

	public static bool CheckRayIntersectionWater ( Ray ray )
	{
		return CheckRayIntersectionWater ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersectionWater ( Ray ray, float indistance )
	{
        if ( indistance <= 0.01f ) // ERROR CHECK
            return false;

        GameObject [] waterObjects = GameObject.FindGameObjectsWithTag ( "Water" );
		foreach ( GameObject waterObject in waterObjects )
		{
			if ( waterObject.collider != null )
			{
				waterObject.collider.isTrigger = false;
			}
		}
		/*GameObject [] waterObjects = GameObject.FindGameObjectsWithTag ( "Water" );
		foreach ( GameObject waterObject in waterObjects )
		{
			if ( waterObject.collider != null )
			{
				waterObject.collider.isTrigger = false;
			}
		}
		
		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = (1 << 4);
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		//localhitInfo.distance = ray.origin.y;
		//localhitInfo.point = new Vector3( ray.origin.x, 0, ray.origin.z );
		
		foreach ( GameObject waterObject in waterObjects )
		{
			if ( waterObject.collider != null )
			{
				waterObject.collider.isTrigger = true;
			}
		}
		
		return tempResult;*/

        int layerMask = (1 << 4);
		
		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		
		Vector3 newPos = new Vector3();
		newPos = ray.origin + ray.direction.normalized*indistance;
		
		if ((( newPos.y < 0 )&&( ray.origin.y > 0 ))||(( newPos.y > 0 )&&( ray.origin.y < 0 )))
		{
			tempResult = true;
		}
		// Credits to robotive
		if ( tempResult )
		{
			// Calculate y hit
			//if ( indistance != Mathf.Infinity )
			//{
				newPos = new Vector3 ( ray.origin.x, ray.origin.y, ray.origin.z );
				newPos.y = 0;
				newPos.x += (ray.origin.y/ray.direction.normalized.y)*ray.direction.normalized.x;
				newPos.z += (ray.origin.y/ray.direction.normalized.y)*ray.direction.normalized.z;
				hitInfo.point = newPos;
				hitInfo.distance = (newPos- ray.origin).magnitude;
			//}
			
			localhitInfo = hitInfo;

            Debug.DrawLine (ray.origin, hitInfo.point);
		}
        else
        {
            //hitInfo.distance = Mathf.Infinity;
            
            // Do another raytrace
            tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		    if ( hitInfo.point == Vector3.zero )
		    {
			    hitInfo.distance = Mathf.Infinity;
		    }
            localhitInfo = hitInfo;

            if ( hitInfo.collider != null )
                if ( hitInfo.collider.isTrigger )
                    Debug.Log( "Is Trigger!" );

            if ( tempResult )
			    Debug.DrawLine (ray.origin, hitInfo.point);
		    else
			    Debug.DrawRay( ray.origin, ray.direction, Color.red );
        }

        foreach ( GameObject waterObject in waterObjects )
		{
			if ( waterObject.collider != null )
			{
				waterObject.collider.isTrigger = true;
			}
		}
		
		return tempResult;
	}

	public static bool CheckRayIntersectionSnow ( Ray ray )
	{
		return CheckRayIntersectionSnow ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersectionSnow ( Ray ray, float indistance )
	{
        if ( indistance <= 0.01f ) // ERROR CHECK
            return false;

		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = 1 << 11;
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		return tempResult;
	}
	
	// -----------------------------------------------------
	// Actor raytrace
	public static bool CheckRayIntersectionActor ( Ray ray )
	{
		return CheckRayIntersectionRock ( ray, Mathf.Infinity );
	}

	public static bool CheckRayIntersectionActor ( Ray ray, float indistance )
	{
        if ( indistance <= 0.01f ) // ERROR CHECK
            return false;

		bool tempResult = false;
		RaycastHit hitInfo = new RaycastHit ();
		int layerMask = (1 << 8 )|(1 << 9);
		tempResult = Physics.Raycast( ray, out hitInfo, indistance, layerMask );
		if ( hitInfo.point == Vector3.zero )
		{
			hitInfo.distance = Mathf.Infinity;
		}
		
		localhitInfo = hitInfo;

		if ( tempResult )
			Debug.DrawLine (ray.origin, hitInfo.point);
		else
			Debug.DrawRay( ray.origin, ray.direction );
		
		return tempResult;
	}

	// -----------------------------------------------------
	// Getters 
	public static RaycastHit GetHitInfo ( )
	{
		//var hitInfo : RaycastHit = new RaycastHit ();
		//hitInfo = localhitInfo;
		return localhitInfo;
	}

    // -----------------------------------------------------
	// Bounds
    public static bool InEntityBounds ( Vector3 pos )
    {
        if ( Mathf.Abs( pos.y ) > 3500 )
        {
            return false;
        }
        return true;
    }
    
    // -----------------------------------------------------
	// Water 
	public static bool InWater ( Vector3 pos )
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
		/*if ( pos.y < WaterHeight( pos ) )
		{
			return true;
		}

        // OMG LOOP THROUGH THE WATER VOLUME
        //CWaterVolume [] waterVolumes = (CWaterVolume[])GameObject.FindObjectsOfType( typeof( CWaterVolume ) );
        CWaterVolume [] waterVolumes = globalWaterVolumes.getCleanArray();
        if ( waterVolumes != null )
        foreach ( CWaterVolume waterVolume in waterVolumes )
        {
            if ( waterVolume.collider.bounds.Contains( pos ) )
            {
                return true;
            }
        }

		return false;*/
        return Water.PositionInside( pos );
	}
	
    // Get the height of water at the current position
	public static float WaterHeight ( Vector3 pos )
	{
		/*float _WaveHeight = WaveHeight;
		float _Period = 0.08f;
		float _Time = (Time.time/1f);
		float centerDist = Mathf.Sqrt(pos.x*pos.x+pos.z*pos.z)/(100f/_WaveHeight);
		centerDist *= centerDist*4f;
		float currentY = 0;
		currentY += Mathf.Sin( (_Time*12f+(pos.x*0.4f+pos.z)*0.1f)*1.7f*_Period )*8f*centerDist;
		currentY += Mathf.Cos( (_Time*10f+(pos.x-pos.z*0.7f)*0.1f)*1.2f*_Period )*10f*centerDist;
		currentY += Mathf.Cos( (_Time*23f+(-pos.x+pos.z*0.1f)*0.1f)*1.2f*_Period )*7f*centerDist;*/
		/*if ( pos.y < currentY )
		{
			return true;
		}
		return false;*/
		//return currentY;
        return Water.OceanHeight( pos );
	}
	
    // Flow fields
	public static Vector3 WaterVelocity ( Vector3 pos )
	{
		/*Vector3 result = new Vector3 ( 0,0,0 );
		float _WaveHeight = WaveHeight;
		float _Period = 0.08f;
		float _Time = (Time.time/1f);
		float centerDist = Mathf.Sqrt(pos.x*pos.x+pos.z*pos.z)/(100f/_WaveHeight);
		centerDist *= centerDist*4f;
		float currentY = 0;
		currentY += Mathf.Cos( (_Time*12f+(pos.x*0.4f+pos.z)*0.1f)*1.7f*_Period )*8f*centerDist;
		currentY -= Mathf.Sin( (_Time*10f+(pos.x-pos.z*0.7f)*0.1f)*1.2f*_Period )*10f*centerDist;
		currentY -= Mathf.Sin( (_Time*23f+(-pos.x+pos.z*0.1f)*0.1f)*1.2f*_Period )*7f*centerDist;*/
		/*if ( pos.y < currentY )
		{
			return true;
		}
		return false;*/
		/*result.y = currentY;
		return result * Time.smoothDeltaTime;*/
        return Water.GetFlowField( pos );
	}


	// -----------------------------------------------------
	// Debug information
	/*public static void OnGUIS ()
	{
		GUI.Label ( new Rect( 25, 75, 200, 100), "Last Raytrace Distance: " + localhitInfo.distance);
	}*/
}
