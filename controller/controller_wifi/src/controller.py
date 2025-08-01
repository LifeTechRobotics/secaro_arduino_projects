from kivy.app import App
from kivy.uix.widget import Widget
from kivy.uix.popup import Popup
from kivy.core.text import LabelBase
from kivy.lang import Builder
from kivy.config import Config
from kivy.factory import Factory
from kivy.clock import Clock

import threading
import os
import socket
import time

Config.set('graphics', 'width', 1100)
Config.set('graphics', 'height', 600)
Config.set('kivy', 'exit_on_escape', 0)

kv_path = os.path.join(os.path.dirname(__file__), "ui.kv")
kv = Builder.load_file(kv_path)

# 日本語フォントを登録
font_path = os.path.join(os.path.dirname(__file__), "ipaexg.ttf")
LabelBase.register(name='Japanese', fn_regular=font_path)

# UDP通信用
UDP_PORT_CONTROL_BROADCAST = 4210
UDP_PORT_CONTROL = 4211

class CustomPopup(Popup):
    pass

class MyLayout(Widget):
    def __init__(self):
        super().__init__()
        self.connected = False
        self.msg_header = "[Remote Controller] "

        # スレッド用
        self.thread_running = False
        self.thread_stop = threading.Event()

        # デバイススキャン用
        self.robots = {}  # {name: ip}
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', UDP_PORT_CONTROL_BROADCAST))
        self.sock.setblocking(False)

        # 操縦対象のロボットのIP
        self.target_ip = ""

    # デバイススキャン（WiFi）
    def scan_devices_wifi(self):
        while not self.thread_stop.is_set():
            try:
                data, addr = self.sock.recvfrom(1024)
                msg = data.decode()
                # print(f"Received: {msg} from {addr}")
                if ":" in msg:
                    name, ip = msg.split(":")
                    if name not in self.robots:
                        self.robots[name] = ip
            except BlockingIOError:
                pass

            # time.sleep(0.1)

        print(self.msg_header + f"スキャン終了")

    # デバイススキャンスレッド（WiFi）
    def start_scan_devices_wifi(self):
        self.ids.scan_button.text = "スキャン中..."
        self.ids.scan_button.disabled = True

        # 初期化
        self.robots = {}
        self.ids.device_list.clear_widgets()
        self.thread_stop.clear()

        # スキャンスレッドを起動
        self.scan_thread = threading.Thread(target=self.scan_devices_wifi, daemon=True)
        self.scan_thread.start()
        self.thread_running = True

        # 1sを待つ
        time.sleep(1)

        # 1s毎にデバイスリスト更新するよう
        self.event = Clock.schedule_interval(self.clock_callback, 1)

    # デバイスリスト更新Callback（WiFi）
    def clock_callback(self, dt):
        # デバイス追加
        for name, ip in self.robots.items():
            btn = Factory.NameButton(text=f"{name} ({ip})")
            btn.bind(on_press=lambda instance, a=ip: self.select_target_robot(instance, a))
            self.ids.device_list.add_widget(btn)
            status = Factory.StatusButton(text="未選択")
            self.ids.device_list.add_widget(status)

        if len(self.robots) > 0:
            # デバイスリスト更新停止
            self.event.cancel()

            # スキャン停止
            self.thread_stop.set()
            self.scan_thread.join(1)
            self.thread_running = False
            self.ids.scan_button.text = "Wi-Fiロボットを探す"
            self.ids.scan_button.disabled = False


    # 操作するロボットの選択
    def select_target_robot(self, instance, ip):
        self.target_ip = ip

        # 前回の選択を外す
        if (self.connected):
            self.status_button.text = "未選択"

        # 選択されたデバイスの対応するステータスボタンを取得
        parent = instance.parent
        siblings = parent.children
        index = siblings.index(instance)
        if index > 0:
            self.status_button = siblings[index - 1]
        
        self.status_button.text = "選択中"
        self.connected = True

    # メッセージボックス表示
    def show_message(self, message, need_connect):
        self.popup = CustomPopup()
        self.popup.ids.msg_label.text = message
        if need_connect:
            self.popup.ids.ok_button.bind(on_press=self.connect)
        else:
            self.popup.ids.ok_button.bind(on_press=self.popup.dismiss)
        self.popup.open()

    # 前進
    def forward(self):
        try:
            if self.connected:
                cmd_string = 'F\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 後退
    def back(self):
        try:
            if self.connected:
                cmd_string = 'B\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 左旋回
    def left(self):
        try:
            if self.connected:
                cmd_string = 'L\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 右旋回
    def right(self):
        try:
            if self.connected:
                cmd_string = 'R\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 停止
    def stop(self):
        try:
            if self.connected:
                cmd_string = 'S\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 速度設定（左）
    def set_speed_l(self):
        try:
            if self.connected:
                cmd_string = 'l' + str(self.ids.left_speed.value) + '\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 速度設定（右）
    def set_speed_r(self):
        try:
            if self.connected:
                cmd_string = 'r' + str(self.ids.right_speed.value) + '\n'
                # print(self.msg_header + f"IP: {self.target_ip} 送信: {cmd_string}")
                self.sock.sendto(cmd_string.encode('utf-8'), (self.target_ip, UDP_PORT_CONTROL))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

class RemoteApp(App):
    def build(self):
        self.title = "Remote Controller"
        self.main_window = MyLayout()
        return self.main_window
    
    def on_stop(self):
        try:
            # スレッド終了
            if self.main_window.thread_running:
                self.main_window.thread_stop.set()
                self.main_window.scan_thread.join(1)

            # ソケット終了
            self.main_window.sock.close()
            self.main_window.connected = False
            print(self.main_window.msg_header + f"通信終了" )
        except Exception as e:
            print(self.main_window.msg_header + "[ERROR] " + e)

if __name__ == '__main__':
    RemoteApp().run()