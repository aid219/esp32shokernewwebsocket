#include <ArduinoSort.h>

#define tempSensor1 33
#define tempSensor2 32
#define tempSensor3 35
#define tempSensor4 34
#define tempSensor5 39
#define tempSensor6 36

float temperature1 = 0;
float temperature2 = 0;
float temperature3 = 0;
float temperature4 = 0;
float temperature5 = 0;
float temperature6 = 0;

int coef[23] = {78, 77, 81, 82, 85, 86, 87, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 95, 96, 96, 96};
int adc[23] = {4100, 4000, 3750, 3530, 3340, 3150, 3000, 2860, 2730, 2600, 2490, 2370, 2240, 2120, 2000, 1880, 1760, 1650, 1525, 1400, 1280, 1150, 1040};
unsigned int tempArray[153] = {365988, 345168, 325399, 306511, 288748, 270967, 254144, 238363, 223570, 209686, 196839, 184867, 173713, 163316, 153618, 144568, 136118, 128224, 120846, 113946, 107491, 101448, 95789, 90402, 85357, 80629, 76197, 72041, 68140, 64478, 61039, 57808, 54770, 51892, 49186, 46640, 44243, 41985, 39859, 37855, 35965, 34183, 32500, 30886, 28361, 27921, 26560, 
25272, 24054, 22901, 21810, 20777, 19798, 18871, 17992, 
17159, 16369, 15609, 14908, 14232, 13591, 12983, 12401, 11855,
 11333, 10836, 10364, 10000, 9651, 9238, 8844, 8470, 8113,
  7773, 7449, 7141, 6847, 6500, 6299, 6044, 5804, 5568, 5347,
  5135, 4933, 4740, 4555, 4378, 4211, 4050, 3895, 3748, 3600,
  3473, 3344, 3220, 3102, 2988, 2880, 2776, 2676, 2580, 2489,
  2401, 2316, 2235, 2158, 2083, 2012, 1943, 1877, 1813, 1752, 
  1694, 1638, 1583, 1531, 1481, 1433, 1386, 1342, 1299, 1257,
  1217, 1179, 1141, 1106, 1063, 1037, 1007, 976, 946, 917,
  889, 863, 837, 812, 788, 765, 742, 720, 700, 679,
  660, 641, 622, 605, 587, 571, 555, 539, 524, 510};


float findTemp(float resist){
  for(int i = 0; i < 154; ++i){
    if(resist < tempArray[i]){

    }else{
   
      return i - 40;
    }
  }
  return 0;
}

float getTempAverage(uint8_t pin, int cycles, int resist) {
        float sus = 0;
        float cf = 0;
        float arr[cycles];
        for (int i = 0; i < cycles; ++i){
            pinMode(pin, INPUT);
            arr[i] = analogRead(pin);
        }
        sortArray(arr, cycles);
        sus = arr[cycles/2];
        for(int i = 0; i < 23; ++i){
          if(sus < adc[i]){
            cf = 105;
          }else{
            cf = coef[i];
            break;
          }
        }
        int res2 = resist;
        float res = ((sus * (cf/100000 * res2))/(3.25 - (sus * (cf/100000))));
        pinMode(pin, OUTPUT);
        return res;
    }

float severalTemps(uint8_t pin, int count, int resist){
  float nums = 0;
  for(int i = 0; i < count; ++i){
    float a = findTemp(getTempAverage(pin, 30, resist));
    nums += a;
  }
  return nums/count;

}


void getAllTemps(){
  temperature3 = severalTemps(tempSensor3, 30, 42700);
  temperature4 = severalTemps(tempSensor4, 30, 42700);
  temperature5 = severalTemps(tempSensor5, 30, 42700);
  temperature6 = severalTemps(tempSensor6, 30, 42700);
}