
#Make sure to install the following libraries
#pip install pyserial
#pip install pynput
import serial.tools.list_ports as port_list

import CA_Engine ;
import Basic_Console;
console = Basic_Console.BasicConsole()
from pynput.keyboard import Key, Listener
index = 0
_ports = list(port_list.comports())
ports = [port.device for port in _ports]
console.clear_screen()
def printOptions(options, index, info = None):
    console.clear_last_line(2)
    console.printOptions(options, index)
    if info == None:
        info = _ports[index].description
    console.print(info)
printOptions(ports, index)

def on_release(key):
    global index
    
    if key == Key.left:
        index = max(index - 1, 0)
       # console.print(CA_Engine.renderOptions(ports, index))
    elif key == Key.right:
        index = min(index + 1, len(ports) - 1)
       # console.print(CA_Engine.renderOptions(ports, index))
    printOptions(ports, index)
    if key == Key.esc:
        # Stop listener
        return False

# Collect events until released
with Listener(
        on_release=on_release) as listener:
    listener.join()

engine = CA_Engine.SyncEngine()

