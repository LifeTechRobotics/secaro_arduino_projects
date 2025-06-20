from kivy.app import App
from kivy.uix.widget import Widget
from kivy.uix.popup import Popup
from kivy.core.text import LabelBase
from kivy.lang import Builder
from kivy.config import Config
from kivy.factory import Factory
from kivy.clock import Clock

import serial
import serial.tools.list_ports
import bluetooth
import threading
import os

Config.set('graphics', 'width', 1100)
Config.set('graphics', 'height', 600)
Config.set('kivy', 'exit_on_escape', 0)

kv_path = os.path.join(os.path.dirname(__file__), "ui.kv")
kv = Builder.load_file(kv_path)

# 日本語フォントを登録
font_path = os.path.join(os.path.dirname(__file__), "ipaexg.ttf")
LabelBase.register(name='Japanese', fn_regular=font_path)

class CustomPopup(Popup):
    pass

class MyLayout(Widget):
    def __init__(self):
        super().__init__()
        self.connected = False
        self.devices = []
        self.msg_header = "[Remote Controller] "

    # デバイススキャン
    def scan_devices(self):
        self.devices = bluetooth.discover_devices(duration=8, lookup_names=True)

    # デバイススキャンスレッド
    def start_scan_devices(self):
        if self.connected:
            self.show_message("接続済みです。", False)
            return
                
        self.ids.scan_button.text = "スキャン中..."
        self.ids.scan_button.disabled = True

        # デバイスリストをクリア
        self.devices = []
        self.ids.device_list.clear_widgets()

        # 1s毎にデバイスリスト更新するよう設定
        self.event = Clock.schedule_interval(self.clock_callback, 1)
        threading.Thread(target=self.scan_devices, daemon=True).start()

    # デバイスリスト更新Callback
    def clock_callback(self, dt):
        # デバイス追加
        for addr, name in self.devices:
            btn = Factory.NameButton(text=f"{name} ({addr})")
            btn.bind(on_press=lambda instance, a=addr: self.pair_with_device(instance, a))
            self.ids.device_list.add_widget(btn)
            status = Factory.StatusButton(text="未接続")
            self.ids.device_list.add_widget(status)

        if len(self.devices) > 0:
            # デバイスリスト更新停止
            self.event.cancel()
            self.ids.scan_button.text = "Bluetooth デバイスをスキャン"
            self.ids.scan_button.disabled = False

    # 手動ペアリングへ誘導
    def pair_with_device(self, instance, address):
        if self.connected:
            self.show_message("接続済みです。", False)
            return
        self.address = address.replace(':', '')
        self.show_message("Bluetoothデバイスをペアリングしてください。\n\nペアリングが終わりましたらOKを押してください。", True)

        # 選択されたデバイスの対応するステータスボタンを取得
        parent = instance.parent
        siblings = parent.children
        index = siblings.index(instance)
        if index > 0:
            self.status_button = siblings[index - 1]

    # メッセージボックス表示
    def show_message(self, message, need_connect):
        self.popup = CustomPopup()
        self.popup.ids.msg_label.text = message
        if need_connect:
            self.popup.ids.ok_button.bind(on_press=self.connect)
        else:
            self.popup.ids.ok_button.bind(on_press=self.popup.dismiss)
        self.popup.open()

    # Bluetoothデバイスへ接続
    def connect(self, instance):
        # メッセージボックスを閉じる
        self.popup.dismiss()

        # 接続を行う
        self.port = self.find_bluetooth_com_port()
        if self.port != None:
            self.ser = serial.Serial(self.port, 115200, timeout=1)
            self.connected = True
            print(self.msg_header + f"通信開始 {self.port}")

            # 接続成功
            self.status_button.text = "接続済み"
        else:
            # 接続失敗
            self.show_message("デバイスへ接続できませんでした。ペアリングを確認してください。\n\nペアリングが終わりましたらOKを押してください。", True)

    # BluetoothデバイスのCOMポート特定
    def find_bluetooth_com_port(self):
        ports = []
        for p in serial.tools.list_ports.comports():
            if p.description.find("Bluetooth") == -1:
                continue
            if p.hwid.find(self.address) >= 0:
                ports.append(p.device)

        for port in ports:
            try:
                ser = serial.Serial(port, 115200, timeout=1, write_timeout=1)
                ser.write(b'P\n')
                response = ser.readline()
                if response:
                    print(self.msg_header + f"PING成功: {port}")
                    ser.close()
                    return port  # ポートが見つかった
            except Exception as e:
                print(self.msg_header + f"[ERROR] {e}")
                continue
        return None

    # 前進
    def forward(self):
        try:
            if self.connected:
                cmd_string = 'F\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 後退
    def back(self):
        try:
            if self.connected:
                cmd_string = 'B\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 左旋回
    def left(self):
        try:
            if self.connected:
                cmd_string = 'L\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 右旋回
    def right(self):
        try:
            if self.connected:
                cmd_string = 'R\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 停止
    def stop(self):
        try:
            if self.connected:
                cmd_string = 'S\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 速度設定（左）
    def set_speed_l(self):
        try:
            if self.connected:
                cmd_string = 'l' + str(self.ids.left_speed.value) + '\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

    # 速度設定（右）
    def set_speed_r(self):
        try:
            if self.connected:
                cmd_string = 'r' + str(self.ids.right_speed.value) + '\n'
                # print(self.msg_header + f"送信: {cmd_string}")
                self.ser.write(cmd_string.encode('utf-8'))
        except Exception as e:
            print(self.msg_header + f"[ERROR] {e}")

class RemoteApp(App):
    def build(self):
        self.title = "Remote Controller"
        self.main_window = MyLayout()
        return self.main_window
    
    def on_stop(self):
        try:
            if self.main_window.connected:
                self.main_window.ser.close()
                print(self.main_window.msg_header + f"通信終了 {self.main_window.port}" )
        except Exception as e:
            print(self.main_window.msg_header + "[ERROR] " + e)

if __name__ == '__main__':
    RemoteApp().run()