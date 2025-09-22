#include "hdlc_reader.h"
#include "esphome/core/log.h"
#include <string.h>

namespace esphome {
namespace hdlc_reader {

static const char *TAG = "hdlc_reader.component";

void HDLCreader::setup() {
  // Serial/UART device initialization is typically done here.
  // Note that a number of read/write methods are available in the UARTDevice
  // class. See "uart/uart.h" for details.
    this->telegram_ = new char[this->max_telegram_len_];

}

void HDLCreader::loop() {
  // Reading up to a limited number of bytes, to make sure that this loop()
  // won't lock up the system on a continuous incoming stream of bytes.
  //return;
  if (this->available()){
    const char c = this->read();
    
	//if (this->bytes_read_ >=30) {
	//	this->parse_data() ;
	//    this->reset_telegram_();	    
	//}

    // Check for buffer overflow.
    if (this->bytes_read_ >= this->max_telegram_len_) {
      ESP_LOGE(TAG, "Error: telegram larger than buffer (%d bytes)\ntelegram lenght is %d", this->max_telegram_len_,this->bytes_read_);
      ESP_LOGE(TAG, telegram_);
      this->reset_telegram_();
      return;
    }

    // Store the byte in the buffer.
    this->telegram_[this->bytes_read_] = c;
    this->bytes_read_++;
    this->last_read_time_=millis() ;
  } 
  if ((millis() - last_read_time_ >=receive_timeout_) && bytes_read_>0) {
		this->parse_telegram_() ;
	    this->reset_telegram_();	    
      
  }
  
} 

void HDLCreader::dump_config() {
  ESP_LOGCONFIG(TAG,
                "HDLCreader16:\n"
                "  Max telegram length: %d\n"
                "  Receive timeout: %.1fs",
                this->max_telegram_len_, this->receive_timeout_ / 1e3f);
  if (this->request_interval_ > 0) {
    ESP_LOGCONFIG(TAG, "  Request Interval: %.1fs", this->request_interval_ / 1e3f);
  }
}

void HDLCreader::reset_telegram_() {
  this->bytes_read_ = 0;
}


void HDLCreader::parse_telegram_() {
    // Example parsing method
    // Translates data received into buffer_data_ and stores it in parsed_value_ for publishing
    size_t buffer_pos_{0};
    ESP_LOGD(TAG, "Parsing data");
    if (telegram_[buffer_pos_++]!=0x0f){
		ESP_LOGE( TAG, "Wrong data header, must be 0x0f");
		return;
	}
    
	char buf[20];
	char lenght;
    
    std::string res;
    for (size_t i = 0; i < bytes_read_; i++) {
        if (i > 0) {
            res += ' ';
        }
        sprintf(buf, "%02X", telegram_[i]);
        res += buf;
    }
    ESP_LOGD(TAG, "%s", res.c_str());
    
	// check for date time stamp lenght





	ESP_LOGV(TAG, "id: %x", ByteToInt(buffer_pos_));
	buffer_pos_+=4;
	lenght= telegram_[buffer_pos_];
 
    // check for date time stamp lenght
	if (telegram_[buffer_pos_] !=0) {
	    //and debug log date time stamp
        std::string res;
		sprintf(buf,"%d-%d-%d ",(telegram_[buffer_pos_+1]<<8)|telegram_[buffer_pos_+2],telegram_[buffer_pos_+3],telegram_[buffer_pos_+4]);
		res += buf;
		sprintf(buf,"%2d:%02d:%02d\n",telegram_[buffer_pos_+6],telegram_[buffer_pos_+7],telegram_[buffer_pos_+8]);
		res += buf;
		ESP_LOGV(TAG, "%s", res.c_str());
	}
	buffer_pos_+=lenght+1;
	//ESP_LOGD(TAG, "bzte %02x", telegram_[buffer_pos_] );	
	
	if (telegram_[buffer_pos_++] == 2) {
	    uint8_t values_count_ =telegram_[buffer_pos_++];
	    //ESP_LOGD(TAG, "bzte %02x", values_count_ );
	    for (uint8_t i=0;i<values_count_ ;i++){
	        switch (uint8_t tp=telegram_[buffer_pos_++]){
	            case 0x09:
	            {
	               uint8_t string_lenght_ =telegram_[buffer_pos_++];
	               strncpy ( buf, telegram_ + buffer_pos_, string_lenght_ );
	               buf[string_lenght_] = '\0';
	               ESP_LOGV(TAG, "%d: retezec delky %d :%s" ,i ,string_lenght_, buf);
	               buffer_pos_+=string_lenght_;
	               break;
	            }
	            case 0x06:
	             {
	                uint32_t value= ByteToInt(buffer_pos_);
	                ESP_LOGV(TAG, "%d: longint %d",i , value );
	                buffer_pos_+=4;
	                _parsedMessage.set_int_value(i, value);
	                break;
	             }
	             case 0x16:
	             {
	                ESP_LOGV(TAG, "%d: byte %d",i , telegram_[buffer_pos_] );
	                buffer_pos_++;
	                break;
	             }
	            default:
	            {
	                ESP_LOGE(TAG, "wrong tzpe of data %d", tp );
	            }
	        
	        }
	        if (buffer_pos_ > bytes_read_) {
	            ESP_LOGE(TAG, "incorect count of values" ); 
	        }
	        
	    }
	}
	_parsedMessage.debug_log();
}

uint32_t HDLCreader::ByteToInt( size_t pos_){

	uint32_t result=0;
	result = (uint8_t)telegram_[pos_]<<24;
	result |=(uint8_t)telegram_[pos_+1]<<16;
	result |=(uint8_t)telegram_[pos_+2]<<8;
	result |=(uint8_t)telegram_[pos_+3];

    return result;
}

void ParsedMessage::set_text_value(uint8_t number, char *value){
    
}

void ParsedMessage::set_int_value(uint8_t number, uint32_t value){
    switch (number) {
        case 4:{power_limiter_value_=value; break;}
        case 12:{instantanous_active_power_import_=value; break;}
        case 13:{instantanous_active_power_importL1_=value; break;}
        case 14:{instantanous_active_power_importL2_=value; break;}
        case 15:{instantanous_active_power_importL3_=value; break;}
        case 20:{cumulative_active_import_energy_=value; break;}
        case 21:{cumulative_active_import_energy_rate_1_=value; break;}
        case 22:{cumulative_active_import_energy_rate_2_=value; break;}
        case 23:{cumulative_active_import_energy_rate_3_=value; break;}
        case 24:{cumulative_active_import_energy_rate_4_=value; break;}
    }
        
    
}
void ParsedMessage::set_byte_value(uint8_t number, uint8_t value){
    
}

void ParsedMessage::debug_log(){
    /*
    *datetime_stamp_;
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
		*/

	ESP_LOGD(TAG, "4: power_limiter_value_ %d", power_limiter_value_);
	ESP_LOGD(TAG, "12: instantanous active power import_ %d",instantanous_active_power_import_);
	ESP_LOGD(TAG, "13: instantanous active power import L1 %d",instantanous_active_power_importL1_);
	ESP_LOGD(TAG, "14: instantanous active power import L2 %d",instantanous_active_power_importL2_);
	ESP_LOGD(TAG, "15: instantanous active power import L3 %d",instantanous_active_power_importL3_);
	ESP_LOGD(TAG, "20: cumulative active import energy %d", cumulative_active_import_energy_);
	ESP_LOGD(TAG, "21: cumulative active import energy rate 1 %d", cumulative_active_import_energy_rate_1_);
	ESP_LOGD(TAG, "22: cumulative active import energy rate 2 %d", cumulative_active_import_energy_rate_2_);
	ESP_LOGD(TAG, "23: cumulative active import energy rate 3 %d", cumulative_active_import_energy_rate_3_);
	ESP_LOGD(TAG, "24: cumulative active import energy rate 4 %d", cumulative_active_import_energy_rate_4_);
	
	
	//ESP_LOGD(TAG, "_%d", );
	
		
}

} // namespace hdlc_reader
} // namespace esphome
