#!/bin/bash
sign_tool=../../../../../tools/bin/sign_tool/sign_tool_pltuni

# # ======================= 随机生成签名私钥 & 根公钥 ==============================================
# 生成根私钥/公钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_oem_root_private_key.pem
openssl ec -in ec_bp256_oem_root_private_key.pem -pubout -out ec_bp256_oem_root_public_key.pem
# 生成ssb私钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_ssb_private_key.pem
# 生成flashboot私钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_flash_private_key.pem
# 生成flashboot_backup私钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_flashboot_backup_private_key.pem
# 生成loaderboot私钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_loaderboot_private_key.pem
# 生成param私钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_param_private_key.pem
# 生成app私钥
openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_app_private_key.pem
