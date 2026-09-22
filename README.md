# STM32 GPS 骑行码表

**中文** | [English](README.en.md)

基于 **STM32F405RGT6 + FreeRTOS + LVGL 9.4** 的嵌入式骑行信息终端，在 240 × 320 触摸屏上显示离线地图、GPS 速度、累计里程、经纬度、运动计时、坡度估算和电池状态。设备支持中英文界面及深浅色显示模式。

## 实物效果

| 地图 · Roadmap | 骑行数据 · General | 设置 · Settings |
| :---: | :---: | :---: |
| <img src="docs/images/roadmap.jpg" alt="离线地图与定位标记" width="280"> | <img src="docs/images/general.jpg" alt="速度、里程、坡度、经纬度与计时" width="280"> | <img src="docs/images/settings.jpg" alt="语言选择与显示模式切换" width="280"> |

## 主要功能

- **GPS 数据采集**：USART2 + DMA 接收 NMEA 数据，解析 GGA / VTG，获取经纬度、卫星数和速度；包含地图坐标转换代码。
- **骑行统计**：实时速度、累计里程、运动计时及运行状态显示。
- **离线地图**：通过 FatFs 读取 SD 卡上的 BMP 瓦片，以 3 × 3 网格显示；支持触摸拖动、3～14 级缩放及定位居中。
- **坡度估算**：读取 LSM6DSM 加速度数据，根据设备倾角计算坡度百分比。
- **电源与按键**：电池电压采集、充电状态检测、电池图标、按键扫描和关机回调。
- **界面设置**：地图、骑行数据、设置三页切换，提供中英文选项和深浅色模式。

## 中英文界面

设备进入 **Settings → Language**，选择 **English** 或 **中文** 即可切换已有翻译的页面标签与设置文字。原始程序中的运行状态 `Running / Stopped` 仍使用英文。

项目文档也提供两种语言：本页为中文，点击顶部 **English** 查看完整英文介绍。

## 硬件与软件

| 模块 | 配置 |
| --- | --- |
| 主控 | STM32F405RGT6，168 MHz |
| 显示屏 | ST7789，240 × 320，SPI1 + DMA，16 位色深 |
| 电容触摸 | CST816，I²C2 |
| GPS | NMEA GGA / VTG 串口模块，USART2，9600 baud |
| 惯性传感器 | LSM6DSM，I²C1 |
| 存储 | SDIO SD 卡 + FatFs |
| 电源检测 | ADC1、充电检测 GPIO、供电控制 GPIO |
| 系统与 GUI | FreeRTOS / CMSIS-RTOS2、LVGL 9.4.0、STM32 HAL |
| 开发工具 | Keil MDK-ARM、STM32CubeMX |

具体引脚见 [`speed_meter.ioc`](speed_meter.ioc) 和 [`Core/Inc/main.h`](Core/Inc/main.h)。仓库未附原理图、PCB 或完整硬件 BOM。

## 软件结构

[`Core/Src/freertos.c`](Core/Src/freertos.c) 创建 GUI、GPS、惯性传感器、电源和按键五个任务。主任务初始化 LVGL、显示与触摸接口，挂载 SD 卡并运行 UI；设备任务采集数据，通过 LVGL 数据绑定和回调更新控件。

```text
.
├── application/ui/     # 页面、数据模型、翻译和瓦片坐标计算
├── interface/          # GPS、传感器、触摸、按键和电源
├── Core/               # 主程序、任务和外设初始化
├── Drivers/            # CMSIS 与 STM32 HAL
├── Middlewares/        # LVGL、FreeRTOS、FatFs 等
├── FATFS/              # SD 卡文件系统适配
├── USB_DEVICE/         # USB 设备配置
├── MDK-ARM/            # Keil 工程和启动文件
├── docs/images/        # 实物效果图
└── speed_meter.ioc     # CubeMX 工程配置
```

## 编译与运行

1. 克隆仓库：

   ```bash
   git clone https://github.com/SCr7-gif/stm32-gps-bike-computer.git
   cd stm32-gps-bike-computer
   ```

2. 准备 Keil MDK-ARM。原工程指定 **ARM Compiler 5.06 update 7（build 960）** 和 **Keil.STM32F4xx_DFP 3.1.1**；CubeMX 配置记录的固件包为 **STM32Cube FW_F4 V1.28.3**。
3. 打开 [`MDK-ARM/speed_meter.uvprojx`](MDK-ARM/speed_meter.uvprojx)，选择 `speed_meter` 目标并执行 Build / Rebuild。
4. 核对板卡供电和外设连接，在 Keil 中配置实际使用的调试器与下载接口后烧录。
5. 准备离线地图 SD 卡，启动设备，在有 GPS 信号的环境中检查定位及速度数据。

### 地图资源

地图代码通过 LVGL 读取如下路径：

```text
C:/map/{zoom}/{x}/{y}/tile.bmp
```

`C:` 是 LVGL 的 FatFs 逻辑盘符，并非电脑的 Windows C 盘。SD 卡根目录下应放置 `map` 文件夹，例如 `map/14/13372/7134/tile.bmp`。

瓦片大小为 **256 × 256 像素 BMP**，需要覆盖目标区域和所需缩放等级，并与代码使用的坐标体系匹配。**仓库不包含地图瓦片或下载工具，需要另行准备。**

## 项目说明

- 仓库包含设备工程源码及依赖、实物照片和中英文说明，保留原有固件业务逻辑与第三方版权声明。
- 地图功能为离线浏览和定位展示，当前源码未提供路线规划、转向导航或 GPX 文件导入导出。
- 里程、电池电量和坡度均采用当前代码中的估算方法，实际使用需结合硬件进行标定。
- 已检查 Keil 工程的 712 个源码引用均存在；本次整理未进行完整编译、烧录或硬件复测。照片为用户提供的实物效果。

## 第三方组件

LVGL、FreeRTOS、STM32 HAL / CMSIS、FatFs 及传感器驱动分别适用其自身许可。已有许可文件和版权声明均予以保留；仓库未对全部代码追加统一开源许可证。
