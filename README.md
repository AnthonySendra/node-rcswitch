node-rcswitch
=============

[![NPM version](https://badge.fury.io/js/rcswitch.svg)](http://badge.fury.io/js/rcswitch)

Node bindings for the [rcswitch RaspberryPi port](https://github.com/r10r/rcswitch-pi).

It should be compatible with Node 0.8, 0.10 and 0.12 (tested only on 0.10 and 0.12). Successfully tested on Raspbian Wheezy up-to-date as of 2015-02-28.

## Requirements

* [WiringPi must be installed](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) in order to compile.
* Both the data and the power Pins of the 315/433Mhz receiver/emitter must be connected to the RPi. Note the number of the WiringPi data Pin. (see http://wiringpi.com/pins/)
*    The user must have permission to access the Raspberry Pi GPIO pins. There are two options:
    1. Start node with root access (not a good idea and can be unreliable).
    2. The better method is to expose the GPIO pins in `/sys/class/gpio` using the [gpio tool](https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/) that comes with WiringPi. Example:
    ```
    sudo gpio export 27 in
    ```
    And give the user permission to access the GPIO pins:
    ```
    sudo chgrp -R dialout /sys/class/gpio/gpio27/
    sudo chmod -R g+rw /sys/class/gpio/gpio27/
    ```
    The above 3 lines can be added to `/etc/rc.local` to allow the GPIO pins to be exposed on start-up.

## Usage
```javascript
var rcswitch = require('rcswitch'); // Might throw an error if wiring pi init failed, or exit process if no root (must work on that)

rcswitch.enableTransmit(0); // Use data Pin 0
rcswitch.switchOn("10110", 1); // Switch on the first unit of 10110 (code 1x23x) group
rcswitch.switchOff("11000", 2); // Switch off the second unit of 11000 (code 12xxx) group
```

```javascript
var rcswitch = require('rcswitch');

var pin = 27;

if(rcswitch.enableReceive(pin)) {
  setInterval(function() {
      if (rcswitch.available()) {
          console.log(rcswitch.getReceivedValue());
          rcswitch.resetAvailable();
      }
  }, 1000);
}
```

## API

### Configuration

#### rcswitch.enableTransmit(`pin`)

Enable transmission on the given pin (make it an OUTPUT). Must be called before any other functions.

* `pin` - (Number) data Pin to use following [the WiringPi schema](http://wiringpi.com/pins/)

Return true if `pin` is an integer, false otherwise.

#### rcswitch.disableTransmit()

Disable transmission.

Return true.


#### rcswitch.enableReceive(`pin`)

Enable receive on the given pin (make it an INPUT). Must be called before any other functions.

* `pin` - (Number) data Pin to use following [the WiringPi schema](http://wiringpi.com/pins/)

Return true if `pin` is an integer, false otherwise.

#### rcswitch.disableReceive()

Disable receive.

Return true.

### Receive

#### rcswitch.available()

Returns true if data has been received and is waiting to be retrieved from the buffer.

#### rcswitch.resetAvailable()

Resets the data available flag.

Returns true;

#### rcswitch.getReceivedValue()

Fetches buffered received data.

Returns value as a Uint32.

### Transmit - Type A

![Type A switch](https://raw.github.com/marvinroger/node-rcswitch/master/img/type_a.jpg "Type A switch")

#### rcswitch.switchOn(`group`, `switch`)

Switch a remote switch on (Type A with 10 pole DIP switches).

* `group` - (String) code of the switch group (refers to DIP switches 1, 2, 3, 4 and 5 where "1" = on and "0" = off - e.g. if all DIP switches are on it's "11111")
* `switch` - (Number) switch number (can be 1 (if DIP switch A is on), 2 (if DIP switch B is on) and so on until 4)

Return true.

#### rcswitch.switchOff(`group`, `switch`)

Switch a remote switch off (Type A with 10 pole DIP switches).

* `group` - (String) code of the switch group (refers to DIP switches 1, 2, 3, 4 and 5 where "1" = on and "0" = off - e.g. if all DIP switches are on it's "11111")
* `switch` - (Number) switch number (can be 1 (if DIP switch A is on), 2 (if DIP switch B is on) and so on until 4)

Return true.

### Transmit - Type B

![Type B switch](https://raw.github.com/marvinroger/node-rcswitch/master/img/type_b.jpg "Type B switch")

#### rcswitch.switchOn(`group`, `switch`)

Switch a remote switch on (Type B with two rotary/sliding switches).

* `group` - (Number) group (can be 1, 2, 3, 4)
* `switch` - (Number) switch (can be 1, 2, 3, 4)

Return true.

#### rcswitch.switchOff(`group`, `switch`)

Switch a remote switch off (Type B with two rotary/sliding switches).

* `group` - (Number) group (can be 1, 2, 3, 4)
* `switch` - (Number) switch (can be 1, 2, 3, 4)

Return true.

### Transmit - Type C

#### rcswitch.switchOn(`family`, `group`, `switch`)

Switch a remote switch on (Type C Intertechno).

* `family` - (String) familycode (can be a, b, c, d, e, f)
* `group` - (Number) group (can be 1, 2, 3, 4)
* `switch` - (Number) switch (can be 1, 2, 3, 4)

Return true.

#### rcswitch.switchOff(`family`, `group`, `switch`)

Switch a remote switch off (Type C Intertechno).

* `family` - (String) familycode (can be a, b, c, d, e, f)
* `group` - (Number) group (can be 1, 2, 3, 4)
* `switch` - (Number) switch (can be 1, 2, 3, 4)

Return true.

### Other

#### rcswitch.send(`codeWord`)

Send raw codeword.

* `codeword` - (String) codeword

Return true.
