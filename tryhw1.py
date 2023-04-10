from uwimg import *
im = load_image("data/dog.jpg")
a = nn_resize(im, im.h*2, im.w*2)
save_image(a, "dog7th-bl")