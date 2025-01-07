import os
import sys
# Ported from https://github.com/vtmattedi/ConsoleAdventure/blob/main/Game/Base/ConsoleHelp.js
# Play Console Adventure: www.consoleadventure.com
# ANSI escape codes: https://en.wikipedia.org/wiki/ANSI_escape_code
# Terminal Handling implementations


# ANSI control sequences => color = CSI n m
class ControlSequences:
    CSI = '\x1b['
    OSC = '\x1b]'
    Reset = '\x1b[0m'

# n number for colors and 8/24bit color constructor
class DefaultColors:
    BLACK = 30
    RED = 31
    GREEN = 32
    YELLOW = 33
    BLUE = 34
    MAGENTA = 35
    CYAN = 36
    WHITE = 37
    LIGHTBLACK_EX = 90
    LIGHTRED_EX = 91
    LIGHTGREEN_EX = 92
    LIGHTYELLOW_EX = 93
    LIGHTBLUE_EX = 94
    LIGHTMAGENTA_EX = 95
    LIGHTCYAN_EX = 96
    LIGHTWHITE_EX = 97
    BG_BLACK = 40
    BG_RED = 41
    BG_GREEN = 42
    BG_YELLOW = 43
    BG_BLUE = 44
    BG_MAGENTA = 45
    BG_CYAN = 46
    BG_WHITE = 47
    BG_RESET = 49

    @staticmethod
    def custom_colors(num, background=False):
        text = '38'
        if background:
            text = '48'
        if isinstance(num, list):
            if len(num) == 3:
                text += f';2;{num[0]};{num[1]};{num[2]}'
            else:
                return text + f';5;{num[0]}'
            return text
        else:
            return text + f';5;{num}'

# ANSI text decoration n
class Decorations:
    Bold = 1
    Dim = 2
    Italic = 3
    Underlined = 4
    Blink = 5
    Reverse = 7
    Strikethrough = 9
    no_underline = 24

# Custom console error
class ConsoleNotImplemented(Exception):
    def __init__(self):
        super().__init__("The ConsoleHelper was not properly implemented.")
        self.name = "ConsoleError"

# Abstract class
# Each system may have a different implementation
class ConsoleImplementation:
    def fillBar(self):
        raise ConsoleNotImplemented()

    def insert_color(self):
        raise ConsoleNotImplemented()

    def insert_format(self):
        raise ConsoleNotImplemented()

    def clear_screen(self):
        raise ConsoleNotImplemented()

    def clear_line(self):
        raise ConsoleNotImplemented()

    def clear_last_line(self):
        raise ConsoleNotImplemented()

    def getWidth(self):
        raise ConsoleNotImplemented()

    def show_cursor(self):
        raise ConsoleNotImplemented()

    def print(self):
        raise ConsoleNotImplemented()

    def setTitle(self):
        raise ConsoleNotImplemented()

