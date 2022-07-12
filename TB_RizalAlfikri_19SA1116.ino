// Library yang digunakan
#include <DHT_U.h>
#include <DHT.h>
#include <virtuabotixRTC.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <string.h>
#include <EEPROM.h>

// Konfigurasi PIN
#define CLKPIN 14
#define DATAPIN 15
#define RSTPIN 16
#define DHTPIN 17
#define DHTTYPE DHT11
const int RELAY_PIN = 10; 


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
int bufferKeyNumerik;
float batasSuhu;
bool fanOn, kipasNyala;
int password;
char CustomKey;

// Konfigurasi Menu
// >>>>> seleksi ganti password <<<<<
LiquidLine lineSimpanSuhu(0,3,"1. Simpan Suhu");
LiquidLine lineSimpanPassword(0,3,"1. Simpan Password");
LiquidLine lineTersimpan(5,2, "Tersimpan!");
LiquidLine lineTerhapus(5,2, "Terhapus!");

// >>>>> SCREEN MASUKKAN VALUE <<<<<
LiquidLine lineJudulValue(5,0,"Masukkan Value");
LiquidLine lineValue(3,1,bufferKeyNumerik);
LiquidScreen screenMasukkanValueSuhu(lineJudulValue,lineValue,lineSimpanSuhu);
LiquidScreen screenMasukkanValuePassword(lineJudulValue,lineValue,lineSimpanPassword);
LiquidScreen screenTersimpan(lineTersimpan);
LiquidScreen screenTerhapus(lineTerhapus);

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
LiquidLine lineBatasCurrentDisp(0,3,"Batas Suhu : ", batasSuhu);
LiquidLine lineSuhu(0,1,"Suhu :",suhu);
LiquidLine lineKelembapan(0,2, "Kelembapan :",kelembapan);
LiquidScreen screenStatus(lineJudulTemp, lineSuhu, lineKelembapan, lineBatasCurrentDisp);
// -------------------------

// ----- PASSWORD -----
LiquidLine lineJudulPassword(5,0,"Password");
LiquidLine lineGantiPassword(0,1,"1. Ganti Password");
LiquidLine lineResetPassword(0,2,"2. Reset Password");
LiquidScreen screenMenuPassword(lineJudulPassword, lineGantiPassword, lineResetPassword);

// ----- BATAS SUHU -----
LiquidLine lineJudulSuhu(5,0,"Suhu");
LiquidLine lineBatasCurrent(0,3,"Batas Suhu : ", batasSuhu);
LiquidLine lineEditSuhu(0,3,"1. Edit Suhu");
LiquidScreen screenSuhu(lineJudulSuhu,lineBatasCurrent, lineEditSuhu);

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
LiquidMenu menuIsiValSuhu(lcd,screenMasukkanValueSuhu);
LiquidMenu menuIsiValPassword(lcd,screenMasukkanValuePassword);
LiquidMenu menuTersimpan(lcd,screenTersimpan);
LiquidMenu menuTerhapus(lcd,screenTerhapus);
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

void isiValueBatasSuhu(){
    menuSystem.add_menu(menuIsiValSuhu);
    menuSystem.change_menu(menuIsiValSuhu);
    bufferKeyNumerik = 0;
}

void isiValuePassword(){
    menuSystem.add_menu(menuIsiValPassword);
    menuSystem.change_menu(menuIsiValPassword);
    bufferKeyNumerik = 0;
}

void simpanDataBatasSuhu(){
    batasSuhu = (float)bufferKeyNumerik;
    EEPROM.put(9,batasSuhu);
    menuSystem.add_menu(menuTersimpan);
    menuSystem.change_menu(menuTersimpan);
    delay(1000);
    menuSystem.change_menu(menuBatasSuhu);
}

void simpanDataPassword(){
    password = bufferKeyNumerik;
    EEPROM.put(10,password);
    menuSystem.add_menu(menuTersimpan);
    menuSystem.change_menu(menuTersimpan);
    delay(1000);
    menuSystem.change_menu(menuPassword);
}

void resetDataPassword(){
    password = 0000;
    EEPROM.update(10,password);
    menuSystem.add_menu(menuTerhapus);
    menuSystem.change_menu(menuTerhapus);
    delay(1000);
    menuSystem.change_menu(menuPassword);
}

void FanOn(){
    fanOn = true;
    kipasNyala = true;
}

void FanOff(){
    fanOn = false;
    kipasNyala = false;
}

void FanOnOff(){
    if (fanOn == true)
        {
            digitalWrite(RELAY_PIN, LOW);
        } else if(fanOn == false)
        {
            digitalWrite(RELAY_PIN, HIGH);
        }
}

void CheckSuhu(){
    if (kipasNyala == true && fanOn == false && suhu < batasSuhu)
    {
        fanOn = true;
    } else if (kipasNyala == true && fanOn == true && suhu < batasSuhu)
    {
        fanOn = true;
    } else if (kipasNyala == false && suhu >= batasSuhu)
    {
        fanOn = true;
    } else if (kipasNyala == false && suhu < batasSuhu)
    {
        fanOn = false;
    }
    
    
    
    
    
}


void setup()
{
    // Set Waktu
    //myRTC.setDS1302Time(00, 50, 16, 6, 7, 10, 2022);
    pinMode(RELAY_PIN, OUTPUT);
    dht.begin();
    myRTC.updateTime();
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);
    EEPROM.get(9,batasSuhu);
    EEPROM.get(10,password);

    menuSystem.update();
    lineMenuPassword.attach_function(1,goToPasswordMenu);
    lineMenuBatasSuhu.attach_function(1,goToBatasSuhu);
    lineMenuFan.attach_function(1,goToFan);
    lineEditSuhu.attach_function(1,isiValueBatasSuhu);
    lineSimpanSuhu.attach_function(1,simpanDataBatasSuhu);
    lineGantiPassword.attach_function(1,isiValuePassword);
    lineResetPassword.attach_function(1,resetDataPassword);
    lineSimpanPassword.attach_function(1,simpanDataPassword);
    lineFanOn.attach_function(1,FanOn);
    lineFanOff.attach_function(1,FanOff);


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
    //Serial.println(batasSuhuInt);
    
    

    // Tangkap inputan Keyboard Navigasi
    CustomKey = keypad2.getKey();

    switch (CustomKey)
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
        break;
    case '0' ... '9':   
        bufferKeyNumerik = bufferKeyNumerik *10 +(CustomKey - '0');
        break;
    }

    CheckSuhu();
    FanOnOff();
    
        
    // Setting MenuInterval millis();
    unsigned long waktuMenu = millis();

    if (waktuMenu - waktuMenuSebelum >= menuInterval)
    {
        waktuMenuSebelum = waktuMenu;
        menuSystem.update();
        
    }


    
}

