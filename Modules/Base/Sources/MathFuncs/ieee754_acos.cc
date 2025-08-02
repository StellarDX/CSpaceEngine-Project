/*
    角度arccos函数，基于GLibC中的四倍精度算法改编而来。

    License provided by GLibC:
    Quaduple expansions are
    Copyright (C) 2001 Stephen L. Moshier <moshier@na-net.ornl.gov>
    and are incorporated herein by permission of the author.  The author
    reserves the right to distribute this material elsewhere under different
    copying permissions.  These modifications are distributed here under the
    following terms:

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <https://www.gnu.org/licenses/>.
*/

#include "CSE/Base/MathFuncs.h"
#include "CSE/Base/Algorithms.h"
#include "CSE/Base/ConstLists.h"

_CSE_BEGIN

_EXTERN_C

/* arccos(x)
 * Method :
 *      arccos(x)  = 90 - arcsin(x)
 *      arccos(-x) = 90 + arcsin(x)
 * For |x| <= 0.375
 *      arccos(x) = 90 - arcsin(x)
 * Between .375 and .5 the approximation is
 *      arccos(0.4375 + x) = arccos(0.4375) + x P(x) / Q(x)
 * Between .5 and .625 the approximation is
 *      arccos(0.5625 + x) = arccos(0.5625) + x rS(x) / sS(x)
 * For x > 0.625,
 *      arccos(x) = 2 arcsin(sqrt((1-x)/2))
 *      computed with an extended precision square root in the leading term.
 * For x < -0.625
 *      arccos(x) = 180 - 2 asin(sqrt((1-|x|)/2))
 *
 * Special cases:
 *      if x is NaN, return x itself;
 *      if |x|>1, return NaN with invalid signal.
 *
 * Functions needed: sqrt.
 */
