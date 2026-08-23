import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID

from .. import CONF_Z906_ID, Z906Component, z906_ns

Z906Switch = z906_ns.class_("Z906Switch", switch.Switch, cg.Component)
SwitchChannel = z906_ns.enum("SwitchChannel", is_class=True)

CONF_STATUS = "status"

CHANNELS = {
    CONF_STATUS: ("set_status_switch", SwitchChannel.STATUS, "mdi:information"),
}

def _channel_schema(icon):
    return switch.switch_schema(Z906Switch, icon=icon).extend(cv.COMPONENT_SCHEMA)

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
        await switch.register_switch(var, conf)
        cg.add(var.set_channel(channel))
        cg.add(var.set_parent(hub))
        cg.add(getattr(hub, setter)(var))