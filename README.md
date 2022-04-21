# XIAO_RP2040_SerialDemo
Windows10の通知メッセージをシリアル通信を介してXIAO RP2040に送りSSD1306 OLEDディスプレイに表示するデモ。

Demonstration of serial communication between Windows10 PC to XIAO RP2040 ( as Arduino ) with SSD1306 OLED display.

## 必要なもの
* XIAO RP2040 (Raspberry Pi PicoなどRP2040を搭載したマイコンでも可)
* SSD1306 OLEDディスプレイ (G8U2に対応している類似ディスプレイでも可)

## SSD1306の配線図(I2C)
![kairozu](https://user-images.githubusercontent.com/17384543/164484175-bd2331f0-9817-428b-8ba1-bf13879d8990.png)

## XIAO RP2040側アプリケーション
1. [Arduino IDE](https://www.arduino.cc/en/software)をダウンロードしてインストールする。
2. [ArduinoSerialCommandフォルダ](/ArduinoPlugin/ArduinoSerialCommand/)をダウンロードして「マイドキュメント\Arduino\libraries」に配置する。
3. [Arduinoスケッチ](/sketch/SerialCommand.ino)をダウンロードして実行する。
4. Arduino IDEを起動し、「ファイル」→「環境設定」→「追加のボードマネージャURL：」に https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json を追加する。
5. 「ツール」→「ボード」→「Seeed RP2040 Boards」→「Seeed XIAO RP2040」を選択する。
7. XIAO RP2040をUSB接続し、「ツール」→「シリアルポート」からXIAO RP2040のCOMポートを選択する。
    * ポートが分からない場合はデバイスマネージャーを開いて「ポート(COM と LPT)」から探す。
9. 「スケッチ」→「マイコンボードに書き込む」を選択して書き込む。
10. コンパイルはできるがボードへの書き込みがうまくいかない場合、BOOTSELボタンを押しながらXIAO RP2040をUSB接続し、「ユーザフォルダ\AppData\Local\Temp\arduino_build_xxxxx\」に生成されているuf2ファイルを手動でUSBストレージ(RPI-RP2)にコピーしてもよい。

## Windows側アプリケーション
1. [Visual Studioのサイト](https://visualstudio.microsoft.com/ja/downloads/)からVisual Studio 2022 コミュニティ版を[ダウンロード](https://visualstudio.microsoft.com/ja/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false)する。
1. インストーラーを起動し、「ユニバーサルWindowsプラットフォーム開発」にチェックを入れてインストールする。
1. [csフォルダ](/cs/)をダウンロードし、CustomSerialDeviceAccess.slnを開くとVisual Studioが起動する。
1. F5キーを押すとアプリケーションが起動する。
    * 1ページ目、XIAO RP2040をUSB接続するとリストにデバイスが追加されるので、選択して「Connect to device」をクリックする。
    * 2ページ目、「Data Terminal Ready Enabled=On」「Request To Send Enabled=On」にする。
    * 3ページ目を開くと通知リスナーへのアクセス許可画面が出るので「許可」を選択すると通知内容がリストに読み込まれ、XIAO RP2040に送信される。

## 備考
* [SSD1306にスクロールコマンドを送るサンプル](https://github.com/olikraus/u8g2/wiki/u8g2reference#sendf)
  *  `u8g2.sendF("caaaaaac", 0x027, 0, 3, 0, 7, 0, 255, 0x2f);`
  *  [もうちょっと詳しいサンプルコード](https://github.com/olikraus/u8g2/blob/master/sys/arduino/u8g2_page_buffer/HardwareScroll/HardwareScroll.ino)
* Earle F. Philhower, III版のボードを追加する手順
  * 「ファイル」→「環境設定」→「追加のボードマネージャURL：」に https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json を追加する。
  * 「ツール」→「ボード」→「Raspberry Pi RP2040 Boards」→「Raspberry Pi  Pico」を選択する。
  * 公式版との違いなど詳しい解説：[Raspberry Pi Picoの2つのArduinoサポートを比較](https://lipoyang.hatenablog.com/entry/2021/11/27/202142)
* XIAO RP2040のフルカラーRGB LEDの制御は[Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)を使うとよいらしい。
  * Arduino IDEの「ツール」→「ライブラリを管理」の画面を開き、「Adafruit NeoPixel」で検索すると簡単に追加できる。
  * 参考：[Seeed XIAO RP2040のフルカラーLED2種類を制御する](https://qiita.com/MergeCells/items/73273dac1d1327cfa3f2)

## 注意事項
上記デモはMicrosoftのCode Samplesをベースに作成しています。使用する際は各コードのライセンスを確認ください。

* [Serial Arduino sample](https://docs.microsoft.com/en-us/samples/microsoft/windows-universal-samples/serialarduino/)
* [Custom serial device sample](https://docs.microsoft.com/en-us/samples/microsoft/windows-universal-samples/customserialdeviceaccess/)

XIAO RP2040側は下記ライブラリを使用しています。
* [ArduinoSerialCommand (by Steven Cogswell)](https://github.com/scogswell/ArduinoSerialCommand)
* [U8g2 (by Oli Kraus)](https://github.com/olikraus/u8g2)
