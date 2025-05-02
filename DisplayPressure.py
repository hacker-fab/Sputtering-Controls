import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

# === SERIAL SETUP ===
ser = serial.Serial('/dev/tty.usbmodem1101', 9600)
time.sleep(2)  # Let Arduino reset

# === HELP MENU ===
def print_help():
    print("\n=== Help Menu ===")
    print("This program runs a PI control loop to achieve a desired pressure")
    print("inside the sputtering chamber.\n")
    print("Press the button on Arduino to shut off\n")
    print("Input Instructions:")
    print("- Enter the decimal value and the exponent separately when prompted.")
    print("- Example: For 5.4e-4 (hPa), enter:")
    print("    Desired Pressure Decimal: 5.4")
    print("    Desired Pressure Magnitude: -4")
    print("\nAvailable Commands:")
    print("  h : Show this help menu")
    print("  d : Start with dynamic pressure axis scaling (zoomed view)")
    print("  f : Start with fixed pressure axis scaling (full view)")
    print("=================\n")

print_help()

# === USER INPUT FOR PRESSURE ===
frac_str = input("Enter pressure decimal (e.g., 5.4): ").strip()
exp_str = input("Enter pressure exponent (e.g., -4): ").strip()
command = f"PSET,{frac_str},{exp_str}\n"
ser.write(command.encode())
print(f"Sent: {command.strip()}")

# === COMMAND SELECTION ===
use_dynamic_scaling = None
while True:
    cmd = input("Enter command [h/d/f]: ").strip().lower()
    if cmd == 'h':
        print_help()
    elif cmd == 'd':
        use_dynamic_scaling = True
        ser.write(b"s\n")
        break
    elif cmd == 'f':
        use_dynamic_scaling = False
        ser.write(b"s\n")
        break
    else:
        print("Invalid input. Type 'h' for help.")

# === PLOTTING ===
time_data = []
desired_data = []
measured_data = []

fig, ax = plt.subplots()

def update(frame):
    try:
        line = ser.readline().decode('utf-8').strip()
        print(line)

        if line.startswith("DATA,"):
            _, t_str, desired_str, measured_str = line.split(',')
            t = float(t_str) / 1000.0
            desired = float(desired_str)
            measured = float(measured_str)

            time_data.append(t)
            desired_data.append(desired)
            measured_data.append(measured)

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

            if use_dynamic_scaling:
                # Zoomed-in dynamic view
                ax.set_xlim(left=max(0, time_data[-1] - 30), right=time_data[-1] + 2)
                all_pressures = desired_data + measured_data
                min_y = min(all_pressures)
                max_y = max(all_pressures)
                y_margin = 0.1 * (max_y - min_y) if max_y > min_y else 1e-4
                ax.set_ylim(bottom=max(0, min_y - y_margin), top=max_y + y_margin)

            else:
                # Full view - keep limits from earliest to latest and all pressure values
                ax.set_xlim(left=0, right=max(30, time_data[-1] + 2))
                all_pressures = desired_data + measured_data
                min_y = min(all_pressures)
                max_y = max(all_pressures)
                y_margin = 0.1 * (max_y - min_y) if max_y > min_y else 1e-4
                ax.set_ylim(bottom=max(0, min_y - y_margin), top=max_y + y_margin)

            # Annotate current values
            latest_text = f"Desired: {desired:.2e}\nMeasured: {measured:.2e}"
            ax.annotate(
                latest_text,
                xy=(1.0, 1.0),
                xycoords='axes fraction',
                ha='right',
                va='top',
                fontsize=10,
                color='blue',
                bbox=dict(boxstyle='round,pad=0.3', fc='white', ec='gray', alpha=0.7)
            )

    except ValueError:
        pass

ani = animation.FuncAnimation(fig, update, interval=100, cache_frame_data=False)
plt.tight_layout()
plt.show()
