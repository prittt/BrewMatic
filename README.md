# AdaptiveWienerFilter
This project contains a quite fast C++ implementation of the adaptive Wiener filter. The source code requires Opencv library to properly work.

## Algorithms
The adaptive Wiener's filter estimates the local mean and variance around each pixel:

![equation](μ=5)

![equation](http://http://latex.codecogs.com/gif.latex?\mu%20=%20\displaystyle\frac{1}{NM}\displaystyle\sum_{n_1,n_2%20\in%20\eta}%20a%20(n_1,%20n_2))

and

![equation](http://latex.codecogs.com/gif.latex?\sigma^2%20=%20\displaystyle\frac{1}{NM}\displaystyle\sum_{n_1,n_2%20\in%20\eta}%20a^2(n_1,%20n_2)%20-%20\mu^2)

where η is the N-by-M local neighborhood of each pixel in the image A. wiener2 then creates a pixelwise Wiener filter using these estimates,

![equation](http://latex.codecogs.com/gif.latex?b(n_1,%20n_2)%20=%20\mu%20+%20\displaystyle\frac{\sigma^2}{\sigma^2%20+%20\nu^2}%20\displaystyle%20(a(n_1,n_2)%20-%20\mu))

where  is the noise variance. If the noise variance is not given, wiener2 uses the average of all the local estimated variances.

Please notice that the formula reported by some paper and by the Matlab reference is not correct (the wrong formula is displayed below for completeness).

![equation](http://latex.codecogs.com/gif.latex?b(n_1,%20n_2)%20=%20\mu%20+%20\displaystyle\frac{\sigma^2%20-%20\nu^2}{\sigma^2}%20\displaystyle%20(a(n_1,n_2)%20-%20\mu))
