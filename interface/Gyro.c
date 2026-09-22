#include "Gyro.h"
#include "FreeRTOS.h"
#include "task.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include "ui.h"

extern osThreadId_t gyro_taskHandle;
extern ui_t *ui;

static int32_t GYRO_Read(void *handle, uint8_t reg_addr, uint8_t *buf, uint16_t len)
{
    return (int32_t)HAL_I2C_Mem_Read(handle, LSM6DSM_I2C_ADD_L, reg_addr, 1, buf, len, 1000);
}

static int32_t GYRO_Write(void *handle, uint8_t reg_addr, const uint8_t *buf, uint16_t len)
{
    return (int32_t)HAL_I2C_Mem_Write(handle, LSM6DSM_I2C_ADD_L, reg_addr, 1, (uint8_t *)buf, len, 1000);
}

int32_t GYRO_Init(stmdev_ctx_t *dev_ctx)
{
    uint8_t device_id = 0;
    vTaskDelay(pdMS_TO_TICKS(15));
    lsm6dsm_device_id_get(dev_ctx, &device_id);
    if (device_id != LSM6DSM_ID)
    {
        printf("LSM6DSM: Wrong device ID\n");
        return -1;
    }

    uint8_t rst = 1;
    lsm6dsm_reset_set(dev_ctx, PROPERTY_ENABLE);
    do
    {
        lsm6dsm_reset_get(dev_ctx, &rst);
    } while (rst);
    lsm6dsm_xl_data_rate_set(dev_ctx, LSM6DSM_XL_ODR_12Hz5);
    /* Configure filtering chain(No aux interface)
     * Accelerometer - analog filter
     */
    lsm6dsm_xl_filter_analog_set(dev_ctx, LSM6DSM_XL_ANA_BW_400Hz);
    /* Accelerometer - LPF1 path (LPF2 not used) */
    // lsm6dsm_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSM_XL_LP1_ODR_DIV_4);
    /* Accelerometer - LPF1 + LPF2 path */
    lsm6dsm_xl_lp2_bandwidth_set(dev_ctx,
                                 LSM6DSM_XL_LOW_NOISE_LP_ODR_DIV_100);
    lsm6dsm_gy_data_rate_set(dev_ctx, LSM6DSM_GY_ODR_12Hz5);
    lsm6dsm_int1_route_t int1_route = {
        .int1_drdy_xl = PROPERTY_ENABLE,
        .int1_drdy_g = PROPERTY_ENABLE,
    };
    lsm6dsm_pin_int1_route_set(dev_ctx, int1_route);
    return 0;
}

void LSM_INT1_IRQHandler(void)
{
    BaseType_t higherTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(gyro_taskHandle, &higherTaskWoken);
    portYIELD_FROM_ISR(higherTaskWoken);
}
void LSM_INT2_IRQHandler(void) {}

float slope_percentage;

float calculate_slope_percentage(float acceleration_mg[3])
{
    float slope_percentage = 0.0f;
    // mod = sqrt(x^2 + y^2 + z^2)
    float mod = sqrtf(acceleration_mg[0] * acceleration_mg[0] +
                      acceleration_mg[1] * acceleration_mg[1] +
                      acceleration_mg[2] * acceleration_mg[2]);

    // 归一化加速度
    float ax = acceleration_mg[0] / mod / 1000.0f;
    float ay = acceleration_mg[1] / mod / 1000.0f;
    float az = acceleration_mg[2] / mod / 1000.0f;

    if (az < 0)
    {
        az = -az;
    }

    // pitch = atan2(ay, sqrt(ax^2 + az^2))
    float pitch = atan2f(ay, sqrtf(ax * ax + az * az));
    // roll = atan2(ax, sqrt(ay^2 + az^2))
    float roll = atan2f(ax, sqrtf(ay * ay + az * az));
    // tilt = acos(az / sqrt(ax^2 + ay^2 + az^2))
    float total_tilt = acosf(az / sqrtf(ax * ax + ay * ay + az * az));
    // slope_percentage = tan(pitch) * 100.0f;
    slope_percentage = tanf(total_tilt) * 100.0f;

    // slope_percentage = tan(pitch) * 100.0f;

    return slope_percentage;
}

void update_slope_cb(void *argument)
{
    lv_subject_set_float(&ui->data.slope, slope_percentage);
}

void gyro_task_func(void *argument)
{
    vTaskDelay(200);
    int16_t data_raw_acceleration[3], data_raw_angular_rate[3];
    float acceleration_mg[3], angular_rate_mdps[3];
    stmdev_ctx_t dev_ctx = {
        .mdelay = vTaskDelay,
        .read_reg = GYRO_Read,
        .write_reg = GYRO_Write,
        .handle = &hi2c1,
    };
    printf("GYRO task started\n");
    if (GYRO_Init(&dev_ctx) != 0)
    {
        printf("GYRO init failed\n");
    }
    for (;;)
    {
        // 触发方式 => 中断int1 => 数据就绪  => i2c被打断 => 无法再次运行
        // 解决办法: (1) 不被打断 提高优先级 添加临界区
        // (2) 不使用数据就绪中断 使用80ms延迟
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        lsm6dsm_status_reg_t status_reg;
        lsm6dsm_status_reg_get(&dev_ctx, &status_reg);
        if (status_reg.xlda)
        {

            /* Read acceleration field data */
            memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
            lsm6dsm_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
            acceleration_mg[0] =
                lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[0]);
            acceleration_mg[1] =
                lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[1]);
            acceleration_mg[2] =
                lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[2]);
            // 异步更新
            slope_percentage = calculate_slope_percentage(acceleration_mg);
            // printf("slope_percentage: %f\n", slope_percentage);
            lv_async_call(update_slope_cb, NULL);
        }
        if (status_reg.gda)
        {
            /* Read angular rate field data */
            memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
            lsm6dsm_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
            angular_rate_mdps[0] =
                lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[0]);
            angular_rate_mdps[1] =
                lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[1]);
            angular_rate_mdps[2] =
                lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[2]);
        }
        vTaskDelay(80);
    }
}
