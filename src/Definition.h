#ifndef __DEFINITION_H__
#define __DEFINITION_H__

/* マーカ領域 */
typedef struct{
	CvPoint2D32f centroid;			// 重心
	CvRect boundingBox;					// バウンディングボックス
	float size;									// サイズ
	float distanceFromOrigin;							// 原点からの距離
} MarkerRegion;

#endif