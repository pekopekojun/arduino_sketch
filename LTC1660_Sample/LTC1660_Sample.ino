#include <SPI.h>

static String SerialMsg;
static uint16_t code[8] = {127, 255, 383, 511, 639, 767, 895, 1023};
static uint8_t code_idx;
static uint8_t state;
static uint16_t vref = 5000;

#define STATE_IDLE (0)
#define STATE_CH_SELECT (1)

const String CH_NAME[] = {"A", "B", "C", "D", "E", "F", "G", "H"};

void list_and_set_code(void)
{
  Serial.println("---- DAC LIST ----");
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("CH:");
    Serial.print(i + 1);
    Serial.print("[");
    Serial.print(CH_NAME[i]);
    Serial.print("],");

    Serial.print("CODE:");
    Serial.print(code[i]);
    Serial.print("(");
    Serial.print(((uint32_t)vref * (uint32_t)code[i]) / 1024UL);
    Serial.println("[mv])");

    setDAC(i + 1, code[i]);
  }
  Serial.println("---- DAC LIST END ----");
}

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
  Serial.begin(115200);
  Serial.setTimeout(3000);

  Serial.println("Startup");
  Serial.print("VREF:");
  Serial.print(vref);
  Serial.println("[mV]");

  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  state = STATE_IDLE;
  list_and_set_code();
}

void loop()
{
  uint16_t i;
  if (Serial.available())
  {
    SerialMsg = Serial.readStringUntil('\n');

    i = SerialMsg.toInt();
    if (state == STATE_IDLE)
    {
      if (i > 0 && i <= 8)
      {
        code_idx = i - 1;
        state = STATE_CH_SELECT;
        Serial.print("Selected Channel:");
        Serial.print(SerialMsg);
        Serial.print("[");
        Serial.print(CH_NAME[code_idx]);
        Serial.println("]");

        Serial.print("Set Code:");
      }
      else
      {
        Serial.println("Syntax Error");
      }
    }
    else
    {
      if (i > 1023)
      {
        i = 0;
      }
      code[code_idx] = i;
      state = STATE_IDLE;
      //Serial.println("");
      Serial.print(i);
      Serial.print("(");
      /* 0 to (1023/1024)Vref */
      Serial.print(((uint32_t)vref * (uint32_t)i) / 1024UL);
      Serial.println("[mv])");
      Serial.println("OK!");

      list_and_set_code();
    }
  }
}
