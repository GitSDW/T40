#include <opencv2/opencv.hpp>
#include "cxx_util.h"

using namespace cv;
using namespace std;

int scanf_cmd(void)
{
    int buf;
    std::cout << "Insert Commend : ";
    std::scanf("%d", &buf);

    return buf;
}

int scanf_index(void)
{
    int buf;
    std::scanf("%d", &buf);

    return buf;
}

int scanf_string(char *buf)
{
    // char buf[30];
    std::scanf("%s", buf);

    return 1;
}