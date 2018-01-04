from datetime import datetime
import ctypes

so = ctypes.cdll.LoadLibrary
lib = so("./test.so")
t = datetime.utcnow()
lib.wrapper_genproof()
t = datetime.utcnow()-t
print t.total_seconds()*1000

