### 実行環境
- Windows 11
- Python 3.8

# 実行方法
二つの方法で実行できます。
- exeファイルから起動
- Pythonファイルから起動

## exeファイルから起動
直接実行する場合、exe\controller.exeをダブルクリックします。

## Pythonファイルから起動
ソースを変更したり、プログラミングを行う場合、環境を整った上、Pythonファイルを実行します。

### 必要なライブラリのインストール
- Kivy 2.3.1
- pyserial 3.5
- pybluez 0.30

※コマンドの実行は、Windowsコマンドプロンプトを利用します。
※Windowsコマンドプロンプトは、「ファイル名を指定して実行」でcmdを指定して起動できます。

1. プロジェクトディレクトリの準備
    ```
    cd secaro_arduino_projects
    ```

2. バーチャル環境の作成
    ```
    python -m venv venv
    ```

3. バーチャル環境の有効化
    ```
    venv\Scripts\activate
    ```

4. 必要なパッケージのインストール

    ```
    python -m pip install --upgrade pip
    pip install kivy
    pip install pyserial
    pip install pybluez
    ```
    pybluezのインストールでエラーになった場合、以下を試す：
    ```
    pip install git+https://github.com/pybluez/pybluez.git#egg=pybluez
    ```

5. 実行
    ```
    python controller\src\controller.py
    ```

6. バーチャル環境の無効化
    ```
    deactivate
    ```