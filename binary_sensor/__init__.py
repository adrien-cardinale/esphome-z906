import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor

from .. import CONF_Z906_ID, Z906Component

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema().extend(
    {
        cv.GenerateID(CONF_Z906_ID): cv.use_id(Z906Component),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_Z906_ID])
    var = await binary_sensor.new_binary_sensor(config)
    cg.add(hub.set_power_binary_sensor(var))