// Library yang digunakan
#include <DHT_U.h>
#include <DHT.h>
#include <virtuabotixRTC.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <string.h>

// Konfigurasi PIN
#define CLKPIN 14
#define DATAPIN 15
#define RSTPIN 16
#define DHTPIN 17
#define DHTTYPE DHT11


// Konfigurasi untuk keypad
const byte ROWS2 = 4;
const byte COLS2 = 4;

char keys2[ROWS2][COLS2] = {
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'}};
byte rowPins2[ROWS2] = {5, 4, 3, 2};
byte colPins2[COLS2] = {9, 8, 7, 6};

Keypad keypad2 = Keypad(
    makeKeymap(keys2),
    rowPins2,
    colPins2,
    ROWS2,
    COLS2);

// Konfigurasi untuk LCD
LiquidCrystal_I2C lcd (0x27, 20, 4);

// Konfigurasi SensorDHT
DHT dht(DHTPIN, DHTTYPE);

// Konfigurasi Modul RTC
virtuabotixRTC myRTC(CLKPIN,DATAPIN,RSTPIN);

// Konfigurasi Variabel
float kelembapan, suhu;
String waktu, tanggal;
char bufferWaktu[10];
char bufferTanggal[10];
const unsigned long menuInterval = 1000;
unsigned long waktuMenuSebelum = 0;
float batasSuhu = 0;

// Konfigurasi Menu
// >>>>> seleksi ganti password <<<<<
LiquidLine selectGanti(0,1, ">>");
LiquidLine selectGanti2(0,2,">>");
LiquidLine selectGanti3(0,3,">>");
LiquidLine lineSimpan(0,3,"2. Simpan");

// ----- SCREEN MAIN -----
LiquidLine lineJudulTanggal(5, 0, "Lihat Waktu");
LiquidLine lineJam(0, 1, "Jam :",bufferWaktu);
LiquidLine lineTanggal(0, 2, "Tgl :",bufferTanggal);
// create a screen from the above lines
LiquidScreen screenMain(lineJudulTanggal, lineJam, lineTanggal);
// --------------------------

// ----- STATUS SCREEN -----
// a line of two string literals and a float variable
LiquidLine lineJudulTemp(5, 0, "Lihat Suhu");
LiquidLine lineSuhu(0,1,"Suhu :",suhu);
LiquidLine lineKelembapan(0,2, "Kelembapan :",kelembapan);
LiquidScreen screenStatus(lineJudulTemp, lineSuhu, lineKelembapan);
// -------------------------

// ----- PASSWORD -----
LiquidLine lineJudulPassword(5,0,"Password");
LiquidLine lineGantiPassword(0,1,"1. Ganti Password");
LiquidLine lineHapusPassword(0,2,"2. Hapus Password");
LiquidScreen screenMenuPassword(lineJudulPassword, lineGantiPassword, lineHapusPassword);

// ----- BATAS SUHU -----
LiquidLine lineJudulSuhu(5,0,"Suhu");
LiquidLine lineBatasCurrent(0,1,"Batas Suhu : ", batasSuhu);
LiquidLine lineEditSuhu(0,2,"1. Edit Suhu : ");
LiquidScreen screenSuhu(lineJudulSuhu,lineBatasCurrent, lineEditSuhu, lineSimpan);

// ----- FAN ON/OFF -----
LiquidLine lineJudulFan(5,0,"Fan On/Off");
LiquidLine lineFanOn(0,1,"1. Fan On");
LiquidLine lineFanOff(0,2,"2. Fan Off");
LiquidScreen screenFan(lineJudulFan, lineFanOn, lineFanOff);

// ----- SETTINGS -----
LiquidLine lineJudulSettings(5,0,"Settings");
LiquidLine lineMenuPassword(0,1,"1. Password");
LiquidLine lineMenuBatasSuhu(0,2,"2. Batas Suhu");
LiquidLine lineMenuFan(0,3,"3. Fan On/Off");
LiquidScreen screenMenuSettings(lineJudulSettings,lineMenuPassword, lineMenuBatasSuhu, lineMenuFan);


// ----- MENU -----
// create a menu from the screens
LiquidMenu menu(lcd, screenMain, screenStatus);
LiquidMenu menuPassword(lcd, screenMenuPassword);
LiquidMenu menuBatasSuhu(lcd, screenSuhu);
LiquidMenu menuSettings(lcd,screenMenuSettings);
LiquidMenu menuFan(lcd,screenFan);
// ----------------

// ----- SYSTEM MENU -----
LiquidSystem menuSystem(menuSettings, menuPassword, menuBatasSuhu, menuFan);

// ----- FUNCTION -----
void goToPasswordMenu(){
    menuSystem.change_menu(menuPassword);
}

void goToBatasSuhu(){
    menuSystem.change_menu(menuBatasSuhu);
}

void goToFan(){
    menuSystem.change_menu(menuFan);
}




void setup()
{
    // Set Waktu
    //myRTC.setDS1302Time(00, 50, 16, 6, 7, 10, 2022);
    dht.begin();
    myRTC.updateTime();
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);

    menuSystem.update();
    lineMenuPassword.attach_function(1,goToPasswordMenu);
    lineMenuBatasSuhu.attach_function(1,goToBatasSuhu);
    lineMenuFan.attach_function(1,goToFan);

    menuSystem.add_menu(menu);
    menuSystem.change_menu(menu);


}


void loop()
{
    // Baca Temperatur
    suhu = dht.readTemperature();
    // Baca Kelembapan
    kelembapan = dht.readHumidity();
    // Update Waktu
    myRTC.updateTime();

    /*Deskripsi Waktu*/
    waktu = String(myRTC.hours)+ ":" + String(myRTC.minutes);
    /*Deskripsi Tanggal*/
    strcpy(bufferWaktu, waktu.c_str());
    tanggal = String(myRTC.dayofmonth) + "/" + 
    (myRTC.month) + "/" + (myRTC.year);
    strcpy(bufferTanggal, tanggal.c_str());

    // Tangkap inputan Keyboard Navigasi
    char customKey = keypad2.getKey();

    switch (customKey)
    {
    case 'A':
        menuSystem.next_screen();
        break;
    case 'B':
        menuSystem.previous_screen();
        break;
    case 'C':
        menuSystem.switch_focus();
        break;
    case '#':
        menuSystem.change_menu(menuSettings);
        break;
    case '*':
        menuSystem.call_function(1,true);
        break;
    case 'D':
        menuSystem.add_menu(menu);
        menuSystem.change_menu(menu);
    }


    // Setting MenuInterval millis();
    unsigned long waktuMenu = millis();

    if (waktuMenu - waktuMenuSebelum >= menuInterval)
    {
        waktuMenuSebelum = waktuMenu;
        menuSystem.update();
    }

    
}

