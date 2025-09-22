import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import(
    CONF_ID,
    DEVICE_CLASS_ENERGY,
    UNIT_KILOWATT_HOURS,
)

from . import HdlcReader, CONF_HDLC_ID

DEPENDENCIES = ["hdlc_readers"]

CONFIG_SCHEMA = cv.Schema(
   	{
        cv.GenerateID(CONF_HDLC_ID): cv.use_id(HDLCreader),
        cv.Optional("cumulative_active_import"): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOWATT_HOURS,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_TOTAL_INCREASING,
            STATE_CLASS_TOTAL_INCREASING,
        ),
    }
) .extend(cv.COMPONENT_SCHEMA)
 

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)