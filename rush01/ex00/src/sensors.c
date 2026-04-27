#include "sensors.h"

int16_t read_internal_temp() {
    ADMUX = 0;
    ADCSRA |= (1 << ADPS0); // Prescaler of 128
    ADCSRA |= (1 << ADPS1); // Prescaler of 128
    ADCSRA |= (1 << ADPS2); // Prescaler of 128
    ADMUX |= (1 << MUX3); // select channel
    ADMUX |= (1 << REFS0) | (1 << REFS1); // select internal 1.1V ref
    ADCH = 0;
    ADCL = 0;
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    ADCSRA |= (1 << ADEN); // enable adc
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return (int16_t) ((float) ADC * (float) 1024/ (float)380); // Read ADC value
    // return (ADC - 324) * 100 / 122; // Read ADC value
    // return ADC;
}

void trigger_measurement(uint8_t slave_adr) {
    i2c_start();
    i2c_write(slave_adr); // Send slave address with write bit to set the register pointer
    i2c_write(0b10101100); // Send slave address with write bit to set the register pointer
    i2c_write(0b00110011); // Send slave address with write bit to set the register pointer
    i2c_write(0b00000000); // Send slave address with write bit to set the register pointer
}

uint8_t temp_connection_check(uint8_t slave_adr) { //check if slave is connected by trying to read the temperature)
    i2c_start();
    i2c_write(slave_adr+1); // Send slave address with write bit to set the register pointer
    i2c_write(0x71); // Send slave address
    uint8_t data = i2c_read_nack(); // Read data from slave without sending ACK
    i2c_stop();
    return data;
    // if (data & (1 << 3)) {
    //     return 1; // Slave is connected
    // } else {
    //     return 0; // Slave is not connected
    // }
}

int32_t get_temp(uint8_t slave_adr, float *temp, float *humidity) { //read slave temperature
    uint8_t buf[7];
    float temp_cdeg, hum_cpct;
    while (temp_connection_check(slave_adr) & (1 << 8)) {
        _delay_ms(1000); //wait for slave to be connected
    }


    i2c_start();
    i2c_write(slave_adr + 1); // Send slave address with read bit
    for(int i=0;i<7;i++) buf[i]=i2c_read((i < 6) ? 1 : 0); // Read data from slave without sending ACK

    uint32_t raw_hum  = ((uint32_t)buf[1] << 12)
                      | ((uint32_t)buf[2] <<  4)
                      | ((uint32_t)buf[3] >>  4);

    uint32_t raw_temp = (((uint32_t)buf[3] & 0x0F) << 16)
                      |  ((uint32_t)buf[4] <<  8)
                      |   (uint32_t)buf[5];
    i2c_stop();

                                    // 2^20
    temp_cdeg = (float)((raw_temp / (float) (1048576UL / 200)) - 50);
    hum_cpct  = (float)((raw_hum) / (float) (1048576UL / 100));
    *temp = temp_cdeg;
    *humidity = hum_cpct;
}