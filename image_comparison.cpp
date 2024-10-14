#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <fstream>
#include <cctype>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
// #include <unistd.h>

#include "image_comparison.h"
#include "c_util.h"

using namespace cv;
using namespace std;

int64_t cv_time = 0;

void resizeImage(cv::Mat& image, int width, int height);

Mat reduceImageQuality(const Mat& inputImage, int quality) {
    // 이미지의 화질을 낮추기 위해 JPEG 포맷으로 압축 후 다시 디코딩
    std::vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(quality); // JPEG 압축 품질을 지정한 수준으로 설정

    std::vector<uchar> buf;
    imencode(".jpg", inputImage, buf, compression_params);
    Mat result = imdecode(buf, IMREAD_COLOR);

    return result;
}

int package_find(char *imgpath1, char *imgpath2, int thhold) {
    // int64_t cv_time = sample_gettimeus();
    // int64_t cv_buf;
    // int boxscale=0, boxscale2=0;
    int box_cnt=0;

    // 이미지 파일 경로 설정
    string image1Path = imgpath1;
    string image2Path = imgpath2;

    cv::Mat img1 = cv::imread(image1Path);
    cv::Mat img2 = cv::imread(image2Path);

    if (img1.empty() || img2.empty()) {
        cerr << "Can't Open File!" << ends;
        cerr << image1Path << ends;
        cerr << image2Path << ends;
        return -2;
    }

    resizeImage(img2, 1920/3, 1080/3);

    // cerr << imgpath1 << "W:" << img1.rows << "H:" << img1.cols << endl;
    // cerr << imgpath2 << "W:" << img2.rows << "H:" << img2.cols << endl;

    // cv::Rect roi(160, 180, 1600, 900-50);
    cv::Rect roi(160/3, 180/3, 1600/3, 900/3-50/3);
    img1 = img1(roi);
    img2 = img2(roi);
    
    int height = img1.rows;
    // int width = img1.cols;

    // int borderSize = 100;
    // cv::Rect roi(borderSize, height/4, width - 2 * borderSize, height*3/4 - borderSize);
    // img1 = img1(roi);
    // img2 = img2(roi);
    // dp("box3\n");
    cerr << "find try! : " << endl;
    try {
        cv::Mat gray1, gray2;
        // cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
        // cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

        cv::cvtColor(img1, gray1, cv::COLOR_BGR2HSV);
        cv::cvtColor(img2, gray2, cv::COLOR_BGR2HSV);

        


        // Clahe al /////////////////////////////////////////////////////////
        // cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(16, 9));
        // cv::Mat equalized1, equalized2;
        // clahe->apply(gray1, equalized1);
        // clahe->apply(gray2, equalized2);
        // /////////////////////////////////////////////////////////////////////

        // cv::imwrite("/tmp/mnt/sdcard/before_clahe.jpg", equalized1);
        // cv::imwrite("/tmp/mnt/sdcard/after_clahe.jpg", equalized2);


        // Hist al //////////////////////////////////////////////////////////
        // // 각 이미지의 평균 밝기 계산
        Scalar mean_intensity_img1 = mean(gray1);
        Scalar mean_intensity_img2 = mean(gray2);
        Scalar mean_avrg = (mean_intensity_img1+mean_intensity_img2) / 2;

         // 조명 보정을 위해 이미지의 밝기 정규화
        Mat normalized_img1, normalized_img2;
        convertScaleAbs(gray1, normalized_img1, 127.0 / mean_avrg[0], 0);
        convertScaleAbs(gray2, normalized_img2, 127.0 / mean_avrg[0], 0);

        cv::equalizeHist(normalized_img1, normalized_img1);
        cv::equalizeHist(normalized_img2, normalized_img2);

        // cv::imwrite("/tmp/mnt/sdcard/before_hist.jpg", normalized_img1);
        // cv::imwrite("/tmp/mnt/sdcard/after_hist.jpg", normalized_img2);
        /////////////////////////////////////////////////////////////////////

        // cv_buf = (sample_gettimeus() - cv_time)/1000;
        // cv_time = sample_gettimeus();
        // cerr << "find set threshold!"<< thhold << endl;
        // cv::Mat bin_img1, bin_img2;
        // cv::threshold(gray1, bin_img1, thhold, 255, cv::THRESH_BINARY);
        // cv::threshold(gray2, bin_img2, thhold, 255, cv::THRESH_BINARY);

        // dp("box1\n");
        // cv_buf = (sample_gettimeus() - cv_time)/1000;
        // cv_time = sample_gettimeus();
        cerr << "find gray!" << endl;
        cv::Mat diff_image;
        cv::absdiff(normalized_img1, normalized_img2, diff_image);

        // cv_buf = (sample_gettimeus() - cv_time)/1000;
        // cv_time = sample_gettimeus();
        cerr << "find set threshold!"<< endl;
        cv::Mat bin_img;
        cv::threshold(diff_image, bin_img, thhold, 255, cv::THRESH_BINARY);

        // cv::adaptiveThreshold(diff_image, bin_img, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 467, 37);

        // imwrite("/tmp/mnt/sdcard/bin.jpg", bin_img);

        // dp("box2\n");
        // cv_buf = (sample_gettimeus() - cv_time)/1000;
        // cv_time = sample_gettimeus();
        cerr << "find contours!" << endl;
        vector<vector<cv::Point>> contours;
        cv::findContours(bin_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // cv::Rect boundingRect2;
  
        for(size_t i = 0; i< contours.size(); i++) {
            cv::Rect boundingRect = cv::boundingRect(contours[i]);

            if (boundingRect.width > 15 && boundingRect.height > 15) {
                // box_cnt++;

                // Maximum Scale Box
                // boxscale = boundingRect.width * boundingRect.height;
                // if (boxscale > boxscale2){
                    // boxscale2 = boxscale;
                // }
                // Box Find area
                // if (boundingRect.x >= width/2 && boundingRect.x <= height/2) {
                // if (boundingRect.y+boundingRect.height >= height/3) {
                if (boundingRect.y >= height/3 && boundingRect.y+boundingRect.height > height/2) {
                        // cv::rectangle(img2, boundingRect, cv::Scalar(0, 255, 0), 2);
                        // NULL;
                    // }
                    box_cnt++;
                    cv::rectangle(img2, boundingRect, cv::Scalar(0, 255, 0), 2);
                }
                // cv::rectangle(img2, boundingRect, cv::Scalar(0, 255, 0), 2);
            }
        }
        // dp("box count:%d\n", ); 
    } catch (Exception& e) {
        box_cnt = 0xFF;
        dp("Too Many Box Count! Changed Camera Position!\n");
    }
    // cv::rectangle(img2, boundingRect2, cv::Scalar(0, 255, 0), 2);
    // dp("box4\n");

    cerr << "find end! : " << endl;

    cv::imwrite("/dev/shm/box_result.jpg", img2);

    return box_cnt;
}

int box_resize(char *imgpath1, char *outpath) {
    // int64_t cv_time = sample_gettimeus();
    // int64_t cv_buf;
    // int boxscale=0, boxscale2=0;
    int box_cnt=0;

    // 이미지 파일 경로 설정
    string imagePath = imgpath1;
    string outPath = outpath;

    cv::Mat img1 = cv::imread(imagePath);
    // cv::imwrite("/tmp/mnt/sdcard/box_a.jpg", img1);

    if (img1.empty()) {
        cerr << "Can't Open File!" << ends;
        cerr << imagePath << ends;
        return -2;
    }

    // resizeImage(img1, 1920/3, 1080/3);

    // cerr << imgpath1 << "W:" << img1.rows << "H:" << img1.cols << endl;
    // cerr << imgpath2 << "W:" << img2.rows << "H:" << img2.cols << endl;

    // cv::Rect roi(160, 180, 1600, 900-50);
    cv::Rect roi(408, 459, 1104, 612);

    img1 = img1(roi);

    cv::imwrite(outPath, img1);
    // cv::imwrite("/tmp/mnt/sdcard/box_r.jpg", img1);

    return box_cnt;
}

int package_sistic(char *imgpath1, char *imgpath2) {
    // int64_t cv_time = sample_gettimeus();
    // int64_t cv_buf;

    // 이미지 파일 경로 설정
    string imagePath1 = imgpath1;
    string imagePath2 = imgpath2;

    // 이미지 불러오기
    Mat image1 = imread(imagePath1, IMREAD_GRAYSCALE);
    Mat image2 = imread(imagePath2, IMREAD_GRAYSCALE);

    // 이미지 예외 처리
    if (image1.empty() || image2.empty()) {
        cerr << "Can't Open File!" << endl;
        cerr << imagePath1 << ends;
        cerr << imagePath2 << ends;
        return -1;
    }

    resizeImage(image1, 1920/3, 1080/3);
    resizeImage(image2, 1920/3, 1080/3);

#if 1
    cerr << "sistic make!" <<endl;
    // ORB 객체 생성
    Ptr<ORB> orb = ORB::create();

    cerr << "sistic point cal!" << endl;
    // 키 포인트와 디스크립터 계산
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    orb->detectAndCompute(image1, Mat(), keypoints1, descriptors1);
    orb->detectAndCompute(image2, Mat(), keypoints2, descriptors2);

    cerr << "sistic point match!" << endl;
    // 특징점 매칭 
    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    cerr << "sistic filtering!" << endl;
    // 좋은 매칭 필터링
    double minDist = min_element(matches.begin(), matches.end(),
        [](const DMatch& m1, const DMatch& m2) { return m1.distance < m2.distance; })->distance;

    vector<DMatch> goodMatches;
    for (const DMatch& match : matches) {
        // dp("distance:%f %f\n", match.distance, minDist);
        if (match.distance < 3 * minDist) {
            goodMatches.push_back(match);
        }
    }

    cerr << "sistic metrix cal!" << endl;
    // 좋은 매칭을 사용하여 변환 행렬 계산
    if (goodMatches.size() > 10) {
        try {
            vector<Point2f> pts1, pts2;
            for (const DMatch& match : goodMatches) {
                pts1.push_back(keypoints1[match.queryIdx].pt);
                pts2.push_back(keypoints2[match.trainIdx].pt);
            }

    
            Mat H = findHomography(pts1, pts2, RANSAC);

            // 이미지를 변환 행렬을 사용하여 보정
            Mat correctedImage;
            warpPerspective(image1, correctedImage, H, image1.size());

            imwrite("/dev/shm/corimg1.jpg", correctedImage);
        } catch (Exception& e) {
            cerr << "Fail comparison points!" << endl;

            imwrite("/dev/shm/corimg1.jpg", image1);
            ///////////////// Log Point /////////////////
        }
    }
    else {
        cerr << "Not enough comparison points:" << goodMatches.size() << endl;

        imwrite("/dev/shm/corimg1.jpg", image1);
        ///////////////// Log Point /////////////////
    }
#else
    Ptr<SIFT> sift = SIFT::create();

    vector<KeyPoint> kp1, kp2;
    Mat des1, des2;
    sift->detectAndCompute(image1, noArray(), kp1, des1);
    sift->detectAndCompute(image2, noArray(), kp2, des2);

    BFMatcher bf(NORM_L2);
    vector<vector<DMatch>> matches;
    bf.knnMatch(des1, des2, matches, 2);

    vector<DMatch> good;
    for (size_t i = 0; i < matches.size(); ++i) {
        if (matches[i][0].distance < 0.75 * matches[i][1].distance) {
            good.push_back(matches[i][0]);
        }
    }


    if (good.size() > 10) {
        try {
            vector<Point2f> src_pts, dst_pts;
            for (size_t i = 0; i < good.size(); ++i) {
                src_pts.push_back(kp1[good[i].queryIdx].pt);
                dst_pts.push_back(kp2[good[i].trainIdx].pt);
            }

            // 변환 매트릭스 계산
            Mat H = findHomography(src_pts, dst_pts, RANSAC, 5.0);

            Mat correctedImage;
            warpPerspective(image1, correctedImage, H, image1.size());

            imwrite("/dev/shm/corimg1.jpg", correctedImage);
        } catch (Exception& e) {
            cerr << "Fail comparison points!" << endl;

            imwrite("/dev/shm/corimg1.jpg", image1);
            ///////////////// Log Point /////////////////
        }
    }
    else {
        cerr << "Not enough comparison points!" << endl;

        imwrite("/dev/shm/corimg1.jpg", image1);
    }
#endif
    // 결과 이미지를 디스크에 저장
    cerr << "Sistic End" << endl;

    return 0;
}



double calculateSimilarity(char *imgpath1, char *imgpath2) {
        // 이미지 파일 경로 설정
    string imagePath1 = imgpath1;
    string imagePath2 = imgpath2;

    // 이미지 불러오기
    // cv::Mat imag1 = imread(imagePath1, IMREAD_GRAYSCALE);
    // cv::Mat imag2 = imread(imagePath2, IMREAD_GRAYSCALE);

    // resizeImage(imag1, 1920/3, 1080/3);
    // resizeImage(imag2, 1920/3, 1080/3);

    // cerr << imgpath1 << "W:" << img1.rows << "H:" << img1.cols << endl;
    // cerr << imgpath2 << "W:" << img2.rows << "H:" << img2.cols << endl;

    cv::Mat imag1 = cv::imread(imagePath1);
    cv::Mat imag2 = cv::imread(imagePath2);

    if (imag1.empty() || imag2.empty()) {
        cerr << "Can't Open File!" << ends;
        cerr << imagePath1 << ends;
        cerr << imagePath2 << ends;
        return -2;
    }

    resizeImage(imag1, 1920/3, 1080/3);
    resizeImage(imag2, 1920/3, 1080/3);

    cv::Rect roi(160/3, 180/3, 1600/3, 900/3-3);
    imag1 = imag1(roi);
    imag2 = imag2(roi);

    cout << "cvtColor" << endl;

    // Mat blurred1, blurred2;
    // GaussianBlur(gray_image1, blurred1, Size(5, 5), 0);
    // GaussianBlur(gray_image2, blurred2, Size(5, 5), 0);

    // // 평균 필터링을 사용하여 플리커 제거
    // Mat flickerRemoved1, flickerRemoved2;
    // // blur(blurred1, flickerRemoved1, Size(5, 5));
    // // blur(blurred2, flickerRemoved2, Size(5, 5));
    // imag1 = reduceImageQuality(imag1, 30);
    // imag1 = reduceImageQuality(imag2, 30);

    Mat gray_image1, gray_image2;
    cvtColor(imag1, gray_image1, COLOR_BGR2GRAY);
    cvtColor(imag2, gray_image2, COLOR_BGR2GRAY);

    // imwrite("/tmp/mnt/sdcard/flickerremove1.jpg", flickerRemoved1);
    // imwrite("/tmp/mnt/sdcard/flickerremove2.jpg", flickerRemoved2);

    // 밝기 측정
    cout << "mean" << endl;
    double brightness1 = mean(gray_image1)[0];
    double brightness2 = mean(gray_image2)[0];

    // 두 이미지의 밝기의 평균을 계산
    double average_brightness = (brightness1 + brightness2) / 2;

    cout << "Average Brightness: " << average_brightness << endl;

    // 평균 밝기로 이미지 보정
    cout << "convertTo" << endl;
    double ratio = average_brightness / brightness1;
    Mat corrected_image1, corrected_image2;
    gray_image1.convertTo(corrected_image1, -1, ratio, 0);
    gray_image2.convertTo(corrected_image2, -1, ratio, 0);

    cout << "calcHist" << endl;
    cv::Mat histImage1, histImage2;
    int hisSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::calcHist(&corrected_image1, 1, 0, cv::Mat(), histImage1, 1, &hisSize, &histRange);
    cv::calcHist(&corrected_image2, 1, 0, cv::Mat(), histImage2, 1, &hisSize, &histRange);

    // HISTCMP_CORREL        = 0,
    // HISTCMP_CHISQR        = 1,
    // HISTCMP_INTERSECT     = 2,
    // HISTCMP_BHATTACHARYYA = 3,
    // HISTCMP_HELLINGER     = HISTCMP_BHATTACHARYYA, //!< Synonym for HISTCMP_BHATTACHARYYA
    // HISTCMP_CHISQR_ALT    = 4,
    // HISTCMP_KL_DIV        = 5

    //     typedef struct SIMIL_T
    // {
    //     double correl:
    //     double chisqr;
    //     double intersect;
    //     double bhattacharyya;
    //     double kl_div;
    // } Simil_t;

    double similarity = cv::compareHist(histImage1, histImage2, cv::HISTCMP_CHISQR);

    return similarity;
}


int calculateSimilarity2(char *imgpath1, char *imgpath2, Simil_t2* sim_t) {
        // 이미지 파일 경로 설정
    string imagePath1 = imgpath1;
    string imagePath2 = imgpath2;

    // 이미지 불러오기
    // cv::Mat imag1 = imread(imagePath1, IMREAD_GRAYSCALE);
    // cv::Mat imag2 = imread(imagePath2, IMREAD_GRAYSCALE);

    // resizeImage(imag1, 1920/3, 1080/3);
    // resizeImage(imag2, 1920/3, 1080/3);

    // cerr << imgpath1 << "W:" << img1.rows << "H:" << img1.cols << endl;
    // cerr << imgpath2 << "W:" << img2.rows << "H:" << img2.cols << endl;

    cv::Mat imag1 = cv::imread(imagePath1);
    cv::Mat imag2 = cv::imread(imagePath2);

    if (imag1.empty() || imag2.empty()) {
        cerr << "Can't Open File!" << ends;
        cerr << imagePath1 << ends;
        cerr << imagePath2 << ends;
        return -2;
    }

    resizeImage(imag1, 1920/3, 1080/3);
    resizeImage(imag2, 1920/3, 1080/3);

    cv::Rect roi(160/3, 180/3, 1600/3, 900/3-3);
    imag1 = imag1(roi);
    imag2 = imag2(roi);

    cout << "cvtColor" << endl;

    // Mat blurred1, blurred2;
    // GaussianBlur(gray_image1, blurred1, Size(5, 5), 0);
    // GaussianBlur(gray_image2, blurred2, Size(5, 5), 0);

    // // 평균 필터링을 사용하여 플리커 제거
    // Mat flickerRemoved1, flickerRemoved2;
    // // blur(blurred1, flickerRemoved1, Size(5, 5));
    // // blur(blurred2, flickerRemoved2, Size(5, 5));
    // imag1 = reduceImageQuality(imag1, 30);
    // imag1 = reduceImageQuality(imag2, 30);

    Mat gray_image1, gray_image2;
    cvtColor(imag1, gray_image1, COLOR_BGR2GRAY);
    cvtColor(imag2, gray_image2, COLOR_BGR2GRAY);

    // imwrite("/tmp/mnt/sdcard/flickerremove1.jpg", flickerRemoved1);
    // imwrite("/tmp/mnt/sdcard/flickerremove2.jpg", flickerRemoved2);

    // 밝기 측정
    cout << "mean" << endl;
    double brightness1 = mean(gray_image1)[0];
    double brightness2 = mean(gray_image2)[0];

    // 두 이미지의 밝기의 평균을 계산
    double average_brightness = (brightness1 + brightness2) / 2;

    cout << "Average Brightness: " << average_brightness << endl;

    // 평균 밝기로 이미지 보정
    cout << "convertTo" << endl;
    double ratio = average_brightness / brightness1;
    Mat corrected_image1, corrected_image2;
    gray_image1.convertTo(corrected_image1, -1, ratio, 0);
    gray_image2.convertTo(corrected_image2, -1, ratio, 0);

    cout << "calcHist" << endl;
    cv::Mat histImage1, histImage2;
    int hisSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::calcHist(&corrected_image1, 1, 0, cv::Mat(), histImage1, 1, &hisSize, &histRange);
    cv::calcHist(&corrected_image2, 1, 0, cv::Mat(), histImage2, 1, &hisSize, &histRange);

    // HISTCMP_CORREL        = 0,
    // HISTCMP_CHISQR        = 1,
    // HISTCMP_INTERSECT     = 2,
    // HISTCMP_BHATTACHARYYA = 3,
    // HISTCMP_HELLINGER     = HISTCMP_BHATTACHARYYA, //!< Synonym for HISTCMP_BHATTACHARYYA
    // HISTCMP_CHISQR_ALT    = 4,
    // HISTCMP_KL_DIV        = 5

    //     typedef struct SIMIL_T
    // {
    //     double correl:
    //     double chisqr;
    //     double intersect;
    //     double bhattacharyya;
    //     double kl_div;
    // } Simil_t;

    sim_t->correl = cv::compareHist(histImage1, histImage2, cv::HISTCMP_CORREL);
    cout << "HISTCMP_CORREL        : " << sim_t->correl << endl;
    sim_t->chisqr = cv::compareHist(histImage1, histImage2, cv::HISTCMP_CHISQR);
    cout << "HISTCMP_CHISQR        : " << sim_t->chisqr << endl;
    sim_t->intersect = cv::compareHist(histImage1, histImage2, cv::HISTCMP_INTERSECT);
    cout << "HISTCMP_INTERSECT     : " << sim_t->intersect << endl;
    sim_t->bhattacharyya = cv::compareHist(histImage1, histImage2, cv::HISTCMP_BHATTACHARYYA);
    cout << "HISTCMP_BHATTACHARYYA : " << sim_t->bhattacharyya << endl;
    sim_t->kl_div = cv::compareHist(histImage1, histImage2, cv::HISTCMP_KL_DIV);
    cout << "HISTCMP_KL_DIV        : " << sim_t->kl_div << endl;

    return 1;
}

// int main(int argc, char ** argv) {
// 	int ret = 0;
//     double sim = 0.0;
// 	// string before_img = "./before.jpg";
// 	// string after_img = "./after.jpg";
//     char *before_img;
//     char *after_img;
// 	int threshold = 100;

// 	int option;
//     if (argc != 7) {
//         dp("Usage: %s -b <before.jpg> -a <after.jpg> -t <threshold value>\n", argv[0]);
//         return -1;
//     }
// 	while((option = getopt(argc, argv, "b:a:t:")) != -1) {
// 		switch (option) {
// 			case 'b':
// 				before_img = optarg;
// 				break;
// 			case 'a':
// 				after_img = optarg;
// 				break;
// 			case 't':
// 				threshold = stoi(optarg);
// 				break;
// 			default:
// 				dp("Usage: %s -b <before.jpg> -a <after.jpg> -t <threshold value>", argv[0]);
// 				return -1;
// 		}
// 	}

// 	ret = package_sistic(before_img, after_img);
// 	if(ret < 0) {
// 		dp("Package Sistci Fail!\n");
// 		return ret;
// 	}

// 	ret = package_find(after_img, threshold);
// 	if(ret < 0) {
// 		dp("Package Find Fail!\n");
// 		return ret;
// 	}
//     else {
//         dp("Box Count : %d\n", ret);
//         if (ret == 0) {
//             sim = calculateSimilarity(after_img);
//             // dp("similarity:%f %\n", sim);
//             std::cout << "Similarity:" << sim << "\n" << std::ends;
//         }
//     }

// 	return 0;
// }


// double calculateImageSimilarity(const Mat& img1, const Mat& img2) {
//     Mat gray1, gray2;
//     cvtColor(img1, gray1, COLOR_BGR2GRAY);
//     cvtColor(img2, gray2, COLOR_BGR2GRAY);

//     Mat diff;
//     absdiff(gray1, gray2, diff);

//     threshold(diff, diff, 30, 255, THRESH_BINARY);

//     int nonZeroCount = countNonZero(diff);
//     double similarity = ((gray1.total() - nonZeroCount) / static_cast<double>(gray1.total())) * 100.0;

//     return similarity;
// }

// vector<Mat> splitImage(const Mat& image) {
//     int segmentWidth = image.cols / 4;
//     int segmentHeight = image.rows / 4;

//     vector<Mat> segments;

//     for (int i = 0; i < 4; ++i) {
//         for (int j = 0; j < 4; ++j) {
//             Rect roi(j * segmentWidth, i * segmentHeight, segmentWidth, segmentHeight);
//             Mat segment = image(roi);
//             segments.push_back(segment);
//         }
//     }

//     return segments;
// }

// int divisions_similarity(char *imgpath1, char *imgpath2) {
//     // Replace with the actual file paths of your images

//     string imagePath1 = imgpath1;
//     string imagePath2 = imgpath2;

//     Mat img1 = imread(imagePath1);
//     Mat img2 = imread(imagePath2);

//     if (img1.empty() || img2.empty()) {
//         cerr << "Error loading images." << endl;
//         return -1;
//     }

//     vector<Mat> segments1 = splitImage(img1);
//     vector<Mat> segments2 = splitImage(img2);

//     for (int i = 0; i < 16; ++i) {
//         double similarity = calculateImageSimilarity(segments1[i], segments2[i]);

//         if (similarity <= 70.0) {
//             cout << "Segment " << i + 1 << ": Similarity " << similarity << "%" << endl;
//         }
//     }

//     return 0;
// }

// void mosaic(cv::Mat& image, int x, int y, int width, int height, int size) {
//     // 모자이크 처리할 부분 선택
//     cv::Mat roi = image;

//     // 모자이크 처리
//     cv::resize(roi, roi, cv::Size(size, size), 0, 0, cv::INTER_NEAREST);
//     cv::resize(roi, roi, cv::Size(width, height), 0, 0, cv::INTER_NEAREST);


//     // 원본 이미지에 모자이크 처리된 부분 적용
//     roi.copyTo(image(cv::Rect(x, y, width, height)));
// }

// void mosaic(cv::Mat& img, int x, int y, int w, int h, int size) {
//     // 모자이크 처리 반경
//     int mosaic_radius = 50;

//     // 좌표 주변을 모자이크 처리
//     cv::Rect roi(x - mosaic_radius, y - mosaic_radius, w + mosaic_radius * 2, h + mosaic_radius * 2);
//     cv::Mat roi_mat = img(roi);
//     cv::resize(roi_mat, roi_mat, cv::Size(50, 50), 0, 0, cv::INTER_NEAREST);
// }

// void resizeImage(cv::Mat& image, int width, int height) {
//     cv::resize(image, image, cv::Size(width, height));
// }

// // int thumbnail_make(int x, int y, int width, int height) {
// int thumbnail_make(Thum_Data_t cont) {
//     int i, w, h;


//     // 이미지 로드
//     cv::String imagePath = "/dev/shm/thumbnail.jpg";
//     cv::Mat originalImage = cv::imread(imagePath);

//     dp("Thumbnail file load!\n");

//     // 모자이크 처리할 좌표 및 크기 설정
//     // int x = 100, y = 100, width = 120, height = 120;
//     int mosaicSize = 150;

//     // 모자이크 처리 및 크기 조절
//     cv::Mat processedImage = originalImage.clone();
//     dp("Thumbnail make clone!\n");
//     // for(i=0;i<10;i++) {
//     //     if(cont.flag[i]){
//     //         w = ((cont.ex[i]/150)+1)*150;
//     //         h = ((cont.ey[i]/150)+1)*150;
//     //         dp("thum:%d %d %d %d\n", cont.x[i], cont.y[i], w, h);

//     //         mosaic(processedImage, cont.x[i], cont.y[i], w, h, mosaicSize);

//     //     }
//     // }
//     mosaic(processedImage, 100, 180, 300, 400, mosaicSize);
//     resizeImage(processedImage, 640, 360);

//     // 결과 이미지 저장
//     cv::String outputPath = "/dev/shm/thumbnail_last.jpg";
//     cv::imwrite(outputPath, processedImage);

//     return 0;
// }

void resizeImage(cv::Mat& image, int width, int height) {
    cv::resize(image, image, cv::Size(width, height));
}


int thumbnail_make(Thum_Data_t cont) {
    int x, y, w, h;
    // 이미지 파일 경로
    std::string inputImagePath = "/dev/shm/thumbnail.jpg";
    std::string outputImagePath = "/dev/shm/thumbnail_last.jpg";

    // 좌표 및 크기 설정 (x, y, w, h)
    // int x = 100;
    // int y = 180;
    // int w = 400;
    // int h = 550;

    // 이미지 로드
    Mat image = imread(inputImagePath);

    Mat mosaic = image.clone();
    // 모자이크 처리
    try {
        for (int i=0; i<10; i++) {
            if (cont.flag[i]) {
                x = cont.x[i];
                y = cont.y[i];
                w = (((cont.ex[i] - cont.x[i])/150)+1)*150;
                h = (((cont.ey[i] - cont.y[i])/150)+1)*150;
                if (x > 0 && y > 0 && w > 0 && h > 0) {
                    Rect roi(x, y, w, h);
                    Mat region = mosaic(roi);
                    resize(region, region, Size(5, 5), 0, 0, INTER_NEAREST);
                    resize(region, region, Size(w, h), 0, 0, INTER_NEAREST);

                    // 모자이크된 영역을 원본 이미지에 복사
                    region.copyTo(mosaic(roi));
                }
            }
        }
    } catch (Exception& e) {
        cerr << "Thumbnail Mosaic Fail!" << endl;
        ///////////////// Log Point /////////////////
    }

    resizeImage(mosaic, 640, 480);

    // 모자이크 처리된 이미지 저장
    imwrite(outputImagePath, mosaic);

    return 0;
}


/*int facecrop_make(Fdpd_Data_t cont) {
    int cx, cy, size, x, y;
    // 이미지 파일 경로
    std::string inputImagePath = "/dev/shm/face.jpg";
    std::string outputImagePath = "/dev/shm/face_crop" + std::to_string(face_crop_cnt) + ".jpg";

    cx = (cont.ul_x + cont.br_x)/2;
    cy = (cont.ul_y + cont.br_y)/2;
    
    if ( (cont.br_y - cont.ul_y) > (cont.br_x - cont.ul_x) )
        size = cont.br_y - cont.ul_y;
    else
        size = cont.br_x - cont.ul_x;    
    
    size = size*2;
    if (size > 500) size = 500;

    x = cx - (size/2);
    y = cy - (size/2);

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if (x+size >= 1920) x = 1920-size-1;
    if (y+size >= 1080) y = 1080-size-1;
    
    // }
    // else {
    //     x = cx - 250;
    //     y = cy - 250;

    //     if (x < 0) x = 0;
    //     if (y < 0) y = 0;

    //     if (x+500 > 1920) x = 1920-500-1;
    //     if (y+500 > 1080) y = 1080-500-1;
    // }



    dp("cx:%d cy:%d size:%d x:%d y:%d\n", cx, cy, size, x, y);
    try {
        // 이미지 로드
        Mat image = imread(inputImagePath);

        // 입력 좌표와 크기로 이미지를 크롭
        Rect roi(x, y, size, size);
        Mat croppedImage = image(roi);

        resizeImage(croppedImage, 500, 500);

        vector<int> compression_params;
        compression_params.push_back(IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        // 크롭된 이미지 저장
        imwrite(outputImagePath, croppedImage, compression_params);
        face_crop_cnt++;
        return 0;

    } catch (Exception& e) {
        cerr << "Face Crop Fail!" << endl;
        return -1;
    }
}*/


int facecrop_make(Fdpd_Data_t cont) {
    // int cx, cy, size, x, y;
    // 이미지 파일 경로
    std::string inputImagePath = "/dev/shm/face.jpg";
    std::string outputImagePath = "/dev/shm/face_crop" + std::to_string(face_crop_cnt) + ".jpg";

    // cx = (cont.ul_x + cont.br_x)/2;
    // cy = (cont.ul_y + cont.br_y)/2;
    
    // if ( (cont.br_y - cont.ul_y) > (cont.br_x - cont.ul_x) )
    //     size = cont.br_y - cont.ul_y;
    // else
    //     size = cont.br_x - cont.ul_x;    
    
    // size = size*2;
    // // if (size > 500) size = 500;

    // x = cx - (size/2);
    // y = cy - (size/2);

    // if (x < 0) x = 0;
    // if (y < 0) y = 0;

    // if (x+size >= 1920) x = 1920-size-1;
    // if (y+size >= 1080) y = 1080-size-1;
    
    // }
    // else {
    //     x = cx - 250;
    //     y = cy - 250;

    //     if (x < 0) x = 0;
    //     if (y < 0) y = 0;

    //     if (x+500 > 1920) x = 1920-500-1;
    //     if (y+500 > 1080) y = 1080-500-1;
    // }

    float s1cx = cont.ul_x + (cont.br_x-cont.ul_x) / 2.0;
    float s1cy = cont.ul_y + (cont.br_y-cont.ul_y) / 2.0;
    float m = std::max((cont.br_x-cont.ul_x), (cont.br_y-cont.ul_y));


    try {
        // // 이미지 로드
        // Mat image = imread(inputImagePath);

        // // 입력 좌표와 크기로 이미지를 크롭
        // Rect roi(x, y, size, size);
        // Mat croppedImage = image(roi);

        // resizeImage(croppedImage, 500, 500);

       

        // // 크롭된 이미지 저장
        // imwrite(outputImagePath, croppedImage, compression_params);


        // Define the source points
        std::vector<cv::Point2f> src = {
            cv::Point2f(s1cx - m, s1cy - m),
            cv::Point2f(s1cx - m, s1cy + m),
            cv::Point2f(s1cx + m, s1cy - m),
            cv::Point2f(s1cx + m, s1cy + m)
        };

        // Define the destination points for 500x500 image
        std::vector<cv::Point2f> dst = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 500),
            cv::Point2f(500, 0),
            cv::Point2f(500, 500)
        };

        cv::Mat trans = cv::getAffineTransform(src.data(), dst.data());

        Mat image = imread(inputImagePath);

        cv::Mat wImg;
        cv::warpAffine(image, wImg, trans, cv::Size(500, 500), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

        vector<int> compression_params;
        compression_params.push_back(IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        imwrite(outputImagePath, wImg, compression_params);

        // dp("cx:%d cy:%d size:%d x:%d y:%d\n", cx, cy, size, x, y);


        face_crop_cnt++;
        return 0;

    } catch (Exception& e) {
        cerr << "Face Crop Fail!" << endl;
        return -1;
    }
}

void *facecrop_make_thread(void *argc) {
    // int cx, cy, size, x, y;
    // 이미지 파일 경로

    Fdpd_Data_t *cont = (Fdpd_Data_t*)argc;
    std::string inputImagePath = "/dev/shm/face" + std::to_string(cont->cnt) + ".jpg";
    std::string outputImagePath = "/dev/shm/face_crop" + std::to_string(face_crop_cnt) + ".jpg";
    face_crop_cnt++;

    float s1cx = cont->ul_x + (cont->br_x-cont->ul_x) / 2.0;
    float s1cy = cont->ul_y + (cont->br_y-cont->ul_y) / 2.0;
    float m = std::max((cont->br_x-cont->ul_x), (cont->br_y-cont->ul_y));
    int flag_cnt = cont->cnt;

    face_end_f[flag_cnt] = true;


    try {
        // Define the source points
        std::vector<cv::Point2f> src = {
            cv::Point2f(s1cx - m, s1cy - m),
            cv::Point2f(s1cx - m, s1cy + m),
            cv::Point2f(s1cx + m, s1cy - m),
            cv::Point2f(s1cx + m, s1cy + m)
        };

        // Define the destination points for 500x500 image
        std::vector<cv::Point2f> dst = {
            cv::Point2f(0, 0),
            cv::Point2f(0, 500),
            cv::Point2f(500, 0),
            cv::Point2f(500, 500)
        };

        cv::Mat trans = cv::getAffineTransform(src.data(), dst.data());

        Mat image = imread(inputImagePath);

        cv::Mat wImg;
        cv::warpAffine(image, wImg, trans, cv::Size(500, 500), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

        vector<int> compression_params;
        compression_params.push_back(IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        imwrite(outputImagePath, wImg, compression_params);

        
        face_end_f[flag_cnt] = false;
        return 0;

    } catch (Exception& e) {
        cerr << "Face Crop Fail!" << endl;
        face_crop_cnt--;
        cont->cnt--;
        face_end_f[flag_cnt] = false;
        return 0;
    }
}


double calculateSharpness(const std::string& imagePath) {
    // 이미지 로드
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Can't Open Image." << std::endl;
        return -1.0;
    }

    // 이미지의 라플라시안 변환 계산
    cv::Mat laplacian;
    cv::Laplacian(image, laplacian, CV_64F);

    // 라플라시안 변환의 절대값 계산
    cv::Mat absLaplacian = cv::abs(laplacian);

    // 라플라시안 변환의 평균값 계산
    double sharpness = cv::mean(absLaplacian)[0];

    return sharpness;
}

double Sharpness_cal(char *imgpath1) {
    // 이미지 파일 경로
    std::string imagePath = imgpath1;

    // 선명도 계산
    double sharpness = calculateSharpness(imagePath);

    if (sharpness >= 0) {
        std::cout << "Sharpness: " << sharpness << std::endl;
    }

    return sharpness;
}


pair<double, double> calculateFocusAndSharpness(const string& imagePath) {
    // 이미지 로드
    Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    if (image.empty()) {
        cerr << "Can not open Image!" << endl;
        return make_pair(-1.0, -1.0);
    }

    // 이미지의 라플라시안 변환 계산
    Mat laplacian;
    Laplacian(image, laplacian, CV_64F);

    // 라플라시안 변환의 절대값 계산
    Mat absLaplacian = abs(laplacian);

    // 라플라시안 변환의 평균값 계산 (선명도)
    double sharpness = mean(absLaplacian)[0];

    // 이미지의 그레이스케일 변화의 표준 편차 계산 (초점)
    Scalar meanValue, stdDevValue;
    meanStdDev(image, meanValue, stdDevValue);
    double focus = stdDevValue[0] * stdDevValue[0];

    return make_pair(focus, sharpness);
}


int focus_and_sharpness_cal(char *imgpath1, Focus_Sharpness2 *fs_t) {
    // 이미지 파일 경로
    std::string imagePath = imgpath1;

    try {
        // 초점과 선명도 계산
        pair<double, double> result = calculateFocusAndSharpness(imagePath);

        if (result.first >= 0 && result.second >= 0) {
            cout << "path: " << imagePath<< endl;
            cout << "Focus: " << result.first << endl;
            cout << "Sharpness: " << result.second << endl;
        }

        fs_t->focus = result.first;
        fs_t->sharpness = result.second;

        return 0;
    } catch (Exception& e) {
        cerr << "Focus and Sharpness Cal Fail!" << endl;
        return -1;
    }
}

#ifdef __BOX_ALGORITH__

#define _RESIZE_RATIO           0.5
#define _IMAGE_X                1600                        // 원본 영상의 가로 크기
#define _IMAGE_Y                900                         // 원본 영상의 세로 크기

#define _REF_IMG                "/tmp/mnt/sdcard/box_origin.jpg"    // 초기 비교 영상

#define _OUTPUT_PATH_LABEL      "/tmp/mnt/sdcard/"

// std::vector<std::string> get_files_inDirectory(const std::string& _path, const std::string& _filter);

int test_box_al(void)
{
    // int _IMAGE_COUNT =0;
    // std::vector<std::string> img_files = get_files_inDirectory(_IMG_PATH, _IMG_EXTENSION);
    // std::vector<std::string> img_files = "/dev/shm/box.jpg"
    // std::vector<std::string> img_path;
    // _IMAGE_COUNT = img_files.size();
    // for (int i = 0; i < _IMAGE_COUNT; ++i) img_path.push_back(_IMG_PATH + img_files[i]);

    std::vector<cv::Mat> img_array;
    cv::Mat img_ref = imread(_REF_IMG, cv::IMREAD_COLOR);
    cv::resize(img_ref, img_ref, cv::Size(img_ref.cols * _RESIZE_RATIO, img_ref.rows * _RESIZE_RATIO), 0, 0, cv::INTER_AREA);
    cv::Mat bilateral_ref;
    cv::bilateralFilter(img_ref, bilateral_ref, 5, 100, 100);

    cv::Mat dil_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25));
    cv::Mat erd_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(23, 23));

    // for (int i = 0; i < _IMAGE_COUNT; ++i) {
        std::string path = "/dev/shm/box0.jpg";
        std::cout << path << std::endl;
        cv::Mat tmp = imread(path, cv::IMREAD_COLOR);

        cv::Mat resize_tmp = tmp;
        cv::resize(tmp, resize_tmp, cv::Size(tmp.cols * _RESIZE_RATIO, tmp.rows * _RESIZE_RATIO), 0, 0, cv::INTER_AREA);
        
        cv::Mat bilateral;
        cv::bilateralFilter(resize_tmp, bilateral, 5, 100, 100);

        cv::Mat sub;
        cv::absdiff(bilateral_ref, bilateral, sub);

        cv::Mat channels[3];
        cv::cvtColor(sub, sub, cv::COLOR_BGR2HSV);
        cv::split(sub, channels);
        cv::Mat blue = channels[2];
        //cv::cvtColor(sub, blue, cv::COLOR_BGR2HSV);

        cv::Mat threshold;
        cv::threshold(blue, threshold, 25, 255, cv::THRESH_BINARY);

        cv::Mat median;
        cv::medianBlur(threshold, median, 11);

        cv::Mat dilate, erode;
        cv::dilate(median, dilate, dil_kernel);
        cv::erode(dilate, erode, erd_kernel);
        cv::dilate(erode, dilate, dil_kernel);
        cv::erode(dilate, erode, erd_kernel);

        cv::Mat img_labels, stats, centroids;
        int numOfLables = cv::connectedComponentsWithStats(erode, img_labels, stats, centroids, 8, CV_32S);
        std::cout << stats << std::endl;
    
        for (int j = 1; j < numOfLables; j++) {
            int area = stats.at<int>(j, cv::CC_STAT_AREA);
            if (area < 1000) continue;
            int left = stats.at<int>(j, cv::CC_STAT_LEFT);
            int top = stats.at<int>(j, cv::CC_STAT_TOP);
            int width = stats.at<int>(j, cv::CC_STAT_WIDTH);
            int height = stats.at<int>(j, cv::CC_STAT_HEIGHT);

            cv::rectangle(resize_tmp, cv::Point(left, top), cv::Point(left + width, top + height), cv::Scalar(0, 0, 255), 1);
            cv::putText(resize_tmp, std::to_string(j), cv::Point(left + 20, top + 20), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 1);
        }

        std::string img_name = "find_box_tmp.jpg";
        std::string img_erode = "find_box_erode.jpg";
        cv::imwrite(_OUTPUT_PATH_LABEL + img_name, resize_tmp);
        cv::imwrite(_OUTPUT_PATH_LABEL + img_erode, erode);

        img_array.push_back(erode);
    
    // }
    dp("image loaded!\n");



    return 0;
}

