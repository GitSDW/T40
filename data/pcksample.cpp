int package_detecte(void)
{
    int br=0, ar=0;
    int boxscale=0, boxscale2=0;
    int box_cnt=0;

    br = access("/customer/before.jpg", R_OK);
    ar = access("/customer/after.jpg", R_OK);
    if ((br < 0) || (ar < 0)) {
        dp("[%s]Err br:%d ar:%d\n", __FUNCTION__, br, ar);
        return -1;
    }

    // package_sistic();

    std::string image1_path = "/customer/before.jpg";
    std::string image2_path = "/customer/after.jpg";

    cv::Mat img1 = cv::imread(image1_path);
    cv::Mat img2 = cv::imread(image2_path);

    if (img1.empty() || img2.empty()) {
        std::cerr << "Can't Open File!" << std::ends;
        return -2;
    }
    int height = img1.rows;
    int width = img1.cols;

    int borderSize = 10;
    cv::Rect roi(borderSize, borderSize, width - 2 * borderSize, height - 2 * borderSize);
    img1 = img1(roi);
    img2 = img2(roi);
    
    cv::Mat gray1, gray2;
    cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

    cv::equalizeHist(gray1, gray1);
    cv::equalizeHist(gray2, gray2);

    cv::Mat diff_image;
    cv::absdiff(gray1, gray2, diff_image);

    cv::Mat bin_img;
    cv::threshold(diff_image, bin_img, 50, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bin_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Rect boundingRect2;

    for(size_t i = 0; i< contours.size(); i++) {
        cv::Rect boundingRect = cv::boundingRect(contours[i]);

        if (boundingRect.width >= 50 || boundingRect.height > 50) {
            box_cnt++;
            // Maximum Scale Box
            boxscale = boundingRect.width * boundingRect.height;
            if (boxscale > boxscale2){
                boxscale2 = boxscale;
                // boundingRect2 = boundingRect;
            }
            // Box Find area
            // if (boundingRect.x >= width/2 && boundingRect.x <= height/2) {
            if (boundingRect.y+boundingRect.width >= width/2 && boundingRect.y+boundingRect.height <= height/2) {
                    // cv::rectangle(img2, boundingRect, cv::Scalar(0, 255, 0), 2);
                    // NULL;
                // }
            }
            cv::rectangle(img2, boundingRect, cv::Scalar(0, 255, 0), 2);
        }
    }
    // cv::rectangle(img2, boundingRect2, cv::Scalar(0, 255, 0), 2);

    cv::imwrite("/customer/result.jpg", img2);

    dp("Box Count : %d\n", box_cnt);

    return 0;
    
}