# 実行環境
- Windows 11
- Python 3.8
- Kivy 2.3.1

# ライブラリ
- pyserial
- pybluez

# インストール
python -m pip install --upgrade pip

pip install kivy
pip install pyserial

pip install pybluez

エラーの場合以下を試す：

pip install git+https://github.com/pybluez/pybluez.git#egg=pybluez

# 実行方法
python controller.py