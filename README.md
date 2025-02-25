# Decompositon-based Models for Fluid Dynamics

## 文档

文档请访问：[POD/DMD models for fluid dynamics](https://www.windtunnel.cn/fluiddecomposition/) 。

有完整的数学推导和代码解释。


## 代码和运行

代码运行

```bash
git clone https://github.com/qchen-fdii-cardc/fluiddecomposition.git
cd fluiddecomposition
cmake -B build .
cmake --build build
./build/debug/pod_demo
./build/debug/dmd_demo
python ./scripts/visualize_flow.py
```
