import gmpy2
import numpy as np

# 设置高精度上下文（256位二进制精度）
ctx = gmpy2.get_context()
ctx.precision = 512
xrad = (gmpy2.mpfr("180", precision=5000)) / (gmpy2.const_pi(precision=5000))

for i in np.arange(0, 10.375, 0.125):
    xhi = gmpy2.mpfr(i, precision=500)
    print(gmpy2.round2(gmpy2.atan(i) * xrad, 113))
