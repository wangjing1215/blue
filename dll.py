from ctypes import *
pDll = CDLL(r"C:\Users\NING MEI\PycharmProjects\pythonProject\dll\Dll1.dll")

re = pDll.find()

print(re)