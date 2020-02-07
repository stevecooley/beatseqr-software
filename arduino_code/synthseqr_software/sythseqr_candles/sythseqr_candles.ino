#include <jled.h>

JLed candles[22] = {
  JLed(2).Candle().Forever(),
  JLed(3).Candle().Forever(),
  JLed(4).Candle().Forever(),
  JLed(5).Candle().Forever(),
  JLed(7).Candle().Forever(),
  JLed(9).Candle().Forever(),
  JLed(22).Candle().Forever(),
  JLed(24).Candle().Forever(),
  JLed(26).Candle().Forever(),
  JLed(28).Candle().Forever(),
  JLed(30).Candle().Forever(),
  JLed(32).Candle().Forever(),
  JLed(34).Candle().Forever(),
  JLed(36).Candle().Forever(),
  JLed(38).Candle().Forever(),
  JLed(40).Candle().Forever(),
  JLed(42).Candle().Forever(),
  JLed(44).Candle().Forever(),
  JLed(46).Candle().Forever(),
  JLed(48).Candle().Forever(),
  JLed(50).Candle().Forever(),
  JLed(52).Candle().Forever()
};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i <= 21; i++)
  {
    candles[i].Update();
  }
}
