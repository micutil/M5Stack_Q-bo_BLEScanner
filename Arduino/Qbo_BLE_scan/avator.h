#pragma once
#include <M5Stack.h>

class Mouth
{
private:
  int x;
  int y;
  int minWidth;
  int maxWidth;
  int minHeight;
  int maxHeight;
  int lastX;
  int lastY;
  int lastW;
  int lastH;
  float openRatio;
  uint32_t primaryColor;
  uint32_t secondaryColor;
  void clear(void);
  void _draw(int x1, int y1, int x2, int y2);
public:
  // constructor
  Mouth();
  Mouth(int x, int y,
  int minWidth, int maxWidth,
  int minHeight, int maxHeight,
  uint32_t primaryColor, uint32_t secondaryColor);
  void setOpenRatio(float ratio);
  void open(int percent);
  void draw(float breath);
};

class Eye
{
private:
  int x;
  int y;
  int r;
  int offsetX;
  int offsetY;
  int lastX;
  int lastY;
  int lastR;
  bool leftRight;
  float openRatio;
  uint32_t primaryColor;
  uint32_t secondaryColor;
  void clear(void);
  void drawCircle(int x, int y, int r);
  void drawRect(int x, int y, int w, int h);
public:
  // constructor
  Eye();
  Eye(int x, int y, int r, uint32_t primaryColor, uint32_t secondaryColor);
  void setOpenRatio(float ratio);
  void setOffset(int offsetX, int offsetY);
  void open(boolean isOpen);
  void draw(float breath);
  void setDrawLeft(bool lr);
};

class EyeTrpz
{
private:
  int x;
  int y;
  int w1;
  int w2;
  int h;
  int offsetX;
  int offsetY;
  int lastX;
  int lastY;
  int lastH;
  bool leftRight;
  float openRatio;
  uint32_t primaryColor;
  uint32_t secondaryColor;
  void clear(void);
  void drawTrpz(int x, int y, int h);
  void drawRect(int x, int y, int w, int h);
public:
  // constructor
  EyeTrpz();
  EyeTrpz(int x, int y, int w1, int w2, int h, uint32_t primaryColor, uint32_t secondaryColor);
  void setOpenRatio(float ratio);
  void setOffset(int offsetX, int offsetY);
  void open(boolean isOpen);
  void draw(float breath);
  void setDrawLeft(bool lr);
};


class Avator
{
private:
  Mouth mouth;
  EyeTrpz eyeR;
  EyeTrpz eyeL;
  Eye eyeballR;
  Eye eyeballL;
  float breath;
  boolean isEyeOpen;
  float EyeOpenRatio;
  void drawLoop(void);
public:
  // constructor
  Avator(void);

  void openMouth(int percent);
  void openEye(boolean isOpen);
  void setMouthOpen(float f);
  void setEyeOpen(float f);
  void setBreath(float f);
  void setGaze(float vertical, float horizontal);
  void setExpression(void); // TODO
  void init(void);
  void draw(void);
};
