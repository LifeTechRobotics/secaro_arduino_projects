# Secaro for Arduino のプロジェクトファイル
- LED点滅

  led/  
  └─ led.ino  


- サーボモータ動作

  servo/  
  └─ servo.ino  


- ロボット動作

  robot  
  └─ robot.ino  


- ラジコン動作

  remote  
  └─ remote_bluetooth/　　　　　　　# Bluetooth版  
  　　　└─ remote_bluetooth.ino  
  └─ remote_wifi/　　　　　　　　　　# Wi-Fi版  
　　　　└─ remote_wifi.ino  
 

- 自律走行

  auto  
　└─ auto_bluetooth/　　　　　　　　# Bluetooth版  
　　　　└─ auto_bluetooth.ino  
　└─ auto_wifi/　　　　　　　　　　　# Wi-Fi版  
　　　　└─ auto_wifi.ino  


- ラジコン動作＆自律走行用コントローラー

  controller  
　└─ controller_bluetooth/　　　　　# Bluetooth版  
　　　　├─ exe/  
　　　　│        └─ controller.exe　# 実行ファイル  
　　　　├─ src/  
　　　　│        ├─ controller.py  
　　　　│        ├─ ui.kv  
　　　　│        └─ ipaexg.ttf　# フォントのリソース  
　　　　└─ README.md  
　└─ controller_wifi/　　　　　　　　# Wi-Fi版  
　　　　├─ exe/  
　　　　│        └─ controller.exe  
　　　　├─ src/  
　　　　│        ├─ controller.py  
　　　　│        ├─ ui.kv  
　　　　│        └─ ipaexg.ttf  
　　　　└─ README.md  
