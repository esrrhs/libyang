libyang是消息生成工具，简单粗暴版的google protobuf。
采用fakescript的template库编写，通过xml定义生成C Struct定义及打包解包函数，通过tag方式支持版本前后兼容
使用方法：
1.编辑sample.xml，定义消息
2.执行run.bat
3.使用sample.h头文件
附fakescript地址：https://github.com/esrrhs/fakescript
