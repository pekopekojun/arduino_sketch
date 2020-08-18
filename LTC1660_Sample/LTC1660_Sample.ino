#include <SPI.h>

boolean isFirst = true;

/*
 * Pin10:SS   [->]
 * Pin11:MOSI [->]
 * Pin12:MISO [<-]
 * Pin13:SCK  [->]
 */
void setDAC(int address, int value)
{
  int h = address << 4 | value >> 6;
  int l = value << 2;
  digitalWrite(SS, LOW);
  SPI.transfer(h);
  SPI.transfer(l);
  digitalWrite(SS, HIGH);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(SS, OUTPUT); //Pin10
  digitalWrite(SS, HIGH);

  //
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (isFirst)
  {
    isFirst = false;
    setDAC(1, 0x3ff);
    setDAC(2, 0x3ff / 2);
    setDAC(3, 0x3ff / 3);
  }
}
