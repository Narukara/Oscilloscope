# Digital Oscilloscope Based on STM32

<img src=".assets/IMG_20220112_192252.jpg" alt="IMG_20220112_192252" style="zoom: 15%;" />

### Main parameters

4 Modes: ROLL, SINGLE, NORMAL, AUTO (the latter three are collectively referred to as TRIGGER modes).

2 States: RUN / HOLD (waveform freeze).

Coupling Method: DC / AC / ground. Input impedance 1MΩ. BNC interface.

Vertical Sensitivity: 0.1 - 5 V/div.

The ADC maximum sampling rate is 1MSa/s. The input circuit bandwidth is about 2MHz.

Time base: 500ms-50ms (roll mode), 20ms-20us (trigger mode).

Trigger mode: rising edge / falling edge trigger.

1kHz, 0-3.3V square wave calibration signal.

Powered by USB interface, the overall power consumption is about 0.5W.

More detailed introduction, circuit schematic diagram, PCB design, in `./doc` directory (in Chinese)

---

### Notes on board version

There are two versions of the oscilloscope board. The mode button on the first version was incorrectly connected to the USB port and the button didn't work properly. **Most of the documentation describes the first version. **The green board is the first version. The hardware and software have since been corrected, and the new version has resolved this issue. **Hardware design and code in this repository are the second version. **The black board is the second version.

---

### Collaborators

[AJingshou](https://github.com/AJingshou)

Wang Ya

---

# 基于 STM32 的数字示波器

### 主要参数

4种模式: 滚动 ROLL, 单次 SINGLE, 常规 NORMAL, 自动 AUTO (后三种统称触发模式).

2种状态: RUN / HOLD (波形冻结).

耦合方式: 直流 / 交流 / 接地. 输入阻抗 1MΩ. BNC接口.

垂直灵敏度: 0.1 - 5 V/div.

ADC最高采样率 1MSa/s. 输入电路带宽约 2MHz.

时基: 500ms-50ms (滚动模式), 20ms-20us (触发模式).

触发方式: 上升沿 / 下降沿触发.

自带 1kHz, 0-3.3V 方波校准信号

USB 接口供电, 整体功耗约 0.5W.

更详细的介绍, 电路原理图, PCB设计, 在`./doc`目录下

---

### 关于电路板版本的说明

示波器电路板有两个版本。第一个版本上的 mode 按钮错误地连接到了 USB 接口上，按钮工作不正常。**大部分文档介绍的都是第一个版本。**绿色板子就是第一版。之后对硬件和软件进行了修正，新版本已解决此问题。**仓库内的硬件设计和代码都是第二个版本。**黑色板子就是第二版。

---

### 合作者

[AJingshou](https://github.com/AJingshou)

Wang Ya