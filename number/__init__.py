import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_ID

from .. import CONF_Z906_ID, Z906Component, z906_ns

Z906Number = z906_ns.class_("Z906Number", number.Number, cg.Component)
NumberChannel = z906_ns.enum("NumberChannel", is_class=True)

CONF_VOLUME = "volume"

CHANNELS = {
    CONF_VOLUME: ("set_volume_number", NumberChannel.VOLUME, "mdi:volume-high"),
}

def _channel_schema(icon):
    return (
        number.number_schema(Z906Number, icon=icon, unit_of_measurement="%")
        .extend(cv.COMPONENT_SCHEMA)
    )

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_Z906_ID): cv.use_id(Z906Component),
        **{
            cv.Optional(key): _channel_schema(icon)
            for key, (_, _, icon) in CHANNELS.items()
        },
    }
)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_Z906_ID])
    for key, (setter, channel, _icon) in CHANNELS.items():
        if key not in config:
            continue
        conf = config[key]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await number.register_number(var, conf, min_value=0.0, max_value=100.0, step=1.0)
        cg.add(var.set_channel(channel))
        cg.add(var.set_parent(hub))
        cg.add(getattr(hub, setter)(var))