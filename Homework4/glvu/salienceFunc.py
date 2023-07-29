import cv2 
import numpy as np

def compute_energy(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    dx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    dy = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    energy = cv2.magnitude(dx, dy)
    return energy.astype(np.uint32)

def energy_to_image(energy):
    energy_map = energy.astype(np.float32)
    cv2.normalize(energy_map, energy_map, 0, 255, cv2.NORM_MINMAX)
    energy_map = energy_map.astype(np.uint8)
    return cv2.applyColorMap(energy_map, cv2.COLORMAP_JET)

def main():
    img = cv2.imread('boy.png')
    energy = compute_energy(img)
    energy_gray = energy.astype(np.uint8)
    energy_color = energy_to_image(energy)

    cv2.imshow('Input', img)
    cv2.imshow('Energy Gray', energy_gray)
    cv2.imshow('Energy Color', energy_color)
    cv2.imwrite('energe.png',energy_gray)
    cv2.waitKey(0)

if __name__ == '__main__':
    main()