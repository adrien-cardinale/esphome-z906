import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

CODEOWNERS = ["@adrien-cardinale"]
DEPENDENCIES = ["uart"]
MULTI_CONF = True

z906_ns = cg.esphome_ns.namespace("z906")
Z906Component = z906_ns.class_("Z906Component", cg.Component)

CONF_Z906_ID = "z906_id"
CONF_CONSOLE_UART_ID = "console_uart_id"
CONF_AMP_UART_ID = "amp_uart_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Z906Component),
        cv.Required(CONF_CONSOLE_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Required(CONF_AMP_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Optional("amp_presence_pin", default=4): cv.int_range(min=0, max=39),
        cv.Optional("console_presence_pin", default=9): cv.int_range(min=0, max=39),
        cv.Optional("alim_pin", default=8): cv.int_range(min=0, max=39),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    console_uart = await cg.get_variable(config[CONF_CONSOLE_UART_ID])
    amp_uart = await cg.get_variable(config[CONF_AMP_UART_ID])
    cg.add(var.set_console_uart(console_uart))
    cg.add(var.set_amp_uart(amp_uart))
    cg.add(
        var.set_adc_pins(
            config["console_presence_pin"],
            config["amp_presence_pin"],
            config["alim_pin"],
        )
    )