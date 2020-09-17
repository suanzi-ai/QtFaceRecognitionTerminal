# 人脸识别终端（Qt版本）

[官网](https://www.suanzi.ai)

### 获取项目代码

可以直接[下载](https://github.com/suanzi-ai/QtFaceRecognitionTerminal/archive/master.zip)。

或者也可以直接克隆方式：

```bash
sudo apt install git
git clone https://github.com/suanzi-ai/QtFaceRecognitionTerminal.git
```

### 编译

程序目前只支持 `hi3516dv300`，需要确保你有 `arm-himix200-linux` 交叉编译环境。

```bash
pip3 install cmake ninja # install build tools
cd QtFaceRecognitionTerminal
cp user.env.sample user.env # you may want to modify it
./build.sh
```