__Float64 __cdecl __IEEE854_ACOS128F_C64F(__Float64 _X)
{
    __Float64 x = _X;
    __Float64 a, z, r, w, p, q, s, t, f2;

    if (isnan(x)) {return __Float64::FromBytes(BIG_NAN_DOUBLE);}

    a = abs(x);
    if (a == 1.0L)
    {
        if (x > 0.0L) {return 0.0;}	// arccos(1) = 0
        else {return 180;}	// acos(-1)= 180
    }
    else if (a > 1.0L)
    {
        return __Float64::FromBytes(BIG_NAN_DOUBLE); // arccos(|x| > 1) is NaN
    }

    if (a < 0x1p-106L) {return 90;}

    static const __Float64
        R2D = 57.2957795130823208767981548141051703324054724665643215491602438612028471483215526324409689958511109442;

    // asin(x) = x + x^3 pS(x^2) / qS(x^2)
    // 0 <= x <= 0.5
    // peak relative error 1.9e-35
    // converted to degrees by StellarDX
    static const __Float64
        /*pS0 = -8.358099012470680544198472400254596543711E2L,
        pS1 =  3.674973957689619490312782828051860366493E3L,
        pS2 = -6.730729094812979665807581609853656623219E3L,
        pS3 =  6.643843795209060298375552684423454077633E3L,
        pS4 = -3.817341990928606692235481812252049415993E3L,
        pS5 =  1.284635388402653715636722822195716476156E3L,
        pS6 = -2.410736125231549204856567737329112037867E2L,
        pS7 =  2.219191969382402856557594215833622156220E1L,
        pS8 = -7.249056260830627156600112195061001036533E-1L,
        pS9 =  1.055923570937755300061509030361395604448E-3L,*/
        pS0 = -150445.782224472249795572503204582737786798L,
        pS1 = +661495.31238413150825630090904933486596874L,
        pS2 = -1211531.23706633633984536468977365819217942L,
        pS3 = +1195891.88313763085370759948319622173397394L,
        pS4 = -687121.55836714920460238672620536889487874L,
        pS5 = +231234.36991247766881461010799522896570808L,
        pS6 = -43393.250254167885687418219271924016681606L,
        pS7 = +3994.545544888325141803669588500519881196L,
        pS8 = -130.483012694951288818802019511098018657594L,
        pS9 = +0.19006624276879595401107162546505120880064L,

        /*qS0 = -5.014859407482408326519083440151745519205E3L,
        qS1 =  2.430653047950480068881028451580393430537E4L,
        qS2 = -4.997904737193653607449250593976069726962E4L,
        qS3 =  5.675712336110456923807959930107347511086E4L,
        qS4 = -3.881523118339661268482937768522572588022E4L,
        qS5 =  1.634202194895541569749717032234510811216E4L,
        qS6 = -4.151452662440709301601820849901296953752E3L,
        qS7 =  5.956050864057192019085175976175695342168E2L,
        qS8 = -4.175375777334867025769346564600396877176E1L,
        qs9 =  1.000000000000000000000000000000000000000E0;*/
        qS0 = -15754.6454733323973957122801577592449886221741L,
        qS1 = +76361.2175886686762494938136773118090037822340L,
        qS2 = -157013.8080570920843186674328750290776060290209L,
        qS3 = +178307.7617901357482760502917179520524022049450L,
        qS4 = -121941.6451331482548970782474055473506041747737L,
        qS5 = +51339.9760996414890281199944389245470587138627L,
        qS6 = -13042.1731860495201001263105407568565718105918L,
        qS7 = +1871.1485638929214746528741248821897077028979L,
        qS8 = -131.1732986805199057040174904947681623872928L,
        qS9 = +3.14159265358979323846264338327950288419717L;

    if (a < 0.4375L)
    {
        // Arcsine of x
        z = x * x;
        p = (((((((((pS9.x * z
                     + pS8.x) * z
                    + pS7.x) * z
                   + pS6.x) * z
                  + pS5.x) * z
                 + pS4.x) * z
                + pS3.x) * z
               + pS2.x) * z
              + pS1.x) * z
             + pS0.x) * z;
        q = ((((((((qS9.x * z
                    + qS8.x) * z
                   + qS7.x) * z
                  + qS6.x) * z
                 + qS5.x) * z
                + qS4.x) * z
               + qS3.x) * z
              + qS2.x) * z
             + qS1.x) * z
            + qS0.x;
        r = R2D.x * x + x * p / q;
        z = 90 - r;
        return z;
    }

    if (a < 0.5L)
    {
        static const __Float64
            Acos04375 = 64.0555202276299937126492273123694515423114001090821170423472901842012081975591009227681224698248498125,
            Acos04375Complementary = 115.9444797723700062873507726876305484576885998909178829576527098157987918024408990772318775301751501875;

        // acos(0.4375 + x) = acos(0.4375) + x rS(x) / sS(x)
        // -0.0625 <= x <= 0.0625
        // peak relative error 2.1e-35
        // converted to degrees by StellarDX
        static const __Float64
            /*P0 =  2.177690192235413635229046633751390484892E0L,
            P1 = -2.848698225706605746657192566166142909573E1L,
            P2 =  1.040076477655245590871244795403659880304E2L,
            P3 = -1.400087608918906358323551402881238180553E2L,
            P4 =  2.221047917671449176051896400503615543757E1L,
            P5 =  9.643714856395587663736110523917499638702E1L,
            P6 = -5.158406639829833829027457284942389079196E1L,
            P7 = -1.578651828337585944715290382181219741813E1L,
            P8 =  1.093632715903802870546857764647931045906E1L,
            P9 =  5.448925479898460003048760932274085300103E-1L,
            P10 = -3.315886001095605268470690485170092986337E-1L,*/
            P0 = +391.98423460237445434122839407525028728056L,
            P1 = -5127.6568062718903439829466190990572372314L,
            P2 = +18721.376597794420635682406317265877845472L,
            P3 = -25201.576960540314449823925251862287249954L,
            P4 = +3997.8862518086085168934135209065079787626L,
            P5 = +17358.6867415120577947249989430514993496636L,
            P6 = -9285.1319516937008922494231128963003425528L,
            P7 = -2841.5732910076547004875226879261955352634L,
            P8 = +1968.5388886268451669843439763662758826308L,
            P9 = +98.080658638172280054877696780933535401854L,
            P10 = -59.685948019720894832472428733061673754066L,

            /*Q0 = -1.958219113487162405143608843774587557016E0L,
            Q1 =  2.614577866876185080678907676023269360520E1L,
            Q2 = -9.990858606464150981009763389881793660938E1L,
            Q3 =  1.443958741356995763628660823395334281596E2L,
            Q4 = -3.206441012484232867657763518369723873129E1L,
            Q5 = -1.048560885341833443564920145642588991492E2L,
            Q6 =  6.745883931909770880159915641984874746358E1L,
            Q7 =  1.806809656342804436118449982647641392951E1L,
            Q8 = -1.770150690652438294290020775359580915464E1L,
            Q9 = -5.659156469628629327045433069052560211164E-1L,
            Q10 = 1.000000000000000000000000000000000000000E0L;*/
            Q0 = -6.1519267810503870143800110557358756044977L,
            Q1 = +82.1393861881669545744672390106021543920385L,
            Q2 = -313.8720800112213588235187915249335663718700L,
            Q3 = +453.6330173933902243439310465807956353847934L,
            Q4 = -100.7333152898948448406726403298551311784874L,
            Q5 = -329.4151174231513460144259697188388782942715L,
            Q6 = +211.9281940245716518651790830267665951075683L,
            Q7 = +56.7625994280165338442552870196538341685670L,
            Q8 = -55.6109240550059883028416753770952385453737L,
            Q9 = -1.7778764390500451753495355970855728154043L,
            Q10 = +3.14159265358979323846264338327950288419717L;

        t = a - 0.4375L;

        p = ((((((((((P10.x * t
                      + P9.x) * t
                     + P8.x) * t
                    + P7.x) * t
                   + P6.x) * t
                  + P5.x) * t
                 + P4.x) * t
                + P3.x) * t
               + P2.x) * t
              + P1.x) * t
             + P0.x) * t;

        q = (((((((((Q10.x * t
                     + Q9.x) * t
                    + Q8.x) * t
                   + Q7.x) * t
                  + Q6.x) * t
                 + Q5.x) * t
                + Q4.x) * t
               + Q3.x) * t
              + Q2.x) * t
             + Q1.x) * t
            + Q0.x;

        r = p / q;
        if (x < 0.0L) {r = Acos04375Complementary.x - r;}
        else {r = Acos04375.x + r;}
        return r;
    }

    if (a < 0.625L)
    {
        static const __Float64
            Acos05625 = 55.7711336721874219845584934392228377746478465538143848898275067794154234198581820650126489030458732827,
            Acos05625Complementary = 124.2288663278125780154415065607771622253521534461856151101724932205845765801418179349873510969541267173;

        static const __Float64
            // acos(0.5625 + x) = acos(0.5625) + x rS(x) / sS(x)
            // -0.0625 <= x <= 0.0625
            // peak relative error 3.3e-35
            // converted to degrees by StellarDX
            /*rS0 =  5.619049346208901520945464704848780243887E0L,
            rS1 = -4.460504162777731472539175700169871920352E1L,
            rS2 =  1.317669505315409261479577040530751477488E2L,
            rS3 = -1.626532582423661989632442410808596009227E2L,
            rS4 =  3.144806644195158614904369445440583873264E1L,
            rS5 =  9.806674443470740708765165604769099559553E1L,
            rS6 = -5.708468492052010816555762842394927806920E1L,
            rS7 = -1.396540499232262112248553357962639431922E1L,
            rS8 =  1.126243289311910363001762058295832610344E1L,
            rS9 =  4.956179821329901954211277873774472383512E-1L,
            rS10 = -3.313227657082367169241333738391762525780E-1L,*/
            rS0 = +1011.42888231760227377018364687278044389966L,
            rS1 = -8028.9074929999166505705162603057694566336L,
            rS2 = +23718.051095677366706632386729553526594784L,
            rS3 = -29277.586483625915813383963394554728166086L,
            rS4 = +5660.6519595512855068278650017930509718752L,
            rS5 = +17652.0139982473332757772980885843792071954L,
            rS6 = -10275.243285693619469800373116310870052456L,
            rS7 = -2513.7728986180718020473960443327509774596L,
            rS8 = +2027.2379207614386534031717049324986986192L,
            rS9 = +89.211236783938235175803001727940502903216L,
            rS10 = -59.63809782748260904634400729105172546404L,

            /*sS0 = -4.645814742084009935700221277307007679325E0L,
            sS1 =  3.879074822457694323970438316317961918430E1L,
            sS2 = -1.221986588013474694623973554726201001066E2L,
            sS3 =  1.658821150347718105012079876756201905822E2L,
            sS4 = -4.804379630977558197953176474426239748977E1L,
            sS5 = -1.004296417397316948114344573811562952793E2L,
            sS6 =  7.530281592861320234941101403870010111138E1L,
            sS7 =  1.270735595411673647119592092304357226607E1L,
            sS8 = -1.815144839646376500705105967064792930282E1L,
            sS9 = -7.821597334910963922204235247786840828217E-2L,
            sS10 = 1.000000000000000000000000000000000000000E0;*/
            sS0 = -14.5952574636702856448060361136303137019576L,
            sS1 = +121.8647296495822399328155775693910968304904L,
            sS2 = -383.8984087688389392734477154368191004924278L,
            sS3 = +521.1340339551761092314940733385193225464266L,
            sS4 = -150.9340375373553866392783245200875319645678L,
            sS5 = -315.5090246921959542501601304697638172627817L,
            sS6 = +236.5707733159557006516050875761556773582279L,
            sS7 = +39.9213361120036570227223148798915033412032L,
            sS8 = -57.0244569343447968594080214679151754208322L,
            sS9 = -0.24572272726693789889252807126981941027009L,
            sS10 = 3.14159265358979323846264338327950288419717L;

        t = a - 0.5625L;

        p = ((((((((((rS10.x * t
                      + rS9.x) * t
                     + rS8.x) * t
                    + rS7.x) * t
                   + rS6.x) * t
                  + rS5.x) * t
                 + rS4.x) * t
                + rS3.x) * t
               + rS2.x) * t
              + rS1.x) * t
             + rS0.x) * t;

        q = (((((((((sS10.x * t
                     + sS9.x) * t
                    + sS8.x) * t
                   + sS7.x) * t
                  + sS6.x) * t
                 + sS5.x) * t
                + sS4.x) * t
               + sS3.x) * t
              + sS2.x) * t
             + sS1.x) * t
            + sS0.x;

        if (x < 0.0L) {r = Acos05625Complementary.x - p / q;}
        else {r = Acos05625.x + p / q;}
        return r;
    }

    z = (1.0 - a) * 0.5;
    s = __IBM_SQRT64F(z);
    /* Compute an higher precision square root from
     the Newton iteration  s -> 0.5 * (s + z / s).
     The change w from s to the improved value is
        w = 0.5 * (s + z / s) - s  = (s^2 + z)/2s - s = (z - s^2)/2s.
      Express s = f1 + f2 where f1 * f1 is exactly representable.
      w = (z - s^2)/2s = (z - f1^2 - 2 f1 f2 - f2^2)/2s .
      s + w has higher precision.  */
    // 此处原先是为弧度优化的，但是转成角度以后加上这段误差反而变大了
    /*a = s;
    a.parts.lsw = 0;
    f2 = s - a;
    w = z - a * a;
    w = w - 2.0 * a * f2;
    w = w - f2 * f2;
    w = (w * 180.L) / (CSE_2PI * s);*/

    // Arcsine of s
    p = (((((((((pS9.x * z
                 + pS8.x) * z
                + pS7.x) * z
               + pS6.x) * z
              + pS5.x) * z
             + pS4.x) * z
            + pS3.x) * z
           + pS2.x) * z
          + pS1.x) * z
         + pS0.x) * z;

    q = ((((((((qS9.x * z
                + qS8.x) * z
               + qS7.x) * z
              + qS6.x) * z
             + qS5.x) * z
            + qS4.x) * z
           + qS3.x) * z
          + qS2.x) * z
         + qS1.x) * z
        + qS0.x;

    // 这段就有些回天乏术了，10，15，30这样的角的余弦值已经无法用任何浮点精确表示。
    // 实测不用加别的优化计算精度就已和标准库保持的比较好，个别值已经超越标准库。
    r = R2D.x * s + (s * p / q);

    if (x < 0.0L) {w = 90 - r;}
    else {w = r;}
    return 2.0 * w;
}

