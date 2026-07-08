# CCS 项目导入说明

## 方法一：通过 Project → Import CCS Projects...（推荐）

1. 打开 Code Composer Studio
2. 点击菜单栏 **Project → Import CCS Projects...**
3. **Select search-directory** → 点击 **Browse...**
4. 选择本文件夹：`2026电赛/MSPM0G3507/`
   （注意：是选择 MSPM0G3507 文件夹本身，**不是**里面的 targetConfigs 子文件夹）
5. 在 Discovered projects 列表中应出现 **2026E_LineFollower**
6. 勾选该项目 → 点击 **Finish**

## 方法二：通过 File → Import

1. `File → Import → Code Composer Studio → CCS Projects`
2. 后续步骤与方法一相同

## 导入后 - 配置 SysConfig

本项目的引脚配置需要通过 SysConfig 图形化工具来完成：

1. 右键项目 → **New → SysConfig**
2. 配置以下外设（按代码中的宏定义配）：
   - **ADC0**: 通道 IN0~IN7（8路灰度，PA0~PA7）
   - **TIMA0**: 两路 PWM（PB0~PB1，频率10kHz）
   - **I2C1**: OLED 显示（PC0~PC1）
   - **UART1**: 与 STM32 通信（PA8~PA9，115200）
   - **GPIO**: 按键、电机方向、STBY
3. SysConfig 会自动生成 `ti_msp_dl_config.h` 和 `ti_msp_dl_config.c`

## 常见问题

**Q: Import 时没有发现项目？**
A: 检查导入路径是否正确，应该是 `MSPM0G3507` 文件夹，里面的 `.project` 文件必须存在。

**Q: 编译报错找不到 `ti_msp_dl_config.h`？**
A: 需要先在 CCS 中创建 SysConfig 文件（右键 → New → SysConfig），配置完成后会自动生成。

**Q: 引脚与代码不匹配？**
A: 在 SysConfig 中按代码 `motor.h`、`grayscale.h` 等头文件中的宏定义配置引脚映射。
