# 人脸识别终端（Qt版本）

[公司官网: www.suanzi.ai](https://www.suanzi.ai)

## 获取项目代码

可以直接[下载](https://github.com/suanzi-ai/QtFaceRecognitionTerminal/archive/master.zip)代码压缩包。或者也可以直接克隆方式：

```bash
sudo apt install git
git clone https://github.com/suanzi-ai/QtFaceRecognitionTerminal.git
```

## 交叉编译

程序目前只支持 `Hi3516DV300`，需要确保你有 `arm-himix200-linux` 交叉编译环境，安装在/opt/hisi-linux/x86-arm/目录下。

在开发机上运行下面命令进行编译：
```bash
pip3 install cmake ninja        # 安装编译工具，第一次编译才需要运行
cd QtFaceRecognitionTerminal
cp user.env.sample user.env     # 可以修改user.env中的内容，指定第三方依赖的下载目录
./build.sh                      # 编译过程保证全程联网，下载相关依赖
```

## 部署和运行

首先通过SSH登录人脸识别终端。设备的ip地址显示在界面的左上角，用户名为root，密码为szkj。

然后，运行下列命令，关闭主程序
```bash
cd /user/quface-app
supervisord ctl stop face-terminal
```

将交叉编译生成的可执行程序face-terminal，通过SCP拷贝到人脸识别终端上，覆盖/user/quface-app/bin目录中的face-terminal。

最后，再次SSH登录人脸识别终端，运行下列命令，重启设备。
```bash
cd /user/quface-app
chmod +x ./bin/face-terminal
supervisord ctl start face-terminal
reboot
```

## 代码模块说明
该项目一共包含4个模块，代码和详细说明请点击
* [app模块](src/app)
* [ui模块](src/ui)
* [lib模块](src/lib)
* [service模块](src/service)

## 第三方SDK文档
人脸识别终端的代码依赖两个SDK，分别是quface和quface-io。

* 人脸识别算法SDK：quface [[在线文档]](http://cdn.suanzi.ai/docs/quface/index.html)

* 底层硬件控制SDK：quface-io [[在线文档]](http://cdn.suanzi.ai/docs/quface-io/index.html)

## Web后台API文档
人脸识别终端主程序，通过Web API与Web后台进行通信。

用户同样可以使用Web API，完成与远程云服务器的通信。[[在线文档]](https://cdn.suanzi.ai/docs/api)