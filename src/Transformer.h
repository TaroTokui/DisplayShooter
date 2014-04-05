/*********************************************************************
	Transformerクラス			「Transformer.h」

	概要:
		座標変換を行うクラス

	Date:
		Created: 2009/01/17	By でん
 *********************************************************************/

#ifndef __TRANSFORMER_H__
#define __TRANSFORMER_H__

#include <stdlib.h>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#include "Definition.h"

class Transformer{

public:

	/* コンストラクタ */
	Transformer();

	/* デストラクタ */
	virtual ~Transformer();

	/********************************************************************
													初期化 / 解放
	 ********************************************************************/
	/* 初期化 */
	void initialize(int targetDisplayWidth, int targetDisplayHeight);

	/* 解放 */
	void finalize();

	/********************************************************************
									ディスプレイの四隅を検出する
	 ********************************************************************/
	CvPoint2D32f getImpactPoint(MarkerRegion *sortedCornerRegions, CvPoint2D32f aimingPoint);

protected:

	CvMat *matrix3x3;
	CvMat *srcPointMatrix2x1, *dstPointMatrix2x1;
	CvMat tmpSrcHeader, tmpDstHeader;

	CvPoint2D32f dstPoints[4];

	void createMatrix(CvPoint2D32f *srcPoints, CvPoint2D32f *dstPoints, CvMat *dstMatrix3x3);

	/* 変換を行う */
	CvPoint2D32f transformPoint( CvPoint2D32f aimingPoint, CvMat *srcMatrix3x3 );

};
#endif