// 폴더 내 파일이름 불러오기
// std::vector<std::string> get_files_inDirectory(const std::string& _path, const std::string& _filter) {
//     std::string searching = _path + _filter;

//     std::vector<std::string> return_;

//     struct _finddata_t fd;
//     intptr_t handle;

//     if ((handle = _findfirst(searching.c_str(), &fd)) == -1L)
//         std::cout << "No file in directory!" << std::endl;

//     do
//     {
//         if (fd.name[0] == '.') continue;
//         if (fd.name[0] == '.') continue;
//         return_.push_back(fd.name);
//     } while (_findnext(handle, &fd) == 0);

//     _findclose(handle);
//     return return_;
// }

#endif

void absdiff_normalize_arr(cv::Mat grayA, cv::Mat grayB, cv::Mat image_org) {
    cv::Mat diff_image;
    cv::absdiff(grayA, grayB, diff_image);
    // cv::imshow("diff_image", diff_image);
    // cv::waitKey(0);

    // otsu threshold
    cv::Mat thresh;
    cv::threshold(diff_image, thresh, 85, 255, cv::THRESH_BINARY);
    // cv::imshow("binary thresh", thresh);
    // cv::waitKey(0);

    // 외곽선 정보 검출
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(thresh, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 컬러 영상으로 변환
    cv::Mat dst;
    cv::cvtColor(thresh, dst, cv::COLOR_GRAY2BGR);
    int h1 = dst.rows;
    int w1 = dst.cols;
    // int channel1 = dst.channels();

    // original picture loading
    int h2 = image_org.rows;
    int w2 = image_org.cols;
    // int channel2 = image_org.channels();

    double htimes = static_cast<double>(h2) / h1;
    double wtimes = static_cast<double>(w2) / w1;

    int minrectw = static_cast<int>(w1 / 10);
    int minrecth = static_cast<int>(h1 / 10);
    int maxrectw = static_cast<int>(w1 - 10);
    int maxrecth = static_cast<int>(h1 - 10);

    for (size_t i = 0; i < contours.size(); ++i) {
        // cv::Scalar c = cv::Scalar(255, 0, 0);
        cv::Rect rect = cv::boundingRect(contours[i]);

        if (rect.width > minrectw && rect.height > minrecth && rect.width < maxrectw && rect.height < maxrecth) {
            cv::drawContours(dst, contours, static_cast<int>(i), cv::Scalar(0, 0, 255), 2);
            cv::putText(dst, std::to_string(i), contours[i][0], cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0, 255, 0), 1);
            cv::rectangle(dst, rect, cv::Scalar(0, 255, 0), 2);

            // draw rectangle on original image
            int x1 = static_cast<int>(rect.x * wtimes);
            int y1 = static_cast<int>(rect.y * htimes);
            int w1 = static_cast<int>(rect.width * wtimes);
            int h1 = static_cast<int>(rect.height * htimes);
            cv::rectangle(image_org, cv::Rect(x1, y1, w1, h1), cv::Scalar(0, 255, 0), 2);
        }
    }

    // cv::imshow("countours", dst);
    // cv::waitKey(0);

    // cv::imshow("results", image_org);
    // cv::waitKey(0);
}

