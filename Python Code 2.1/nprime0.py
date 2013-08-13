'''
calculates nprime0 

Created on 2013-7-8

@author: fatestudio
'''
n0 = 0x1622bd795fec898f
W = 64

y1 = 1
y2 = 0
for i in range(2, W + 1):
    if(2 ** (i - 1) < ((n0 * y1) % (2 ** i))):
        y2 = y1 + 2 ** (i - 1) 
    else:
        y2 = y1
    y1 = y2
    y2 = 0
    print("y1:")
    print(hex(y1))
    print("y2:")
    print(hex(y2))
    print("2**i:")
    print(hex(2**(i)))
    print("2**(i-1):")
    print(hex(2**(i-1)))
    print
    
print(hex(y1))
print(hex(2 ** W - y1))