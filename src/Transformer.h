/*********************************************************************
	Transformer�N���X			�uTransformer.h�v

	�T�v:
		���W�ϊ����s���N���X

	Date:
		Created: 2009/01/17	By �ł�
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

	/* �R���X�g���N�^ */
	Transformer();

	/* �f�X�g���N�^ */
	virtual ~Transformer();

	/********************************************************************
													������ / ���
	 ********************************************************************/
	/* ������ */
	void initialize(int targetDisplayWidth, int targetDisplayHeight);

	/* ��� */
	void finalize();

	/********************************************************************
									�f�B�X�v���C�̎l�������o����
	 ********************************************************************/
	CvPoint2D32f getImpactPoint(MarkerRegion *sortedCornerRegions, CvPoint2D32f aimingPoint);

protected:

	CvMat *matrix3x3;
	CvMat *srcPointMatrix2x1, *dstPointMatrix2x1;
	CvMat tmpSrcHeader, tmpDstHeader;

	CvPoint2D32f dstPoints[4];

	void createMatrix(CvPoint2D32f *srcPoints, CvPoint2D32f *dstPoints, CvMat *dstMatrix3x3);

	/* �ϊ����s�� */
	CvPoint2D32f transformPoint( CvPoint2D32f aimingPoint, CvMat *srcMatrix3x3 );

};
#endif