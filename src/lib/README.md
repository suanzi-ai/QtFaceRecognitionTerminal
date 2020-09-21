## lib 模块说明

src/lib 是数据对象和全局配置的封装。

### 模块介绍
* config: 全局配置模型

    封装了所有的配置信息，可以从Web后台或API接受动态更新，从而控制人脸识别过程和IO操作；
* image_package: 摄像头图像的数据对象

    封装了红外和彩色图像的不同VPSS通道的MMZ图像数据；
* detection_data: 人脸检测结果数据对象

    封装了红外和彩色图像上人脸检测结果；
* recongize_data: 人脸识别结果数据对象

    封装了彩色图像的人脸识别和红外图像的活体识别结果；
* pingpang_buffer: Qt线程之间的数据缓冲队列

    src/app中核心线程之间通信的数据缓冲队列，用于缓存`ImagePackage`、`DetectionData`和`RecongizeData`数据。