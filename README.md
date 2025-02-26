# Decompositon-based Models for Fluid Dynamics

## 文档

文档请访问：[POD/DMD models for fluid dynamics](https://www.windtunnel.cn/fluiddecomposition/) 。

有完整的数学推导和代码解释。


## 代码和运行

代码运行，代码需要使用cmake编译，

- 在Windows上，需要安装MSVC编译器，并且Eigen3的头文件位置需要在`CMakeLists.txt`中指定。
- 在Linux上，需要安装g++编译器，并且需要安装Eigen3库（`libeigen3-dev`）。

```bash
git clone https://github.com/qchen-fdii-cardc/fluiddecomposition.git
cd fluiddecomposition
cmake -B build .
cmake --build build
./build/debug/pod_demo
./build/debug/dmd_demo
python ./scripts/visualize_flow.py
```
