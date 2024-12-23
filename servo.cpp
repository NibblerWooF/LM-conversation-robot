#include <SCServo.h>



SCSCL sc;



#define S_RXD 16
#define S_TXD 17
 
float mapTo(float val, float I_Min, float I_Max, float O_Min, float O_Max){
  return(((val-I_Min)*((O_Max-O_Min)/(I_Max-I_Min)))+O_Min);
}
int ts(int angle){
  float an = mapTo((float)angle,0,300,40,983);
  return an;
}



void servo_init()
{
  
  Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  sc.pSerial = &Serial1;
  delay(20);
  Serial.print("\r\n********************** The servo initialization is complete  ************************\r\n");
  
}


void angle_set(int ID, float angle ,int speed)
{
  if(angle>180) angle = 180;
  if(angle<0) angle = 0;
  sc.WritePosEx(ID, ts(angle), speed, 0); 
}