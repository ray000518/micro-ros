# micro-ros
如果編譯失敗可以把platformio.ini和main.cpp換成mirco_ros.txt裡的那套 或 去https://github.com/micro-ROS/micro_ros_arduino/releases 下載jazzy的套件
將下載的 .zip 解壓縮，把資料夾名稱改為 micro_ros_arduino。（路徑確認為：mircorostest/lib/micro_ros_arduino/src/esp32/libmicroros.a）
將整個 micro_ros_arduino 資料夾放進你專案底下的 lib/ 目錄中

打開 Windows 的 PowerShell (以系統管理員身分執行)。

輸入 usbipd list 確認 ESP32 的 BUSID。

重新把 USB 綁定給 WSL：

usbipd attach --wsl --busid <你的BUSID>

回到 Ubuntu 視窗一，重新給予權限並啟動 Agent：

Bash
sudo chmod 666 /dev/ttyUSB0
source /opt/ros/jazzy/setup.bash
source ~/microros_ws/install/setup.bash
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
按一下裸板上的 EN 鍵。

在視窗二輸入：ros2 topic echo /suction_status

在視窗三發送指令：
全部關閉：ros2 topic pub --once /suction_command std_msgs/msg/Int32 "{data: 0}"

模組 1 吸取：ros2 topic pub --once /suction_command std_msgs/msg/Int32 "{data: 1}"

模組 1 釋放：ros2 topic pub --once /suction_command std_msgs/msg/Int32 "{data: 2}"

模組 2 吸取：ros2 topic pub --once /suction_command std_msgs/msg/Int32 "{data: 3}"

模組 2 釋放：ros2 topic pub --once /suction_command std_msgs/msg/Int32 "{data: 4}"

==================================================================================

ESP32
GPIO 4 ──> [繼電器 IN 1] (控制泵)
GPIO 5 ──> [繼電器 IN 2] (控制閥)
GND ─────> [繼電器 GND]


[ 6V 電源 ]
   │      │
   │(+)   │(-) ─────────────────────────────────┐
   │      │                                     │
   │      └─(分接)─┐                            │
   │               │                            │
   ▼               ▼                            ▼
[繼電器1 COM]   [繼電器2 COM]                 (負極接點)
[繼電器1 NO ]   [繼電器2 NO ]                 (負極接點)
   │               │                            │
   │(正極供電)      │(正極供電)                   │
   ▼               ▼                            │
[真空泵]        [電磁閥]                         │
   │               │                            │
   └───────────────┴────────────────────────────┘ (迴路完成)