_END_EXTERN_C

_CSE_END

#if 0 // Test program for arccos generated by Deepseek
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <random>

#include <CSE/Base.h>

using namespace cse;

// 辅助函数
double radians_to_degrees(double rad)
{
    return rad * 180.0 / M_PI;
}

void print_comparison(double x)
{
    double custom_result = __IEEE854_ACOS128F_C64F(x);
    double std_result = radians_to_degrees(acos(x));
    double diff = custom_result - std_result;

    std::cout << std::scientific << std::setprecision(17);
    std::cout << "x = " << x << "\n";
    std::cout << "  Custom: " << custom_result << "\n";
    std::cout << "  Stdlib: " << std_result << "\n";
    std::cout << "  Diff:   " << diff << "\n";
    std::cout << "  RelErr: " << (diff / std_result) << "\n\n";
}

int main()
{
    // 特殊值
    std::cout << "=== Special Values ===\n";
    print_comparison(std::numeric_limits<double>::quiet_NaN());
    print_comparison(1.0);
    print_comparison(-1.0);
    print_comparison(0.0);

    // 边界值
    std::cout << "=== Boundary Values ===\n";
    print_comparison(1.0 - std::numeric_limits<double>::epsilon());
    print_comparison(-1.0 + std::numeric_limits<double>::epsilon());
    print_comparison(std::numeric_limits<double>::min());
    print_comparison(-std::numeric_limits<double>::min());
    print_comparison(0x1p-106); // 函数中的特殊边界

    // 常见角度
    std::cout << "=== Common Angles ===\n";
    print_comparison(0.8660254037844386467637231707529);  // 30°
    print_comparison(0.7071067811865475244008443621048);  // 45°
    print_comparison(0.5);  // 60°
    print_comparison(0);  // 90°
    print_comparison(-0.5); // 120°

    // 区间划分
    std::cout << "=== Interval Partitioning ===\n";
    print_comparison(0.1);      // 0-0.4375
    print_comparison(0.4);      // 0-0.4375
    print_comparison(0.4375);   // 边界
    print_comparison(0.45);     // 0.4375-0.5
    print_comparison(0.5);      // 边界
    print_comparison(0.5625);   // 0.5-0.625
    print_comparison(0.6);      // 0.5-0.625
    print_comparison(0.625);    // 边界
    print_comparison(0.7);      // 0.625-1.0
    print_comparison(0.8);      // 0.625-1.0
    print_comparison(0.999);    // 接近1
}
#endif
