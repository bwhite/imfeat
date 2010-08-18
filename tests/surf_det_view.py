import cv
import imfeat
import Image

def rgb2gray(imgc):
    img = cv.CreateImage((imgc.width, imgc.height), cv.IPL_DEPTH_8U, 1 )
    cv.CvtColor( imgc, img, cv.CV_BGR2GRAY );
    return img,imgc

def displaypts(img, pts):
    c =  [(255,0,0), (0,0,255)]
    [cv.Circle(img, (pt['x'], pt['y']), cv.Round(3.75 * pt['scale']), c[pt['sign']], 1) for pt in pts]

#cvStartWindowThread()
cv.NamedWindow("Window")
#capture = cv.CaptureFromCAM(-1)
#cv.QueryFrame(capture)
img,imgc = rgb2gray(cv.LoadImage('lena.jpg'))
while 1:
    #img,imgc = rgb2gray(cv.QueryFrame(capture))
    img_pil = Image.fromstring('L', (img.width, img.height), img.tostring())
    points = imfeat.surf.make_points(img_pil)
    #print(points)
    displaypts(imgc, points)
    cv.ShowImage("Window", imgc)
    if cv.WaitKey(30) != -1:
        break
