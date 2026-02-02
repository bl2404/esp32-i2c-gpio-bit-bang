# esp32-i2c-gpio-bit-bang
Send/Receive information on I2C protocol using general purpose pins only

- project is using internal pull-up ressistors (are set in init() method) there was a problem with external resistors and it was not working as expected.
- https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf - i2c protocol described for MPU6050 
- https://www.ti.com/lit/an/sbaa565/sbaa565.pdf?ts=1765083604931 - basics of I2c in general page 13 example
- https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf - registers described

- paste `idf.py monitor` output to `plotter.py` to see the chart (kind of osciloscope)

- circuit:
![c1](./images/circuit1.pngi =x25)
![c2](./images/circuit2.png =x250)
