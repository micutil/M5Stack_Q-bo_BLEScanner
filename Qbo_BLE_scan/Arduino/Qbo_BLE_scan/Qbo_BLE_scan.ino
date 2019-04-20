/*******************************************************************
 * Q-bo BLE advertise detector
 * with Q-bo avator
 * for M5Stack
 * 
 * ver 1.5: 2019/01/11
 * ver 1.4: 2018/12/11
 * ver 1.3: 2018/11/04
 * ver 1.2: 2018/10/19
 * ver 1.1: 2018/ 6/26
 * ver 1.0: 2018/ 6/16
 * 
 * CC BY-NC-SA 4.0
 * by Micono Utilities (Michio Ono)
 * 
 *******************************************************************/

/*******************************************************************
 * refer to PlayMP3FromSDToDAC for play mp3 file in microSD
 */
#pragma mark - Depend ESP8266Audio and ESP8266_Spiram libraries
/* 
cd ~/Arduino/libraries
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
*/

/*******************************************************************
 * refer to SampleScan for BLE scan of Q-bo
 * 
   Based on Neil Kolban example for IDF:
   https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

/*******************************************************************
 * refer to M5Stack_PlayMP3_Avator_Text for play mp3 with avotar
 * 
 * https://github.com/robo8080/M5Stack_PlayMP3_Avatar_Test
 * by robo8080
 * 
 * https://github.com/meganetaaan/m5stack-avatar
 * by meganetaaan 
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <M5Stack.h>
#include "M5StackUpdater.h"
#include "Free_Fonts.h"
#include "avator.h"
#include "fsutil.h"

#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#include <WiFi.h>

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *afile;
AudioOutputI2S *aout;
AudioFileSourceID3 *id3;

//#define useSPIFFS
#ifdef useSPIFFS
  #include <SPIFFS.h>
  fs::SPIFFSFS qbFS=SPIFFS;
#else
  fs::SDFS qbFS=SD;
#endif

Avator *avator;
int   qbompin=35;     //Mouth LED pin number
float vMax=4096.0;    //Max analog value of mouth LED voltage
float spkvol = 15.0;  //Initial speaker volue level
bool voicePlay=false; //is playing
int   ypos;           //Information display Y position

String qbInfoDir="/QboScanInfo";
String qboadd="000000000000";
String qbocnt="0000";
String qbocod="0000";
bool hasQboData=false;

/*
 * 音声再生
 */
void playMp3(String mp3fpn)
{
  afile = new AudioFileSourceSD(mp3fpn.c_str());
  Serial.print("afile");
  if(afile) {
    id3 = new AudioFileSourceID3(afile);
    Serial.print("id3");
    if(id3) {
      /*
      aout = new AudioOutputI2S(0, 1); // Output to builtInDAC
      aout->SetGain(spkvol/100.0);
      aout->SetOutputModeMono(true);
      mp3 = new AudioGeneratorMP3();
      */
      voicePlay=mp3->begin(id3, aout);
      Serial.println((int)voicePlay);
    }
  }
}

/*
 * 再生音声ファイルID番号
 */
void playQboSoundNum(int id) {
  String fp="/GMVRC/D231300525000/DS000.MP3";
  char iddecs[6],idfns[14];
  int fdridx;

  //File name
  sprintf(iddecs,"DS%03d",id&0x7F);
  fp.replace("DS000", iddecs);
  Serial.print("File name="); Serial.println(iddecs);
  
  //Folder name
  fdridx=444+((id&0xF80)>>7);
  sprintf(idfns,"D231300525%03d",fdridx);
  fp.replace("D231300525000", idfns);
  
  Serial.print("Mp3 Path="); Serial.println(fp);
/*
  //描画するとハングアップしやすくなるので描画しない
  ypos=230-M5.Lcd.fontHeight(GFXFF);
  //M5.Lcd.drawString("INFO:", 10, ypos, GFXFF);
  int reqcnt=strtol(qbocnt.c_str(), NULL, 16);//数値化
  M5.Lcd.drawString("               ", 100, ypos, GFXFF);
  M5.Lcd.drawString(String(reqcnt), 100, ypos, GFXFF);
  M5.Lcd.drawString(String(fdridx), 150, ypos, GFXFF);
  M5.Lcd.drawString(iddecs,         200, ypos, GFXFF);

  delay(50);
*/
  playMp3(fp);
}

/*
 * 再生ファイルID
 */
void playQboSound(String ids) {
  int id=strtol(ids.c_str(), NULL, 16)-1;
  Serial.print("ids="); Serial.println(ids);
  Serial.print("id="); Serial.println(id);
  playQboSoundNum(id);
}

/*
 * 再生が必要かどうか
 */
void playNeed()
{
  if(hasQboData==true) {
    playQboSound(qbocod);
    hasQboData=false;
  }
}

/*
 * 再生を停止する
 */
