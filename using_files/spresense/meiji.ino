#include <Audio.h>
#include <Camera.h>
#include <Adafruit_ILI9341.h>
#include <DNNRT.h>
#include <SDHCI.h>
#define TFT_DC  9
#define TFT_CS  10
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define OFFSET_X  (54)
#define OFFSET_Y  (0)
#define CLIP_WIDTH (224)
#define CLIP_HEIGHT (224)
#define DNN_WIDTH  (28)
#define DNN_HEIGHT  (28)


AudioClass *theAudio;

SDClass SD;
DNNRT dnnrt;

DNNVariable input(DNN_WIDTH*DNN_HEIGHT);
const char label[8] = {'0','1','2','3','4','5','6','7'};

void CamCB(CamImage img) {

  if (!img.isAvailable()) {
    Serial.println("Image is not available. Try again");
    return;
  }

  // カメラ画像の切り抜きと縮小
  CamImage small;
  CamErr err = img.clipAndResizeImageByHW(small
                     , OFFSET_X, OFFSET_Y
                     , OFFSET_X + CLIP_WIDTH -1
                     , OFFSET_Y + CLIP_HEIGHT -1
                     , DNN_WIDTH, DNN_HEIGHT);
  if (!small.isAvailable()){
    putStringOnLcd("Clip and Reize Error:" + String(err), ILI9341_RED);
    return;
  }

  // 認識用モノクロ画像を設定
  uint16_t* imgbuf = (uint16_t*)small.getImgBuff();
  float *dnnbuf = input.data();
  for (int n = 0; n < DNN_HEIGHT*DNN_WIDTH; ++n) {
    dnnbuf[n] = (float)(((imgbuf[n] & 0xf000) >> 8) 
                      | ((imgbuf[n] & 0x00f0) >> 4))/255.;
  }
  // 推論の実行
  dnnrt.inputVariable(input, 0);
  dnnrt.forward();
  DNNVariable output = dnnrt.outputVariable(0);
  int index = output.maxIndex();

  // 推論結果の表示
  //　音の長さを調節したいときは下のsound_timeの数字を調節してください
  int sound_time = 500;
  String gStrResult;
  if (output[index] > 0.7) {
    if (index < 7) {
      gStrResult = String(label[index]) + String(":") + String(output[index]);
        switch(index){
        case 0:
          theAudio->setBeep(1,-40,262);
          usleep(sound_time * 1000);
          break;
        case 1:
          theAudio->setBeep(1,-40,294);
          usleep(sound_time * 1000);
          break;
        case 2:
          theAudio->setBeep(1,-40,330);
          usleep(sound_time * 1000);
          break;
        case 3:
          theAudio->setBeep(1,-40,349);
          usleep(sound_time * 1000);
          break;
        case 4:
          theAudio->setBeep(1,-40,392);
          usleep(sound_time * 1000);
          break;
        case 5:
          theAudio->setBeep(1,-40,440);
          usleep(sound_time * 1000);
          break;
        case 6:
          theAudio->setBeep(1,-40,494);
          usleep(sound_time * 1000);
          break;
        case 7:
          theAudio->setBeep(1,-40,523);
          usleep(sound_time * 1000);
          break;
        }
    } else {
      gStrResult = String("Error");  
      theAudio->setBeep(1,-90,0);
      usleep(sound_time * 1000);
    }
  } else {
    gStrResult = String("Not Sure"); 
    theAudio->setBeep(1,-90,0);
    usleep(sound_time * 1000);
  }


  Serial.println(gStrResult);

  // 推論結果のディスプレイ表示
  img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
  uint16_t* imgBuf = (uint16_t*)img.getImgBuff(); 
  drawBox(imgBuf, OFFSET_X, OFFSET_Y, CLIP_WIDTH, CLIP_HEIGHT, 5, ILI9341_RED); 
  display.drawRGBBitmap(0, 0, (uint16_t*)imgBuf, 320, 224);
  putStringOnLcd(gStrResult, ILI9341_YELLOW);
}

void setup() {
  // 音のセットアップ
  theAudio = AudioClass::getInstance();
  theAudio->begin();
  puts("initialization Audio Library");

  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, 0, 0);
  
  Serial.begin(115200);
  // SDカードの挿入待ち
  while (!SD.begin()) { 
    putStringOnLcd("Insert SD card", ILI9341_RED); 
    Serial.println("Insert SD card");
  }
  // SDカードにある学習済モデルの読み込み
  Serial.println("Lording"); 
  File nnbfile = SD.open("model.nnb");
  Serial.println("Lording Done"); 
  // 学習済モデルでDNNRTを開始
  int ret = dnnrt.begin(nnbfile);
  if (ret < 0) {
    putStringOnLcd("dnnrt.begin failed" + String(ret), ILI9341_RED);
    Serial.println("dnnrt.begin failed" + String(ret)); 
    return;
  }

  display.begin();
  display.setRotation(3);
  theCamera.begin();
  theCamera.startStreaming(true, CamCB);

}

void loop() {
  // put your main code here, to run repeatedly:

}
