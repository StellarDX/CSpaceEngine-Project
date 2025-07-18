<!--DOCTYPE html>
<html>
    <head>
        <title>CSpaceEngine Renewed</title>
        <style type="text/css">
        div.Head
        {
            width:100%;
            text-align:center;
        }
        div.Head h1
        {
            font-weight: bold;
            background: -webkit-linear-gradient(-66.5607189deg, rgb(41,82,190), rgb(41,82,190) 40%, rgb(254,226,223) 60%, rgb(251,172,19) 75%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            color: transparent;
        }
        </style>
    </head>
    <body-->
<!--a href="README_EN.md">English</a-->
<div class="Head" align="center" style="width:100%;">
    <h1 style="font-weight: bold;background: -webkit-linear-gradient(-66.5607189deg, rgb(41,82,190), rgb(41,82,190) 40%, rgb(254,226,223) 60%, rgb(251,172,19) 75%);-webkit-background-clip: text;-webkit-text-fill-color: transparent;color: transparent;">CSpaceEngine·文明启程</h1>
    <p>开源的SpaceEngine静态库，适配SE0.990</p>
    <p>许可证：<a href="https://www.gnu.org/licenses/old-licenses/gpl-2.0.html">GPL-3.0</a></p>
</div>
<h2>功能</h2>
<ul>
    <li>GLSL数据类型和内置函数</li>
    <li>日期，时间及儒略日</li>
    <li>高等数学</li>
    <li>类似Python的随机数生成器</li>
    <li>SpaceEngine SC文件读写</li>
</ul>
<h3>来自GLSL的数据类型和函数</h3>
<ul>
    <li>n维向量模板，2-4维是独立数据类型，4维以上为std::array的增强</li>
    <li>矩阵，理论上不限大小，支持线性代数计算，可以当容器使用</li>
    <li>一些来自GLSL的函数，支持vec2-4以及std::array</li>
</ul>
<h3>日期时间</h3>
<p>这个功能的使用方法和Qt的日期和时间类型相似，但是比Qt的简单</p>
<h3>高等数学</h3>
<ul>
    <li>坐标转换，导数，积分，微分方程，反函数</li>
</ul>
<h3>类似Python的随机数生成器</h3>
<p>基本就是让C++的随机生成更好用些而已，顺便加了个自定义分布</p>
<h3>SpaceEngine SC文件读写</h3>
<ul>
    <li>一个经历了至少3次的更新换代的，基于LR(1)算法的sc文件读取</li>
    <li>SC文件输出流</li>
</ul>
<h3>功能很基础物理引擎(重构中)</h3>
<!--ul>
    <li>轨道跟踪器，用于轨道根数和状态向量的转换，可以实时跟踪(目前只支持椭圆轨道，Bug多慎用)</li>
    <li>一些很基础的转换，比如双星轨道转开普勒轨道根数</li>
    <li>行星轨道分布模型</li>
    <li>卫星TLE工具</li>
    <li>洛希极限，希尔球和拉格朗日点，用于多体问题</li>
    <li>IAU自转模型实时跟踪工具 (<b>未来可能添加IAU和标准自转模型互转，高难</b>)</li>
    <li>自带了多个状态方程模板已经状态方程，尽管它们绝大多数是不精确的 (<b>将改为材料实体类，封装基本属性，状态方程及熔化曲线</b>)</li>
    <li>基于流体静力平衡的行星物理模型(搁置了两年多的“世纪难题”在此宣告破解!) (<b>将引入温度影响，向气态行星兼容</b>)</li>
    <li><b>(未实现)基于恒星结构方程的恒星物理模型 (新的高难世纪难题待破解)</b></li>
    <li><b>星等-光度计算</b></li>
    <li><b>(未完全实现)光谱分析</b></li>
    <li><b>(未实现)基本热力学 (又是世纪难题)</b></li>
</ul-->
<h2>编译</h2>
<p>编译器支持和编译选项相关内容详见<a href="https://github.com/StellarDX/CSpaceEngine-Documents">文档</a></p>

<h2>遇到问题或想要新功能？</h2>
<p>如果发现无法解决的问题或者bug，在issues告诉我。</p>
    <!--/body>
</html-->
