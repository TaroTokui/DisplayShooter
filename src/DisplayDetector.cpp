#include "DisplayDetector.h"

/* コンストラクタ */
DisplayDetector::DisplayDetector(){
	trackingDistanceThreshold = 50;
} // DisplayDetector

/* デストラクタ */
DisplayDetector::~DisplayDetector(){	
} // DisplayDetector

/********************************************************************
												初期化 / 解放
 ********************************************************************/
/* 初期化 */
void DisplayDetector::initialize(int maxMarkerRegions, float trackingDistanceThreshold){
	
	maxRegions = maxMarkerRegions;
	unsortedNewMarkerRegions = new MarkerRegion[maxRegions];
	sortedMarkerRegions = new MarkerRegion[maxRegions];
	this->trackingDistanceThreshold = trackingDistanceThreshold;

} // initialize

/* 解放 */
void DisplayDetector::finalize(){
	delete [] sortedMarkerRegions;
	delete [] unsortedNewMarkerRegions;
} // finalize

/********************************************************************
								ディスプレイの四隅を検出する
 ********************************************************************/
/* 引数で与えられた2値化後の画像から */
int DisplayDetector::detectDisplayCorners(IplImage *srcImageGRAY, MarkerRegion *dstSortedMarkerRegions, float maxSizeThreshold, float minSizeThreshold){

	/* マーカの検出 */
	int numDetectedRegions = detectMarkerRegions(srcImageGRAY, unsortedNewMarkerRegions, maxRegions, maxSizeThreshold, minSizeThreshold);

	/* 並び替える */
	if (numDetectedRegions == 4){
		trackMarkers(unsortedNewMarkerRegions, numDetectedRegions, sortedMarkerRegions);
	}

	/* コピーする */
	for ( int i = 0; i < 4; i++ ){
		dstSortedMarkerRegions[i] = sortedMarkerRegions[i];
	}

	return numDetectedRegions;

} // detectDisplayCorners

/* マーカ領域の検出 */
int DisplayDetector::detectMarkerRegions(IplImage *srcBinarizedImageGRAY, MarkerRegion *dstRegions, int maxRegions, float maxSizeThreshold, float minSizeThreshold){

	CvSeq *contour = 0;
	int regionCounter = 0;

	CvMemStorage *storage = cvCreateMemStorage(0);

	cvFindContours( srcBinarizedImageGRAY, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );		// 外周のみの輪郭を求める

	if( contour != 0 ){

		/* 領域リストをチェック */
		while ( contour && regionCounter < maxRegions ){

			double regionSize = fabs( cvContourArea(contour, CV_WHOLE_SEQ) );

			if( regionSize < minSizeThreshold || regionSize > maxSizeThreshold ){
				contour = contour->h_next;
				continue;
			}

			dstRegions[regionCounter].size = (float)regionSize;
			dstRegions[regionCounter].boundingBox = cvBoundingRect(contour, 0);
			dstRegions[regionCounter].centroid.x = dstRegions[regionCounter].boundingBox.x + dstRegions[regionCounter].boundingBox.width / 2.0f;			// 重心は、簡単にバウンディングボックスの中心とする
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

/* コーナー座標を左上から反時計回りに並び替える。
	 この並び替え手法が通用するのは、カメラがあまり傾いていない場合のみ */
void DisplayDetector::sortMarkersCCW(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions){
	
	MarkerRegion tmp;

	//printf("新規ソート\n");
	/* とりあえず、左上(0,0)からの距離の小さい順に並び替える(左上、左下、右上、右下の順になる) */
	for ( int i = 0; i < 4; i++ ){
		for ( int j = i + 1; j < 4; j++ ){
			if ( srcNewUnsortedRegions[i].distanceFromOrigin > srcNewUnsortedRegions[j].distanceFromOrigin ){
				tmp = srcNewUnsortedRegions[i];
				srcNewUnsortedRegions[i] = srcNewUnsortedRegions[j];
				srcNewUnsortedRegions[j] = tmp;
			}
		}
	}

	/* 2番目と3番目を入れ替える */
	tmp = srcNewUnsortedRegions[2];
	srcNewUnsortedRegions[2] = srcNewUnsortedRegions[3];
	srcNewUnsortedRegions[3] = tmp;

	/* コピーする */
	for ( int i = 0; i < 4; i++ ){
		dstSortedRegions[i] = srcNewUnsortedRegions[i];
	}

} // sortMarkersCCW

/* 前フレームの位置から一番近い点と対応付けする。
	 srcRegionsは今フレームで新しく検出されたマーカ(未ソート)、dstRegionsは前フレームのマーカ位置(ソート済み)。
	 今フレームで検出されたマーカ位置が、前フレームで検出されたマーカ位置の一つと十分近い場合、
	 前フレームで検出されたマーカ位置を今フレームで検出された対応するマーカ位置に更新する。
	 4つのマーカのうち、1つでもマッチしないものがあれば、前フレームとの対応付けは一切おこなわず、
	 今フレームで検出されたマーカ位置を新しくソートしなおす */
void DisplayDetector::trackMarkers(MarkerRegion *srcNewUnsortedRegions, int numDetectedRegions, MarkerRegion *dstSortedRegions){

	int matchedSrcIndex[4];
	int matchCounter = 0;
	for ( int i = 0; i < 4; i++ ){
		matchedSrcIndex[i] = -1;
	}

	/* 今フレームで検出されたマーカ位置とフレームで検出されたマーカが近い場合対応付け、位置を更新する */
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

	/* 1つでもマッチしなかったら最初からやりなおす */
	if ( matchCounter < 4 ){
		sortMarkersCCW(srcNewUnsortedRegions, numDetectedRegions, dstSortedRegions);
	
	}else{
		
		/* 更新 */
		//printf("Update\n");
		for ( int i = 0; i < 4; i++ ){
			dstSortedRegions[ i ] = srcNewUnsortedRegions[ matchedSrcIndex[i] ];
		}
	}

} // trackMarkers
