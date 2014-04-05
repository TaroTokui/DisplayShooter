/*********************************************************************
	DisplayDetector�N���X			�uDisplayDetector.h�v

	�T�v:
		2�l���摜����f�B�X�v���C��4�R�[�i�[�����o����B
		���o����4�R�[�i�[����ёւ���B

	Date:
		Created: 2009/01/17	By �ł�
 *********************************************************************/

#ifndef __DISPLAYDETECTOR_H__
#define __DISPLAYDETECTOR_H__

#include <stdlib.h>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include "Definition.h"

static const float DISPLAYDETECTOR_DEFAULT_TRACKING_DISTANCE_THRESHOLD = 50.0f;

class DisplayDetector{

public:

	/* �R���X�g���N�^ */
	DisplayDetector();

	/* �f�X�g���N�^ */
	virtual ~DisplayDetector();

	/********************************************************************
													������ / ���
	 ********************************************************************/
	/* ������ */
	void initialize(int maxMarkerRegions, float trackingDistanceThreshold = DISPLAYDETECTOR_DEFAULT_TRACKING_DISTANCE_THRESHOLD);

	/* ��� */
	void finalize();

	/********************************************************************
									�f�B�X�v���C�̎l�������o����
	 ********************************************************************/
	/* �����ŗ^����ꂽ2�l����̉摜���� */
	int detectDisplayCorners(IplImage *srcImageGRAY, MarkerRegion *dstSortedMarkerRegions, float maxSizeThreshold, float minSizeThreshold);

protected:

	CvMoments moments;

	int maxRegions;

	MarkerRegion *unsortedNewMarkerRegions;
	MarkerRegion *sortedMarkerRegions;

	float trackingDistanceThreshold;

	/* �}�[�J�̈�̌��o */
	int detectMarkerRegions(IplImage *srcBinarizedImageGRAY, MarkerRegion *dstRegions, int maxRegions, float maxSizeThreshold, float minSizeThreshold);

	/* �R�[�i�[���W�����ォ�甽���v���ɕ��ёւ���B
		 ���̕��ёւ���@���ʗp����̂́A�J���������܂�X���Ă��Ȃ��ꍇ�̂� */
	void sortMarkersCCW(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions);

	/* �O�t���[���̈ʒu�����ԋ߂��_�ƑΉ��t������B
		 srcRegions�͍��t���[���ŐV�������o���ꂽ�}�[�J(���\�[�g)�AdstRegions�͑O�t���[���̃}�[�J�ʒu(�\�[�g�ς�)�B
		 ���t���[���Ō��o���ꂽ�}�[�J�ʒu���A�O�t���[���Ō��o���ꂽ�}�[�J�ʒu�̈�Ə\���߂��ꍇ�A
		 �O�t���[���Ō��o���ꂽ�}�[�J�ʒu�����t���[���Ō��o���ꂽ�Ή�����}�[�J�ʒu�ɍX�V����B
		 4�̃}�[�J�̂����A1�ł��}�b�`���Ȃ����̂�����΁A�O�t���[���Ƃ̑Ή��t���͈�؂����Ȃ킸�A
		 ���t���[���Ō��o���ꂽ�}�[�J�ʒu��V�����\�[�g���Ȃ��� */
	void trackMarkers(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions);

};
#endif