import esphome.codegen as cg
from esphome.components import spi, number
import esphome.config_validation as cv

CODEOWNERS = ["@ellsclytn"]
DEPENDENCIES = ["spi"]
AUTO_LOAD = ["number", "fan"]
MULTI_CONF = True

CONF_INITIAL_VALUE = "initial_value"
MCP4XXX_MAX_VALUE = 128

mcp4xxx_ns = cg.esphome_ns.namespace("mcp4xxx")
MCP4XXX = mcp4xxx_ns.class_("MCP4XXX", number.Number, cg.Component, spi.SPIDevice)

CONFIG_SCHEMA = (
    number.number_schema(MCP4XXX)
    .extend(
        {
            cv.Optional(CONF_INITIAL_VALUE, default=0): cv.int_range(
                min=0, max=MCP4XXX_MAX_VALUE
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(spi.spi_device_schema(cs_pin_required=True))
)


async def to_code(config):
    var = await number.new_number(
        config,
        min_value=0,
        max_value=MCP4XXX_MAX_VALUE,
        step=1,
    )
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    cg.add(var.set_initial_value(config[CONF_INITIAL_VALUE]))
