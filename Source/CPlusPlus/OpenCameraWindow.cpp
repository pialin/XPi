//OpenCV相关头文件
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
//用于获取当前时间
#include <time.h>

//引入C++的string类
#include <string>

//自定义函数声明
void  MouseClickCallback(int EventType, int ClickPosX, int ClickPosY, int flags, void* userdata);
int GetTextPos(std::string Text, cv::Rect TextFrameRect, float &FontScale, cv::Point &TextOrigin);

//全局变量
//字体类型
const int FontType = cv::FONT_HERSHEY_SIMPLEX;

//字体线宽
const int TextLineThickness = 1;

//字体线型
const int TextLineType = cv::LINE_AA;

//字体颜色(白色)
const cv::Scalar FontColor = cv::Scalar(255, 255, 255);

//本函数用于调用摄像头，并开启一个窗口实时显示摄像头画面
//NumArg代表调用此函数时输入参数的个数;ArgVector是输入参数字符串数组的头指针
int main(int NumArg, char** ArgVector)
{
    //显示所用的OpenCV版本
    std::cout << "Built with OpenCV " << CV_VERSION << std::endl;     

    //建立名为Camera的窗口
    cv::namedWindow("Camera",cv::WINDOW_AUTOSIZE);

    //实例化VideoCapture类，用于对视频进行读取或者调用摄像头
    cv::VideoCapture VideoCaptureInstance;

    //初始化VideoCaptureInsatnce，参数"0"代表打开默认摄像头，也可给定一个视频文件路径用于读取视频
    VideoCaptureInstance.open(0);

    //判断摄像头是否打开成功
    if(!VideoCaptureInstance.isOpened())
    {
        //如果摄像头打开失败，返回错误信息
        std::cerr << "Couldn't open camera!" << std::endl;

        //退出程序，并给出代表函数执行失败的返回值-1
        return -1;
    }

    //获取视频帧的分辨率
    unsigned int FrameWidth = VideoCaptureInstance.get(cv::CAP_PROP_FRAME_WIDTH);
    unsigned int FrameHeight = VideoCaptureInstance.get(cv::CAP_PROP_FRAME_HEIGHT);

    //获取摄像头帧率 
    double CameraFps = VideoCaptureInstance.get(cv::CAP_PROP_FPS);

    //计算帧间的间隔时间（单位：毫秒）
    unsigned int FrameGapTime = 1/CameraFps*1000;

    //显示帧分辨率 
    std::cout << "Frame size: " << FrameWidth << "*" << FrameHeight << std::endl;

    //显示帧率
    std::cout << "Frames per second: " << CameraFps << std::endl;

    //按钮的相对大小（相对于整个视频帧的尺寸）
    const float ButtonSize = 0.15;

    //按钮之间和按钮和边缘的间距大小（相对于整个视频帧的尺寸）
    const float ButtonGapWidth =  0.06;

    //设置“分析”按钮的位置和尺寸      
    const cv::Rect AnalyseButtonArea = cv::Rect
        (
         //按钮左上角坐标
         FrameWidth * (1 - ButtonGapWidth*2 - ButtonSize*2), FrameHeight * (1 - ButtonGapWidth - ButtonSize),

         //按钮的宽度和高度                          
         FrameWidth * ButtonSize, FrameHeight * ButtonSize
        );

    //设置“结束”按钮的位置和尺寸      
    const cv::Rect ExitButtonArea = cv::Rect
        (
         //按钮左上角坐标
         FrameWidth * (1 - ButtonGapWidth - ButtonSize), FrameHeight * (1 - ButtonGapWidth - ButtonSize),

         //按钮的宽度和高度                          
         FrameWidth * ButtonSize, FrameHeight * ButtonSize
        );


    float AnalyseFontScale, ExitFontScale;
    cv::Point AnalyseTextOrigin, ExitTextOrigin;

    //设置"分析"按钮和“退出”按钮上显示的文字
    const std::string AnalyseButtonAnnotation = "Analyse";
    const std::string ExitButtonAnnotation = "Exit";

    //将文字绘制到“分析”按钮所在的框内
    GetTextPos
        (
         //在“分析”按钮上绘制的字符串
         AnalyseButtonAnnotation,

         //绘制文字的框范围
         AnalyseButtonArea,

         //字体缩放尺度
         AnalyseFontScale,

         //字体插入的原点
         AnalyseTextOrigin
        );

    //将文字绘制到“结束”按钮所在的框内
    GetTextPos
        (
         //在“结束”按钮上绘制的字符串
         ExitButtonAnnotation,

         //绘制文字的框范围
         ExitButtonArea,

         //字体缩放尺度
         ExitFontScale,

         //字体插入的原点
         ExitTextOrigin
        );

    //CameraFrame用于暂存视频帧，GuiFrame用于存储实际显示的帧
    cv::Mat CameraFrame, GuiFrame;

    //用于存储当前时间(1970年1月1日至今经过的秒数) time_t为time.h中定义的long类型数据
    std::string CurrentTime;

    //保存帧图像的文件名   
    std::string CapturedFrameFileName;

    //用于存储按下按键的Ascii码
    signed char PressedKeyAscii;

    //用于保存tensorflow程序输出的物体类别
    std::string ResultText;

    //设置鼠标点击回调函数
    //cv::setMouseCallback("Camera",MouseClickCallback);

    while(true)
    {
        //获取视频帧
        VideoCaptureInstance >> CameraFrame;

        //判断是否成功取到图像帧,如果无法取到帧则跳出循环，退出函数
        if(CameraFrame.empty()) break;

        //复制一份视频帧到显示帧
        GuiFrame = CameraFrame;

        //对显示帧设置按钮内颜色为黑色半透明
        GuiFrame(AnalyseButtonArea) = CameraFrame(AnalyseButtonArea) * 0.3;
        GuiFrame(ExitButtonArea) = CameraFrame(ExitButtonArea) * 0.3;

        //在背景对象上绘制文字
        //绘制“分析”按钮文字
        cv::putText
            (
             //绘制文字的背景对象
             GuiFrame,

             //绘制的字符串
             AnalyseButtonAnnotation,

             //文字插入的原点
             AnalyseTextOrigin,

             //字体类型
             FontType,

             //字体缩放尺度
             AnalyseFontScale,

             //字体颜色
             FontColor, 

             //字体线宽
             TextLineThickness,

             //字体线型(抗混叠线型)
             TextLineType

                 );
        //绘制“分析”按钮文字
        cv::putText
            (
             //绘制文字的背景对象
             GuiFrame,

             //绘制的字符串
             ExitButtonAnnotation,

             //文字插入的原点
             ExitTextOrigin,

             //字体类型
             FontType,

             //字体缩放尺度
             ExitFontScale,

             //字体颜色
             FontColor, 

             //字体线宽
             TextLineThickness,

             //字体线型(抗混叠线型)
             TextLineType

                 );




        //刷新显示帧
        cv::imshow("Camera",GuiFrame);

        //waitKey函数等待键盘按键按下，所给参数为等待的时长（单位：毫秒）,若给定参数为0,则表示进行无限期地等待;waitKey返回按键值的ASCII码，
        PressedKeyAscii = cv::waitKey(FrameGapTime);

        //判断被按下的按键是否是空格键（Ascii码为32），若是则存储视频帧并进行处理
        if(PressedKeyAscii == 32)
        {              
            //获取当前系统时间
            CurrentTime = std::to_string(time(NULL));

            CapturedFrameFileName += ".jpg" ;

            //保存帧图像
            cv::imwrite(CapturedFrameFileName,CameraFrame);   

            //TODO 加上tensorflow代码
            cv::waitKey(3000);
            ResultText = "Hello OpenCV!";

            //复制一份视频帧至显示帧
            GuiFrame = CameraFrame;
            //在显示帧上绘制文本 
            cv::putText
                (
                 //所要绘制的对象                               
                 GuiFrame,

                 //文本内容
                 ResultText,

                 //绘制起始点
                 cv::Point(0,30),

                 //字体
                 FontType,

                 //文字缩放系数,
                 1,

                 //文字颜色
                 cv::Scalar(255,255,255),

                 //文字线宽
                 TextLineThickness,  

                 //文字线型
                 TextLineType
                     );

            //绘制文字后更新窗口显示内容  
            cv::imshow("Camera",GuiFrame);

            //无限期等待按键
            PressedKeyAscii = cv::waitKey(0);

            //如果按下的是空格键，则继续采集和显示图像
            if(PressedKeyAscii == 32)  continue;

            //如果按下的是除空格键外的其他按键，则结束程序
            else if(PressedKeyAscii >= 0) break;
        }

        //在此判断返回值大于等于0目的是对任意键响应，即按除空格键外任意键结束程序
        else if(PressedKeyAscii >= 0) break;

    }

    //关闭创建的窗口Camera
    cv::destroyWindow("Camera");

    //返回0代表程序正常结束
    return 0;
}

