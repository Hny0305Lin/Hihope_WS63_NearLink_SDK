#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mbedtls")

set(SOURCES
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/aes.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/aesni.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/aria.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/asn1parse.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/asn1write.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/base64.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/bignum.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/camellia.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ccm.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/chacha20.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/chachapoly.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/cipher.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/cipher_wrap.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/cmac.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/constant_time.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ctr_drbg.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/debug.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/des.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/dhm.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ecdh.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ecdsa.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ecjpake.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ecp.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ecp_curves.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/entropy.c
	# ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/entropy_poll.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/error.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/gcm.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/hmac_drbg.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/hkdf.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/md.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/md5.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/memory_buffer_alloc.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/mps_reader.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/mps_trace.c
	# ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/net_sockets.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/nist_kw.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/oid.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/padlock.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pem.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pk.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pk_wrap.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pkcs5.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pkcs12.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pkparse.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/pkwrite.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/platform.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/platform_util.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/poly1305.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ripemd160.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/rsa.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/rsa_alt_helpers.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/sha1.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/sha256.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/sha512.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_cache.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_ciphersuites.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_cli.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_cookie.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_debug_helpers_generated.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_msg.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_srv.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_ticket.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_tls.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_tls13_client.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_tls13_generic.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_tls13_server.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/ssl_tls13_keys.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/threading.c
	# ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/timing.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/version.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/version_features.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509_create.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509_crl.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509_crt.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509_csr.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509write_crt.c
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/x509write_csr.c
)

set(PUBLIC_HEADER
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include/psa
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include/mbedtls
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/3rdparty/everest/include/everest
)

set(PRIVATE_HEADER
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include/psa
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include/mbedtls
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/3rdparty/everest/include/everest
)

set(PRIVATE_DEFINES
	SYSCALLS_H
)

if ((NOT ${CHIP} STREQUAL "ws63") AND (NOT ${CHIP} STREQUAL "ws53"))
	list(APPEND SOURCES
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_aead.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_cipher.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_client.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_driver_wrappers.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_ecp.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_hash.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_mac.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_rsa.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_se.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_slot_management.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_crypto_storage.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/psa_its_file.c
	)
endif()

if ((${CHIP} STREQUAL "brandy") OR (${CHIP} STREQUAL "sw21"))
	list(INSERT PRIVATE_DEFINES 0
		MBEDTLS_USER_CONFIG_FILE="mbedtls_platform_hardware_config.h"
	)

	list(APPEND SOURCES
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/entropy_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cipher_adapt.c
	)

	list(APPEND PRIVATE_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/internal_include
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/platform/wearable
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/cipher_adapt_include
	)
elseif ((${CHIP} STREQUAL "ws63") OR (${CHIP} STREQUAL "ws53"))
	list(APPEND PRIVATE_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/configs/vendor
	)
	list(APPEND PUBLIC_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/configs/vendor
	)
	list(APPEND SOURCES
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/library/net_sockets.c
	)
endif()

if("MBEDTLS_HARDEN_OPEN" IN_LIST DEFINES)
	list(APPEND PRIVATE_DEFINES
		MBEDTLS_USER_CONFIG_FILE="mbedtls_platform_hardware_config.h"
	)
	list(APPEND SOURCES
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cipher_adapt.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cipher_common.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cmac_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/ecc_harden_common.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/ecc_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/hkdf_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/pbkdf2_hmac_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/bignum_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/entropy_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/dfx.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/gcm_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/ccm_harden.c

		# ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/hash_harden_common.c
		# ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/hash_harden.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/hash_harden_common.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/hash_harden_adapt.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/sha1.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/sha256.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/sha512.c

		# ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/aes_harden.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/aes.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/aes_harden_adapt.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/rsa.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/rsa_harden_adapt.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/ecp.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/ecp_harden_adapt.c
	)
	list(APPEND PRIVATE_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/internal_include
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/platform/connect
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/cipher_adapt_include
	)
	list(APPEND PUBLIC_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/cipher_adapt_include
	)
elseif("FUZZ_TEST" IN_LIST DEFINES)
	list(APPEND PRIVATE_DEFINES
		MBEDTLS_USER_CONFIG_FILE="mbedtls_platform_hardware_config.h"
		MBEDTLS_CONFIG_FILE="mbedtls_platform_fuzz_config.h"
	)
	list(APPEND SOURCES
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/aes.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/aes_harden_adapt.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cipher_adapt.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cipher_common.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/cmac_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/ecc_harden_common.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/ecc_harden.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/hash_harden_common.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/hash_harden_adapt.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/sha1.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/sha256.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/sha512.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/hkdf_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/pbkdf2_hmac_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/bignum_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/entropy_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/dfx.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/gcm_harden.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/ccm_harden.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/rsa.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/rsa_harden_adapt.c

		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/ecp.c
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/ecp_harden_adapt.c
	)
	list(APPEND PRIVATE_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/internal_include
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/platform/connect
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/cipher_adapt_include
	)
	list(APPEND PUBLIC_HEADER
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/internal_include
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/platform/connect
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/cipher_adapt_include
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src
	)
else()
	if(${OS} STREQUAL "liteos")
		list (APPEND PRIVATE_DEFINES
			MBEDTLS_ECP_RESTARTABLE
		)
	endif()
endif()

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
	-Wno-error=switch-default
	-Wno-error=unused-parameter
    -Wno-error=unused-variable
    -Wno-error=strict-prototypes
    -Wno-error=sign-compare
    -Wno-error=return-type
    -Wno-error=unused-value
    -Wno-error=comment
    -Wno-error=pointer-sign
    -Wno-error=attributes
    -Wno-error=uninitialized
    -Wno-error=unused-function
	-Wno-error=format
	-Wno-error=incompatible-pointer-types
)

if("${ARCH}" STREQUAL "riscv31")
list (APPEND COMPONENT_CCFLAGS
	-Wno-error=jump-misses-init
)
endif()

if("${ARCH}" STREQUAL "cortex_m7")
	list (APPEND COMPONENT_CCFLAGS
			-Wno-error=jump-misses-init
	)
endif()


set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
