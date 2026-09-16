# micro-ros
打開 Windows 的 PowerShell (以系統管理員身分執行)。

輸入 usbipd list 確認 ESP32 的 BUSID。

重新把 USB 綁定給 WSL：

usbipd attach --wsl --busid <你的BUSID>

回到 Ubuntu 視窗一，重新給予權限並啟動 Agent：

Bash
sudo chmod 666 /dev/ttyUSB0
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
按一下裸板上的 EN 鍵。 這次你一定會立刻看到 Session established。

在視窗二輸入：ros2 topic echo /suction_status

在視窗三發送指令：

Bash
ros2 topic pub --once /suction_command std_msgs/msg/Bool "{data: false}"
