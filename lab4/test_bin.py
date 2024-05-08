import struct

f = open("lab4_points.txt", mode="rb")
 
# Reading file data with read() method


num = struct.unpack('d'*11, f.read(8 * 11))
print(num)


 
 
# Closing the opened file
f.close()
