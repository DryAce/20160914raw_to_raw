// rawデータ化したCTから指定範囲抽出　新たにrawデータ化
// Sep.2016 Yokkaichi

#include<myopencv.h>

using namespace std;
#define IMAGE_ROWS 512
#define IMAGE_COLS 512

int main(void) {

	// RAWデータの読み込み
	char filename[20];
	cout << "INPUT FILENAME : ";
	cin >> filename;					// ファイル名指定

										//ifstream ifs(filename, ios::in | ios::binary);;							// ファイルポインタ,ファイルオープン（バイナリ形式で読み込み）
										//
										//if (!ifs.is_open())						// ファイルオープンに失敗した時
										//{
										//	cout << "CAN'T OPEN [" << filename << "]" << endl;
										//	cout << "INPUT 'r' RESTART, ELSE CONCLUDE : ";
										//	char key1;
										//	cin >> key1;
										//	switch(key1)
										//	{
										//	case 'r':
										//		goto START;
										//		break;
										//	default:
										//		exit(1);
										//		break;
										//	}
										//}

	FILE* ifp;
	ifp = fopen(filename, "rb");

	if (!ifp)						// ファイルオープンに失敗した時
	{
		cout << "CAN'T OPEN [" << filename << "]" << endl;
		cout << "INPUT 'r' RESTART, ELSE CONCLUDE : ";
		char key1;
		cin >> key1;
		switch (key1)
		{
		case 'r':

			break;
		default:
			exit(1);
			break;
		}
	}


	vector<Mat> input_raw;					// 画像保存用Mat配列

	while (!feof(ifp))					// ファイルの末尾になるまで読み込み
	{
		Mat image(IMAGE_COLS, IMAGE_ROWS, CV_16S);
		fread_s(image.data, sizeof(short) * IMAGE_COLS * IMAGE_ROWS, sizeof(short), IMAGE_COLS * IMAGE_ROWS, ifp);
		//image = ~image;					// 白黒反転
		if (image.cols == 0)break;
		input_raw.push_back(image);			// 保存用配列に格納
		cout << "#";
	}
	fclose(ifp);						// ファイルクローズ
	cout << endl << "IMAGE ROADED." << endl;
	cout << "DEPTH = " << input_raw.size() << endl; // 画像枚数表示
	cout << endl << "PRESS ENTER TO VIEW IMAGE" << endl;
	cin.get();




	////

	namedWindow("Axial", CV_WINDOW_AUTOSIZE);
	namedWindow("Sagittal", CV_WINDOW_AUTOSIZE);
	namedWindow("Coronal", CV_WINDOW_AUTOSIZE);

	const int axial_depth_max = input_raw.size();
	const int sagittal_depth_max = IMAGE_COLS;
	const int coronal_depth_max = IMAGE_ROWS;

	int axial_depth_val = 0;
	int sagittal_depth_val = 0;
	int coronal_depth_val = 0;


	// axialに加えてsagittal,coronal画像を生成
	while (1) {

		//createTrackbar("DEPTH", "RAW", &slider_val, input.size() - 1);
		createTrackbar("AXIAL_DEPTH", "Axial", &axial_depth_val, axial_depth_max - 1);
		createTrackbar("SAGITTAL_DEPTH", "Sagittal", &sagittal_depth_val, sagittal_depth_max - 1);
		createTrackbar("CORONAL_DEPTH", "Coronal", &coronal_depth_val, coronal_depth_max - 1);

		// axial
		Mat axial_image, axial_image_bgr;
		input_raw[axial_depth_val].convertTo(axial_image, CV_8U, 0.1275, 127.5);
		cvtColor(axial_image, axial_image_bgr, CV_GRAY2BGR);
		line(axial_image_bgr, Point(sagittal_depth_val, 0), Point(sagittal_depth_val, IMAGE_ROWS), Scalar(0, 255, 0), 1);
		line(axial_image_bgr, Point(0, coronal_depth_val), Point(IMAGE_COLS, coronal_depth_val), Scalar(0, 0, 255), 1);
		imshow("Axial", axial_image_bgr);

		// sagittal
		Mat sagittal_image, sagittal_image_bgr;
		sagittal_image = Mat(IMAGE_ROWS, axial_depth_max, CV_8UC1);
		for (int x = 0; x < axial_depth_max; x++) {
			Mat buf;
			input_raw[x].convertTo(buf, CV_8U, 0.1275, 127.5);
			for (int y = 0; y < IMAGE_ROWS; y++) {
				sagittal_image.at<unsigned char>(Point(x, y)) = buf.at<unsigned char>(Point(sagittal_depth_val, y));
				if (sagittal_image.at<unsigned char>(Point(x, y)) >= 256) sagittal_image.at<unsigned char>(Point(x, y)) = 255;
				else if (sagittal_image.at<unsigned char>(Point(x, y)) < 0) sagittal_image.at<unsigned char>(Point(x, y)) = 0;
			}
		}
		cvtColor(sagittal_image, sagittal_image_bgr, CV_GRAY2BGR);
		line(sagittal_image_bgr, Point(axial_depth_val, 0), Point(axial_depth_val, IMAGE_ROWS), Scalar(255, 0, 0), 1);
		line(sagittal_image_bgr, Point(0, coronal_depth_val), Point(axial_depth_max - 1, coronal_depth_val), Scalar(0, 0, 255), 1);
		imshow("Sagittal", sagittal_image_bgr);

		// coronal
		Mat coronal_image, coronal_image_bgr;
		coronal_image = Mat(axial_depth_max, IMAGE_COLS, CV_8UC1);
		for (int y = 0; y < axial_depth_max; y++) {
			Mat buf;
			input_raw[y].convertTo(buf, CV_8U, 0.1275, 127.5);
			for (int x = 0; x < IMAGE_COLS; x++) {
				coronal_image.at<unsigned char>(Point(x, y)) = buf.at<unsigned char>(Point(x, coronal_depth_val));
				if (coronal_image.at<unsigned char>(Point(x, y)) >= 256) coronal_image.at<unsigned char>(Point(x, y)) = 255;
				else if (coronal_image.at<unsigned char>(Point(x, y)) < 0) coronal_image.at<unsigned char>(Point(x, y)) = 0;
			}
		}
		cvtColor(coronal_image, coronal_image_bgr, CV_GRAY2BGR);
		line(coronal_image_bgr, Point(0, axial_depth_val), Point(IMAGE_COLS, axial_depth_val), Scalar(255, 0, 0), 1);
		line(coronal_image_bgr, Point(sagittal_depth_val, 0), Point(sagittal_depth_val, axial_depth_max - 1), Scalar(0, 255, 0), 1);
		imshow("Coronal", coronal_image_bgr);

		int key = waitKey(30);
		if (key == 32) goto END;
	}

END:
	return 0;
}