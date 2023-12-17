# ROUV
Firmware for Remotely Operated Underwater Vehicle university project 
TODO:
- Captain mode (Electronically drive ROUV with drivers)
    - Motor commands (On, duty_cycle, off)
        - Use keyboard layout for percentages:
            - a-q-w-e-r (0, 25, 50, 75, 100) left motor
            - s-f-t-y-h (0, 25, 50, 75, 100) center motor
            - d-u-i-o-p (0, 25, 50, 75, 100) right motor
            - k-j (clockwise, anticlockwise) 
            
            Example:
            - qk => left motor 25% clockwise
    - Autopilot (replay saved commands in buffer)
