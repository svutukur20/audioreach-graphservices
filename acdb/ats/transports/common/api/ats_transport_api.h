#ifndef _ATS_TRANSPORT_API_H_
#define _ATS_TRANSPORT_API_H_
/**
*=============================================================================
* \file ats_transport_api.h
* \brief
*                    A T S  T R A N S P O R T  A P I
*
*   This file defines APIs for initializing/deinitializing ATS transports.
*
* \copyright
*     Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*     SPDX-License-Identifier: BSD-3-Clause
*
*=============================================================================
*/

/*------------------------------------------
* Includes
*------------------------------------------*/
#include "ar_osal_types.h"

/**
*	\brief
*		The command + response callback used to pass request to the 
*       application layer and return responses to the transport layer
*
*	\detdesc
*
*	\param [in] req_buffer: The request buffer containing the command to execute
*	\param [in] req_buffer_length: The length of the request buffer
*	\param [in/out] resp_buffer: The response buffer returned after executing the command
*	\param [out] resp_buffer_length: The length of the response buffer
*
*	\return
*		0 on success, non-zero on failure
*/
typedef void(*ats_cmd_rsp_callback_t)(
    uint8_t *req_buffer, uint32_t req_buffer_length, 
    uint8_t **resp_buffer, uint32_t *resp_buffer_length);

/**
*	\brief
*		Initializes the ATS TCP/IP server
*
*	\detdesc
*		Entry function to start ats_server threads
*		ats_server will start a listening thread on a local IP address on the
*		designated port.
*		ats communicates with the Gateway Server for any connection to external clients
*
*		Note: Only one client can be connected to this target at a time. The use of a mutex
*		ensures this.
*
*	\param [in] ats_cb: A callback to ats_exectue_command
*
*	\return
*		0 on success, non-zero on failure
*/
int32_t ats_transport_init(ats_cmd_rsp_callback_t cmd_rsp_callback);

/**
* \brief tcpip_cmd_server_deinit
*		De-initializes the transport by realeasing resources aquired during ats_transport_init
* \return 0 on success, non-zero on failure
*/
int32_t ats_transport_deinit(void);

#endif /*_ATS_TRANSPORT_API_H_*/

