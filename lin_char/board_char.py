import serial
import sys
import glob
import time

BAUD = 115200

def get_portlist():
    if sys.platform.startswith('win'):
        ports = ['COM' + str(i + 1) for i in range(256)]

    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this is to exclude your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')

    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')

    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    if len(result) < 1:
        print("No serial ports found. Reconnect and try again")
    return result

def select_port():
    portlist = get_portlist()

    # list the ports seen on the machine
    print("You have the following ports:")
    counter = 1
    for p in portlist:
        print("%i - %s"%(counter,p))
        counter = counter + 1

    # let the user choose the correct port
    index = int(input("Select the index of the port corresponding to the Arduino: "))
    while index > len(portlist):
        print("Please select the correct index")
        index = int(input("Select the index of the port corresponding to the Arduino: "))

    # connect to the port
    return portlist[index-1]

def serial_write(ser, data):
    if sys.platform.startswith('win'):
        ser.write([data ,])
    else:
        ser.write(data)

"""
def check_arduino(arduino):
    start = time.time()
    while (True):
        if(arduino.in_waiting):
            return True
        if (time.time() - start > 0.5):
            return False
"""

def start_transaction(arduino, payload):
    for _ in range(1000):
        serial_write(arduino, payload)
        time.sleep(0.001)

def main():
    arduino = serial.Serial(select_port(), BAUD, timeout=1)
    file_name = input("Input file name to save S sweep: ")
    csv = open(file_name, 'w')
    csv.write("SWEEPING S\n")
    csv.write("Step, Positive, Negative, Diff\n")
    start_transaction(arduino, b'S')
    while (True):
        data_in = str(arduino.readline())
        try:
            p_index = data_in.index('P')
            n_index = data_in.index('N')
            d_index = data_in.index('D')
            write = data_in[3:p_index]+","+data_in[p_index+1:n_index]+","+data_in[n_index+1:d_index]+","+data_in[d_index+1:-5]+"\n"
            csv.write(write)
        except ValueError:
            csv.close()
            break

    file_name = input("Input file name to save B sweep: ")
    csv = open(file_name, 'w')
    csv.write("SWEEPING B\n")
    csv.write("Step, Positive, Negative, Diff\n")
    start_transaction(arduino, b'B')
    while (True):
        data_in = str(arduino.readline())
        try:
            p_index = data_in.index('P')
            n_index = data_in.index('N')
            d_index = data_in.index('D')
            write = data_in[3:p_index]+","+data_in[p_index+1:n_index]+","+data_in[n_index+1:d_index]+","+data_in[d_index+1:-5]+"\n"
            csv.write(write)
        except ValueError:
            csv.close()
            break

    return 1

if __name__ == '__main__':
    main()
