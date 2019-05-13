from math import floor
from math import ceil

if __name__ == '__main__':
    a = 1245425543
    b = 135
    c = 175
    d = 59
    e = 74

    sa = bin(a).replace("0b", "").rjust(32, "0")
    print(sa)

    sb = bin(b).replace("0b", "").rjust(8, "0")
    print(sb)

    sc = bin(c).replace("0b", "").rjust(8, "0")
    print(sc)

    sd = bin(d).replace("0b", "").rjust(8, "0")
    print(sd)

    se = bin(e).replace("0b", "").rjust(8, "0")
    print(se)

    sm = se + sd + sc + sb
    print(sm == sa)
