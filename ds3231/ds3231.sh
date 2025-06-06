#!/bin/bash

# echo 0x68 | sudo tee /sys/class/i2c-adapter/i2c-1/delete_device
sudo rmmod rtc_ds1307
i2c_addr=0x68
bus=1

# 将 BCD 转换为十进制
bcd2dec() {
  val=$(i2cget -y $bus $i2c_addr $1)
  val=$((0x${val#0x}))
  echo $(( (val >> 4) * 10 + (val & 0x0F) ))
}

# 读取时间寄存器并格式化输出
year=$(bcd2dec 0x06)
month=$(bcd2dec 0x05)
day=$(bcd2dec 0x04)
hour=$(bcd2dec 0x02)
minute=$(bcd2dec 0x01)
second=$(bcd2dec 0x00)

# 读取温度寄存器
temp_msb=$(i2cget -y $bus $i2c_addr 0x11)
temp_lsb=$(i2cget -y $bus $i2c_addr 0x12)

# 去掉 0x 前缀
temp_msb=$((0x${temp_msb#0x}))
temp_lsb=$((0x${temp_lsb#0x}))

# 合成温度（MSB 是整数，LSB 的高两位是小数部分 × 0.25）
temp_frac=$(( (temp_lsb >> 6) * 25 ))
temperature=$(echo "scale=2; $temp_msb + $temp_frac / 100" | bc)

# 输出结果
printf "DateTime: 20%02d-%02d-%02d %02d:%02d:%02d UTC\n" $year $month $day $hour $minute $second
printf "Temperature: %.2f °C\n" $temperature

# echo ds3231 0x68 | sudo tee /sys/class/i2c-adapter/i2c-1/new_device
sudo modprobe rtc_ds1307
