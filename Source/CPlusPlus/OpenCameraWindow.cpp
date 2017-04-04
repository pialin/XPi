#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

int main(int NumArg, char** ArgVector)				//本函数用于调用摄像头，并开启一个窗口实时显示摄像头画面
								//ArgCount代表调用此函数时输入参数的个数;ArgVector是输入参数字符串数组的头指针
{
    std::cout << "Built with OpenCV " << CV_VERSION << std::endl;     
                                                                //显示所用的OpenCV版本

    cv::namedWindow("Camera",cv::WINDOW_AUTOSIZE);		//建立名为Camera的窗口
	
    cv::VideoCapture VideoCaptureInstance;			//实例化VideoCapture类，用于对视频进行读取或者调用摄像头
    VideoCaptureInstance.open(0);				//初始化VideoCaptureInsatnce，参数"0"代表打开默认摄像头，也可给定一个视频文件路径用于读取视频
	
    if (!VideoCaptureInstance.isOpened())			//判断摄像头是否打开成功
    {	
	std::cerr << "Couldn't open camera!" << std::endl;	//如果摄像头打开失败，返回错误信息
	return -1;						//退出程序，并给出代表函数执行失败的返回值-1
    }

    unsigned int FrameWidth = VideoCaptureInstance.get(cv::CAP_PROP_FRAME_WIDTH);
                                                                //获取视频帧的分辨率
    unsigned int FrameHeight = VideoCaptureInstance.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    double CameraFps = VideoCaptureInstance.get(cv::CAP_PROP_FPS);
                                                                //获取摄像头帧率 
    unsigned int FrameGapTime = 1/CameraFps*1000;               //计算帧间的间隔时间（单位：毫秒）

    std::cout << "Frame size: " << FrameWidth << "*" << FrameHeight << std::endl;
                                                                //显示帧分辨率 
    std::cout << "Frames per second: " << FrameGapTime << std::endl;
                                                                //显示帧率
                                                                                    
    cv::Mat CameraFrame;
    while (1)
    {
	VideoCaptureInstance >> CameraFrame;

	if (CameraFrame.empty()) break;				//判断是否成功取到图像帧,如果无法取到帧则跳出循环，退出函数
		
	cv::imshow("Camera",CameraFrame);

	if (cv::waitKey(FrameGapTime) >= 0) break;		//waitKey函数等待键盘按键按下，所给参数为等待的时长（单位：毫秒）,
								//若给定参数为0,则表示进行无限期地等待;waitKey返回按键值的ASCII码，
								//在此判断返回值大于等于0目的是对任意键响应，即按任意键结束摄像头
    }
	
    cv::destroyWindow("Camera");				//关闭创建的窗口Camera
    return 0;							//返回0代表程序正常结束
}
