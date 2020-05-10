# Agora With Qt Quick

This opensource project shows how to integrate [Agora](www.agora.io) video SDK with Qt Quick(32bit) and implements  video broadcasting


The following features are included in this sample project:

- Join / leave channel
- Set role as broadcaster or audience
- Mute / unmute audio
- Change camera
- Setup resolution, frame rate and bit rate


Qt Quick

- UI 

This project use Qt Quick as UI and  Agora SDK capture,encode,transfer, decode, render video and audio.

Qt Quick reference [Qt Quick website](https://doc.qt.io/qt-5/qtquick-index.html)

Agora SDK reference [Agora Document](https://docs.agora.io/cn/Interactive Broadcast/product_live?platform=All Platforms)


## Developer Environment Requirements
* Win7 or higher/Mac
* Qt Creator

### Run the sample program

Register your account with [Agora.io Registration](https://dashboard.agora.io/cn/signup/) and create your own test project to get the AppID.Fill in the AppID into source code `agorartcengine.cpp`

```c++
    context.appId = ""; // Specify your APP ID here
```

Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the **sdk** to the "Agora-with-QT" folder in project(the old one may be over written).

* Windows： copy folder sdk to the directory where AgoraVideoCall.pro is located in.
* 
* Mac：`libs/AgoraRtcEngineKit.framework` copy to lib folder

Finaly, use Qt Creator to open `AgoraVideoCall.pro` file, then build and run.

## Running Environment
* Windows/Mac

## Contact us

- For potential issues, you may take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Would like to see how Agora SDK is used in more complicated real use case? Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase)
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Developer Forum](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Agora-with-QT/issues)

## License

The MIT License (MIT)
