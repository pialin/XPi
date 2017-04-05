#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <time.h>                                               //用于获取当前时间
#include <string>                                               //引入C++的string类

int main(int NumArg, char** ArgVector)				//本函数用于调用摄像头，并开启一个窗口实时显示摄像头画面
								//ArgCount代表调用此函数时输入参数的个数;ArgVector是输入参数字符串数组的头指针
{
    std::cout << "Built with OpenCV " << CV_VERSION << std::endl;     
                                                                //显示所用的OpenCV版本

    cv::namedWindow("Camera",cv::WINDOW_AUTOSIZE);		//建立名为Camera的窗口
	
    cv::VideoCapture VideoCaptureInstance;			//实例化VideoCapture类，用于对视频进行读取或者调用摄像头
    VideoCaptureInstance.open(0);				//初始化VideoCaptureInsatnce，参数"0"代表打开默认摄像头，也可给定一个视频文件路径用于读取视频
	
    if(!VideoCaptureInstance.isOpened())			//判断摄像头是否打开成功
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
                                                                                    
    cv::Mat CameraFrame;                                        //用于作为显示图像帧的暂存矩阵
    std::string CurrentTime;                                     //用于存储当前时间(1970年1月1日至今经过的秒数) time_t为time.h中定义的long类型数据
    std::string CapturedFrameFileName;                          //保存帧图像的文件名   
    char PressedKeyAscii;                                       //用于存储按下按键的Ascii码
    std::string ResultText;                                     //用于保存tensorflow程序输出的物体类别
    while(1)
    {
	VideoCaptureInstance >> CameraFrame;

	if(CameraFrame.empty()) break;				//判断是否成功取到图像帧,如果无法取到帧则跳出循环，退出函数
		
	cv::imshow("Camera",CameraFrame);                       //显示捕获得到的图像帧

        PressedKeyAscii = cv::waitKey(FrameGapTime);            //waitKey函数等待键盘按键按下，所给参数为等待的时长（单位：毫秒）,
								//若给定参数为0,则表示进行无限期地等待;waitKey返回按键值的ASCII码，

	if(PressedKeyAscii == 32) 	                        //在此判断返回值大于等于0目的是对任意键响应，即按任意键结束摄像头
        {   
            CurrentTime = std::to_string(time(NULL));                           //获取当前系统时间
            CapturedFrameFileName = "../CapturedFrame/" + CurrentTime ;
            CapturedFrameFileName += ".jpg" ;
                                                                //获取帧图像的保存文件名   
            cv::imwrite(CapturedFrameFileName,CameraFrame);     //保存帧图像
            //cv::waitKey(3000); 
            ResultText = "Hello \n OpenCV!";                    //TODO 加上tensorflow代码

            cv::putText                                         //在帧图像上绘制文本 
            (                                           
                CameraFrame,                                    //所要绘制的对象
                ResultText,                                     //文本内容
                cv::Point(0,0),                                 //绘制起始点
                cv::FONT_HERSHEY_COMPLEX,                       //字体
                1,                                              //文字缩放系数
                cv::Scalar(255,255,255),                        //文字颜色
                1,                                              //文字线宽
                cv::LINE_AA                                     //文字线型
            );
            PressedKeyAscii = cv::waitKey(0);                   //无限期等待按键
            if(PressedKeyAscii == 32)  continue;                //如果按下的是空格键，则继续采集和显示图像
            else if(PressedKeyAscii >= 0) break;                //如果按下的是除空格键外的其他按键，则结束程序
        }
        else if(PressedKeyAscii >= 0) break;                    //在此判断返回值大于等于0目的是对任意键响应，即按除空格键外任意键结束程序
 
    }
	
    cv::destroyWindow("Camera");				//关闭创建的窗口Camera
    return 0;							//返回0代表程序正常结束
}
