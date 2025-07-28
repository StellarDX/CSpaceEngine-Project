import gmpy2

# 设置高精度上下文（256位二进制精度）
ctx = gmpy2.get_context()
ctx.precision = 512

def extract_quadruple_parts(x):

    # 获取符号（1 表示负，0 表示正）
    sign = 1 if gmpy2.sign(x) < 0 else 0

    # 获取指数（调整偏置）
    exponent = gmpy2.get_exp(x)
    biased_exponent = exponent + 16382  # binary128 的指数偏置是 16383

    # 获取尾数（正规化形式）
    mantissa = x.as_mantissa_exp()[0]  # 返回 (mantissa, exponent)

    return sign, biased_exponent, mantissa

def HexFormat(x):
    # 指定输出56位十六进制小数
    k = 90
    # 计算缩放因子：scale = 16^k = 2^(4*k)
    scale = gmpy2.exp2(4 * k)

    # 缩放结果：scaled_val = a * scale
    scaled_val = x * scale

    # 四舍五入到整数并转换为十六进制字符串
    integer_val = gmpy2.mpz(gmpy2.round2(scaled_val, 0))
    s_hex = gmpy2.digits(integer_val, 16).upper()

    # 确保字符串长度为k（不足补0，超过截断）
    if len(s_hex) < k:
        s_hex = s_hex.zfill(k)
    elif len(s_hex) > k:
        s_hex = s_hex[:k]

    # 格式化为0.xxx的十六进制小数
    return "0." + s_hex

def PrintSinCosFP(x, a, b):
    # 输出结果（按26、26、23位分组）
    sresult = HexFormat(a);
    cresult = HexFormat(b);
    s_grouped_result = sresult[:28] + " " + sresult[28:54] + " " + sresult[54:77]
    c_grouped_result = cresult[:28] + " " + cresult[28:54] + " " + cresult[54:77]

    sign0, biased_exponent0, mantissa0 = extract_quadruple_parts(gmpy2.mpfr(x, precision=113))
    # sign1, biased_exponent1, mantissa1 = extract_quadruple_parts(gmpy2.mpfr(a, precision=75*4+1))
    mantissastr = f"{mantissa0:X}"[1:]

    print(f"// x = {x:.37f}", end=' ')
    print(f"{biased_exponent0:X}{mantissastr}")
    print(f"// sin(x) = {s_grouped_result}")
    print(f"0x{sresult[:28]}p0L, 0x0.{sresult[28:54]}p-104L,")
    print(f"// cos(x) = {c_grouped_result}")
    print(f"0x{cresult[:28]}p0L, 0x0.{cresult[28:54]}p-104L,")
    print("")

def ComputeSinCos(x):
    # 输入值 x
    xhi = gmpy2.mpfr(x, precision=500)
    xrad = xhi * (gmpy2.const_pi(precision=5000) / gmpy2.mpfr("180", precision=5000))

    # 计算 sin(x)
    a = gmpy2.sin(xrad)
    b = gmpy2.cos(xrad)

    PrintSinCosFP(xhi, a, b)

for i in range(0, 65):
    ComputeSinCos(gmpy2.mpfr(0.703125 * i))

print('-----------------------------------------------------------------------')

for i in range(0, 6):
    rad2deg = (2 * gmpy2.const_pi(precision=5000)) / gmpy2.mpfr("512", precision=5000)
    print(gmpy2.mpfr(rad2deg ** (2 * i + 1) / gmpy2.factorial(2 * i + 1), precision=113))
    print(gmpy2.mpfr(rad2deg ** (2 * i + 1) / gmpy2.factorial(2 * i + 1), precision=237))
print('')
for i in range(1, 6):
    rad2deg = (2 * gmpy2.const_pi(precision=5000)) / gmpy2.mpfr("512", precision=5000)
    print(gmpy2.mpfr(rad2deg ** (2 * i) / gmpy2.factorial(2 * i), precision=113))
    print(gmpy2.mpfr(rad2deg ** (2 * i) / gmpy2.factorial(2 * i), precision=237))
