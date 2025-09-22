#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace hdlc_reader {

class ParsedMessage {
    public:
		char *datetime_stamp_;
		char *COSEM_logical_device_name_;
		char *push_setup_;
		char *serial_number_;
		uint8_t disconnect_status_;
		uint32 power_limiter_value_;
		uint8_t relay_1_status_;
		uint8_t relay_2_status_;
		uint8_t relay_3_status_;
		uint8_t relay_4_status_;
		uint8_t relay_5_status_;
		uint8_t relay_6_status_;
		char* currently_active_energy_tarif_;
		uint32 instantanous_active_power_import_;
		uint32 instantanous_active_power_importL1_;
		uint32 instantanous_active_power_importL2_;
		uint32 instantanous_active_power_importL3_;
		uint32 instantanous_active_power_export_;
		uint32 instantanous_active_power_exportL1_;
		uint32 instantanous_active_power_exportL2_;
		uint32 instantanous_active_power_exportL3_;
		uint32 cumulative_active_import_energy_;
		uint32 cumulative_active_import_energy_rate_1_;
		uint32 cumulative_active_import_energy_rate_2_;
		uint32 cumulative_active_import_energy_rate_3_;
		uint32 cumulative_active_import_energy_rate_4_;
		uint32 cumulative_active_export_energy_;
		char *consumer_message_text_;	 
		
	    void set_text_value(uint8_t number, char *value);
	    void set_int_value (uint8_t number, uint32_t value);
	    void set_byte_value(uint8_t number, uint8_t value);
	    
        void debug_log();
        
        bool telegram_complete_;
        
	    
};


class HDLCreader : public uart::UARTDevice, public Component {
  public:
    void setup() override;
    void loop() override;
    void dump_config() override;

  	void set_max_telegram_length(size_t length) { this->max_telegram_len_ = length; }
  	void set_request_interval(uint32_t interval) { this->request_interval_ = interval; }
  	void set_receive_timeout(uint32_t timeout) { this->receive_timeout_ = timeout; }

    void set_sensor_cumulative_active_import(sensor::Sensor *sensor) { cumulative_active_import = sensor; }

  protected:
	size_t max_telegram_len_;
	uint32_t receive_timeout_;
	uint32_t request_interval_;

  	char *telegram_{nullptr};
  	size_t bytes_read_{0};
  	uint32_t last_read_time_{0};
  	uint32_t last_request_time_{0};


  	void reset_telegram_();
  	bool request_interval_reached_();

  	void parse_telegram_();  // A method to parse the data read from the sensor hardware
    ParsedMessage _parsedMessage = ParsedMessage();

  private:
	uint32_t ByteToInt( size_t pos_);


};



}  // namespace hdlc_reader
}  // namespace esphome