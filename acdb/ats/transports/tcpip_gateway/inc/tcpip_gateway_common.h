#ifndef _TCPIP_GATEWAY_COMMON_H_
#define _TCPIP_GATEWAY_COMMON_H_
/**
*=============================================================================
* \file tcpip_gateway_common.h
*
*                      T C P I P  G A T E W A Y  C O M M O N
*                             H E A D E R  F I L E
* 
* \brief
*      Common header file used by GATEWAY modules.
* \cond
*  Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved. 
*  SPDX-License-Identifier: BSD-3-Clause
* \endcond
*=============================================================================
*/

#include "ar_osal_types.h"
#include "ar_osal_log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GATEWAY_LOG_TAG  "GATEWAY"
#define GATEWAY_ERR(...)  AR_LOG_ERR(GATEWAY_LOG_TAG, __VA_ARGS__)
#define GATEWAY_DBG(...)  AR_LOG_DEBUG(GATEWAY_LOG_TAG, __VA_ARGS__)
#define GATEWAY_INFO(...)  AR_LOG_INFO(GATEWAY_LOG_TAG, __VA_ARGS__)

#endif
