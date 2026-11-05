#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h> // 更改為 Int32 標頭檔

// --- 定義第一套模組硬體腳位 ---
#define PUMP1_PIN 4  
#define VALVE1_PIN 5 

// --- 定義第二套模組硬體腳位 ---
#define PUMP2_PIN 18 
#define VALVE2_PIN 19

// 宣告單一 ROS 2 實體即可
rcl_subscription_t subscriber;
rcl_publisher_t publisher;

std_msgs__msg__Int32 sub_msg; // 更改訊息型態為 Int32
std_msgs__msg__Int32 pub_msg; 

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

// ==========================================
// 整合狀態機的回呼函式
// ==========================================
void subscription_callback(const void * msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  
  // 預設將收到的數值作為回傳狀態
  pub_msg.data = msg->data; 

  switch (msg->data) {
    case 0:
      // 0：關閉兩個模組 (全部待命)
      digitalWrite(PUMP1_PIN, LOW);
      digitalWrite(VALVE1_PIN, LOW);
      digitalWrite(PUMP2_PIN, LOW);
      digitalWrite(VALVE2_PIN, LOW);
      break;

    case 1:
      // 1：模組 1 吸取
      digitalWrite(VALVE1_PIN, LOW); 
      delay(50);                     
      digitalWrite(PUMP1_PIN, HIGH); 
      break;

    case 2:
      // 2：模組 1 釋放
      digitalWrite(PUMP1_PIN, LOW);  
      delay(50);                     
      digitalWrite(VALVE1_PIN, HIGH);
      delay(500); 
      digitalWrite(VALVE1_PIN, LOW); 
      break;

    case 3:
      // 3：模組 2 吸取
      digitalWrite(VALVE2_PIN, LOW); 
      delay(50);                     
      digitalWrite(PUMP2_PIN, HIGH); 
      break;

    case 4:
      // 4：模組 2 釋放
      digitalWrite(PUMP2_PIN, LOW);  
      delay(50);                     
      digitalWrite(VALVE2_PIN, HIGH);
      delay(500); 
      digitalWrite(VALVE2_PIN, LOW); 
      break;

    default:
      // 收到未知數值：基於安全，強制全部關閉，並回傳狀態 0
      digitalWrite(PUMP1_PIN, LOW);
      digitalWrite(VALVE1_PIN, LOW);
      digitalWrite(PUMP2_PIN, LOW);
      digitalWrite(VALVE2_PIN, LOW);
      pub_msg.data = 0;
      break;
  }
  
  // 動作執行完畢後，發布狀態回饋
  rcl_publish(&publisher, &pub_msg, NULL);
}

void setup() {
  set_microros_transports(); 
  
  // 初始化所有硬體腳位，預設皆為關閉 (LOW)
  pinMode(PUMP1_PIN, OUTPUT);
  pinMode(VALVE1_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(VALVE2_PIN, OUTPUT);
  
  digitalWrite(PUMP1_PIN, LOW); 
  digitalWrite(VALVE1_PIN, LOW); 
  digitalWrite(PUMP2_PIN, LOW); 
  digitalWrite(VALVE2_PIN, LOW); 
  
  delay(2000);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  
  rclc_node_init_default(&node, "suction_controller_node", "", &support);

  // 初始化單一訂閱者，型態改為 Int32
  rclc_subscription_init_default(
    &subscriber, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "suction_command");

  // 初始化單一發布者，型態改為 Int32
  rclc_publisher_init_default(
    &publisher, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "suction_status");

  // 初始化 Executor，處理 1 個訂閱者
  rclc_executor_init(&executor, &support.context, 1, &allocator);
  rclc_executor_add_subscription(&executor, &subscriber, &sub_msg, &subscription_callback, ON_NEW_DATA);
}

void loop() {
  delay(10);
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
}