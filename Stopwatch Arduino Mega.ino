#include <SPI.h>
#include <SD.h>
#include <dht.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>

File dataLog;
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
RTC_DS1307 RTC;
dht DHT;

int value, value2,
    valueHari, valueBulan, valueTahun, valueJam1, valueMenit1, valueDetik1, valueJamAkhir, valueMenitAkhir, valueDetikAkhir, valueDowntime, valueMenitTotal,
    temp,
    inc,
    hours1,
    minut,
    addR = 11, addR2 = 12, addRAlarm = 13,
    addRHari = 15, addRBulan = 16, addRTahun = 17, addRJam1 = 18, addRMenit1 = 19, addRJamAkhir = 20, addRMenitAkhir = 21, addRDetikAkhir = 22, addRDetik1 = 23, totalDowntime = 10, addRValueSecondTotal = 24, addRValueMenitTotal = 25,
    mode = 0;
int tSeconds = 0, tMinutes = 0, hours = 0;
int centiseconds = 0, sSeconds = 0, sMinutes = 0;
int next = A1;
int INC = A0;
int set_mad = A2;
int shw_dat = A3;
int ledRed = 3;
int ledGreen = 2;
int DHT11_PIN = 22;
int HOUR, MINUT, SECOND = 0;
int tempe = 0, humi = 0;
int countValue = 0;
int countSecondValue = 0;
int countMinuteValue = 0;
int valueJamTotal = 0;
int valueDetikTotal = 0;
int valueSecondTotal = 0;
char filename[12] = "00000000.CSV";


void setup() {
  Serial.begin(9600);
  Serial.println("Initializing SD Card...");
  Wire.begin();
  RTC.begin();
  //   lcd.init();
  lcd.clear();
  //  lcd.backlight();
  lcd.begin(20, 4);
  pinMode(INC, INPUT);
  pinMode(next, INPUT);
  pinMode(set_mad, INPUT);
  pinMode(shw_dat, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  digitalWrite(next, HIGH);
  digitalWrite(set_mad, HIGH);
  digitalWrite(INC, HIGH);
  digitalWrite(shw_dat, HIGH);
  if (!RTC.isrunning()) {
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    digitalWrite(ledRed, HIGH);
    delay(1000);
    digitalWrite(ledRed, LOW);
  }
  else {
    digitalWrite(ledGreen, HIGH);
    delay(1000);
    digitalWrite(ledGreen, LOW);
    Serial.println("initialization done.");
  }
}

void loop() {
  Serial.println("MASUK LOOP");
  int temp = 0, val = 1, temp4;
  DHT.read11(DHT11_PIN);
  humi = DHT.humidity;
  tempe = DHT.temperature;
  if (digitalRead(shw_dat) == 0)
  {
    mode++;
  }
  // --------------------------------- Mode Stopwatch dan EEPROM ----------------------------------------------------------
  if (digitalRead(shw_dat) == 0 && mode == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" LOSSTIME   COUNTER     ");
    lcd.setCursor(2, 1);
    lcd.print("START");
    //                Reset Hitungan Ke START ke 00:00:00
    lcd.setCursor(0, 2);
    lcd.print("00");
    lcd.print(":");
    lcd.print("00");
    lcd.print(":");
    lcd.print("00");
    // Reset Hitungan Ke STOP ke 00:00:00
    lcd.setCursor(12, 2);
    lcd.print("00");
    lcd.print(":");
    lcd.print("00");
    lcd.print(":");
    lcd.print("00");
    lcd.setCursor(14, 1);
    lcd.print("STOP");
    lcd.setCursor(6, 3);
    lcd.print("00:00:00");
    while (1) {
      if (digitalRead(next) == 0) {
        stopwatchFunction();
      }
      if (digitalRead(set_mad) == 0)
        break;
    }
    lcd.clear();
  }
  //  --------------------------------- Mode Stopwatch dan EEPROM ----------------------------------------------------------
  // --------------------------------- Mode Total Downtime dan EEPROM ----------------------------------------------------------
  if (digitalRead(shw_dat) == 0 && mode == 2) {   //set Stopwatch
    lcd.clear();
    digitalWrite(ledRed, LOW);

    lcd.setCursor(0, 0);
    lcd.print("   TOTAL LOSSTIME  ");
    valueHari = EEPROM.read(addRHari);
    valueBulan = EEPROM.read(addRBulan);
    valueTahun = EEPROM.read(addRTahun);
    lcd.setCursor(5, 1);
    printDigits(valueHari);
    lcd.print("/");
    printDigits(valueBulan);
    lcd.print("/");
    lcd.print("2021");
    lcd.setCursor(0, 2);
    lcd.print(" JUMLAH  |  DURASI  ");
    lcd.setCursor(5, 3);
    lcd.print(countValue);
    if (countValue < 9) {
      lcd.print("   |   ");
    } else {
      lcd.print("  |   ");
    }
    lcd.print(valueSecondTotal);
    lcd.print(" s");
    while (1) {
      if (digitalRead(set_mad) == 0)
        break;
    }
    lcd.clear();
  }
  //  --------------------------------- Mode Count Button dan EEPROM ----------------------------------------------------------

  if (mode > 2) {
    mode = 0;
  }
  //  lcd.clear();
  DateTime now = RTC.now();
  lcd.setCursor(4, 0);
  lcd.print("PT PANASONIC");
  lcd.setCursor(0, 1);
  lcd.print("MANUFAKTUR INDONESIA");
  lcd.setCursor(1, 2);
  printDigits(now.hour());
  lcd.print(":");
  printDigits(now.minute());
  lcd.print(":");
  printDigits(now.second());
  lcd.setCursor(0, 3);
  printDigits(now.day());
  lcd.print("/");
  printDigits(now.month());
  lcd.print("/");
  lcd.print(now.year());
  lcd.setCursor(14, 2);
  lcd.print("T:");
  lcd.print(tempe);
  lcd.print((char)223);
  lcd.setCursor(19, 2);
  lcd.print("C");
  lcd.setCursor(14, 3);
  lcd.print("H:");
  lcd.print(humi);
  lcd.print("%");
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  //  delay(100);
  //  lcd.clear();
}
//------------------------------------------------------ VOID LOOP END ------------------------------------------------------------------

