# DFRobot_BMV080

- [English Version](./README.md)

这款 BMV080 PM2.5 传感器模块设计紧凑、精度高且测量范围广。其核心采用博世最新研发的 BMV080 传感器元件——全球最小的 PM 空气质量传感器。其体积比市场上同类产品小 450 多倍。尽管尺寸大幅缩减，但性能丝毫不减。它不仅能精确测量空气中 PM2.5 颗粒的质量浓度，还支持 PM1 和 PM10 的检测。<br>
传统的 PM2.5 传感器通常依靠风扇或风道将自由漂浮的颗粒引入检测区域，因此体积较大，并伴有风扇噪音和灰尘堆积问题，这增加了维护成本和故障风险。然而，这款传感器采用类似于相机的测量原理，运用激光光学技术，根据自由空间中颗粒的数量和相对速度来计算质量浓度。它巧妙地利用周围自然气流驱动颗粒进入检测区域进行直接测量，无需风扇或强制气流系统，从而消除了维护麻烦，避免了风扇造成的灰尘堆积，显著提高了设备的可靠性。<br>
![Fermion_BMV080](image/Fermion_BMV080.JPG)

## 产品链接（[https://www.dfrobot.com.cn](https://www.dfrobot.com.cn)）
    SKU:SEN0663

## 目录

  * [概述](#概述)
  * [库安装](#库安装)
  * [方法](#方法)
  * [兼容性](#兼容性)
  * [历史](#历史)
  * [创作者](#创作者)

## 概述

DFRobot_BMV080 是一个为驱动博世新型空气质量测量芯片 BMV080 而设计的 Arduino 库。
BMV080 目前是世界上最小的 PM2.5 芯片，它采用激光进行测量。
此库提供了两种通信方式：IIC 和 SPI，以及一些基本的例程。目前，它仅支持 ESP32 平台。

## 库安装

要使用此库，请首先下载库文件，将其粘贴到“Arduino\libraries”目录中，然后打开“示例”文件夹并运行该文件夹中的演示程序。

### 升级SDK（可选）
如果您想要升级博世 BMV080 SDK，可以按照以下步骤操作。<br>

> [!注意]
> 由于博世目前仅提供了 BMV080 的预编译库，因此它只能支持该博世库所支持的平台。此次更新所使用的 SDK 来自博世。您可以从其官方网站下载并进行更新。[点击此处可跳转至官网](https://www.bosch-sensortec.com/products/environmental-sensors/particulate-matter-sensor/bmv080/#documents).<br>
> 本库已预存了版本为 Bosch bmv080-sdk-v11-1-0 的 SDK。您可以直接使用它，或者根据自己的需求替换为最新的 SDK。具体方法如下。

#### 安装SDK
您可以从博世官方网站下载该软件开发工具包。[点击此处可跳转至官网](https://www.bosch-sensortec.com/products/environmental-sensors/particulate-matter-sensor/bmv080/#documents).<br>

#### 替换掉原有的SDK
您需要将 DFRobot_BMV080 库中的文件替换为从下载的 SDK 中获取的文件。这一点至关重要，绝不能出错！！！<br>

##### DFRobot_BMV080 库路径
| OS | Directory|
|---|---|
|Windows | $HOME\Documents\Arduino\libraries\DFRobot_BMV080|
|Linux| $HOME/Arduino/libraries/DFRobot_BMV080|
|macOS | $HOME/Documents/Arduino/libraries/DFRobot_BMV080|
 <br>
解压已下载的 BMV080 SDK，然后按照以下路径进行替换操作。<br>

| Bosch SDK File | DFRobot_BMV080 library |
|---|---|
|api/inc/bmv080.h| src/bmv080.h|
|api/inc/bmv080_defs.h| src/bmv080_defs.h|
 <br>
以下文件可根据您的需求进行替换。只需选择相应的芯片进行更换，或者全部进行更换。<br>

| Bosch SDK File | DFRobot_BMV080 library |
|---|---|
|api/lib/xtensa_esp32/xtensa_esp32_elf_gcc/release/lib_bmv080.a | src/esp32/lib_bmv080.a|
|api/lib/xtensa_esp32/xtensa_esp32_elf_gcc/release/lib_postProcessor.a | src/esp32/lib_postProcessor.a|
|api/lib/xtensa_esp32s2/xtensa_esp32s3_elf_gcc/release/lib_postProcessor.a | src/esp32s2/lib_postProcessor.a|
|api/lib/xtensa_esp32s2/xtensa_esp32s3_elf_gcc/release/lib_bmv080.a | src/esp32s2/lib_bmv080.a|
|api/lib/xtensa_esp32s3/xtensa_esp32s3_elf_gcc/release/lib_postProcessor.a | src/esp32s3/lib_postProcessor.a|
|api/lib/xtensa_esp32s3/xtensa_esp32s3_elf_gcc/release/lib_bmv080.a | src/esp32s3/lib_bmv080.a|

## 方法

```C++
 /**
   * @fn begin
   * @brief 检测是否传感器已连接
   * @return 0 已连接
   * @return 1 未连接
   */
  int begin(void);
  
  /**
   * @fn openBmv080
   * @brief 初始化BMV080传感器
   * @pre 必须先调用此函数，以便为其他函数创建所需的“句柄”
   * @post 该“句柄”必须通过“bmv080_close”函数来销毁
   * @note 它必须在与传感器进行交互的任何其他功能执行之前被调用
   * @return 如果操作成功则返回值为 0，否则则返回一个错误代码
   */
  uint8_t openBmv080(void);

  /**
   * @fn closeBmv080
   * @brief 关闭传感器。此时传感器将停止工作。如果您需要再次使用它，需要调用 openBmv080 函数。
   * @pre 必须最后调用此函数，以便销毁由“bmv080_open"函数创建的"句柄"
   * @return 1 成功
   * @return 0 失败,句柄为空，或者在这之前没有调用“openBmv080 stopBmv080”函数
   */
  bool closeBmv080(void);

  /**
   * @fn resetBmv080
   * @brief 重置一个传感器单元，该单元包括硬件和软件部分
   * @pre 必须使用由“bmv080_open”函数生成的有效“句柄”
   * @post 通过“bmv080_set_parameter”更改的任何参数都会恢复到其默认值
   * @return 1 成功
   * @return 0 失败,在这之前没有调用“openBmv080 stopBmv080”函数
   */
  bool resetBmv080(void);

  /**
   * @fn getBmv080DV
   * @brief 获取 BMV080 传感器的驱动程序版本
   * @param major: 主要版本号
   * @param minor: 次要版本号
   * @param patch: 补丁版本号
   * @return 1 成功
   * @return 0 失败
   */
  bool getBmv080DV(uint16_t &major, uint16_t &minor, uint16_t &patch);

  /**
   * @fn getBmv080ID
   * @brief 获取此传感器的ID号。
   * @param id: 存放ID的数组
   * @return 1 成功
   * @return 0 失败
   */
  bool getBmv080ID(char *id);

  /**
   * @fn getBmv080Data
   * @brief 获取 BMV080 传感器的数据
   * @param PM1: PM1.0 浓度 (ug/m3)
   * @param PM2_5: PM2.5 浓度 (ug/m3)
   * @param PM10: PM10 浓度 (ug/m3)
   * @param allData: 所有数据,这是一个结构体，包含所有数据，包含一下成员
   *                 float runtime_in_sec：运行时间
   *                 float pm2_5_mass_concentration: PM2.5 浓度(ug/m3)
   *                 float pm1_mass_concentration: PM1.0 浓度(ug/m3)
   *                 float pm10_mass_concentration: PM10 浓度(ug/m3)
   *                 float pm2_5_number_concentration: PM2.5 浓度(particles/m3)
   *                 float pm1_number_concentration: PM1.0 浓度(particles/m3)
   *                 float pm10_number_concentration: PM10 浓度(particles/m3)
   *                 bool is_obstructed: 判断传感器是否被遮挡，从而判断数据是否有效。1 表示传感器被遮挡，0 表示传感器未被遮挡
   *                 bool is_outside_measurement_range：判断传感器是否在测量范围外(0..1000 ug/m3)1 表示传感器在范围外，0 表示传感器在范围内
   * @note 此功能每秒至少应被调用一次。
   * @return 1 成功, BMV080 传感器数据有效
   * @return 0 失败, BMV080 传感器数据无效
   */
  bool getBmv080Data(float *PM1, float *PM2_5, float *PM10, bmv080_output_t *allData=NULL);

  /**
   * @fn setBmv080Mode
   * @brief 设置 BMV080 传感器的模式
   * @param mode: 设置的模式可为：CONTINUOUS_MODE 或 DUTY_CYCLE_MODE
   *              CONTINUOUS_MODE: 传感器持续进行测量
   *              DUTY_CYCLE_MODE: 传感器会按照指定的时间间隔进行测量
   * @return 0 成功
   * @return -1 参数错误
   * @return -2 调用顺序有误,或者在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   * @return other 其他错误，请参考bmv080_defs.h中的bmv080_status_code_t相关错误码。
   */
  int setBmv080Mode(uint8_t mode);

  /**
   * @fn stopBmv080
   * @brief 停止测量。如果需要继续进行测量，则需要调用“setBmv080Mode”函数。
   * @pre 必须在数据采集周期结束时调用以确保传感器单元准备好下一个测量周期。
   * @return 1 成功
   * @return 0 错误 
   */
  bool stopBmv080(void);

  /**
   * @fn setIntegrationTime
   * @brief 设置测量窗口时间。
   * @note 在占空循环模式下，该测量窗口也是传感器开启时间。
   * @param integration_time 测量积分时间，单位为毫秒（ms）。
   * @return 0 成功
   * @return -1 integration_time不在有效范围内，必须大于等于1s
   * @return -2 integration_time 必须小于 duty_cycling_period 至少2s
   * @return -3 在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   * @return other 其他错误，请参考bmv080_defs.h中的bmv080_status_code_t相关错误码。
   */
  int setIntegrationTime(float integration_time);

  /**
   * @fn getIntegrationTime
   * @brief 获取当前积分时间。
   * @return 当前积分时间，单位为毫秒（ms）。
   * @return 0 错误，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  float getIntegrationTime(void);

  /**
   * @fn setDutyCyclingPeriod
   * @brief 设置占空循环周期。
   * @n 占空循环周期（积分时间和传感器关闭/休眠时间之和）。
   * @note 此值必须比积分时间至少大2秒。
   * @param duty_cycling_period 占空循环周期，单位为毫秒（ms）。
   * @return 0 成功
   * @return -1 duty_cycling_period 不在有效范围内，必须大于等于12s
   * @return -2 duty_cycling_period 必须大于 integration_time 至少2s.
   * @return -3 传感器还在持续运行种，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   * @return other 其他错误，请参考bmv080_defs.h中的bmv080_status_code_t相关错误码。
   */
  int setDutyCyclingPeriod(uint16_t duty_cycling_period);

  /**
   * @fn getDutyCyclingPeriod
   * @brief 获取当前占空循环周期。
   * @param duty_cycling_period 当前占空循环周期，单位为毫秒（ms）。
   * @return 1 成功
   * @return 0 错误，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  bool getDutyCyclingPeriod(uint16_t *duty_cycling_period);

  /**
   * @fn setObstructionDetection
   * @brief 设置是否启用阻塞检测功能。
   * @param obstructed 1 启用阻塞检测，0 禁用。
   * @return 1 成功
   * @return 0 错误，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  bool setObstructionDetection(bool obstructed);

  /**
   * @fn getObstructionDetection
   * @brief 获取阻塞检测功能是否启用状态。
   * @return 1 阻塞检测启用。
   * @return 0 阻塞检测禁用。
   * @return -1 获取失败，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  int getObstructionDetection(void);

  /**
   * @fn setDoVibrationFiltering
   * @brief 启用或禁用振动过滤功能。
   * @param do_vibration_filtering 1 启用，0 禁用。
   * @return 1 成功
   * @return 0 错误，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  bool setDoVibrationFiltering(bool do_vibration_filtering);

  /**
   * @fn getDoVibrationFiltering
   * @brief 获取振动过滤功能的状态。
   * @return 1 振动过滤启用。
   * @return 0 振动过滤禁用。
   * @return -1 获取失败，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  int getDoVibrationFiltering(void);

  /**
   * @fn setMeasurementAlgorithm
   * @brief 设置测量算法。
   * @param measurement_algorithm 使用的测量算法。
   *                              FAST_RESPONSE：响应迅速模式，适用于需要快速响应的场景
   *                              BALANCED：平衡模式，适用于需要在精度与快速响应之间取得平衡的场景
   *                              HIGH_PRECISION：高精度模式，适用于对精度有高要求的场景
   * @return 1 成功
   * @return 0 错误
   * @return -1 measurement_algorithm 不在有效范围内
   * @return -2 传感器还在持续运行种，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  int setMeasurementAlgorithm(uint8_t measurement_algorithm);

  /**
   * @fn getMeasurementAlgorithm
   * @brief 获取当前使用的测量算法。
   * @return 当前使用的测量算法。
   *         FAST_RESPONSE：响应迅速模式，适用于需要快速响应的场景
   *         BALANCED：平衡模式，适用于需要在精度与快速响应之间取得平衡的场景
   *         HIGH_PRECISION：高精度模式，适用于对精度有高要求的场景
   * @return 0 获取失败，在这之前没有调用“openBmv080 或者 stopBmv080”函数。
   */
  uint8_t getMeasurementAlgorithm(void);

  /**
   * @fn ifObstructed
   * @brief 检测传感器是否被遮挡。
   * @return 1 被遮挡。
   * @return 0 未被遮挡。
   */
  bool ifObstructed(void);
```

## 兼容性

MCU                | Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino uno        |              |       √      |             | 
Mega2560           |              |       √      |             | 
Leonardo           |              |       √      |             | 
ESP32              |      √       |              |             | 
micro:bit          |              |       √      |             | 
raspberry pi       |              |       √      |             |     
<br>

## 历史

- 2025/09/15 - Version 1.0.0  版本

## 创作者

作者: lbx(liubx8023@gmail.com), 2025. (欢迎访问我们的 [网站](https://www.dfrobot.com/))
