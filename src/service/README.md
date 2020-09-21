## service 模块说明

src/service 是与Web后台和API通信的对象，封装了人脸录入、人脸记录上报、全局配置更新等功能。

**注意: 无特殊需求，请勿修改service模块**

### 模块介绍
* http_server: Web API监听线程

    负责Web后台和Web API与人脸识别主程序之间的通信；
* face_server: 人脸底库管理的服务线程

    负责承载face_service和person_service，将http_server的事件发送给对应的服务。
* face_service: 人脸底库管理的API接口

    封装了人脸底库的增加、删除、修改等操作；
* person_service: Web后台数据库的API接口

    封装了人脸身份的查询、识别结果的上报、Web后台信息查询等操作。