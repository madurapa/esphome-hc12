import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
MULTI_CONF = True

hc12_ns = cg.esphome_ns.namespace("hc12")
HC12Component = hc12_ns.class_("HC12Component", cg.Component, uart.UARTDevice)

CONF_UART_ID = "uart_id"
CONF_MAX_BUFFER_SIZE = "max_buffer_size"
CONF_MESSAGE_TERMINATOR = "message_terminator"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HC12Component),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Optional(CONF_MAX_BUFFER_SIZE, default=64): cv.positive_int,
    cv.Optional(CONF_MESSAGE_TERMINATOR, default="\r\n"): cv.string,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], await cg.get_variable(config[CONF_UART_ID]))
    cg.add(var.set_max_buffer_size(config[CONF_MAX_BUFFER_SIZE]))
    cg.add(var.set_message_terminator(config[CONF_MESSAGE_TERMINATOR]))
    await cg.register_component(var, config)