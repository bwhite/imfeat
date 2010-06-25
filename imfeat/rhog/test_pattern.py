import numpy as np
import Image

cell_dia = 9
cells = 20

pix_height = cells * cell_dia + 2
pix_width = cell_dia + 2
img = np.zeros((pix_height, pix_width), dtype=np.uint8)

def save_img(img, fn):
    Image.fromstring('L', img.shape[::-1], img.tostring()).save(fn + '.png')

# Make vertical gradient
for i in range(pix_height):
    for j in range(pix_width):
        img[i, j] = i
save_img(img, 'vert_grad')

# Make vertical gradient
for i in range(pix_height):
    for j in range(pix_width):
        img[i, j] = 255 - i
save_img(img, 'rvert_grad')

# Make horizontal gradient
for i in range(pix_height):
    for j in range(pix_width):
        img[i, j] = j
save_img(img, 'horz_grad')

# Make horizontal gradient
for i in range(pix_height):
    for j in range(pix_width):
        img[i, j] = 255 - j
save_img(img, 'rhorz_grad')


# Make diagonal gradient
for i in range(pix_height):
    for j in range(pix_width):
        img[i, j] = i + j
save_img(img, 'diag_grad')

# Make random
for i in range(pix_height):
    for j in range(pix_width):
        img[i, j] = np.random.randint(0, 256)
save_img(img, 'rand_grad')
