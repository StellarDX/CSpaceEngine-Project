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
    <h1 style="font-weight: bold;background: -webkit-linear-gradient(-66.5607189deg, rgb(41,82,190), rgb(41,82,190) 40%, rgb(254,226,223) 60%, rgb(251,172,19) 75%);-webkit-background-clip: text;-webkit-text-fill-color: transparent;color: transparent;">CSpaceEngine·聚力新生</h1>
    <p>开源的SpaceEngine静态库，适配SE0.990</p>
    <p>许可证：<a href="https://www.gnu.org/licenses/old-licenses/gpl-2.0.html">GPL-2.0</a></p>
</div>
<h2>功能</h2>
<ul>
    <li>GLSL数据类型和内置函数</li>
    <li>日期，时间及儒略日</li>
    <li>高等数学</li>
    <li>类似Python的随机数生成器</li>
    <li>SpaceEngine SC文件读写</li>
    <li>剩下的功能还在重构中...</li>
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
    <li>直角坐标和极坐标之间的转换</li>
    <li>导数，积分，微分方程</li>
    <li>反函数</li>
</ul>
<h3>类似Python的随机数生成器</h3>
<p>基本就是让C++的随机生成更好用些而已，顺便加了个自定义分布</p>
<h3>SpaceEngine SC文件读写</h3>
<ul>
    <li>一个经历了至少3次的更新换代的，基于LR(1)算法的sc文件读取</li>
    <li>SC文件输出流</li>
</ul>
<h2>编译</h2>
<p>此版本已经<b>放弃VS2022并迁移到CMake平台</b>，编译就正常使用CMake即可。</p>
<p>注：由于此版本的一些功能会大量使用format函数，而一些编译器在开启了C++20之下仍未支持此功能，所以这里给出了一个备用解决方案，此方案需安装<a href="https://github.com/fmtlib/fmt">fmtlib</a>。</p>
<p>编译选项详见Building the library(还未上传)</p>
<h2>遇到问题或想要新功能？</h2>
<p>如果发现无法解决的问题或者bug，在issues告诉我。</p>
    <!--/body>
</html-->
