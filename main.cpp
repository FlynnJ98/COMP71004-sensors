
/*
This program will get sensor data and print it on the screen when a particular key is pressed on the keyboard. 
The "CTRL B" key will print the sensor data to the terminal in this case. It will get sensor data such as
Temperature, Humidity, accelerometer & range/ distance. We use a interrupt function to control when the 
Particular key is pressed on the screen the program jumps to this and prints the data to the terminal. 
A while 1 loop was used with a switch case to have the sensor data printed when the key is pressed. 
*/

//Includes the following
#include "mbed.h"
#include "string.h"
#include <cstdio>
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"
#include "LSM6DSLSensor.h"
#include "lis3mdl_class.h"
#include "VL53L0X.h"

// objects for various sensors
static DevI2C devI2c(PB_11,PB_10);
static LPS22HBSensor press_temp(&devI2c);
static HTS221Sensor hum_temp(&devI2c);
static LSM6DSLSensor acc_gyro(&devI2c,0xD4,D4,D5); // high address
static LIS3MDL magnetometer(&devI2c, 0x3C);
static DigitalOut shutdown_pin(PC_6);
static VL53L0X range(&devI2c, &shutdown_pin, PC_7, 0x52);

UnbufferedSerial pc(USBTX, USBRX); // provides access to UART functionality. 

char che; // declares a character called Che. 
volatile char parse = 0; // initialises the parse variable with 0. Volatile means it can change at any time. 

void PCinterrupt(){ // interrupt the program when the value is one or key is pressed. 
    if(pc.readable()){ // checks if the data can be read by the pc
        pc.read(&che, 1); // if readable, store in che 
        parse = 1; // sets the parse variable to 1
    }
}
// functions to print sensor data
void print_t_rh(){
    float value1, value2;
    hum_temp.get_temperature(&value1);
    hum_temp.get_humidity(&value2);

    value1=value2=0;    
    press_temp.get_temperature(&value1);
    press_temp.get_pressure(&value2);
    printf("LPS22HB: [temp] %.2f C, [press] %.2f mbar\r\n", value1, value2);
}

void print_mag(){
    int32_t axes[3]; // intitialise axes data. 
    magnetometer.get_m_axes(axes);
    printf("LIS3MDL [mag/mgauss]:    %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]); // print data of axes's

}

void print_accel(){
    int32_t axes[3];
    acc_gyro.get_x_axes(axes);
    printf("LSM6DSL [acc/mg]:        %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_gyro(){
    int32_t axes[3];
    acc_gyro.get_g_axes(axes);
    printf("LSM6DSL [gyro/mdps]:     %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_distance(){
    uint32_t distance;
    int status = range.get_distance(&distance);
    if (status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X [mm]:            %6u\r\n", distance);
    } else {
        printf("VL53L0X [mm]:                --\r\n");
    }
}

/* Simple main function */
int main() {

    pc.attach(&PCinterrupt); // attach an interrupt function
// initialising sensors and gyroscope
    uint8_t id;
    float value1, value2;

    int32_t axes[3];

    hum_temp.init(NULL);

    press_temp.init(NULL);
    magnetometer.init(NULL);
    acc_gyro.init(NULL);

    range.init_sensor(0x52);

    hum_temp.enable();
    press_temp.enable();

    acc_gyro.enable_x();
    acc_gyro.enable_g();
  
    printf("\033[2J\033[20A");
    printf ("\r\n--- Starting new run ---\r\n\r\n");

    hum_temp.read_id(&id);
    printf("HTS221  humidity & temperature    = 0x%X\r\n", id);

    press_temp.read_id(&id);
    printf("LPS22HB pressure & temperature    = 0x%X\r\n", id);
    magnetometer.read_id(&id);
    printf("LIS3MDL magnetometer              = 0x%X\r\n", id);
    acc_gyro.read_id(&id);
    printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);
    
    printf("\n\r--- Reading sensor values ---\n\r"); ;
    print_t_rh();
    print_mag();
    print_accel();
    print_gyro();
    print_distance();
    printf("\r\n");
    
    while(1) { // while the value is 1, execute the loop
        if(parse){
            switch (che) { // switch case for Character B
                case 'B':
                    printf("\n\r--- Reading sensor values ---\n\r"); ; // prints sensor data 
                    print_t_rh();
                    print_mag();
                    print_accel();
                    print_gyro();
                    print_distance();
                    printf("\r\n");
                    break;
                default:
                    printf(" No Data, press CTRL B for Sensor Data \r\n"); // If any Key other can CTRL B is pressed, print this message. 
                    break;            
            }
            parse = 0; // resets so the terminal does not keep printing the values
        }

        }
        wait_us(500000);
    }
