from uwimg import *
im = load_image("data/dog.jpg")
res = colorize_sobel(im)
mag =res
feature_normalize(mag)
save_image(mag, "magnitude")