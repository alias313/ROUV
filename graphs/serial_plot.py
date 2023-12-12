import time
import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

plt.rcParams["figure.figsize"] = (12, 4)

N = 50

lwL = 2
lwR = 1
axRxlims = [0, N]
axRylims = [0, 600]

_t = np.linspace(0, N)
_y = np.linspace(-10, 200)
dframe = np.full(N+1, fill_value=np.nan, dtype=np.float64)

t = np.tile(_t, (N, 1))
y = np.tile(_y, (N, 1))

for i in range(N):
    t[i, i+1:] = np.nan
    y[i, i+1:] = np.nan

dataList = np.zeros(50, dtype=np.float64)

ser = serial.Serial("COM8", 9600)                       # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate

def getfigax():
    fig, (axL, axR) = plt.subplots(ncols=2, tight_layout=True)

    axL.set(xlabel="Time (s)", ylabel="Temperature (C)")
    titL = axL.set_title("N=0")
    titR = axR.set_title("AVG")
    lineL, = axL.plot([], [], lw=lwL)
    lineR, = axR.plot([], [], lw=lwR)

    return fig, axL, axR, titL, titR, lineL, lineR

### COMPLETE REPLOT ###

# fig, (axL, axR) = plt.subplots(ncols=2, tight_layout=True)

# for frame in range(N):
#     ser.write(b'g')
#     arduinoData_string = ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string
#     arduinoData_array = [float(idx) for idx in arduinoData_string.split()]
#     dataList = np.insert(dataList, frame, arduinoData_array[0])              # Add to the list holding the fixed number of points to animate
#     y = dataList[:50]
    
#     axL.clear()
#     axR.clear()
#     dframe[frame] = time.perf_counter()
#     y2 = 1000*np.diff(dframe)
#     y2_avg = np.nanmean(y2)
#     axL.set_title(f"N={frame}")
#     axL.grid()
#     axL.plot(t[frame], y, lw=lwL)

#     axR.set_title(f"AVG = {y2_avg:.1f} ms  ({1000/y2_avg:.1f}) fps")
#     axR.set_xlim(axRxlims)
#     axR.set_ylim(axRylims)
#     axR.grid()
#     axR.plot(t[frame], y2,  lw=lwR)
#     fig.canvas.draw()

#     plt.pause(0.00001)

### PARTIAL REPLOT ###

def update(frame, frame_time):
    frame_times[frame] = time.perf_counter()
    y2 = 1000*np.diff(frame_times)
    y2_avg = np.nanmean(y2)

    ser.write(b'g')
    arduinoData_string = ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string
    arduinoData_array = [float(idx) for idx in arduinoData_string.split()]
    dataList = np.insert(dataList, frame, arduinoData_array[0])              # Add to the list holding the fixed number of points to animate
    y = dataList[:50]

    lineL.set_data(t[frame], y)
    lineR.set_data(t[frame], y2)
    titL.set_text(f"N={frame}")
    titR.set_text(f"AVG = {y2_avg:.1f} ms ({1000/y2_avg:.1f} fps)")
    axL.autoscale_view(True, True)
    axL.relim()
    fig.canvas.flush_events()
    fig.canvas.draw()

fig, axL, axR, titL, titR, lineL, lineR = getfigax()
fig.suptitle("ROUV Temperature")

ani = animation.FuncAnimation(fig, update, interval=100, fargs=(dframe,), repeat=False, frames=list(range(N)))

# def animate(i, dataList, ser):
#     # ser.write(b'g')                                     # Transmit the char 'g' to receive the Arduino data point
#     arduinoData_string = ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string
#     arduinoData_array = [float(idx) for idx in arduinoData_string.split()]
#     dataList.append(arduinoData_array[0])              # Add to the list holding the fixed number of points to animate


#     dataList = dataList[-50:]                           # Fix the list size so that the animation plot 'window' is x number of points
    
#     ax.clear()                                          # Clear last data frame
#     ax.plot(dataList)                                   # Plot new data frame
    
#     ax.set_ylim([-10, 10])                              # Set Y axis limit of plot
#     ax.set_title("Arduino Data")                        # Set title of figure
#     ax.set_ylabel("Value")                              # Set title of y axis 

# dataList = []                                           # Create empty list variable for later use
                                                        
# fig = plt.figure()                                      # Create Matplotlib plots fig is the 'higher level' plot window
# ax = fig.add_subplot(111)                               # Add subplot to main fig window

# ser = serial.Serial("COM8", 9600)                       # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate
# time.sleep(2)                                           # Time delay for Arduino Serial initialization 

#                                                         # Matplotlib Animation Fuction that takes takes care of real time plot.
#                                                         # Note that 'fargs' parameter is where we pass in our dataList and Serial object. 
# ani = animation.FuncAnimation(fig, animate, frames=100, fargs=(dataList, ser), interval=100) 

plt.show()                                              # Keep Matplotlib plot persistent on screen until it is closed
# ser.close()                                             # Close Serial connection when plot is closed
