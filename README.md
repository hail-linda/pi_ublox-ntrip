# pi_ublox-ntrip

用于运行在树莓派上为ublox提供ntrip服务

致谢：
```bash
https://github.com/hanoi404/ntrip
```

## 结构与工作流程

1. 使用**ntrip_client**向千寻ntrip要求RTCM数据并保存至文件 **\[timestamp\].nema** 
2. 使用**sendNema.py**读取 **\[timestamp\].nema** 并从串口发送至 ublox
3. 使用**ntripinit.sh**和**crontab**对**ntrip_client**与**sendNema.py**做进程保活

## Quick start

1. git clone & cd
```
git clone https://github.com/hail-linda/pi_ublox-ntrip.git
cd pi_ublox-ntrip
```
2. 修改千寻账号密码 与 参考坐标（用概略坐标给差分网络提供参考）
```
cd examples
vim ntrip_client.cc

更改：
    std::string ip = "203.107.45.154";
    int port = 8003;
    std::string user = "qxuesg00000";//
    std::string passwd = "0000000";
    std::string mountpoint = "AUTO";
中的 user 和 passwd

更改
    ntrip_client.set_location(46.00000,125.00000);
中的经纬度
```

3. 添加定时任务
```
sudo vim /etc/crontab
在文档末添加

*/2 *   * * *   pi  pkill py
* *     * * *   pi  bash [pi_ublox-ntrip]/ntripinit.sh > [pi_ublox-ntrip]/log 
```

4. make & reboot
```
make all
sudo shutdwon -r now
```

## TODO
- NTRIP账号外部输入
- GPGAA生成与注入
