# Logitech Z906 ESPHome controller

> [!IMPORTANT]
> This project is still under active development. Some features may not be fully available yet.

ESPHome component that sits between the Logitech Z906 console and amplifier UART lines. The goal is to control the speaker system (volume, input source, power state) directly from ESPHome/Home Assistant. Today it only relays traffic between the console and amplifier and monitors link presence/power state via ADC pins — decoding and actively controlling the speakers is planned but not implemented yet.

## Configuration

```yaml
uart:
  - id: console_uart
    tx_pin: ...
    rx_pin: ...
    baud_rate: 5600
  - id: amp_uart
    tx_pin: ...
    rx_pin: ...
    baud_rate: 5600

z906:
  console_uart_id: console_uart
  amp_uart_id: amp_uart
  console_presence_pin: 9  # optional, default 9
  amp_presence_pin: 4      # optional, default 4
  alim_pin: 8              # optional, default 8
```

- `console_uart_id` / `amp_uart_id`: UART buses connected to the console and amplifier respectively (required).
- `console_presence_pin` / `amp_presence_pin` / `alim_pin`: ADC-capable GPIOs used to detect console presence, amplifier presence, and power supply state.

## Acknowledgments

- [nomis/logitech-z906](https://github.com/nomis/logitech-z906) - for the protocol
- [zarpli/Logitech-Z906](https://github.com/zarpli/Logitech-Z906) - for the inspiration