import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID

from .. import CONF_Z906_ID, Z906Component, z906_ns

z906Select = z906_ns.class_("Z906Select", select.Select, cg.Component)
SelectChannel = z906_ns.enum("SelectChannel", is_class=True)

CONF_INPUT = "input"
CON_EFFECT = "effect"

INPUT_OPTIONS = ["Input 1", "Input 2", "Input 3", "Input 4", "Input 5", "Input 6"]
EFFECT_OPTIONS = ["3D", "2.1", "4.1", "None"]

CHANNELS = {
    CONF_INPUT: ("set_input_select", SelectChannel.INPUT_CHANNEL, INPUT_OPTIONS, "mdi:import"),
    CON_EFFECT: ("set_effect_select", SelectChannel.EFFECT_CHANNEL, EFFECT_OPTIONS, "mdi:surround-sound"),
}

def _channel_schema(icon):
    return select.select_schema(z906Select, icon=icon).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_Z906_ID): cv.use_id(Z906Component),
        **{
            cv.Optional(key): _channel_schema(icon)
            for key, (_, _, _, icon) in CHANNELS.items()
        },
    }
)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_Z906_ID])
    for key, (setter, channel, options, _icon) in CHANNELS.items():
        if key not in config:
            continue
        conf = config[key]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await select.register_select(var, conf, options=options)
        cg.add(var.set_channel(channel))
        cg.add(var.set_parent(hub))
        cg.add(getattr(hub, setter)(var))