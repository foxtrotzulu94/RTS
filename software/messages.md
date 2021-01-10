# Message Formatting Spec

## Bluetooth
General Format is [Header (3)][Action(4)][Parameters (optional, comma separated)]

`RTS` -> Header

    `BEGN` -> Start transmission
        (No parameters)

    `STAT` -> Send status message
        (No Parameters)

    `CTRL` -> Change control variables
        `Var`, `Value` -> Variable name + value

    `MOVD` -> Move Digital
        `F` -> Forward
        `B` -> Backward
        `L` -> Turn left
        `R` -> Turn Right

    `MOVA` -> Move Analogue
        `x`,`y` -> Movement parameters comma separated (+x is right, +y is forward, normalized, [-1,+1])

    `STOP` -> Stop all movement
        (no parameters)

    `TOUT` -> Sets the timeout for the onboard watchdog
        `n` -> time in miliseconds

    `SLEP` -> Makes the device sleep until "BOOT" button is pressed
        (no parameters)

    `HALT` -> Stops + sleep immediately (??)

    `TURR` -> Issue turret movement
        `ANGL` -> Angle at which to move. Turret is aiming forward at 0. Ranges -90,+90

    (TODO: Turret movement, sleep, etc.)