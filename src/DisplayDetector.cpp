#include "DisplayDetector.h"

/* �R���X�g���N�^ */
DisplayDetector::DisplayDetector(){
	trackingDistanceThreshold = 50;
} // DisplayDetector

/* �f�X�g���N�^ */
DisplayDetector::~DisplayDetector(){	
} // DisplayDetector

/********************************************************************
												������ / ���
 ********************************************************************/
/* ������ */
void DisplayDetector::initialize(int maxMarkerRegions, float trackingDistanceThreshold){
	
	maxRegions = maxMarkerRegions;
	unsortedNewMarkerRegions = new MarkerRegion[maxRegions];
	sortedMarkerRegions = new MarkerRegion[maxRegions];
	this->trackingDistanceThreshold = trackingDistanceThreshold;

} // initialize

/* ��� */
void DisplayDetector::finalize(){
	delete [] sortedMarkerRegions;
	delete [] unsortedNewMarkerRegions;
} // finalize

/********************************************************************
								�f�B�X�v���C�̎l�������o����
 ********************************************************************/
/* �����ŗ^����ꂽ2�l����̉摜���� */
int DisplayDetector::detectDisplayCorners(IplImage *srcImageGRAY, MarkerRegion *dstSortedMarkerRegions, float maxSizeThreshold, float minSizeThreshold){

	/* �}�[�J�̌��o */
	int numDetectedRegions = detectMarkerRegions(srcImageGRAY, unsortedNewMarkerRegions, maxRegions, maxSizeThreshold, minSizeThreshold);

	/* ���ёւ��� */
	if (numDetectedRegions == 4){
		trackMarkers(unsortedNewMarkerRegions, numDetectedRegions, sortedMarkerRegions);
	}

	/* �R�s�[���� */
	for ( int i = 0; i < 4; i++ ){
		dstSortedMarkerRegions[i] = sortedMarkerRegions[i];
	}

	return numDetectedRegions;

} // detectDisplayCorners

/* �}�[�J�̈�̌��o */
int DisplayDetector::detectMarkerRegions(IplImage *srcBinarizedImageGRAY, MarkerRegion *dstRegions, int maxRegions, float maxSizeThreshold, float minSizeThreshold){

	CvSeq *contour = 0;
	int regionCounter = 0;

	CvMemStorage *storage = cvCreateMemStorage(0);

	cvFindContours( srcBinarizedImageGRAY, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );		// �O���݂̗̂֊s�����߂�

	if( contour != 0 ){

		/* �̈惊�X�g���`�F�b�N */
		while ( contour && regionCounter < maxRegions ){

			double regionSize = fabs( cvContourArea(contour, CV_WHOLE_SEQ) );

			if( regionSize < minSizeThreshold || regionSize > maxSizeThreshold ){
				contour = contour->h_next;
				continue;
			}

			dstRegions[regionCounter].size = (float)regionSize;
			dstRegions[regionCounter].boundingBox = cvBoundingRect(contour, 0);
			dstRegions[regionCounter].centroid.x = dstRegions[regionCounter].boundingBox.x + dstRegions[regionCounter].boundingBox.width / 2.0f;			// �d�S�́A�ȒP�Ƀo�E���f�B���O�{�b�N�X�̒��S�Ƃ���
			dstRegions[regionCounter].centroid.y = dstRegions[regionCounter].boundingBox.y + dstRegions[regionCounter].boundingBox.height / 2.0f;
			dstRegions[regionCounter].distanceFromOrigin = sqrtf(dstRegions[regionCounter].centroid.x * dstRegions[regionCounter].centroid.x 
				+ dstRegions[regionCounter].centroid.y * dstRegions[regionCounter].centroid.y);

			regionCounter++;
			contour = contour->h_next;
		} // while
	}

	cvReleaseMemStorage( &storage );

	return regionCounter;
} // detectMarkerRegions

