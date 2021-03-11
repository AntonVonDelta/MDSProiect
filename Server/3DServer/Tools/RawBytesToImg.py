
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


path=r"..\TestOpenGL\img.bin"

# image_data = np.array(bytearray(os.urandom(65536))) 
image_data=np.fromfile(path,dtype="uint8")
image = Image.frombytes('RGBA', (300,300), image_data, 'raw')

image.show()