//Setup Includes 
#include "mbed.h"
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"
#include "LSM6DSLSensor.h"
#include "lis3mdl_class.h"
#include "VL53L0X.h"
#include <cstdio>

//Setup objects for various sensors
static DevI2C devI2c(PB_11,PB_10);
static LPS22HBSensor press_temp(&devI2c);
static HTS221Sensor hum_temp(&devI2c);
static LSM6DSLSensor acc_gyro(&devI2c,0xD4,D4,D5); // high address
static LIS3MDL magnetometer(&devI2c, 0x3C);
static DigitalOut shutdown_pin(PC_6);
static VL53L0X range(&devI2c, &shutdown_pin, PC_7, 0x52);


//Setup serial comunication
static UnbufferedSerial pc(USBTX,USBRX); 
int baudrate(115200);

//Setup to receive characters 
volatile bool data_ready = false;
char che;
volatile char parse = 0;

void Pcinterrupt(){
    if(pc.readable()){
        pc.read(&che, 1);
        parse = 1;
    }
}

// receive characters from serial input
//void serial_isr() {
//    if (pc.read(&recv_char, 1)){
 //       data_ready = true;
 //   }
//}

//Setup functions to print sensor data
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
    int32_t axes[3];
    magnetometer.get_m_axes(axes);
    printf("LIS3MDL: [mag/mgauss]:    %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

}

void print_accel(){
    int32_t axes[3];
    acc_gyro.get_x_axes(axes);
    printf("LSM6DSL: [acc/mg]:        %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_gyro(){
    int32_t axes[3];
    acc_gyro.get_g_axes(axes);
    printf("LSM6DSL: [gyro/mdps]:     %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_distance(){
    uint32_t distance;
    int status = range.get_distance(&distance);
    if (status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X: [mm]:            %6u\r\n", distance);
    } else {
        printf("VL53L0X: [mm]:                --\r\n");
    }
}

/*Setup main function */
int main() {
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
  
    //printf("\033[2J\033[20A");
    //Setup list of sensors 
    printf ("\r\n--- Starting new run ---\r\n\r\n");
    printf ("\r\n--- PLEASE SELECT THE SENSOR YOU WOULD LIKE TO READ ---\n A = acc/mg \n G = gyro \n M = mag/mgauss \n D = mm \n T = Temp & Pressure \n");


pc.attach(&Pcinterrupt);
//Print the chosen sensore data depending on case
    while (1){
        if(parse){
            switch (che) {

    case 'h': case 'H':
    hum_temp.read_id(&id);
    //printf("HTS221  humidity & temperature    = 0x%X\r\n", id);
    print_t_rh();
    break;

    case 't': case 'T':
    press_temp.read_id(&id);
    //printf("LPS22HB pressure & temperature    = 0x%X\r\n", id);
    print_t_rh();
    break;

    case 'm': case 'M':
    magnetometer.read_id(&id);
    //printf("LIS3MDL magnetometer              = 0x%X\r\n", id);
    print_mag();
    break;

    case 'g': case 'G':
    acc_gyro.read_id(&id);
    //printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);
    print_gyro();
    break;

    case 'd': case 'D':
    print_distance( );
    break;

    case 'a': case 'A':
    print_accel();
    break;

    }
    parse = 0;
}
    wait_us(5000000);
}
    
    while(1) {
        wait_us(5000000);
    }
}