# ROUV
Firmware for Remotely Operated Underwater Vehicle university project
TODO:
- Captain mode (Electronically drive ROUV with drivers), activate with 'c' input
    - Motor commands (On, duty_cycle, off)
        - Use keyboard layout for percentages:
            - qwer (25, 50, 75, 100) left motor
            - ftyh (25, 50, 75, 100) center motor
            - uiop (25, 50, 75, 100) right motor
            - k, a (clocKwise, Anticlockwise)
            - qk => left motor 25% clockwise
        - Write to motor: 
            - l30k => left motor 30% clockwise
            - r49a => righ motor 49% anticlockwise
            - c99k => center motor 99% clockwise
    - Autopilot (replay saved commands in buffer)