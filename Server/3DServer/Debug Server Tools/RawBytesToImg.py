
# import cv2
import numpy as np
import os
from PIL import Image 

# # Make an array of 120,000 random bytes.
# randomByteArray = bytearray(os.urandom(120000))
# flatNumpyArray = numpy.array(randomByteArray)

# # Convert the array to make a 400x300 grayscale image.
# grayImage = flatNumpyArray.reshape(300, 400)
# cv2.imwrite('RandomGray.png', grayImage)

# # Convert the array to make a 400x100 color image.
# bgrImage = flatNumpyArray.reshape(100, 400, 3)
# cv2.imwrite('RandomColor.png', bgrImage)


path=r"img.bin"
w=300
h=300

for filename in os.listdir("."):
    if ".bin" in filename:
        path=filename
        break

f = open(path, "rb")
frames=int(os.path.getsize(path)/(w*h*4))
range="0-"+str(frames-1)
while True:
    id=int(input("Go to image "+range+": "))

    if id>=frames:
        break

    f.seek(id*w*h*4, 0)

    image_data=np.fromfile(f,dtype="uint8")
    image = Image.frombytes('RGBA', (w,h), image_data, 'raw')

    image.show()
