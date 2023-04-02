
class Emitter:
    def __init__(self, fullPath):
        self.fullPath = fullPath
        self.header = ""
        self.code = ""
        self.lvl = 0

    def increaseLvl(self):
        self.lvl+=1

    def decreaseLvl(self):
        self.lvl-=1
        self.lvl = max(self.lvl, 0)

    def emit(self, code, push=False):
        if push:
            for i in range (self.lvl):
                self.code+="\t"
        self.code += code

    def emitLine(self, code, push=False):
        if push:
            for i in range (self.lvl):
                self.code+="\t"
        self.code += code + '\n'

    def headerLine(self, code):
        self.header += code + '\n'

    def writeFile(self):
        with open(self.fullPath, 'w') as outputFile:
            outputFile.write(self.header + self.code)