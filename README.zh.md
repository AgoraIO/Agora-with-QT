# Agora With Qt Quick

这个开源示例项目演示了如何基于 Qt Quick（32bit） 快速集成 [Agora](www.agora.io) 视频 SDK，实现一对一视频聊天。

在这个示例项目中包含以下功能：

Agora 

- 加入通话和离开通话
- 实现一对一视频聊天
- 静音和解除静音

Qt Quick

- UI 界面

本项目采用了 Qt Quick 的 UI 界面功能，使用了 Agora 提供的声音视频采集、编码、传输、解码和远端渲染功能。

Qt Quick功能实现请参考 [Qt Quick 官方文档](https://doc.qt.io/qt-5/qtquick-index.html)


## 开发环境
* win7以上/Mac
* QtCreator


## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 `agorartcengine.cpp`


    context.appId = ""; // Specify your APP ID here

然后在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 视频通话 + 直播 SDK，并解压后将其中的

* Windows：`sdk/include`、`sdk/lib/` 和 `sdk/dll/` 中的内容复制到本项目的 lib 文件夹下
* Mac：`libs/AgoraRtcEngineKit.framework` 复制到本项目的 lib 文件夹下

最后使用 Qt Creator 打开 `AgoraVideoCall.pro` 工程文件，点击构建成功后即可运行。

## 运行环境
* Windows/Mac



## 联系我们

- 如果你遇到了困难，可以先参阅[常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考[官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考[官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看[社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Agora-with-QT/issues)

## 代码许可

The MIT License (MIT).