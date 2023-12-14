import time
import serial
import numpy as np
import matplotlib
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

ser = serial.Serial("/dev/ttyUSB0", 9600)

def getfigax():
    fig, (axL, axR) = plt.subplots(ncols=2, tight_layout=True)

    axL.set(xlabel="Time (s)", ylabel="Temperature (C)")
    titL = axL.set_title("N=0")
    titR = axR.set_title("AVG")
    lineL, = axL.plot([], [], lw=lwL)
    lineR, = axR.plot([], [], lw=lwR)

    return fig, axL, axR, titL, titR, lineL, lineR

##COMPLETE REPLOT ###

""" fig, (axL, axR) = plt.subplots(ncols=2, tight_layout=True)

for frame in range(N):
    ser.write(b'g')
    arduinoData_string = ser.readline().decode('ascii')
    arduinoData_array = [float(idx) for idx in arduinoData_string.split()]
    dataList = np.insert(dataList, frame, arduinoData_array[0])
    y = dataList[:50]
    
    axL.clear()
    axR.clear()
    dframe[frame] = time.perf_counter()
    y2 = 1000*np.diff(dframe)
    y2_avg = np.nanmean(y2)
    axL.set_title(f"N={frame}")
    axL.grid()
    axL.plot(t[frame], y, lw=lwL)

    axR.set_title(f"AVG = {y2_avg:.1f} ms  ({1000/y2_avg:.1f}) fps")
    axR.set_xlim(axRxlims)
    axR.set_ylim(axRylims)
    axR.grid()
    axR.plot(t[frame], y2,  lw=lwR)
    fig.canvas.draw()

    plt.pause(0.00001)
 """
##PARTIAL REPLOT ###

""" def update(frame, frame_times, dataList, ser):
    frame_times[frame] = time.perf_counter()
    y2 = 1000*np.diff(frame_times)
    y2_avg = np.nanmean(y2)

    ser.write(b'g')
    arduinoData_string = ser.readline().decode('ascii')
    arduinoData_array = [float(idx) for idx in arduinoData_string.split()]
    dataList = np.append(dataList, arduinoData_array[0])
    dataList = dataList[-50:]
    y[frame] = dataList
    print(y[frame])
    lineL.set_data(t[frame], dataList)
    lineR.set_data(t[frame], y2)
    titL.set_text(f"N={frame}")
    titR.set_text(f"AVG = {y2_avg:.1f} ms ({1000/y2_avg:.1f} fps)")
    axL.autoscale_view(True, True)
    axL.relim()
    fig.canvas.flush_events()
    fig.canvas.draw()

    return dataList

fig, axL, axR, titL, titR, lineL, lineR = getfigax()
fig.suptitle("ROUV Temperature")

time.sleep(1)

ani = animation.FuncAnimation(fig, update, interval=100, fargs=(dframe, dataList, ser), repeat=False, frames=list(range(N))) """

### INTELLIGENT AX UPDATE ###
""" def update(frame, frame_times, dataList, ser):
    frame_times[frame] = time.perf_counter()
    y2 = 1000*np.diff(frame_times)
    y2_avg = np.nanmean(y2)
    if frame > 1:
        dataList = y[frame-1]
    ser.write(b'g')
    arduinoData_string = ser.readline().decode('ascii')
    arduinoData_array = [float(idx) for idx in arduinoData_string.split()]
    dataList = np.insert(dataList, frame, arduinoData_array[2])
    dataList = dataList[-50:]
    y[frame] = dataList
    print(y[frame])
    lineL.set_data(t[frame], dataList)
    lineR.set_data(t[frame], y2)
    titL.set_text(f"N={frame}")
    titR.set_text(f"AVG = {y2_avg:.1f} ms ({1000/y2_avg:.1f} fps)")
    # axL.autoscale_view(True, True)
    # axL.relim()
    # fig.canvas.flush_events()
    fig.canvas.draw()

    rescale = False

    if y[frame, frame] < axL.get_ylim()[0]:
        axL.set_ylim(y[frame, frame] - 0.1, axL.get_ylim()[1])
        rescale = True
    if y[frame, frame] > axL.get_ylim()[1]:
        axL.set_ylim(axL.get_ylim()[0], y[frame, frame] + 0.1)
        rescale = True
    if t[frame, frame] > axL.get_xlim()[1]:
        axL.set_xlim(axL.get_xlim()[0], axL.get_xlim()[1] + N/5)
        rescale = True
    
    if fame == len(t) - 1:
        rescale = True
    
    if rescale:
        fig.canvas.draw()

    return dataList

fig, axL, axR, titL, titR, lineL, lineR = getfigax()
fig.suptitle("ROUV Temperature")

time.sleep(1)

ani = animation.FuncAnimation(fig, update, interval=100, fargs=(dframe, dataList, ser), repeat=False, frames=list(range(N))) """
def animate(i, datalist, dataNTU, ser): 
    ser.write(b'g') 
    arduinodata_string = ser.readline().decode('ascii') 
    try:
        arduinodata_array = [float(idx) for idx in arduinodata_string.split()]
        datalist.append(int(arduinodata_array[1]))
        dataNTU.append(int(arduinodata_array[2]))
    except:
        pass

    datalist = datalist[-50:]
    dataNTU = dataNTU[-50:]
    
    ax.clear()
    ax.plot(datalist)

    ay.clear()
    ay.plot(dataNTU)
    
    ax.set_ylim([-10, 6])
    ax.set_title("MPX")
    ax.set_ylabel("m")
    
    ay.set_ylim([-10, 3000])
    ay.set_title("TDS")
    ay.set_ylabel("ppm")

datalist = []
dataNTU = []
                                                        
fig = plt.figure()
ax = fig.add_subplot(121)
ay = fig.add_subplot(122)

ser = serial.Serial("/dev/ttyUSB0", 9600)
time.sleep(1)

ani = animation.FuncAnimation(fig, animate, frames=100, fargs=(datalist, dataNTU, ser), interval=100)

plt.show()
ser.close()
