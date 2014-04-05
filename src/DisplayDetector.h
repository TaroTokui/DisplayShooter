/*********************************************************************
	DisplayDetectorクラス			「DisplayDetector.h」

	概要:
		2値化画像からディスプレイの4コーナーを検出する。
		検出した4コーナーを並び替える。

	Date:
		Created: 2009/01/17	By でん
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

	/* コンストラクタ */
	DisplayDetector();

	/* デストラクタ */
	virtual ~DisplayDetector();

	/********************************************************************
													初期化 / 解放
	 ********************************************************************/
	/* 初期化 */
	void initialize(int maxMarkerRegions, float trackingDistanceThreshold = DISPLAYDETECTOR_DEFAULT_TRACKING_DISTANCE_THRESHOLD);

	/* 解放 */
	void finalize();

	/********************************************************************
									ディスプレイの四隅を検出する
	 ********************************************************************/
	/* 引数で与えられた2値化後の画像から */
	int detectDisplayCorners(IplImage *srcImageGRAY, MarkerRegion *dstSortedMarkerRegions, float maxSizeThreshold, float minSizeThreshold);

protected:

	CvMoments moments;

	int maxRegions;

	MarkerRegion *unsortedNewMarkerRegions;
	MarkerRegion *sortedMarkerRegions;

	float trackingDistanceThreshold;

	/* マーカ領域の検出 */
	int detectMarkerRegions(IplImage *srcBinarizedImageGRAY, MarkerRegion *dstRegions, int maxRegions, float maxSizeThreshold, float minSizeThreshold);

	/* コーナー座標を左上から反時計回りに並び替える。
		 この並び替え手法が通用するのは、カメラがあまり傾いていない場合のみ */
	void sortMarkersCCW(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions);

	/* 前フレームの位置から一番近い点と対応付けする。
		 srcRegionsは今フレームで新しく検出されたマーカ(未ソート)、dstRegionsは前フレームのマーカ位置(ソート済み)。
		 今フレームで検出されたマーカ位置が、前フレームで検出されたマーカ位置の一つと十分近い場合、
		 前フレームで検出されたマーカ位置を今フレームで検出された対応するマーカ位置に更新する。
		 4つのマーカのうち、1つでもマッチしないものがあれば、前フレームとの対応付けは一切おこなわず、
		 今フレームで検出されたマーカ位置を新しくソートしなおす */
	void trackMarkers(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions);

};
#endif