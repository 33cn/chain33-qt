# btyWallet

## 编译
安装 QT 环境(mingw编译)，设置环境变量
```
cd btywallet
qmake btyWallet.pro
make
```

## 打包
- [Windows 下钱包编译打包流程](http://note.youdao.com/noteshare?id=b2fcfe841a9cc919bcf414a7ce2d80e6&sub=63561FE638BF44B1B8B7441B353922D5)
- [Mac 下钱包打包流程参考](http://note.youdao.com/noteshare?id=9370aae5ca4426fbcf2be97c94e3caa6)
- Ubuntu 下钱包打包流程
    + 把最新的 chian33, bityuan, chian33-cli，bityuan.toml 4个文件，都放在 Bityuan-Wallet-Linux 目录下
    + 运行命令 `linuxdeployqt /home/qq/work/qt/btywallet/Bityuan-Wallet-Linux/bityuan -appimage` 打包完成
- 打好的 4 个包放到 `https://gitlab.33.cn/yanchenta/btyres`仓库下，替换其中的 Bityuan-Wallet-Win.exe，Bityuan-Wallet-Win-xp.exe，Bityuan-Wallet-mac.dmg，Bityuan-Wallet-Linux.tar.gz，4个文件。


## 逻辑
### 启动程序
#### 获取数据目录
- 启动前先判断是否已经存在数据目录
- 默认目录 
    + Windows < Vista: C:\Documents and Settings\Username\Application Data\bityuan
    + Windows >= Vista: C:\Users\Username\AppData\Roaming\bityuan
    + Mac: ~/Library/Application Support/bityuan
    + Unix: ~/.bityuan
- Windows 的 数据目录写入注册表 "HKEY_CURRENT_USER\\Software\\bityuan\\bityuan-Qt"

#### 启动 chain33
- 启动 chain33 
- 参数 `-datadir` 数据目录，`-fixtime` 是否启动自动修复时间（默认不开启）
- 循环判断 chain33 进程是否存在，不存在重新启动
- 记录 chain33 奔溃退出信息

#### 获取seed
- getstatus 获取钱包状态
- 是否存在 seed
- 不存在，显示 seed 界面，存在 seed，直接进入主界面
- 新创建 seed 默认创建一个地址，导入 seed 默认创建 5 个地址
- seed 中创建地址
    + 解锁钱包
    + 创建地址
    + 判断地址是否创建成功
    + 锁定钱包

### 其他逻辑
#### 状态
- 循环获取 IsSync 是否同步成功
- 循环获取 GetPeerInfo 更新高度，判断当前连上几个节点
    + peer 减 1 个自身的节点
    + 节点 1个，1 格信号
    + 节点 2-5 个，2 格
    + 5-15，3格
    + 15 以上，4格
- 循环获取 GetWalletStatus 显示当前钱包状态
- 循环获取 GetTimeStatus 判断时间是否同步
- 循环获取 NetInfo 判断是否可以对其他节点提供服务
- Windows 下循环（24h）判断当前磁盘是否少于 5 G，少于给出提醒
- 循环获取 GetAccounts 地址信息及金额

#### 显示交易记录
- 交易记录如果当前是首页，循环获取最新交易记录
- 接口为`WalletTxList`
    + 第一次获取交易记录 `fromTx` 为空字符串，`direction` 为 0
    + 前一页 `direction` 为 1，`fromTx` 为上一次数据中第一条数据中 `height` 和 `index` 的拼接，格式:`QString().sprintf("%013d", txMap["height"].toInt()) + QString().sprintf("%05d", txMap["index"].toInt())`
    + 后一页 `direction` 为 0，`fromTx` 为上一次数据中最后一条数据中 `height` 和 `index` 的拼接
- `ty` == 1，有错误的交易要过滤
- 双击显示交易详情
- 合约地址，前面加上(合约)两个字特殊的地址
    + 16htvcBNSEA7fZhAdLJphDwQRQJaHpyHTp
    + 1GaHYpWmqAJsqRwrpoNcB8VvgKtSwjcHqt
    + 1LFqVvGaRpxbEWCEJVpUHAwXnh5Rt591m
    + 1DzTdTLa5JPpLdNNP2PrV1a6JCtULA7GsT 

#### 退出/重启 chain33
- 停止启动 chain33 线程
- 退出 chain33 
- 循环判断 chain33 进程是否退出
- 启动 chain33 线程
- 用于以下三种情况
    + 退出整个程序，退出 chain33
    + 开启或关闭自动修复时间，重启 chain33
    + 修改数据目录，重启 chain33

#### 开启自动修复时间
- 修改状态，保存在数据目录下 "/wallet/QtConfig.ini" 文件中
- 重启 chain33 修改 `-fixtime` 这个参数

#### 修改数据目录
- 只有 Windows 下版本才有这个功能
- 退出 chain33
- 新的数据目录下，要拷贝 wallet、datadir 和 FriendsAddrList.xml 文件
- 旧的数据删除 datadir、logs 文件夹
- 修改数据目录，修改注册表
- 重启 chain33 修改 `-datadir` 这个参数

#### 离线挖矿授权
- 本地离线地址，绑定授权其他在线地址进行挖矿
- 必须解锁钱包
- 生成绑定交易，调用 CreateBindMiner
- 生成转账交易，调用 CreateRawTransaction
- 签名以上两笔交易，调用 SignRawTx
- 保存成 TXT 文件，可以在区块链浏览器上发送以上两笔签名后的交易

#### 调试窗口
- 直接调用当前目录下 chain33-cli 程序，调用时加参数
- 结果返回有多一行换行符，最好去掉

#### 隐藏地址
- 好友地址提供删除，本地数据目录下“FriendsAddrList.xml”文件中保存好友地址信息
- 我的地址提供隐藏功能，隐藏后标签修改为“Deleted”+原标签名称+当前时间，不显示改地址
- 地址提供导出功能

#### C2C交易（暂时去掉不显示）
- 删除资产管理界面，合并买入、卖出、撤销操作
- 卖出 token
    + token 转入合约，构建交易哈希
    + 构建卖单交易哈希
    + 合并上面两笔交易的哈希，构建合并交易哈希
    + 签名交易
    + 发送交易
- 买入 token
    + BTY 转入 token 合约
    + 构建买单交易
    + 合并上面两笔交易的哈希，构建合并交易哈希
    + 签名交易
    + 发送交易
- 撤销交易
    + 构建撤销交易
    + 转出 token 交易哈希
    + 合并上面两笔交易的哈希，构建合并交易哈希
    + 签名交易
    + 发送交易






