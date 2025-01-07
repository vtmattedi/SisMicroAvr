import sys
import Basic_Console;
console = Basic_Console.BasicConsole()

def renderOptions(options, index):
    optstr = ""
    for i in range(len(options)):
        phrase = f" ${options[i]} "
        if i == index:
            phrase = f">${options[i]}<"
            phrase = console.insert_format(phrase, Basic_Console.Decorations.Bold)     
        optstr += phrase
        optstr += " "
    return optstr[:-1]


class SyncEngine:
    def __init__(self):
        sys.stdin.isatty = True
        self.max_index = 0
        self.index = 0
        while True:
            a = sys.stdin.read()
            
            #console.clear_last_line()
            #console.write(a)
    
    def render(self):
        console.clear()
        console.write("Sync Engine")
        console.write("1. Sync")
        console.write("2. Exit")
        console.write("Enter your choice: ")
        choice = console.read()
        if choice == "1":
            self.sync()
        elif choice == "2":
            return
        else:
            console.write("Invalid choice")
            self.render()