void stopPlaying()
{
  if(mp3) {
    mp3->stop();
  }
  if(id3) {
    id3->close();
    delete id3;
    id3 = NULL;
  }
  if(afile) {
    afile->close();
    delete afile;
    afile = NULL;
  }
  Serial.println("MP3 done");
  delay(10);
}

/************************************
 * 
 * BLE関連の処理
 * 
 ************************************/

BLEScan* pBLEScan;
BLEScanResults foundDevices;
int scanTime = 1; //In seconds
String adv="Advertised Device: Name: , Address: f8:8a:3c:49:0b:81, manufacturer data: 20000101e9 ";
//String lastadv="0000000000";

//BLEAddress qboaddr1("f8:8a:3c:49:0b:81");
//BLEAddress qboaddr2("f8:8a:3c:49:33:77");

//BLE Advertise クラス
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

/*
 * BLE Advertise リザルト 
 */
void MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
  //ヴォリューム0なら戻る
  if(spkvol<=0) return;

  //再生中なら戻る
  if(mp3->isRunning()) return;
  
  //セットされたデータが未再生なら戻る
  if(hasQboData==true) return;
  
  BLEAddress addr = advertisedDevice.getAddress();
  int n = advertisedDevice.toString().length();
  if(n<90&&advertisedDevice.toString().substr(n-10,2).compare("20")==0)//20...がQ-boのマニュファクチャ
  {
    qboadd=String(addr.toString().c_str());                   //アドレス取得
    qboadd.replace(":","");                                   //ファイル名にするためコロンを削除
    
    qbocnt=advertisedDevice.toString().substr(n-8,4).c_str(); //音声カウンター
    //reqcnt=strtol(qbocnt.c_str(), NULL, 16);                  //数値化
    
    qbocod=advertisedDevice.toString().substr(n-4).c_str();   //音声ファイルコード番号

    //保存してあるデータを調べる
    if(checkSpeachData(qboadd,qbocnt)) return;

    Serial.print("Addr.="); Serial.println(addr.toString().c_str());
    Serial.print("Count="); Serial.println(qbocnt.c_str());
    Serial.print("Code=");  Serial.println(qbocod.c_str());   
    //Serial.print("Last Adv.="); Serial.println(lastadv);
  }
}

bool checkSpeachData(String addr, String nwcnt) {
  char svcnt[]="0000";
  String qboinfofile=qbInfoDir+"/"+qboadd+".txt"; //ファイル名作成
  readFile(qbFS,(const char*)qboinfofile.c_str(),svcnt);
  int cnt=strtol(nwcnt.c_str(), NULL, 16);
  int svn=strtol(svcnt, NULL, 16);
  //0,もしsvn==cntなら再生しない
  if(cnt==svn) return false;
  //1,もしsvn=0なら再生
  //2,もしcnt>svnなら再生
  writeFile(qbFS,(const char*)qboinfofile.c_str(),nwcnt.c_str());
  hasQboData=true;
  return true;
}

/*
 * BLE スキャン開始
 */
void scanStart() {
  //最初にsetupで実行済み
  //pBLEScan = BLEDevice::getScan(); //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster

  foundDevices = pBLEScan->start(scanTime);
  //Serial.print("Devices found: "); Serial.println(foundDevices.getCount());Serial.println("");
}

/*******************************************
 * 
 * アバター関連の処理
 * 
 ********************************************/

float breathAmp=1.5;

void volumeChanged() {
  ypos=230-M5.Lcd.fontHeight(GFXFF);
  M5.Lcd.drawString("            ", 100, ypos, GFXFF);
  M5.Lcd.drawString(String(spkvol), 100, ypos, GFXFF);
  playMp3("/GMVRC/D231300525452/DS006.MP3");
}

void breath(void *args)
{
  int c = 0;
  for(;;)
  {
    c = c + 1 % 100;
    float f = breathAmp * sin(c * 2 * PI / 100.0);
    avator->setBreath(f);
    vTaskDelay(50);//33
  }
}

void drawLoop(void *args)
{
  float level;
  for(;;)
  {
    if(mp3->isRunning()) {
      if(aout) {
        level = aout->getLevel();
        level = abs(level) * 6.0;
        if(level > 20000) level = 20000;
        level = (float)level/20000.0;
        avator->setMouthOpen(level);
      }
    } else {
      level = analogRead(qbompin);
      //Serial.println(v);
      if(level>0) {
        level=1.5*random(level);
        if(level>vMax) level=vMax;
        avator->setMouthOpen(level/vMax);
      } else {      
        avator->setMouthOpen(0.0);
      }
    }
    avator->draw();
    vTaskDelay(40);//(33);//
  }
}

void saccade(void *args)
{
  for(;;)
  {
    float vertical = (float)rand()/(float)(RAND_MAX / 2.0) - 1;
    float horizontal = (float)rand()/(float)(RAND_MAX / 2.0) - 1;
    avator->setGaze(vertical, horizontal);
    vTaskDelay(500 + 100 * random(20));
  }
}

