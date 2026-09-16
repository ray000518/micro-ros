# micro-ros
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

在視窗三發送指令：ros2 topic pub --once /suction_command std_msgs/msg/Bool "{data: false}"


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
