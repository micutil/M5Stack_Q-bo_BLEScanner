# Q-bo BLE Scan (v1.6)
Q-bo BLE Scannerは、Robi2の付属品であるQ-boからBLE信号を受け取って、その信号に応じた音声ファイルを再生させるとともに、画面上に表示されたQ-bo風の音声に合わせて顔が動くというものです。

[LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)に対応していて、microSDからの起動には[LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)もインストールされている必要があります。

**注意：Odroid-GOメニューを使う場合（初期状態の場合）は[LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)のインストールは必要はありません。microSDには[Skeleton file](https://wiki.odroid.com/odroid_go/make_sd_card)を使います。**
どのようなプログラムなのかは、以下の像をクリックすると操作中の動画をで観ることができます。(動画は旧バージョンです）<br>
[![preview](images/preview1.png)](https://www.youtube.com/watch?v=-vYVtkeuwOw)

[![preview](images/preview2.png)](https://www.youtube.com/watch?v=OtDAhQ7s60I)

###更新
- Odroid-GO対応
- スリープ対応
- 再生した音声IDの表示を復活させた
- M5Stack 0.27, ESP32 1.02でビルド

## インストール

### ① M5Stack/Odroid-GOに入れるmicroSDを作る
Odroid-GOメニューを使う場合（初期状態の場合）は[Skeleton file](https://wiki.odroid.com/odroid_go/make_sd_card)を作成しておきます。

1. microSDに**Q-boのココロ**の中身をコピーします。
2. 同梱のファイルの中にmicroSDフォルダの中のファイルを、microSDにコピーします。その際に、**すでにフォルダがある場合は結合させて、すでにコピーされているファイルを消さないようにご注意下さい**。
3. これでmicroSDの作成は終了です。

### ビルド済みファイル
- in M5_Gamebin ... 4M minimal SPIFFS partition scheme.
- in MF_Gamebin ... Fire defaut partition scheme.
- in OG_Gamebin ... bin file for Odroid-GO.
- in odroid/firmwares ... fw file for Odroid-GO Menu Control.

### ② USBドライバーをインストールする
M5Stackをコンピュータと繋いで、プログラムの転送など通信するためには、Silicon LabsのUSBDriverをインストールする必要があります。もしまだインストールしてなければ
[シリアル接続の確立方法](https://docs.m5stack.com/#/ja/related_documents/establish_serial_connection)
のサイトを参考にしてインストールして下さい。**Odroid-GOメニューを使う場合は必ずしもドライバーのインストールをする必要はありません**

> なお、Macの場合、インストールしただけではセキュリティが通ってないので、インストール後、環境設定のセキュリティとプライバシーの一般で、インストールしたドライバーの許可をして下さい。
> 
![セキュリティ](images/kyoka.jpg)

### ③ M5Burner_Micで、LovyanLauncherをインストールする
**Odroid-GOメニューを使う場合はこの操作を行ってはいけません!!!**

"[M5Burner_Mic](https://github.com/micutil/M5Burner_Mic)"というプログラムで[LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)をM5StackまたはOdroid-GOにインストールする方法を説明します。

1. [M5Burner_Mic](https://github.com/micutil/M5Burner_Mic) のページから「Download [here](http://micutil.com/download/M5Burner_Mic.zip)」の所でM5Burner\_Micをダウンロードし、解凍して下さい。M5Burner\_Micフォルダはお好みの場所に入れて下さい（**フルパスに日本語名を含まない場所に置くのが無難です**）。
2. M5Stackにインストールする場合：[LovyanLauncher firmware](https://github.com/lovyan03/LovyanLauncher-firmware)のページから「Clone or download」のボタンでダウンロードし解凍。LovyanLauncher-firmware-masterフォルダをM5Burner\_Micのフォルダの中にあるfirmwaresフォルダにコピーして下さい。
3. Odroid-GOにインストールする場合：[Odroid-GO版のLovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher/blob/master/LovyanLauncher/build_odroid_go/LovyanLauncher.bin)のページの「Download」ボタンをクリックするとダウンロードできます。ダウンロードしたLovyanLauncher.binは、M5Burner\_Mic起動後、M5Burner\_Micのウインドウに**ドラッグ＆ドロップ**して下さい。
2. USBケーブルでパソコンとM5StackまたはOdroid-GOを繋げて下さい。
3. M5Burner\_Micをダブルクリックして起動します。
4. USBシリアルポートをM5StackまたはOdroid-GOのポートに設定します。
 - Macの場合はポートに名前がつくので「**SLAB_USBtoUART**」という名前のポートを選んで下さい。
 - Windowsの場合は、**COM3**とか、COM4とかの名前になっています。ひとつしか表示されてなかったら、それがM5Stackのポートでしょう。もしいくつか表示されているようだったら、コントロールパネルから、デバイスマネージャーのポートをみて番号を確認して下さい。例えば以下の図の場合だと**COM4**であるということになります。<br>![ポート番号](images/portnum.jpg)
5. 「Priset」のポップアップメニューで「**LovyanLauncher......**」を選択します。
6. 「**Start**」ボタンをクリックすると、プログラムの転送が開始します。
7. プログラムの転送が終わるとM5StackまたはOdroid-GOは、リセットされ、インストールした**LovyanLauncher**が起動します。

#### LovyanLauncherの操作
	1. メニューの**SD-Updater**を選択して"OK"
	2. microSDに入ってるプログラムの一覧が表示されます。
	3. 目的のプログラムを探して選択し、"OK"
	4. 起動する場合は、"Execute"

#### ODROID-GO Device Firmware Menu Controlsの操作

	1. 電源スイッチ OFF
	2. Bボタンを押したまま, 電源スイッチ On
	3. メニューが表示されたらBボタンを離す

	アプリケーションF/Wの起動
	- A ボタン: アプリケーションF/Wを選択
	- B ボタン: キャンセル
	- Start ボタン: アプリケーションF/Wの書き込み


## Arduinoを使う場合に必要な設定
2. ESP用のAudio関連のライブラリーをダウンロード
	- https://github.com/earlephilhower/ESP8266Audio
	- bhttps://github.com/Gianbacchio/ESP8266_Spiram<br>
	それぞれを「Close or download」でダウンロード
3. 上記でダウンロードした、
	- ESP8266Audio-masterフォルダ
	- ESP8266_Spiram-masterフォルダ<br>
	をArduinoのlibrariesフォルダに入れる
4. ESP8266Audioの内容を2ヶ所
	- https://github.com/robo8080/M5Stack_PlayMP3_Avatar_Test​(by robo8080)の ページに書いてある内容に従って、
	- AudioOutputI2S.h
	- AudioOutputI2S.cpp<br>
	を書き換える。
5. Q-bo BLE Scanner.inoをArduino IDEで開いて下さい。
6. ボード設定のPartition schemeは「**minimal-SPIFFS**」にして下さい。


## 使い方
- Q-boで、ドットコードを読ませます。
- 読み終わると、Q-bo BLE Scannerが、同じ内容を復唱します。
- Cボタンでスリープし、Bボタンで起きます。
 
ボタン

	左のボタン:		ボリュームを下げます。
	中央のボタン:	ボリュームを上げます。
	右のボタン:		スリープ。
	
## 履歴
	ver 1.6: 2019/05/17 Odroid-GO対応, スリープ対応, 音声IDの表示を復活
	ver 1.5: 2019/01/11
	ver 1.4: 2018/12/11
	ver 1.3: 2018/11/04
	ver 1.2: 2018/10/19
	ver 1.1: 2018/ 6/26
	ver 1.0: 2018/ 6/16


## ライセンスと免責
[CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/deed.ja)

- 同梱されている書類すべてに関して、無断転載、掲載、改変を禁じます。
- これらを使ったことにより生じるいかなるトラブルに関しても自己責任においておこなって下さい。
- 作ったばかりなので、まだソースコードがぐちゃぐちゃなので、しばらく、改変禁止とさせて頂きます。リクエスト、アドバイスなどあれば、教えてください。
