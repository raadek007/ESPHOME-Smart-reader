import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_RECEIVE_TIMEOUT

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "text_sensor"]

CONF_HDLC_ID = "hdlc_id"
CONF_MAX_TELEGRAM_LENGTH = "max_telegram_length"
CONF_REQUEST_INTERVAL = "request_interval"

hdlc_reader_ns = cg.esphome_ns.namespace("hdlc_reader")
HDLCreader = hdlc_reader_ns.class_( 
    "HDLCreader", cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HDLCreader),
            cv.Optional(CONF_MAX_TELEGRAM_LENGTH, default=1500): cv.int_,
            cv.Optional(
                CONF_REQUEST_INTERVAL, default="0ms"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(
                CONF_RECEIVE_TIMEOUT, default="200ms"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_max_telegram_length(config[CONF_MAX_TELEGRAM_LENGTH]))
    cg.add(var.set_request_interval(config[CONF_REQUEST_INTERVAL].total_milliseconds))
    cg.add(var.set_receive_timeout(config[CONF_RECEIVE_TIMEOUT].total_milliseconds))
