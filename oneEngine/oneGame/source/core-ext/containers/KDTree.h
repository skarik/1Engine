
#ifndef _CONTAINER_KDTREE_H_
#define _CONTAINER_KDTREE_H_

#include "core/math/Vector3d.h"
#include <vector>

class KDTree
{
public:
	std::vector<KDTree*> lr;
	Vector3d pivot;
	int pivotIndex;
	int axis;
	
//	Change this value to 2 if you only need two-dimensional X,Y points. The search will
//	be quicker in two dimensions.
	static const int numDims = 3;
	
	
	KDTree() {
		//lr = new KDTree[2];
		lr.push_back( NULL );
		lr.push_back( NULL );
	}
	

//	Make a new tree from a list of points.
	static KDTree* MakeFromPoints( std::vector<Vector3d>& points) {
		int* indices = Iota(points.size());
		return MakeFromPointsInner(0, 0, points.size() - 1, points, indices);
	}
	

//	Recursively build a tree by separating points at plane boundaries.
	static KDTree* MakeFromPointsInner(
					int depth,
					int stIndex, int enIndex,
					std::vector<Vector3d>& points,
					int* inds
					) {
		
		KDTree* root = new KDTree();
		root->axis = depth % numDims;
		int splitPoint = FindPivotIndex(points, inds, stIndex, enIndex, root->axis);

		root->pivotIndex = inds[splitPoint];
		root->pivot = points[root->pivotIndex];
		
		int leftEndIndex = splitPoint - 1;
		
		if (leftEndIndex >= stIndex) {
			root->lr[0] = MakeFromPointsInner(depth + 1, stIndex, leftEndIndex, points, inds);
		}
		
		int rightStartIndex = splitPoint + 1;
		
		if (rightStartIndex <= enIndex) {
			root->lr[1] = MakeFromPointsInner(depth + 1, rightStartIndex, enIndex, points, inds);
		}
		
		return root;
	}
	
	
	static void SwapElements(int* arr, int a, int b) {
		int temp = arr[a];
		arr[a] = arr[b];
		arr[b] = temp;
	}
	

//	Simple "median of three" heuristic to find a reasonable splitting plane.
	static int FindSplitPoint(std::vector<Vector3d>& points, int* inds, int stIndex, int enIndex, int axis) {
		float a = points[inds[stIndex]][axis];
		float b = points[inds[enIndex]][axis];
		int midIndex = (stIndex + enIndex) / 2;
		float m = points[inds[midIndex]][axis];
		
		if (a > b) {
			if (m > a) {
				return stIndex;
			}
			
			if (b > m) {
				return enIndex;
			}
			
			return midIndex;
		} else {
			if (a > m) {
				return stIndex;
			}
			
			if (m > b) {
				return enIndex;
			}
			
			return midIndex;
		}
	}
	

//	Find a new pivot index from the range by splitting the points that fall either side
//	of its plane.
	static int FindPivotIndex(std::vector<Vector3d>& points, int* inds, int stIndex, int enIndex, int axis) {
		int splitPoint = FindSplitPoint(points, inds, stIndex, enIndex, axis);
		// int splitPoint = Random.Range(stIndex, enIndex);

		Vector3d pivot = points[inds[splitPoint]];
		SwapElements(inds, stIndex, splitPoint);

		int currPt = stIndex + 1;
		int endPt = enIndex;
		
		while (currPt <= endPt) {
			Vector3d curr = points[inds[currPt]];
			
			if ((curr[axis] > pivot[axis])) {
				SwapElements(inds, currPt, endPt);
				endPt--;
			} else {
				SwapElements(inds, currPt - 1, currPt);
				currPt++;
			}
		}
		
		return currPt - 1;
	}
	
	
	static int* Iota(int num) {
		int* result = new int[num];
		
		for (int i = 0; i < num; i++) {
			result[i] = i;
		}
		
		return result;
	}
	
	
//	Find the nearest point in the set to the supplied point.
	int FindNearest(Vector3d pt) {
		float bestSqDist = 1000000000.0f;
		int bestIndex = -1;
		
		Search(pt, bestSqDist, bestIndex);
		
		return bestIndex;
	}
	

//	Recursively search the tree.
	void Search(Vector3d pt, float& bestSqSoFar, int& bestIndex) {
		float mySqDist = (pivot - pt).sqrMagnitude();
		
		if (mySqDist < bestSqSoFar) {
			bestSqSoFar = mySqDist;
			bestIndex = pivotIndex;
		}
		
		float planeDist = pt[axis] - pivot[axis]; //DistFromSplitPlane(pt, pivot, axis);
		
		int selector = planeDist <= 0 ? 0 : 1;
		
		if (lr[selector] != NULL) {
			lr[selector]->Search(pt, bestSqSoFar, bestIndex);
		}
		
		selector = (selector + 1) % 2;
		
		float sqPlaneDist = planeDist * planeDist;

		if ((lr[selector] != NULL) && (bestSqSoFar > sqPlaneDist)) {
			lr[selector]->Search(pt, bestSqSoFar, bestIndex);
		}
	}
	

//	Get a point's distance from an axis-aligned plane.
	float DistFromSplitPlane(Vector3d pt, Vector3d planePt, int axis) {
		return pt[axis] - planePt[axis];
	}
	

//	Simple output of tree structure - mainly useful for getting a rough
//	idea of how deep the tree is (and therefore how well the splitting
//	heuristic is performing).
	/*string Dump(int level) {
		string result = pivotIndex.ToString().PadLeft(level) + "\n";
		
		if (lr[0] != null) {
			result += lr[0].Dump(level + 2);
		}
		
		if (lr[1] != null) {
			result += lr[1].Dump(level + 2);
		}
		
		return result;
	}*/
};


#endif//_CONTAINER_KDTREE_H_