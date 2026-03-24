# 基于FreeRTOS的简单电控Demo

## 1 项目结构

这个系统是一个基于 STM32 和 FreeRTOS 的多任务电机控制。STM32 负责底层外设驱动，包括 ADC 采样、PWM 电机控制、按键扫描、MPU6050 姿态I2C协议读取和串口打印；FreeRTOS 负责把这些功能拆成独立任务并发运行，让采样、控制、打印互不干扰，系统结构清晰，易移植。

<img src="https://github.com/Superbigbag/freertos-motor-control-demo/blob/main/Picture/5.png" width="240" alt="Keil调试结果">

系统通过反射式传感器判断与地面的距离，并控制直流电机的转速，同时通过MPU6050姿态传感器计算倾斜角。具体来说，AD 任务不断采集传感器并通过队列`xQueue_01`把最新值送给电机任务，电机任务根据 AD 百分比决定速度档位；按键不断刷新判断开关状态，作为所有模块的总开关；MPU6050 任务读取三轴的重力加速度并计算相对于z轴的倾斜角；串口任务以2 Hz速度打印整机状态，输出在电脑端的串口窗口。系统整体上清晰解耦，便于后续继续加入 PID、异常处理等功能。

## 2 硬件清单
| 序号 | 器件名 | 型号 |
| --- | --- | --- |
| 1 | 反射式传感器 | TCRT5000 |
| 2 | 直流电机 | DC 6V TT Geared Motor |
| 3 | 姿态传感器 | MPU6050 |
| 4 | USB转串口 | CH340 |
| 5 | 调试下载器 | ST-LINK V2 |
| 6 | 电机驱动模块 | TB6612 |

配置环境：Win10，keil5，Freertos版本202107.00，芯片:stm32f103c8t6

## 3 系统任务与实现
- **ADC采样任务 (AD Task)**:
    - 采用 **ADC+DMA双缓冲机制**。ADC配置为连续触发、非扫描模式，将红外传感器的模拟值转换后，通过DMA自动搬运至`AD_Value`的存储器变量，并减轻了CPU负担。
    - 将采样得到的最新数据`overwrite`，通过FreeRTOS队列 `xQueue_01` 发送给电机控制任务。

- **电机控制任务 (Motor Task)**:
    - 从 `xQueue_01` 队列中获取传感器数据。
    - 根据数据范围选择电机转速档位（例如，距离近时以50%占空比运行，距离远时以20%占空比维持）。
    - 受Key开关变量控制，仅在总开关开启时运行。

- **按键任务 (Key Task)**:
    - 负责检测按键动作，并翻转作为系统的全局开关状态。
    - 优化了消抖逻辑：利用下降沿判断，仅当“前20ms为高电平且当前20ms为低电平”时才计为一次有效按键，避免了长时间按一次的多次误触发。

- **MPU6050任务**:
    - 通过硬I2C协议读取传感器数据，并计算相对于Z轴的倾斜角。
    - 为保持代码模块化，建立了串口任务，**角度数据**使用全局变量提供给串口打印任务。

- **串口打印任务 (Debug Task)**:
    - 以2Hz的频率，集中打印整个系统的关键状态信息，包括按键开关状态、ADC采样值、电机转速百分比和MPU6050倾斜角，输出到电脑串口助手，实现了统一的状态监控。



## 4 结果展示
 - 搭建实物图：
对应序号见硬件清单

<img src="https://github.com/Superbigbag/freertos-motor-control-demo/blob/main/Picture/7.png" width="400" alt="系统实物图">

 - 系统运行后，串口会周期性打印如下状态信息：

<img src="https://github.com/Superbigbag/freertos-motor-control-demo/blob/main/Picture/8.png" width="350" alt="串口打印结果">

 - 串口的具体打印结果：

<img src="https://github.com/Superbigbag/freertos-motor-control-demo/blob/main/Picture/15.jpg" width="300" alt="串口的具体打印结果">

    Key：系统开关
    AD：采集数据
    Speed：电机转速
    angle：倾斜角（不区分方向）

 - Keil调试结果：

<img src="https://github.com/Superbigbag/freertos-motor-control-demo/blob/main/Picture/13.png" width="420" alt="Keil调试结果">

## 5 硬件使用
1.  **开发环境**: 使用Keil MDK进行代码编译与调试。
2.  **程序下载**: 通过ST-LINK V2将编译好的固件下载到单片机中。
3.  **硬件连接**: 按下面的接线表连接各模块，注意电机部分需供电5V。
4.  **观测结果**: 将CH340串口模块连接至电脑，打开串口助手（波特率9600），便可查看系统实时状态。

<img src="https://github.com/Superbigbag/freertos-motor-control-demo/blob/main/Picture/14.png" width="320" alt="接线表">

## 6 Debug注意事项
1.  **I2C总线问题**：若MPU6050打印的角度值固定为`125.26`或`nan`，可能是硬件I2C总线因接触不良等原因的总线卡死。可尝试更换为**软件I2C**驱动，或执行总线释放操作。
2.  **供电问题**：驱动电机时，电脑USB口供电可能不足，导致电机启动失败。更换为充电宝等独立电源可解决。
3.  **FreeRTOS配置**：
    - 需注意将`SVC_Handler`、`PendSV_Handler`、`SysTick_Handler`三个中断服务函数在`FreeRTOSConfig.h`中启用，以将内核滴答交由FreeRTOS管理。
    - 应根据实际情况设置FreeRTOS的堆栈总大小，避免RAM溢出。
4.  **休眠模式**：ADC模块可通过`ADC_Cmd`控制，MPU6050可通过其内部的`PWR`寄存器控制，以分别进入暂时休眠。
