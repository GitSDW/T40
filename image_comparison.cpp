#include <opencv2/opencv.hpp>
#include <iostream>
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
    int64_t cv_time = sample_gettimeus();
    int64_t cv_buf;
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
    // printf("box3\n");
    cerr << "find try! : " << endl;
    try {
        cv::Mat gray1, gray2;
        cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
        cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

         // 각 이미지의 평균 밝기 계산
        Scalar mean_intensity_img1 = mean(gray1);
        Scalar mean_intensity_img2 = mean(gray2);

         // 조명 보정을 위해 이미지의 밝기 정규화
        Mat normalized_img1, normalized_img2;
        convertScaleAbs(gray1, normalized_img1, 127.0 / mean_intensity_img1[0], 0);
        convertScaleAbs(gray2, normalized_img2, 127.0 / mean_intensity_img2[0], 0);

        cv::equalizeHist(gray1, gray1);
        cv::equalizeHist(gray2, gray2);

        cv_buf = (sample_gettimeus() - cv_time)/1000;
        cv_time = sample_gettimeus();
        cerr << "find set threshold!"<< thhold << " : " << endl;
        // cv::Mat bin_img1, bin_img2;
        // cv::threshold(gray1, bin_img1, thhold, 255, cv::THRESH_BINARY);
        // cv::threshold(gray2, bin_img2, thhold, 255, cv::THRESH_BINARY);

        // printf("box1\n");
        cv_buf = (sample_gettimeus() - cv_time)/1000;
        cv_time = sample_gettimeus();
        cerr << "find gray! : " << endl;
        cv::Mat diff_image;
        cv::absdiff(gray1, gray2, diff_image);

        cv_buf = (sample_gettimeus() - cv_time)/1000;
        cv_time = sample_gettimeus();
        cerr << "find set threshold!"<< thhold << " : " << endl;
        cv::Mat bin_img;
        cv::threshold(diff_image, bin_img, thhold, 255, cv::THRESH_BINARY);

        // imwrite("bin.jpg", bin_img);

        // printf("box2\n");
        cv_buf = (sample_gettimeus() - cv_time)/1000;
        cv_time = sample_gettimeus();
        cerr << "find contours! : " << endl;
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
        // printf("box count:%d\n", ); 
    } catch (Exception& e) {
        box_cnt = 0xFF;
        printf("Too Many Box Count! Changed Camera Position!\n");
    }
    // cv::rectangle(img2, boundingRect2, cv::Scalar(0, 255, 0), 2);
    // printf("box4\n");

    cerr << "find end! : " << endl;

    cv::imwrite("/vtmp/box_result.jpg", img2);

    return box_cnt;
}

int package_sistic(char *imgpath1, char *imgpath2) {
    int64_t cv_time = sample_gettimeus();
    int64_t cv_buf;

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
    cerr << "sistic make! : " <<endl;
    // ORB 객체 생성
    Ptr<ORB> orb = ORB::create();

    cerr << "sistic point cal! : " << endl;
    // 키 포인트와 디스크립터 계산
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    orb->detectAndCompute(image1, Mat(), keypoints1, descriptors1);
    orb->detectAndCompute(image2, Mat(), keypoints2, descriptors2);

    cerr << "sistic point match! : " << endl;
    // 특징점 매칭 
    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    cerr << "sistic filtering! : " << endl;
    // 좋은 매칭 필터링
    double minDist = min_element(matches.begin(), matches.end(),
        [](const DMatch& m1, const DMatch& m2) { return m1.distance < m2.distance; })->distance;

    vector<DMatch> goodMatches;
    for (const DMatch& match : matches) {
        // printf("distance:%f %f\n", match.distance, minDist);
        if (match.distance < 3 * minDist) {
            goodMatches.push_back(match);
        }
    }

    cerr << "sistic metrix cal! : " << endl;
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

            imwrite("/vtmp/corimg1.jpg", correctedImage);
        } catch (Exception& e) {
            cerr << "Fail comparison points!" << endl;

            imwrite("/vtmp/corimg1.jpg", image1);
            ///////////////// Log Point /////////////////
        }
    }
    else {
        cerr << "Not enough comparison points:" << goodMatches.size() << endl;

        imwrite("/vtmp/corimg1.jpg", image1);
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

            imwrite("/vtmp/corimg1.jpg", correctedImage);
        } catch (Exception& e) {
            cerr << "Fail comparison points!" << endl;

            imwrite("/vtmp/corimg1.jpg", image1);
            ///////////////// Log Point /////////////////
        }
    }
    else {
        cerr << "Not enough comparison points!" << endl;

        imwrite("/vtmp/corimg1.jpg", image1);
    }
