#ifndef __DEFINITION_H__
#define __DEFINITION_H__

/* �}�[�J�̈� */
typedef struct{
	CvPoint2D32f centroid;			// �d�S
	CvRect boundingBox;					// �o�E���f�B���O�{�b�N�X
	float size;									// �T�C�Y
	float distanceFromOrigin;							// ���_����̋���
} MarkerRegion;

#endif