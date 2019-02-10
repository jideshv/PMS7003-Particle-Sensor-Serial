// Copyright (c) 2019 Jidesh Veeramachaneni
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).

#pragma once

/**
* The PMS7003Serial class reads and controls
* the Plantower PMS7003 particle sensor.
*
* Datasheet: https://aqicn.org/air/view/sensor/spec/pms7003.pdf (Chinese)
*
* @tparam PMS7003UARTSerial Use USARTSerial on Particle platforms. See example.
*/
template <typename PMS7003UARTSerial>
class PMS7003Serial
{
public:
  /**
  * Construct the PMS7003Serial with the Serial object you want to use.
  *
  * @param serial the Serial object you want to use.
  * @param setpin the MCU pin that SET is connected to.
  */
  PMS7003Serial(PMS7003UARTSerial& serial, int setpin) : m_serial(serial), m_setpin(setpin) {
    m_serial.template begin(9600,SERIAL_8N1);
  }

  ~PMS7003Serial() {
    m_serial.template end();
  }

  enum DataElement { pm1_0, pm2_5, pm10, count0_3um, count0_5um, count1um, count2_5um, count5um, count10um };

  /**
  * Reads true if a new set of particle data has been loaded,
  * which can then be read using GetData()
  */
  bool Read() {
    unsigned long start_time = millis();
    unsigned int recieved_bytes = 0;

    unsigned int last_byte = 0;
    unsigned int read_byte = 0;

    unsigned int checksum = 0;
    unsigned int calc_checksum = 0;

    while ((recieved_bytes < 32) && ((millis() - start_time) < 1000)) {
      if (m_serial.template available() > 0) {
        last_byte = read_byte;
        read_byte = m_serial.template read();
        if (recieved_bytes < 2) { // looking for the header
          if (read_byte == 0x42 && recieved_bytes == 0) {
            calc_checksum += read_byte;
            recieved_bytes++;
          } else if (read_byte == 0x4D && recieved_bytes == 1) {
            calc_checksum += read_byte;
            recieved_bytes++;
          }
        } else if (recieved_bytes < 32) {
          unsigned int val16 = (last_byte << 8) + read_byte;
          switch (recieved_bytes) {
            case 11: // byte number 12
            m_data[pm1_0] = val16;
            break;

            case 13:
            m_data[pm2_5] = val16;
            break;

            case 15:
            m_data[pm10] = val16;
            break;

            case 17:
            m_data[count0_3um] = val16;
            break;

            case 19:
            m_data[count0_5um] = val16;
            break;

            case 21:
            m_data[count1um] = val16;
            break;

            case 23:
            m_data[count2_5um] = val16;
            break;

            case 25:
            m_data[count5um] = val16;
            break;

            case 27:
            m_data[count10um] = val16;
            break;

            case 31:
            checksum = val16;
            calc_checksum -= last_byte;
            calc_checksum -= read_byte;
            break;

            default:
            break;
          }
          calc_checksum += read_byte;
          recieved_bytes++;
        }
      }
    }
    return (calc_checksum == checksum);
  }

  /**
  * Puts the sensor to sleep or wakes it up
  */
  void SetSleep(bool sleep) {
    if (sleep) {
      pinResetFast(m_setpin);
    } else {
      pinSetFast(m_setpin);
    }
  }

  /**
  * Gets the last read data element. Only use after a Read().
  */
  unsigned int GetData(DataElement element) { return m_data[element]; }

private:
  PMS7003UARTSerial& m_serial;
  int m_setpin;
  unsigned int m_data[11];
};
