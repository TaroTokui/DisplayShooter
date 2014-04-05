#include "Transformer.h"

/* コンストラクタ */
Transformer::Transformer(){	
} // Transformer

/* デストラクタ */
Transformer::~Transformer(){	
} // Transformer

/********************************************************************
												初期化 / 解放
 ********************************************************************/
/* 初期化 */
void Transformer::initialize(int targetDisplayWidth, int targetDisplayHeight){
	dstPoints[0].x = 0;
    dstPoints[0].y = 0;
    dstPoints[1].x = 0;
	dstPoints[1].y = (float)targetDisplayHeight;
	dstPoints[2].x = (float)targetDisplayWidth;
	dstPoints[2].y = (float)targetDisplayHeight;
	dstPoints[3].x = (float)targetDisplayWidth;
    dstPoints[3].y = 0;

	matrix3x3 = cvCreateMat( 3, 3, CV_64FC1 );
	srcPointMatrix2x1 = cvCreateMat( 2, 1, CV_64FC1 );
	dstPointMatrix2x1 = cvCreateMat( 2, 1, CV_64FC1 );
} // initialize

/* 解放 */
void Transformer::finalize(){
	cvReleaseMat(&dstPointMatrix2x1);
	cvReleaseMat(&srcPointMatrix2x1);
	cvReleaseMat(&matrix3x3);
} // finalize

/********************************************************************
								ディスプレイの四隅を検出する
 ********************************************************************/
CvPoint2D32f Transformer::getImpactPoint(MarkerRegion *sortedCornerRegions, CvPoint2D32f aimingPoint){

	CvPoint2D32f srcPoints[4];
	srcPoints[0] = sortedCornerRegions[0].centroid;
	srcPoints[1] = sortedCornerRegions[1].centroid;
	srcPoints[2] = sortedCornerRegions[2].centroid;
	srcPoints[3] = sortedCornerRegions[3].centroid;

	/* 変換行列の作成 */
	createMatrix(srcPoints, dstPoints, matrix3x3);

	/* 座標変換 */
	return transformPoint(aimingPoint, matrix3x3);

} // getImpactPoint

void Transformer::createMatrix(CvPoint2D32f *srcPoints, CvPoint2D32f *dstPoints, CvMat *dstMatrix3x3){
	cvGetPerspectiveTransform( srcPoints, dstPoints, dstMatrix3x3 );
} // createMatrix

/* 変換を行う */
CvPoint2D32f Transformer::transformPoint( CvPoint2D32f aimingPoint, CvMat *srcMatrix3x3 ){

	CvMat *srcPointMatrix2x1 = cvCreateMat( 1, 1, CV_64FC2 );
	CvMat *dstPointMatrix2x1 = cvCreateMat( 1, 1, CV_64FC2 );

	srcPointMatrix2x1->data.db[0] = (double)aimingPoint.x;
	srcPointMatrix2x1->data.db[1] = (double)aimingPoint.y;

	cvPerspectiveTransform(srcPointMatrix2x1, dstPointMatrix2x1, srcMatrix3x3);

	CvPoint2D32f dstPoint;
	dstPoint.x = (float)dstPointMatrix2x1->data.db[0];
	dstPoint.y = (float)dstPointMatrix2x1->data.db[1];

	cvReleaseMat(&dstPointMatrix2x1);
	cvReleaseMat(&srcPointMatrix2x1);

	return dstPoint;

} // transformPoint