#endif
    // 결과 이미지를 디스크에 저장
    cerr << "Sistic End : " << endl;

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
//         printf("Usage: %s -b <before.jpg> -a <after.jpg> -t <threshold value>\n", argv[0]);
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
// 				printf("Usage: %s -b <before.jpg> -a <after.jpg> -t <threshold value>", argv[0]);
// 				return -1;
// 		}
// 	}

// 	ret = package_sistic(before_img, after_img);
// 	if(ret < 0) {
// 		printf("Package Sistci Fail!\n");
// 		return ret;
// 	}

// 	ret = package_find(after_img, threshold);
// 	if(ret < 0) {
// 		printf("Package Find Fail!\n");
// 		return ret;
// 	}
//     else {
//         printf("Box Count : %d\n", ret);
//         if (ret == 0) {
//             sim = calculateSimilarity(after_img);
//             // printf("similarity:%f %\n", sim);
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
//     cv::String imagePath = "/vtmp/thumbnail.jpg";
//     cv::Mat originalImage = cv::imread(imagePath);

//     printf("Thumbnail file load!\n");

//     // 모자이크 처리할 좌표 및 크기 설정
//     // int x = 100, y = 100, width = 120, height = 120;
//     int mosaicSize = 150;

//     // 모자이크 처리 및 크기 조절
//     cv::Mat processedImage = originalImage.clone();
//     printf("Thumbnail make clone!\n");
//     // for(i=0;i<10;i++) {
//     //     if(cont.flag[i]){
//     //         w = ((cont.ex[i]/150)+1)*150;
//     //         h = ((cont.ey[i]/150)+1)*150;
//     //         printf("thum:%d %d %d %d\n", cont.x[i], cont.y[i], w, h);

//     //         mosaic(processedImage, cont.x[i], cont.y[i], w, h, mosaicSize);

//     //     }
//     // }
//     mosaic(processedImage, 100, 180, 300, 400, mosaicSize);
//     resizeImage(processedImage, 640, 360);

//     // 결과 이미지 저장
//     cv::String outputPath = "/vtmp/thumbnail_last.jpg";
//     cv::imwrite(outputPath, processedImage);

//     return 0;
// }

void resizeImage(cv::Mat& image, int width, int height) {
    cv::resize(image, image, cv::Size(width, height));
}


int thumbnail_make(Thum_Data_t cont) {
    int x, y, w, h;
    // 이미지 파일 경로
    std::string inputImagePath = "/vtmp/thumbnail.jpg";
    std::string outputImagePath = "/vtmp/thumbnail_last.jpg";

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

    resizeImage(mosaic, 640, 360);

    // 모자이크 처리된 이미지 저장
    imwrite(outputImagePath, mosaic);

    return 0;
}


int facecrop_make(Fdpd_Data_t cont) {
    int cx, cy, size, x, y;
    // 이미지 파일 경로
    std::string inputImagePath = "/vtmp/face.jpg";
    std::string outputImagePath = "/vtmp/face_crop" + std::to_string(face_crop_cnt) + ".jpg";

    cx = (cont.ul_x + cont.br_x)/2;
    cy = (cont.ul_y + cont.br_y)/2;
    size = cont.br_y - cont.ul_y;
    size = size*2;
    x = cx - (size/2);
    y = cy - (size/2);

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if (x+size >= 1920) x = 1920-size-1;
    if (y+size >= 1080) y = 1080-size-1;

    printf("cx:%d cy:%d size:%d x:%d y:%d\n", cx, cy, size, x, y);
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
}