void printDigits(int digits) { //this void function is really useful; it adds a "0" to the //beginning of the number, so that 5 minutes is displayed as "00:05:00", rather than "00:5 :00"
  if (digits < 10) {
    lcd.print("0");
    lcd.print(digits);
  }
  else {
    lcd.print(digits);
  }
}

void lcdOutput() {
  lcd.setCursor(6, 2);
  printDigits(hours);
  lcd.setCursor(9, 2);
  printDigits(tMinutes);
  lcd.setCursor(12, 2);
  printDigits(tSeconds);
  //  delay(100);
}

void stopwatchFunction() {
  int count = 1, sMin, sSec, sCen, RTCHari, RTCBulan, RTCTahun, RTCJam, RTCMenit, RTCDetik, RTCJamAkhir, RTCMenitAkhir, RTCDetikAkhir;
  digitalWrite(ledGreen, LOW);
  Serial.println("1");
  while (1) {
    if (digitalRead(shw_dat) == LOW ) {
      Serial.println("2");
      Serial.println("KELUAR");
      count = 0;
      while (1) {
        loop();
      }
    }
    //if the "Start/Stop" button is pressed, the time begins running
    if (digitalRead(next) == LOW) {
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledRed, LOW);
      Serial.println("MASUK 3");
      DateTime now = RTC.now();
      //    Waktu
      RTCHari = now.day();
      RTCBulan = now.month();
      RTCTahun = now.year();
      RTCJam = now.hour();
      RTCMenit = now.minute();
      RTCDetik = now.second();
      lcd.setCursor(0, 2);
      printDigits(RTCJam);
      lcd.print(":");
      printDigits(RTCMenit);
      lcd.print(":");
      printDigits(RTCDetik);
      EEPROM.write(addRJam1, RTCJam);
      EEPROM.write(addRMenit1, RTCMenit);
      EEPROM.write(addRDetik1, RTCDetik);
      EEPROM.write(addRHari, RTCHari);
      EEPROM.write(addRBulan, RTCBulan);
      EEPROM.write(addRTahun, RTCTahun);
      lcd.setCursor(15, 3);
      lcd.print("    ");
      //      digitalWrite(ledRed, HIGH);
      while (1) {
        int count = 1;
        delay(7);
        lcd.setCursor(6, 3);
        printDigits(sMinutes);
        lcd.setCursor(9, 3);
        printDigits(sSeconds);
        lcd.setCursor(12, 3);
        printDigits(centiseconds);
        centiseconds++;
        sCen = centiseconds;
        if (centiseconds == 100) {
          sSeconds++;
          sSec = sSeconds;
          sMin = sMinutes;
          EEPROM.write(addR, sSec);
          EEPROM.write(addR2, sMin);
          centiseconds = 0;
          if (sSeconds == 60) {
            sMinutes++;
            sMin = sMinutes;
            EEPROM.update(addR2, sMin);
            sSeconds = 0;
          }
        }
        if (digitalRead(set_mad) == 0) {
          Serial.println("MASUK 4");
          digitalWrite(ledRed, LOW);
          delay(200);
          digitalWrite(ledRed, HIGH);
          delay(200);
          digitalWrite(ledRed, LOW);
          delay(200);
          digitalWrite(ledRed, HIGH);
          delay(200);
          digitalWrite(ledRed, HIGH);
          break;
        }
        if (digitalRead(INC) == LOW && count == 1) {
          Serial.println("MASUK 5");
          digitalWrite(ledRed, HIGH);
          digitalWrite(ledGreen, LOW);
          DateTime now = RTC.now();
          RTCJamAkhir = now.hour();
          RTCMenitAkhir = now.minute();
          RTCDetikAkhir = now.second();
          EEPROM.write(addRJamAkhir, RTCJamAkhir);
          EEPROM.write(addRMenitAkhir, RTCMenitAkhir);
          EEPROM.write(addRDetikAkhir, RTCDetikAkhir);
          valueJamAkhir = EEPROM.read(addRJamAkhir);
          valueMenitAkhir = EEPROM.read(addRMenitAkhir);
          valueDetikAkhir = EEPROM.read(addRDetikAkhir);
          value = EEPROM.read(addR);
          value2 = EEPROM.read(addR2);
          valueJam1 = EEPROM.read(addRJam1);
          //          -------------------------------------------
          countMinuteValue += sMin * 60;
          countSecondValue += sSec;
          valueSecondTotal = countSecondValue + countMinuteValue;
          Serial.print("valueSecondTotal ");
          Serial.println(valueSecondTotal);
          //          EEPROM.write(addRValueSecondTotal, valueDetikTotal);
          //          -------------------------------------------
          valueMenit1 = EEPROM.read(addRMenit1);
          valueDetik1 = EEPROM.read(addRDetik1);
          valueHari = EEPROM.read(addRHari);
          valueBulan = EEPROM.read(addRBulan);
          valueTahun = EEPROM.read(addRTahun);
          //          digitalWrite(ledRed, LOW);
          //      Kalo yang dicatat hanya detiknya
          //            lcd.setCursor(16, 3);
          //            lcd.print("    ");
          if (value2 == 0) {
            //              Serial.println("MASUK DETIK");
            //            if (sd_ok) {
            getFileName();
            dataLog = SD.open(filename, FILE_WRITE);
            if (dataLog) {
              //                dataLog.println("   TANGGAL   |  START  |   STOP   |  DOWNTIME  |  SUHU  ");
              dataLog.print("  ");
              dataLog.print(valueHari);
              dataLog.print("/");
              dataLog.print(valueBulan);
              dataLog.print("/");
              dataLog.print(2021);
              dataLog.print("  |  ");
              if (valueJam1 < 10) {
                //                        Serial.println("JAM < 10");
                dataLog.print(0);
                dataLog.print(valueJam1);
                dataLog.print(":");
                if (valueMenit1 < 10) {
                  //                          Serial.println("MENIT < 10");
                  dataLog.print(0);
                  dataLog.print(valueMenit1);
                } else {
                  dataLog.print(valueMenit1);
                }
                dataLog.print(":");
                if (valueDetik1 < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetik1);
                  //                  Serial.println("Detik1 < 10 Saved");
                } else {
                  dataLog.print(valueDetik1);
                  //                  Serial.println("Detik1 > 10 Saved");
                }
              } else {
                //                        Serial.println("JAM > 10");
                dataLog.print(valueJam1);
                dataLog.print(":");
                if (valueMenit1 < 10) {
                  //                          Serial.println("MENIT < 10");
                  dataLog.print(0);
                  dataLog.print(valueMenit1);
                } else {
                  dataLog.print(valueMenit1);
                }
                dataLog.print(":");
                if (valueDetik1 < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetik1);
                  //                  Serial.println("Detik1 < 10 Saved");
                } else {
                  dataLog.print(valueDetik1);
                  //                  Serial.println("Detik1 > 10 Saved");
                }
              }
              dataLog.print("  |  ");
              if (valueJamAkhir < 10) {
                //                Serial.println("JAM AKHIR");
                dataLog.print(0);
                dataLog.print(valueJamAkhir);
                dataLog.print(":");
                if (valueMenitAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueMenitAkhir);
                } else {
                  dataLog.print(valueMenitAkhir);
                }
                dataLog.print(":");
                if (valueDetikAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir < 10 Saved");
                } else {
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir > 10 Saved");
                }
              } else {
                dataLog.print(valueJamAkhir);
                dataLog.print(":");
                if (valueMenitAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueMenitAkhir);
                } else {
                  dataLog.print(valueMenitAkhir);
                }
                dataLog.print(":");
                if (valueDetikAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir < 10 Saved");
                } else {
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir > 10 Saved");
                }
              }
              dataLog.print("   |            ");
              //                      Data yang tersimpan dalam bentuk menit dan detik
              //                      dataLog.print(0);
              //                      dataLog.print(" m ");
              dataLog.print(value);
              dataLog.print(" s");
              //                      Data yang tersimpan dalam bentuk detik
              dataLog.print("       |  ");
              dataLog.print(tempe);
              dataLog.print("C  |  ");
              dataLog.print(valueSecondTotal);
              dataLog.print(" s");
              dataLog.println(" ");
              digitalWrite(ledRed, HIGH);
              delay(200);
              digitalWrite(ledRed, LOW);
              delay(200);
              digitalWrite(ledRed, HIGH);
              delay(200);
              digitalWrite(ledRed, LOW);
              delay(200);
              digitalWrite(ledRed, HIGH);
              delay(200);
              digitalWrite(ledRed, HIGH);
              digitalWrite(ledGreen , LOW);
              Serial.println("SAVED TO SD CARD");
              dataLog.close();
            }
            //            }
          }
          if (value2 > 0) {
            Serial.println("MASUK != 0");
            //            if (sd_ok) {
            getFileName();
            dataLog = SD.open(filename, FILE_WRITE);
            if (dataLog) {
              //                dataLog.println("   TANGGAL   |  START  |   STOP   |  DOWNTIME  |  SUHU  ");
              dataLog.print("  ");
              dataLog.print(valueHari);
              dataLog.print("/");
              dataLog.print(valueBulan);
              dataLog.print("/");
              dataLog.print(2021);
              dataLog.print("  |  ");
              if (valueJam1 == 0) {
                //Serial.println("MASUK ATAS");
                dataLog.print(0);
                dataLog.print(valueJam1);
                dataLog.print(":");
                if (valueMenit1 < 10) {
                  dataLog.print(0);
                  dataLog.print(valueMenit1);
                } else {
                  dataLog.print(valueMenit1);
                }
                dataLog.print(":");
                if (valueDetik1 < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetik1);
                  //                  Serial.println("DetikAkhir < 10 Saved");
                } else {
                  dataLog.print(valueDetik1);
                  //                  Serial.println("DetikAkhir > 10 Saved");
                }
              } else {
                // Serial.println("MASUK BAWAH");
                dataLog.print(valueJam1);
                dataLog.print(":");
                if (valueMenit1 < 10) {
                  //Serial.println("NESTED IF CHECKED");
                  dataLog.print(0);
                  dataLog.print(valueMenit1);
                } else {
                  dataLog.print(valueMenit1);
                }
                dataLog.print(":");
                if (valueDetik1 < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetik1);
                  //                  Serial.println("DetikAkhir < 10 Saved");
                } else {
                  dataLog.print(valueDetik1);
                  //                  Serial.println("DetikAkhir > 10 Saved");
                }
              }
              dataLog.print("  |   ");
              if (valueJamAkhir < 10) {
                //                        Serial.println("MASUK JUGA CUY");
                dataLog.print(0);
                dataLog.print(valueJamAkhir);
                dataLog.print(":");
                if (valueMenitAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueMenitAkhir);
                } else {
                  dataLog.print(valueMenitAkhir);
                }
                dataLog.print(":");
                if (valueDetikAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir < 10 Saved");
                } else {
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir > 10 Saved");
                }
              } else {
                dataLog.print(valueJamAkhir);
                dataLog.print(":");
                if (valueMenitAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueMenitAkhir);
                } else {
                  dataLog.print(valueMenitAkhir);
                }
                dataLog.print(":");
                if (valueDetikAkhir < 10) {
                  dataLog.print(0);
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir < 10 Saved");
                } else {
                  dataLog.print(valueDetikAkhir);
                  //                  Serial.println("DetikAkhir > 10 Saved");
                }
              }
              dataLog.print("   |            ");
              dataLog.print(valueSecondTotal);
              dataLog.print(" s");
              dataLog.print("       |  ");
              dataLog.print(tempe);
              dataLog.print("C  |  ");
              dataLog.print(valueSecondTotal);
              dataLog.print(" s");
              dataLog.println(" ");
              digitalWrite(ledRed, HIGH);
              delay(200);
              digitalWrite(ledRed, LOW);
              delay(200);
              digitalWrite(ledRed, HIGH);
              delay(200);
              digitalWrite(ledRed, LOW);
              delay(200);
              digitalWrite(ledRed, HIGH);
              delay(200);
              digitalWrite(ledRed, HIGH);
              digitalWrite(ledGreen , LOW);
              Serial.println("SAVED TO SD CARD");
              dataLog.close();
            }
            //            }
          }
          count = 2;
          centiseconds = 0;
          sSeconds = 0;
          sMinutes = 0;
          while (1) {
            lcd.setCursor(12, 2);
            printDigits(valueJamAkhir);
            lcd.print(":");
            printDigits(valueMenitAkhir);
            lcd.print(":");
            printDigits(valueDetikAkhir);
            lcd.setCursor(15, 3);
            lcd.print("    ");
            if (digitalRead(set_mad) == 0) {
              //              Pas ditekan kuning nambah satu
              countValue++;
              //              EEPROM.write(totalDowntime, countValue);
              lcd.setCursor(0, 0);
              lcd.print(" LOSSTIME   COUNTER     ");
              value = EEPROM.read(addR);
              value2 = EEPROM.read(addR2);
              valueJam1 = EEPROM.read(addRJam1);
              valueMenit1 = EEPROM.read(addRMenit1);
              valueDetik1 = EEPROM.read(addRDetik1);
              valueJamAkhir = EEPROM.read(addRJamAkhir);
              valueMenitAkhir = EEPROM.read(addRMenitAkhir);
              valueDetikAkhir = EEPROM.read(addRDetikAkhir);
              valueHari = EEPROM.read(addRHari);
              valueBulan = EEPROM.read(addRBulan);
              valueTahun = EEPROM.read(addRTahun);
              lcd.setCursor(2, 1);
              lcd.print("START");
              lcd.setCursor(0, 2);
              lcd.print("00");
              //              printDigits(valueJam1);
              lcd.print(":");
              lcd.print("00");
              //              printDigits(valueMenit1);
              lcd.print(":");
              lcd.print("00");
              //              printDigits(valueDetik1);
              lcd.setCursor(14, 1);
              lcd.print("STOP");
              lcd.setCursor(12, 2);
              lcd.print("00");
              //              printDigits(valueJamAkhir);
              lcd.print(":");
              lcd.print("00");
              //              printDigits(valueMenitAkhir);
              lcd.print(":");
              lcd.print("00");
              //              printDigits(valueDetikAkhir);
              lcd.setCursor(9, 3);
              lcd.print(":");
              lcd.setCursor(12, 3);
              lcd.print(":");
              lcd.setCursor(15, 3);
              int menitToDetik = value2 * 60;
              valueDetikTotal = value + menitToDetik;
              Serial.print("TOTAL LOSSTIME ");
              Serial.println(valueDetikTotal);
              if (valueDetikTotal > 99) {
                printDigits(valueDetikTotal);
                //                lcd.print(" ");
              } else {
                printDigits(valueDetikTotal);
              }
              lcd.setCursor(19, 3);
              lcd.print("s");
              break;
            }
          }
        }
        //        break;
      }
    }
  }
}

//  ----------------------- Untuk membuat file yang sesuai dengan waktu dan tanggal
void getFileName() {
  DateTime now = RTC.now();
  filename[0] = (now.year() / 1000) % 10 + '0'; //To get 1st digit from year()
  filename[1] = (now.year() / 100) % 10 + '0'; //To get 2nd digit from year()
  filename[2] = (now.year() / 10) % 10 + '0'; //To get 3rd digit from year()
  filename[3] = now.year() % 10 + '0'; //To get 4th digit from year()
  filename[4] = now.month() / 10 + '0'; //To get 1st digit from month()
  filename[5] = now.month() % 10 + '0'; //To get 2nd digit from month()
  filename[6] = now.day() / 10 + '0'; //To get 1st digit from day()
  filename[7] = now.day() % 10 + '0'; //To get 2nd digit from day()
  Serial.println(filename);
}