void blinking(void *args)
{
  for(;;)
  {
    avator->setEyeOpen(1);
    vTaskDelay(2500 + 100 * random(20));
    avator->setEyeOpen(0);
    vTaskDelay(300 + 10 * random(20));
  }
}

void startAvatar()
{
  avator = new Avator();
  //xTaskCreatePinnedToCore
  xTaskCreate(
              drawLoop,     /* Function to implement the task */
              "drawLoop",   /* Name of the task */
              4096,      /* Stack size in words */
              NULL,      /* Task input parameter */
              1,         /* Priority of the task */
              NULL);//,      /* Task handle. */
              //1);        /* Core where the task should run */
  xTaskCreate(
              saccade,     /* Function to implement the task */
              "saccade",   /* Name of the task */
              4096,      /* Stack size in words */
              NULL,      /* Task input parameter */
              4,         /* Priority of the task */
              NULL);//,      /* Task handle. */
              //1);        /* Core where the task should run */
  xTaskCreate(
              breath,     /* Function to implement the task */
              "breath",   /* Name of the task */
              4096,      /* Stack size in words */
              NULL,      /* Task input parameter */
              2,         /* Priority of the task */
              NULL);//,      /* Task handle. */
              //1);        /* Core where the task should run */
  xTaskCreate(
              blinking,     /* Function to implement the task */
              "blink",   /* Name of the task */
              4096,      /* Stack size in words */
              NULL,      /* Task input parameter */
              3,         /* Priority of the task */
              NULL);//,      /* Task handle. */
              //1);        /* Core where the task should run */
}


/****************************************
 * 
 * セットアップ 
 * 
 ****************************************/

void setup() {
  M5.begin();
  //Wire.begin();

  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  
  WiFi.disconnect(true);
   
  M5.Lcd.setBrightness(30);
  //WiFi.mode(WIFI_OFF); 
  delay(500);

  pinMode(qbompin, INPUT);
//  pinMode(5, INPUT);
  
  //pinMode(25, OPEN_DRAIN);
  //i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
  //i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN);
  
  M5.Lcd.setFreeFont(FSB18);
  M5.Lcd.drawString("Q-bo BLE Scanner",20, 100, GFXFF); 
  M5.Lcd.setFreeFont(FM9);
  M5.Lcd.setTextColor(TFT_WHITE, 0x00DB88);
  delay(1500);

  Serial.begin(115200);

  //Start SPIFFS or microSD
#ifdef useSPIFFS
  if(!qbFS.begin()) {
#else
  if(!SD.begin()) {
#endif
    Serial.println("File IO failed...");
    return;
  }

  //Q-bo BLE情報保存ディレクトリ作成
  createDir(qbFS,qbInfoDir.c_str());
  
  //Q-bo 顔画像 表示
  M5.Lcd.drawJpgFile(SD, "/Qboface.jpg");
  
  Serial.println("BLE Scanning...");
  //最初に一度だけ行う
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  //scanStart();
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");

  aout = new AudioOutputI2S(0, 1); // Output to builtInDAC
  aout->SetGain(spkvol/100.0);
  aout->SetOutputModeMono(true);
  mp3 = new AudioGeneratorMP3();

  startAvatar();
  //playMp3("/GMVRC/D231300525448/DS041.MP3");
  playMp3("/Message/Start_sound.mp3");
  
  delay(1500);
}

/****************************************
 * 
 * ループ
 * 
 ****************************************/

void loop() {
  // put your main code here, to run repeatedly:
  M5.update();
  
  //A button
  if(M5.BtnA.wasPressed()) {
    spkvol=spkvol-15.0;
    if(spkvol<0) spkvol=0;
    aout->SetGain(spkvol/100.0);
    volumeChanged();
  }

  //B button
  if(M5.BtnB.wasPressed()) {
    spkvol=spkvol+15.0;
    if(spkvol>100) spkvol=100.0;
    aout->SetGain(spkvol/100.0);
    volumeChanged();
  }

  //C button
  if(M5.BtnC.wasPressed()) {
    M5.powerOFF();
  }


  //プレー中か？
  if(mp3->isRunning()) {
    aout->begin();
    aout->SetGain(spkvol/100.0);
    while(mp3->isRunning()){
     if (!mp3->loop()) mp3->stop();
     delay(1);
    }
    delay(500);
    return;
     
  } else {
    
    voicePlay=false;
    aout->SetGain(0);
    aout->stop();

    //プレーするか？
    playNeed();

    //再生されなかったならスキャンスタート
    //mp3->isRunning()==falseならば
    //if(!mp3->isRunning()) scanStart();
    if(hasQboData==false) scanStart();

  }

  delay(5);
}

/******************************************************/