// int main() {
//     // Load your images here
//     cv::Mat grayA = cv::imread("grayA.jpg", cv::IMREAD_GRAYSCALE);
//     cv::Mat grayB = cv::imread("grayB.jpg", cv::IMREAD_GRAYSCALE);
//     cv::Mat image_org = cv::imread("image_org.jpg");

//     if (grayA.empty() || grayB.empty() || image_org.empty()) {
//         std::cerr << "Could not open or find the images!" << std::endl;
//         return -1;
//     }

//     absdiff_normalize_arr(grayA, grayB, image_org);

//     cv::waitKey(0); // Wait for any key press
//     return 0;
// }

// #include <opencv2/opencv.hpp>
// #include <iostream>
// #include <chrono>

const int division_step = 30;
const int stepcount = division_step + division_step - 1;

cv::Mat orbwarp(const cv::Mat& image1, const cv::Mat& image2) {
    cv::Mat gray_image1, gray_image2;
    cv::cvtColor(image1, gray_image1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(image2, gray_image2, cv::COLOR_BGR2GRAY);

    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
    cv::Mat descriptors_1, descriptors_2;

    orb->detectAndCompute(gray_image1, cv::Mat(), keypoints_1, descriptors_1);
    orb->detectAndCompute(gray_image2, cv::Mat(), keypoints_2, descriptors_2);

    cv::BFMatcher bf(cv::NORM_HAMMING, true);
    std::vector<cv::DMatch> matches;
    bf.match(descriptors_1, descriptors_2, matches);

    double minDist = 999.0;
    if (!matches.empty()) {
        minDist = matches[0].distance;
    }

    for (const auto& m : matches) {
        if (m.distance < minDist) {
            minDist = m.distance;
        }
    }

    std::vector<cv::DMatch> good;
    for (const auto& m : matches) {
        if (m.distance < 3 * minDist || m.distance < 20) {
            good.push_back(m);
        }
    }

    std::vector<cv::Point2f> pts1, pts2;
    for (const auto& m : good) {
        pts1.push_back(keypoints_1[m.queryIdx].pt);
        pts2.push_back(keypoints_2[m.trainIdx].pt);
    }

    cv::Mat H1 = cv::findHomography(pts1, pts2, cv::RANSAC);
    cv::Mat correctedImage;
    cv::warpPerspective(image1, correctedImage, H1, image1.size());

    return correctedImage;
}

int findHighestNumberedFile(const std::string& directory) {
    int max_number = -1;
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;

            // 파일 이름이 "0000.jpg" 형식인지 확인
            if (filename.size() == 8 && filename.substr(4) == ".jpg") {
                bool is_number = true;
                for (int i = 0; i < 4; ++i) {
                    if (!isdigit(filename[i])) {
                        is_number = false;
                        break;
                    }
                }

                if (is_number) {
                    int number = std::stoi(filename.substr(0, 4));
                    if (number > max_number) {
                        max_number = number;
                    }
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Could not open directory: " << directory << std::endl;
    }

    return max_number;
}

void nomalize_arr(const cv::Mat& sim_arr, cv::Mat& image_org) {
    cv::Mat img;
    cv::resize(sim_arr, img, cv::Size(300, 300), 0, 0, cv::INTER_LINEAR);
    // cv::imshow("absdiffbysector", img);

    cv::Mat thresh;
    cv::threshold(img, thresh, 85, 255, cv::THRESH_BINARY);
    // cv::imshow("binary thresh", thresh);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(thresh, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat dst;
    cv::cvtColor(thresh, dst, cv::COLOR_GRAY2BGR);
    int h1 = dst.rows;
    int w1 = dst.cols;

    int h2 = image_org.rows;
    int w2 = image_org.cols;

    double htimes = static_cast<double>(h2) / h1;
    double wtimes = static_cast<double>(w2) / w1;

    for (size_t i = 0; i < contours.size(); ++i) {
        cv::drawContours(dst, contours, static_cast<int>(i), cv::Scalar(0, 0, 255), 2);
        cv::putText(dst, std::to_string(i), contours[i][0], cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0, 255, 0), 1);
        cv::Rect rect = cv::boundingRect(contours[i]);
        cv::rectangle(dst, rect, cv::Scalar(0, 255, 0), 2);

        int x1 = static_cast<int>(rect.x * wtimes);
        int y1 = static_cast<int>(rect.y * htimes);
        int w1 = static_cast<int>(rect.width * wtimes);
        int h1 = static_cast<int>(rect.height * htimes);
        cv::rectangle(image_org, cv::Rect(x1, y1, w1, h1), cv::Scalar(0, 255, 0), 2);
    }

    // cv::imshow("countours", dst);
    // cv::imshow("results", image_org);
    // cv::imwrite("/tmp/mnt/sdcard/results.jpg", image_org);

    int num;
    string save_path = "/tmp/mnt/sdcard/caps";
    string save_full = "";

    std::ostringstream oss;
    cout << "Save Name" << endl;
    num = findHighestNumberedFile(save_path);
    if (num >= 0) {
        oss << std::setw(4) << std::setfill('0') << num+1 << ".jpg";
        std::string filename = oss.str();
        save_full = save_path + "/" + filename;
        // save_full = save_path + "box" + std::to_string(num+1) + ".jpg";
    }
    else
        save_full = save_path + "/" + "0000.jpg";

    cout << "Save File : " << save_full << endl;
    imwrite(save_full, image_org);
}

double absdiff_sim(const cv::Mat& image1, const cv::Mat& image2) {
    cv::Mat diff_image;
    cv::absdiff(image1, image2, diff_image);

    cv::Mat thresh;
    cv::threshold(diff_image, thresh, 85, 255, cv::THRESH_BINARY);

    double mean_intensity_img1 = cv::sum(thresh)[0];

    return mean_intensity_img1;
}

int box_change(void) {
    std::string image1_path = "/tmp/mnt/sdcard/box_before.jpg";
    std::string image2_path = "/dev/shm/box0.jpg";

    cv::Mat imageorgA = cv::imread(image1_path);
    cv::Mat imageorgB = cv::imread(image2_path);

    if (imageorgA.empty() || imageorgB.empty()) {
        std::cerr << "Could not open or find the images!" << std::endl;
        return -1;
    }

    imageorgB = orbwarp(imageorgB, imageorgA);

    int height = imageorgA.rows;
    int width = imageorgA.cols;
    int ratioh = height / 10;
    int ratiow = width / 10;

    imageorgA = imageorgA(cv::Rect(ratiow, ratioh, width - 2 * ratiow, height - 2 * ratioh));
    imageorgB = imageorgB(cv::Rect(ratiow, ratioh, width - 2 * ratiow, height - 2 * ratioh));

    cv::Mat imageA, imageB;
    cv::resize(imageorgA, imageA, cv::Size(300, 300));
    cv::resize(imageorgB, imageB, cv::Size(300, 300));

    cv::Mat grayA, grayB;
    cv::cvtColor(imageA, grayA, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imageB, grayB, cv::COLOR_BGR2GRAY);

    cv::equalizeHist(grayA, grayA);
    cv::equalizeHist(grayB, grayB);

    // cv::imshow("equalizeHist1", grayA);
    // cv::imshow("equalizeHist2", grayB);

    absdiff_normalize_arr(grayA, grayB, imageorgB);

    int h = imageA.rows;
    int w = imageA.cols;

    double stepw = static_cast<double>(w) / division_step;
    double steph = static_cast<double>(h) / division_step;

    cv::Mat sim_arr = cv::Mat::ones(stepcount, stepcount, CV_32S);

    auto a = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < stepcount; ++i) {
        for (int j = 0; j < stepcount; ++j) {
            int hs = static_cast<int>(i * (steph / 2));
            int he = static_cast<int>(hs + steph - 1);

            int ws = static_cast<int>(j * (stepw / 2));
            int we = static_cast<int>(ws + stepw - 1);

            cv::Mat cropped_image1 = grayA(cv::Range(hs, he), cv::Range(ws, we));
            cv::Mat cropped_image2 = grayB(cv::Range(hs, he), cv::Range(ws, we));

            double similarity = absdiff_sim(cropped_image1, cropped_image2);

            if (similarity < 0) {
                sim_arr.at<int>(i, j) = 0;
            } else {
                sim_arr.at<int>(i, j) = similarity;
            }
        }
    }

    auto b = std::chrono::high_resolution_clock::now();
    auto c = std::chrono::duration_cast<std::chrono::milliseconds>(b - a);

    std::cout << c.count() << " milliseconds" << std::endl;

    cv::Mat normresult;
    cv::normalize(sim_arr, normresult, 0, 255, cv::NORM_MINMAX, CV_8U);

    nomalize_arr(normresult, imageorgB);

    // cv::waitKey(0);

    return 0;
}
