# Message Formatting Spec

## Bluetooth
General Format is [Header (3)][Action(4)][Parameters (optional, comma separated)]

`RTS` -> Header

    `BEGN` -> Start transmission
        (No parameters)

    `STAT` -> Send status message
        (No Parameters)

    `MOVD` -> Move Digital
        `F` -> Forward
        `B` -> Backward
        `L` -> Turn left
        `R` -> Turn Right

    `MOVA` -> Move Analogue
        `x`,`y` -> Movement parameters comma separated (+x is right, +y is forward, normalized, [-1,+1])

    (TODO: Turret movement, sleep, etc.)