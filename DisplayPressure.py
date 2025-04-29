import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

#Need to update with actual port
ser = serial.Serial('/dev/tty.usbserial-XXXX', 9600)

time_data = []
desired_data = []
measured_data = []

fig, ax = plt.subplots()

def update(frame):
    try:
        line = ser.readline().decode('utf-8').strip()
        
        # Only process lines that start with "DATA,"
        if line.startswith("DATA,"):
            _, t_str, desired_str, measured_str = line.split(',')

            t = float(t_str) / 1000.0  # ms to seconds
            desired = float(desired_str)
            measured = float(measured_str)

            time_data.append(t)
            desired_data.append(desired)
            measured_data.append(measured)

            # Keep last 1000 samples
            if len(time_data) > 1000:
                time_data.pop(0)
                desired_data.pop(0)
                measured_data.pop(0)

            ax.clear()
            ax.plot(time_data, desired_data, label='Desired Pressure')
            ax.plot(time_data, measured_data, label='Measured Pressure')
            ax.set_xlabel("Time (s)")
            ax.set_ylabel("Pressure")
            ax.set_title("Pressure vs Time")
            ax.legend()
            ax.grid(True)

    except ValueError:
        pass  # ignore any malformed lines

ani = animation.FuncAnimation(fig, update, interval=100)
plt.tight_layout()
plt.show()
