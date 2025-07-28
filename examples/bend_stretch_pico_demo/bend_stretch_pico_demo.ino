/* 
 * Reading the one axis soft flex sensor from Nitto Bend Technologies in
 * polled mode
 * Adapted for Raspberry Pi Pico (RP2040)

 * By: Colton Ottley @ Nitto Bend Technologies
 * Date: June 18th, 2019
 * Edited by : Nick Rudh @ Nitto Bend Technologies
 * Date: 1/15/2025
 *  
 * This software is provided "as is", without any warranty of any kind, express or implied,
 * including but not limited to the warranties of merchantability, fitness for a particular purpose,
 * and noninfringement. In no event shall the authors or copyright holders be liable for any claim,
 * damages, or other liability, whether in an action of contract, tort, or otherwise, arising from,
 * out of, or in connection with the software or the use or other dealings in the software.
 * 
 * Refer to one_axis_quick_start_guide.pdf for wiring instructions
 */

#include "Arduino.h"
#include "ads.h"

#ifdef ARDUINO_ARCH_RP2040
#include <Wire.h>
#endif
#define ADS_RESET_PIN      (2)           // GPIO2 on Raspberry Pi Pico
#define ADS_INTERRUPT_PIN  (3)           // GPIO3 on Raspberry Pi Pico

/* Not used in polled mode. Stub function necessary for library compilation */
void ads_data_callback(float * sample, uint8_t sample_type) {}

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing One Axis sensor");
#ifdef ARDUINO_ARCH_RP2040
  Wire.setSDA(4);
  Wire.setSCL(5);
#endif
  
  ads_init_t init;                                // One Axis ADS initialization structure

  init.sps = ADS_100_HZ;                          // Set sample rate to 100 Hz (Interrupt mode)
  init.ads_sample_callback = &ads_data_callback;  // Provide callback for new data
  init.reset_pin = ADS_RESET_PIN;                 // Pin connected to ADS reset line
  init.datardy_pin = ADS_INTERRUPT_PIN;           // Pin connected to ADS data ready interrupt
  init.addr = 0;                                  // Update value if non default I2C address is assinged to sensor

  // Initialize ADS hardware abstraction layer, and set the sample rate
  int ret_val = ads_init(&init);
  
  if(ret_val != ADS_OK)
  {
    Serial.print("One Axis ADS initialization failed with reason: ");
    Serial.println(ret_val);
  }
  else
  {
    Serial.println("One Axis ADS initialization succeeded...");
  }

  // Enable stretch measurements
  ads_stretch_en(true);

  // Start reading data in polled mode
  ads_polled(true);

  // Wait for first sample
  delay(10);
}

void loop() {

  float sample[2];
  uint8_t data_type;

  // Read data from the one axis ads sensor
  int ret_val = ads_read_polled(sample, &data_type);

  // Check if read was successful
  if (ret_val == ADS_OK && data_type == ADS_SAMPLE)
  {
    Serial.print(sample[0]);    // Bend data
    Serial.print(",");
    Serial.println(sample[1]);  // Stretch data
  }

  delay(5);  // Maintain ~100 Hz sample rate
}

