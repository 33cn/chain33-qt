# 安装
- 下载 Qt 地址： http://download.qt.io/archive/qt/ (选择 5.9 及以上版本)。
- 选择对应平台的安装包后安装，选择组件中，一定要勾选 MinGW 和 Qt Creator。
- 安装 git，下载地址：https://git-scm.com/ 。
- Windows 下安装 WinRAR 打包工具。

# 编译
- 下载代码 `git clone https://github.com/33cn/chain33-qt.git`
- 双击打开 chain33-qt.pro 文件，左下角选择 Release 版本，开始构建项，等待构建完成。
- 在 release 目录下 Windows 会生成 chain33-qt.exe 程序，Linux 会生成 chain33-qt 程序，MAC 会生成 chain33-qt.app 文件包。

# 不同平台打包
## Windows
### 构建 chain33.exe 程序
- 根据 https://github.com/33cn/chain33 构建 chain33.exe 和 chain33-cli.exe。
- 用 gox.exe (在 \chain33-qt\src\tool 下) 工具编译 x86 版本的 chain33.exe 和 chain33-cli.exe，步骤：
```
    ./gox -osarch="windows/386" -ldflags="-X github.com/33cn/chain33/common/version.GitCommit=`git rev-parse --short=8 HEAD`" github.com/33cn/chain33/cmd/chain33
    
    ./gox -osarch="windows/386" github.com/33cn/chain33/cmd/cli 
```
- 改名称为 chain33-x86.exe 和 chain33-cli-x86.exe。

### 准备
- 安装 WinRAR 工具，打包程序，可参考 [winrar 打包程序为 exe 文件](https://jingyan.baidu.com/article/6fb756ec9a9e09241858fbc1.html)
- 在 chain33-qt.exe 所在目录下运行命令 `windeployqt.exe btyWallet.exe` 会拷贝所需要的对应 DLL 文件，如果运行时还提示 DLL 文件缺少，可以在 Qt 的安装目录 bin 目录下寻找（例如 C:\Qt\Qt5.10.0\5.10.0\mingw53_32\bin）。
- 同一目录下添加 chain33.exe、chain33-cli.exe、chain33-x86.exe、chain33-cli-x86.exe、chain33.toml 这 5 个文件。

### 打包成可执行程序
- 全选所有文件，右击 "添加到压缩文件"；
- 压缩文件名改为：chain33-qt-Win.exe；
- 点击高级，自解压缩项；
    - 常规，解压路径写为 chain33-qt，选择在 “Prpgram Files” 中创建；
    - 设置，提取后运行 chain33-qt.exe；
    - 高级，添加快捷方式；
    - 文本和图标，标题为：chian33-qt 安装，选择图标 novacoin.ico (在 \chain33-qt\src\res 下)；

## Mac
### 准备 chain33-qt.app
- 根据 https://github.com/33cn/chain33 构建 chain33 和 chain33-cli。
- 拷贝 chain33、chain33-cli、chain33.toml 这 3 个文件到 chain33-qt.app/Contents/MacOS/ 目录下。
- 打开终端，到 chain33-qt.app 所在目录，运行命令：
```
cd .../chain33-qt/release
cp $GOPATH/src/github.com/33cn/chain33/build/chain33 /release/chain33-qt.app/Contents/MacOS/chain33
cp $GOPATH/src/github.com/33cn/chain33/build/chain33-cli /release/chain33-qt.app/Contents/MacOS/chain33-cli
cp $GOPATH/src/github.com/33cn/chain33/build/chain33.toml /release/chain33-qt.app/Contents/MacOS/chain33.toml
otool -L chain33-qt.app/Contents/MacOS/chain33-qt 
macdeployqt chain33-qt.app
```

### 把 chain33-qt.app 打包成 dmg 文件
- 通过 Disk Utility 创建一个空白的 dmg 镜像, File –> New –> Blank Disk Image
- 默认设置 120MB，名称为 chain33-qt-Mac，最终制作出来的 dmg 文件会自动裁剪掉空白的。
- 双击 “chain33-qt-Mac” 出来的空白窗口
- 将上面制作好的 chain33-qt.app 拽到 “chain33-qt-Mac” 里面，再做一个 Applications 快捷方式 (右键–>Make Alias)
- 将做好的 dmg，转换一下就完成了，在 Disk Utility 中如果没有的话，请将它拽回去就可以了，然后右键 chain33-qt-Mac.dmg –> Convert ”chain33-qt-Mac”
- 可参考文档 [Qt Mac 下软件 Release 发布 dmg](https://blog.csdn.net/fox64194167/article/details/38441331)

## Ubuntu
### 打包 chain33-qt-x86_64.AppImage
- 新建一个文件夹为 chain33-qt-Ubuntu，把 chain33 、chain33-cli、chain33-qt、chian33.toml 这 4 个文件，都放在 chain33-qt-Ubuntu 文件夹下。
- 运行命令 `linuxdeployqt ./chain33-qt-Ubuntu/chain33-qt -appimage`
- 改名为 chain33-qt-x86_64.AppImage。

# 配置文件
* 默认不设置，用程序默认配置，src/config/StyleConfig.ini 有模板。
* 字段解析：
        
| 字段名称 | 默认配置 | 备注 |
|---|---|---|
| Chain33Name | chain33 | 底层运行链的名称 |
| AppName | chain33-qt |  |
| AppName_zh | chain33-qt |  |
| AppName_en | chain33-qt |  |
| StyleType | yellow | 只支持两种样式：yellow 和 blue |
| NetworkUrl | http://localhost:8801/ |  |