/* �R�[�i�[���W�����ォ�甽���v���ɕ��ёւ���B
	 ���̕��ёւ���@���ʗp����̂́A�J���������܂�X���Ă��Ȃ��ꍇ�̂� */
void DisplayDetector::sortMarkersCCW(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions){
	
	MarkerRegion tmp;

	//printf("�V�K�\�[�g\n");
	/* �Ƃ肠�����A����(0,0)����̋����̏��������ɕ��ёւ���(����A�����A�E��A�E���̏��ɂȂ�) */
	for ( int i = 0; i < 4; i++ ){
		for ( int j = i + 1; j < 4; j++ ){
			if ( srcNewUnsortedRegions[i].distanceFromOrigin > srcNewUnsortedRegions[j].distanceFromOrigin ){
				tmp = srcNewUnsortedRegions[i];
				srcNewUnsortedRegions[i] = srcNewUnsortedRegions[j];
				srcNewUnsortedRegions[j] = tmp;
			}
		}
	}

	/* 2�Ԗڂ�3�Ԗڂ����ւ��� */
	tmp = srcNewUnsortedRegions[2];
	srcNewUnsortedRegions[2] = srcNewUnsortedRegions[3];
	srcNewUnsortedRegions[3] = tmp;

	/* �R�s�[���� */
	for ( int i = 0; i < 4; i++ ){
		dstSortedRegions[i] = srcNewUnsortedRegions[i];
	}

} // sortMarkersCCW

/* �O�t���[���̈ʒu�����ԋ߂��_�ƑΉ��t������B
	 srcRegions�͍��t���[���ŐV�������o���ꂽ�}�[�J(���\�[�g)�AdstRegions�͑O�t���[���̃}�[�J�ʒu(�\�[�g�ς�)�B
	 ���t���[���Ō��o���ꂽ�}�[�J�ʒu���A�O�t���[���Ō��o���ꂽ�}�[�J�ʒu�̈�Ə\���߂��ꍇ�A
	 �O�t���[���Ō��o���ꂽ�}�[�J�ʒu�����t���[���Ō��o���ꂽ�Ή�����}�[�J�ʒu�ɍX�V����B
	 4�̃}�[�J�̂����A1�ł��}�b�`���Ȃ����̂�����΁A�O�t���[���Ƃ̑Ή��t���͈�؂����Ȃ킸�A
	 ���t���[���Ō��o���ꂽ�}�[�J�ʒu��V�����\�[�g���Ȃ��� */
void DisplayDetector::trackMarkers(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions){

	int matchedSrcIndex[4];
	int matchCounter = 0;
	for ( int i = 0; i < 4; i++ ){
		matchedSrcIndex[i] = -1;
	}

	/* ���t���[���Ō��o���ꂽ�}�[�J�ʒu�ƃt���[���Ō��o���ꂽ�}�[�J���߂��ꍇ�Ή��t���A�ʒu���X�V���� */
	for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ ){

			if ( matchedSrcIndex[j] == -1 ){
				if ( sqrtf( (srcNewUnsortedRegions[i].centroid.x - dstSortedRegions[j].centroid.x) * (srcNewUnsortedRegions[i].centroid.x - dstSortedRegions[j].centroid.x)
					+ (srcNewUnsortedRegions[i].centroid.y - dstSortedRegions[j].centroid.y) * (srcNewUnsortedRegions[i].centroid.y - dstSortedRegions[j].centroid.y) ) < trackingDistanceThreshold ){
					matchedSrcIndex[j] = i;
					matchCounter++;
				}
			}

		}
	}

	/* 1�ł��}�b�`���Ȃ�������ŏ�������Ȃ��� */
	if ( matchCounter < 4 ){
		sortMarkersCCW(srcNewUnsortedRegions, numDetectedRegions, dstSortedRegions);
	
	}else{
		
		/* �X�V */
		//printf("Update\n");
		for ( int i = 0; i < 4; i++ ){
			dstSortedRegions[ i ] = srcNewUnsortedRegions[ matchedSrcIndex[i] ];
		}
	}

} // trackMarkers
