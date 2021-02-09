import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

from matplotlib import gridspec
import cv2


# Change Directory
currDir = os.getcwd()
# newDir = currDir + '/SampleImages'
newDir = currDir + '/20210208'
os.chdir(newDir)

# import sample image files
# fileNames = {'8bitjpegwithanno.jpg', '8bitpngwithanno.png', '8bitTiffwithanno.tif'}
# fileNames = {'8bitjpegwithanno.jpg'}
fileNames = {'20210208_pixRes_cameraRotated_jpg.jpg'} #8bit jpg


# Analysis for 
for i in fileNames:
	print('FILE NAME: ' + i)
	img = cv2.imread(i)
	height, width, channels = img.shape
	# print (height, width, channels)

	# # Histogram
	# n, bins, patches = plt.hist(img[1500,:,:], 50, density=True, facecolor='g', alpha=0.75)
	# plt.xlabel('Intensity')
	# plt.ylabel('Histogram')
	# plt.title('Histogram of Intensity')
	# plt.grid(True)
	# plt.show()
	
	# Line profile (image size ver3000/hor4096)
	y = 2400 #for horizontal line profile
	x = 800 #for vertical line profile 

	fig, (ax1, ax2) = plt.subplots(2, 1)
	fig.set_size_inches(10,5)
	fig.suptitle('Line profiles')
	
	# gs = gridspec.GridSpec(1, 2, width_ratios=[4096, 3000]) 
	# ax1 = plt.subplot(gs[0])
	# ax2 = plt.subplot(gs[1])
	
	ax1.plot(img[y,:1765,:])
	ax1.title.set_text('Horizontal LP y=' + str(y))
	ax1.set_xlabel('x')
	ax1.set_ylabel('Intensity')
	

	ax2.plot(img[:1565,x,:])
	ax2.title.set_text('Vertical LP x=' + str(x))
	ax2.set_xlabel('y')
	ax2.set_ylabel('Intensity')

	plt.show()

	# # image intensity
	# vert_gap = 757
	# hori_gap = 843
	# x0 = 148
	# y0 = 113
	# boxWidth = 435

	# testimg = np.random.rand(3000, 4096)

	# refIntensity = []
	# for ii in range(20):
	# 	refIntensity.append((ii+1)*5)

	# measIntensity = []
	# for j in range(4):
	# 	for k in range(5):
	# 		# testimg[y0+vert_gap*j:y0+vert_gap*j+boxWidth, x0+hori_gap*k:x0+hori_gap*k+boxWidth] = np.ones([boxWidth, boxWidth])
	# 		box = testimg[y0+vert_gap*j:y0+vert_gap*j+boxWidth, x0+hori_gap*k:x0+hori_gap*k+boxWidth]
	# 		measIntensity.append(np.mean(box))

	# r2 = r2_score(refIntensity,measIntensity) #####################HERE!!!!!!!!!

	# plt.plot(refIntensity, measIntensity)
	# plt.xlabel('Reference Intensity (%)')
	# plt.ylabel('Measured Intensity')
	# plt.title('Intensity Linearity (R2 = '+ str(r2) + ')')
	# plt.grid(True)
	# plt.show()









	


