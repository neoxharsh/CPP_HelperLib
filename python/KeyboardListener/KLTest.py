import ctypes

lib = ctypes.WinDLL("libKeyboardListenerPLIB",winmode=0)


class KL(object):

    def __init__(self) -> None:
        lib.KeyboardListener_new.argtypes = []
        lib.KeyboardListener_new.restype = ctypes.c_void_p

        lib.KeyboardListener_setHotKey.argtypes = [ctypes.c_void_p]
        lib.KeyboardListener_setHotKey.restype = ctypes.c_void_p

        lib.KeyboardListener_startHotKeyListener.argtypes = [ctypes.c_void_p]
        lib.KeyboardListener_startHotKeyListener.restype = ctypes.c_void_p

        self.CALLBACK = ctypes.CFUNCTYPE(ctypes.c_void_p)
        lib.KeyboardListener_setCallbackFunction.argtypes = [ctypes.c_void_p, self.CALLBACK]
        lib.KeyboardListener_setCallbackFunction.restype = ctypes.c_void_p

        self.obj = lib.KeyboardListener_new()
    
    def setHotKey(self):
        lib.KeyboardListener_setHotKey(self.obj)
    
    def startHotKeyListener(self):
        lib.KeyboardListener_startHotKeyListener(self.obj)

    def setCallbackFunction(self, fun):
        lib.KeyboardListener_setCallbackFunction(self.obj,fun)
    


if __name__ == "__main__":    
    kl = KL()
    @kl.CALLBACK
    def p():
        print("This is")
    kl.setHotKey()
    kl.setCallbackFunction(p)
    kl.startHotKeyListener()
    input("Hello")