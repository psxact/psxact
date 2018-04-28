
# Observations



# Miscellaneous

The cd-rom motor can exist in the following states:

1. Off
2. On
  * Speed Increasing
  * Speed Decreasing
  * Stable

When the motor is on, it is continuously adjusting its speed
to match an arbitrary "target" speed.

The "target" speed changes based on which sector is trying to
be read, and is calculated/adjusted by the controller.

When no sector is actively being read, the controller can
choose to turn the motor off, or keep it on. If kept on, the
motor will move into the "stable" state and just hold its
current speed.

Deceleration can be achieved in two ways:
1. Using brakes to slow down (faster)
2. Using friction to slow down (slower)

```
Controller ----> Laser Assembly -+->
    |                            |
    +-> Motor -> Spindle --------+
    |               ^
    |               |
    +-> Brakes -----+
```