# Singleton for most VTI terminals and OS usage
class BasicConsole(ConsoleImplementation):
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(BasicConsole, cls).__new__(cls, *args, **kwargs)
        return cls._instance

    def breakLine(self, text, width, ignorenl=False):
        if ignorenl:
            text = text.replace('\n', ' ')
        words = text.split(' ')
        lines = []
        line = ''

        for word in words:
            lineLength = self.getLineWidth(line)
            wordLength = self.getLineWidth(word)
            if lineLength + wordLength > width:
                lines.append(line)
                line = ''
            line += word + ' '
        lines.append(line)
        return '\n'.join(lines)

    def clear_screen(self):
        os.system('cls' if os.name == 'nt' else 'clear')

    def write(self, text):
        sys.stdout.write(text)

    def clear_line(self):
        self.write(ControlSequences.CSI + '2K')

    def clear_last_line(self, times=1):
        for _ in range(times):
            self.write('\x1b[1A')  # Move cursor up one line
            self.clear_line()  # Clear the entire line

    def getWidth(self):
        return os.get_terminal_size().columns

    def show_cursor(self, value=True):
        if value:
            self.write('\u001B[?25h')
        else:
            self.write('\u001B[?25l')

    def insert_color(self, color, text, oldColor=None):
        return ControlSequences.CSI + f'{color}m{text}' + ControlSequences.Reset

    def insert_format(self, format=None, text=''):
        if not text:
            return ''
        if ControlSequences.Reset in text:
            color = format.get('color', DefaultColors.WHITE)
            text = text.replace(ControlSequences.Reset, ControlSequences.CSI + f'{color}m')
        fmt = ''
        addSemi = False
        if format.get('color'):
            fmt += str(format['color'])
            addSemi = True
        if format.get('background'):
            if addSemi:
                fmt += ';'
            fmt += str(format['background'])
            addSemi = True
        if format.get('decoration'):
            decorationArray = format['decoration'] if isinstance(format['decoration'], list) else [format['decoration']]
            for item in decorationArray:
                if addSemi:
                    fmt += ';'
                fmt += str(item)
                addSemi = True
        return ControlSequences.CSI + fmt + f'm{text}' + ControlSequences.Reset

    def fillBar(self, percent, size, char, color, bg_color):
        if not isinstance(percent, (int, float)):
            raise ValueError("Percent must be a number")
        if not isinstance(size, int) or size < 1:
            raise ValueError("Size must be a positive integer")
        if not isinstance(char, str) or len(char) != 1:
            raise ValueError("Char must be exactly 1 char")

        percent = max(0, min(percent, 1))
        cut_off = round(percent * size)

        line = self.insert_color(color, char * cut_off) + self.insert_color(bg_color, char * (size - cut_off))
        return line

    def printOptions(self, options, selectIndex=0, config=None, vertical=False):
        res = ""
        padChar = ' '
        padding = padChar * 3
        if config and config.get('padding'):
            padding = padChar * config['padding']
        width = self.getWidth()
        totalLength = sum(len(item) for item in options) + len(padding) * len(options)
        if totalLength > width:
            padding = " " * 0
        for i, option in enumerate(options):
            line = f"  {option}  "
            if i == selectIndex:
                line = f"> {option} <"
            if vertical:
                res += self.hcenter(line, width, padChar) + '\n'
            else:
                res += line + padding
        res = self.hcenter(res, width)
        if self.getLineWidth(res) > width and not vertical:
            res = res[:width]

        res = res.replace(options[selectIndex], self.insert_format({'decoration': Decorations.Underlined}, options[selectIndex]))
        res = res.replace('>', self.insert_color(DefaultColors.YELLOW, '>'))
        res = res.replace('<', self.insert_color(DefaultColors.YELLOW, '<'))
        if config and isinstance(config.get('colors'), list):
            for item in config['colors']:
                res = res.replace(item['text'], self.insert_color(item['color'], item['text']))

        self.print(res)
        return res

    def hcenter(self, input, size, char=" ", mode=0):
        if not isinstance(input, str):
            return ""
        def centerLine(text):
            if not isinstance(text, str):
                return None
            start = mode != 1
            while self.getLineWidth(text) < size:
                if start:
                    text = char + text
                else:
                    text += char
                if mode == 0:
                    start = not start
            return text

        if '\n' in input:
            lines = input.split('\n')
            maxLength = max(self.getLineWidth(line) for line in lines)
            lines = [line.ljust(maxLength) for line in lines]
            lines = [centerLine(line) for line in lines]
            return '\n'.join(lines)

        return centerLine(input)

    def vcenter(self, input, verticalLength, horizontalLength, char=" ", mode=0):
        diff = verticalLength - len(input)
        center = mode == 2
        for _ in range(diff):
            if center:
                input.append(char * horizontalLength)
            else:
                input.insert(0, char * horizontalLength)
            if mode == 0:
                center = not center
        return input

    def merge(self, leftSprite, rightSprite, options=None):
        if not isinstance(leftSprite, str) or not isinstance(rightSprite, str):
            return None
        rightLines = rightSprite.split('\n')
        leftLines = leftSprite.split('\n')
        maxLengthLeft = max(self.getLineWidth(line) for line in leftLines)
        maxLengthRight = max(self.getLineWidth(line) for line in rightLines)

        if options:
            if options.get('left') and options['left'].get('align'):
                if options['left']['align'] == 'hcenter':
                    leftLines = [self.hcenter(line, maxLengthLeft, ' ') for line in leftLines]
                elif options['left']['align'] == 'vcenter':
                    self.vcenter(leftLines, len(rightLines), maxLengthLeft, ' ')
            if options.get('right') and options['right'].get('align'):
                if options['right']['align'] == 'hcenter':
                    rightLines = [self.hcenter(line, maxLengthRight, ' ') for line in rightLines]
                elif options['right']['align'] == 'vcenter':
                    self.vcenter(rightLines, len(leftLines), maxLengthRight, ' ')

        if len(leftLines) < len(rightLines):
            self.vcenter(leftLines, len(rightLines), maxLengthLeft, ' ', 2)

        mergedLines = '\n'.join(
            leftLine.ljust(maxLengthLeft) + ' ' * (options.get('padding', 4)) + (rightLines[i] if i < len(rightLines) else ' ' * maxLengthRight)
            for i, leftLine in enumerate(leftLines)
        )

        if options and isinstance(options.get('colors'), list):
            for item in options['colors']:
                if isinstance(item['text'], list):
                    for text in item['text']:
                        mergedLines = mergedLines.replace(text, self.insert_color(item['color'], text))
                else:
                    mergedLines = mergedLines.replace(item['text'], self.insert_color(item['color'], item['text']))

        return mergedLines

    def paintSprite(self, sprite, hcutoff, color):
        sprite_array = sprite.split('\n')
        res = ''
        for element in sprite_array:
            res += self.insert_color(color, element[:hcutoff]) + element[hcutoff:] + '\n'
        return res

    def getLineWidth(self, text):
        if not text:
            return 0
        line = text
        while ControlSequences.CSI in line:
            csi_index = line.index(ControlSequences.CSI)
            end_csi = line.index('m', csi_index)
            line = line[:csi_index] + line[end_csi + 1:]
        return len(line)

    def pressSpace(self, phrase="to continue"):
        width = self.getWidth()
        final_phrase = f"Press Spacebar {phrase}."
        final_phrase = self.hcenter(final_phrase, width)
        final_phrase = final_phrase.replace('Spacebar', self.insert_format({'color': DefaultColors.YELLOW, 'decoration': [Decorations.Underlined, Decorations.Blink]}, "Space"))
        self.print(final_phrase)

    def print(self, text=''):
        if text is None:
            self.write('\n')
        else:
            self.write(text + '\n')

    def setTitle(self, title):
        self.write(f'\x1b]2;{title}\x1b\x5c')
