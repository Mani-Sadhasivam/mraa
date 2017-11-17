add_executable (gpio_edison gpio_edison.c)
target_link_libraries (gpio_edison mraa)

if (ONEWIRE)
  add_executable (uart_ow uart_ow.c)
  target_link_libraries (uart_ow mraa)
endif ()

if (FIRMATA)
  add_executable (firmata_curie_imu firmata_curie_imu.c)
  add_executable (i2c_firmata i2c_firmata.c)
  target_link_libraries (firmata_curie_imu mraa)
  target_link_libraries (i2c_firmata mraa)
endif()
