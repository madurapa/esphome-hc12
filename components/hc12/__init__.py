import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
MULTI_CONF = True

# Define namespace and component class
hc12_ns = cg.esphome_ns.namespace("hc12")
HC12Component = hc12_ns.class_("HC12Component", cg.Component, uart.UARTDevice)

# Configuration options
CONF_UART_ID = "uart_id"

# Validate configuration
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HC12Component),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
}).extend(cv.COMPONENT_SCHEMA)

# Code generation
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], 
                          await cg.get_variable(config[CONF_UART_ID]))
    await cg.register_component(var, config)
