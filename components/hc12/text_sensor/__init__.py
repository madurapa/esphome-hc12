import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_FILTERS,
)
from .. import hc12_ns, HC12Component

DEPENDENCIES = ["hc12"]
AUTO_LOAD = ["text_sensor"]

# Component type definition
HC12TextSensor = hc12_ns.class_("HC12TextSensor", text_sensor.TextSensor, cg.Component)

# Configuration constants
CONF_HC12_ID = "hc12_id"

# Configuration schema
CONFIG_SCHEMA = text_sensor.text_sensor_schema(HC12TextSensor).extend({
    cv.GenerateID(): cv.declare_id(HC12TextSensor),
    cv.Required(CONF_HC12_ID): cv.use_id(HC12Component),
    cv.Optional(CONF_FILTERS): text_sensor.validate_filters,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    
    # Register base component and text sensor
    await cg.register_component(var, config)
    await text_sensor.register_text_sensor(var, config)
    
    # Get parent HC12 component
    parent = await cg.get_variable(config[CONF_HC12_ID])
    cg.add(var.set_hc12_component(parent))
    
    # Add filters if specified
    if CONF_FILTERS in config:
        await text_sensor.setup_filters(var, config)