void MouseClickCallback(int EventType, int ClickPosX, int ClickPosY, int flags, void* userdata)
{
    //    if(EventType == cv::EVENT_LBUTTONDOWN) 
    //    {
    //        if(AnalyseButtonRect.contains(Point(ClickPosX, ClickPosY)))
    //        {
    //            cout << "Analyse" << endl;
    //        }
    //        if(ButtonRect.contains(Point(ClickPosX, ClickPosY)))
    //        {
    //            cout << "Analyse" << endl;
    //        }
    //
    //    }
    //
}

//要绘制的字符在框内绘制文字
int GetTextPos
(
 //输入：要绘制的字符串
 const std::string Text,

 //输入：框的范围
 cv::Rect TextFrameRect,

 //输出：字体缩放尺度
 float &FontScale,

 //输出； 字体插入原点
 cv::Point &TextOrigin
 )
{

    //FontScaleTemp作为代替FontScale进行循环变化的变量，在此设置初始的字体缩放尺度
    float FontScaleTemp = 1;

    //设置缩放尺度变化速度
    float FontScaleStep = 0.9;

    //文字基线高度(TextBaseLineTemp用于循环计算，TextBaseLine则保存最终的基线
    int TextBaseLine, TextBaseLineTemp;

    //字符串尺寸(TextSizeTemp用于循环计算，TextSize则保存最终得字符串尺寸)
    cv::Size TextSize, TextSizeTemp;

    //计算文字与边框之间的空隙宽度和高度
    int TextFrameGapWidth = 0.05f * TextFrameRect.width; 
    int TextFrameGapHeight = 0.05f *  TextFrameRect.height;  

    //计算字符串的横向和纵向最大长度
    int TextWidthLimit =TextFrameRect.width - 2* TextFrameGapWidth;
    int TextHeightLimit = TextFrameRect.height - 2* TextFrameGapHeight;

    //LastFitFlag标志上个缩放尺度下字体是否能放入框内
    //CurrentFitFlag标志当前缩放尺度下字体是否能放入框内
    bool LastFitFlag, CurrentFitFlag;

    //测量初始缩放尺度时的字符串的长度
    TextSizeTemp = getTextSize
        (
         //字符串
         Text,

         //字体
         cv::FONT_HERSHEY_COMPLEX,

         //字体缩放尺度
         FontScaleTemp,

         //文字线宽
         TextLineThickness,

         //基线高度
         & TextBaseLineTemp
        );

    //判断初始文字大小看是否能放入框内
    if( TextSizeTemp.width < TextWidthLimit
            && (TextSizeTemp.height + TextBaseLineTemp ) < TextHeightLimit)
    {   
        //如果文字尺寸可以放入框内，更新CurrentFitFlag以及最终的FontScale,TextSize,TextBaseLine
        CurrentFitFlag = true;
        FontScale = FontScaleTemp;
        TextSize = TextSizeTemp;
        TextBaseLine = TextBaseLineTemp;

        //并将字体进行放大
        FontScaleTemp = FontScaleTemp / FontScaleStep;
    }
    else
    {   
        //否则将标志变量置false，并缩小字体缩放尺度
        CurrentFitFlag = false;
        FontScaleTemp = FontScaleTemp * FontScaleStep;

        //并将字体进行缩小
        FontScaleTemp = FontScaleTemp * FontScaleStep;

    }

    //循环计算合适的缩放尺度
    while(true)
    {
        //测量初始缩放尺度时的字符串的长度
        TextSizeTemp = getTextSize
            (
             //字符串
             Text,

             //字体
             cv::FONT_HERSHEY_COMPLEX,

             //字体缩放尺度
             FontScaleTemp,

             //文字线宽
             TextLineThickness,

             //基线高度
             & TextBaseLineTemp
            );

        //更新LastFitFlag 
        LastFitFlag = CurrentFitFlag;

        //判断初始文字大小看是否能放入框内
        if( TextSizeTemp.width  <= TextWidthLimit
                && (TextSizeTemp.height + TextBaseLineTemp) <= TextHeightLimit)
        {   
            //如果文字尺寸可以放入框内，更新CurrentFitFlag以及最终的FontScale,TextSize,TextBaseLine
            CurrentFitFlag = true;
            FontScale = FontScaleTemp;
            TextSize = TextSizeTemp;
            TextBaseLine = TextBaseLineTemp;

            //并将字体尺度进行放大
            FontScaleTemp = FontScaleTemp / FontScaleStep;
        }
        else
        {   
            //否则将标志变量置false，并更新LastFitFlag和CurrentFitFlag
            CurrentFitFlag = false;

            //并缩小字体缩放尺度
            FontScaleTemp = FontScaleTemp * FontScaleStep;

        }

        //当文字尺度由不能放入框内转变为可以(或者由不能转变为可以时)，此时的尺度就是最合适的，不会过大或者过小
        if(LastFitFlag ^ CurrentFitFlag) break;

    }

    //计算文字放置的原点（对于背景图像和框来说，原点位于左上角，但对于文字来说，原点位于左下角的基线位置;int(x+0.5)为对x进行四舍五入的结果)
    TextOrigin.x = int(TextFrameRect.x + TextFrameGapWidth + (TextWidthLimit - TextSize.width)/2.0f + 0.5);
    TextOrigin.y = int(TextFrameRect.y + TextFrameGapHeight + (TextHeightLimit - TextSize.height - TextBaseLine)/2.0f + TextSize.height + 0.5);


    //返回1表示程序正常结束
    return 1;
}
